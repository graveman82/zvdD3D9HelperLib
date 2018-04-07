/* This file is a part of Zavod3D engine project.
It's licensed unser the MIT license (see "License.txt" for details).*/

/* Реализация создания устройства Direct3D9.
*/

#include <vector>
#include <stdio.h>
#include "z3DD3D9HL.h"
#include "z3DD3D9HLPrivVideomode.h"
#include "z3DDebugSystem.h"

namespace z3D_priv
{

uint32_t D3DFormat2Bpp( D3DFORMAT d3dfmt );
bool D3DFormatHasAlpha( D3DFORMAT d3dfmt );
bool D3DFormatHasStencil( D3DFORMAT d3dfmt );

/* Поиск подходящего формата глубины.
    @param d3d указатель на созданный объект главного интерфейса Direct3D9.
    @param bpp число бит на пиксель в экранном буфере.
    @param dpFmt формат пикселей дисплея.
    @param bbFmt формат пикселей заднего буфера.
    @param multiSampleType уровень мультисэмплинга (см. справку DX SDK).
    @param qualityLevels (см. справку DX SDK).
    @param fWindowed true(false) оконный (полноэкранный)режим работы приложения.
    @param fStencilOnly учитывать только форматы буфера глубины, содержащие трафарет.
    @param iAdapter номер видеоадаптера, по отношению к которому проверяются доступные видеорежимы.
    Номер видеоадаптера может варьироваться от нуля до числа, меньшего на единицу числа видеоадаптеров,
    установленных в системе ( см. справку DX SDK ).
    @param deviceType тип устройства Direct3D9 ( см. справку DX SDK ).
    @return формат буфера глубины ( см. справку DX SDK ).
*/
D3DFORMAT D3D9HL_FindApprDepthFormat(LPDIRECT3D9 d3d,
                                     uint8_t bpp,
                                     D3DFORMAT dpFmt,
                                     D3DFORMAT bbFmt,
                                     D3DMULTISAMPLE_TYPE multiSampleType,
                                     DWORD *qualityLevels,
                                     bool fWindowed = false,
                                     bool fStencilOnly = false,
                                     uint32_t iAdapter = D3DADAPTER_DEFAULT,
                                     D3DDEVTYPE deviceType = D3DDEVTYPE_HAL){
    Z3D_ASSERT_HIGH(d3d != 0, "null pointer to main Direct3D object passed", true);

    // Доступные форматы пикселей буфера глубины для выбора согласно справки DX SDK
    std::vector<D3DFORMAT> dsFmtVec;
    dsFmtVec.reserve(7);
    if (bpp == 32) {
        // 32 bit
        dsFmtVec.push_back(D3DFMT_D24S8);
        dsFmtVec.push_back(D3DFMT_D24X4S4);
        if (!fStencilOnly) {
            dsFmtVec.push_back(D3DFMT_D24X8);
            dsFmtVec.push_back(D3DFMT_D32);
        }
    }
    // 16 bit
    dsFmtVec.push_back(D3DFMT_D15S1);
    if (!fStencilOnly)
        dsFmtVec.push_back(D3DFMT_D16);

    dsFmtVec.push_back(D3DFMT_UNKNOWN);

    // Перебор форматов с целью выбора наилучшего из поддерживаемых
    size_t iFmtFound = Z3D_D3D9HL_NOINDEX;
    for (size_t iFmt = 0; iFmt < dsFmtVec.size(); ++iFmt) {
        if (dsFmtVec[iFmt] == D3DFMT_UNKNOWN)
            break;
        // Проверка возможности использования формата в качестве буфера глубины
        HRESULT hr = d3d->CheckDeviceFormat(static_cast<UINT>(iAdapter),
                                            deviceType,
                                            dpFmt,
                                            D3DUSAGE_DEPTHSTENCIL,
                                            D3DRTYPE_SURFACE,
                                            dsFmtVec[iFmt]);

        if (FAILED(hr))continue;

        // Проверка совместимости форматов глубины и заднего буфера
        hr = d3d->CheckDepthStencilMatch(static_cast<UINT>(iAdapter),
                                         deviceType,
                                         dpFmt,
                                         bbFmt,
                                         dsFmtVec[iFmt]);

        if (FAILED(hr))continue;

        // Проверка совместимости формата заднего буфера и уровня мультисэмплинга
        DWORD dsQualityLevels;
        if (multiSampleType != D3DMULTISAMPLE_NONE) {
            hr = d3d->CheckDeviceMultiSampleType(static_cast<UINT>(iAdapter),
                                             deviceType,
                                             dsFmtVec[iFmt],
                                             fWindowed ? TRUE : FALSE,
                                             multiSampleType,
                                             &dsQualityLevels);
            if (FAILED(hr)) continue;
        }
        iFmtFound = iFmt;
        if (multiSampleType != D3DMULTISAMPLE_NONE && qualityLevels != 0 && *qualityLevels > dsQualityLevels)
            *qualityLevels = dsQualityLevels;
        break;
    }

    D3DFORMAT dsFmt = D3DFMT_UNKNOWN;
    if (iFmtFound == Z3D_D3D9HL_NOINDEX)
        return dsFmt;

    dsFmt = dsFmtVec[iFmtFound];
    return dsFmt;
}

} // end of z3D_priv
namespace z3D
{

z3DD3D9HL_ErrCodes D3D9HL_FindVideoModes(z3DD3D9HL_VideoMode* videoModes,
                                         uint16_t* numVideoModes,
                                         LPDIRECT3D9 d3d,
                                         uint8_t bpp,
                                         D3DMULTISAMPLE_TYPE multiSampleType,
                                         DWORD *qualityLevels,
                                         bool fWindowed,
                                         bool fAlphaInBBOnly,
                                         bool fStencilOnly,
                                         uint32_t iAdapter,
                                         D3DDEVTYPE deviceType){
    Z3D_ASSERT_HIGH(d3d != 0, "null pointer to main Direct3D object passed", true);
    if (d3d == 0)
        return Z3D_D3D9HL_INVALIDCALL;
    Z3D_ASSERT_HIGH(numVideoModes != 0, "null passed", true);
    if (numVideoModes == 0)
        return Z3D_D3D9HL_INVALIDCALL;
    Z3D_ASSERT_HIGH( bpp == 16 || bpp == 32, "unacceptable value passed for bpp", true);
    if (!(bpp == 16 || bpp == 32))
        return Z3D_D3D9HL_INVALIDCALL;

    // Доступные форматы пикселей заднего буфера для выбора согласно справки DX SDK
    std::vector<D3DFORMAT> bbFmtVec;
    bbFmtVec.reserve(6);
    if (bpp == 32){
        bbFmtVec.push_back(D3DFMT_A8R8G8B8);        // 0
        if (!fAlphaInBBOnly)
            bbFmtVec.push_back(D3DFMT_X8R8G8B8);    // 1
    }
    else if (bpp == 16){
        bbFmtVec.push_back(D3DFMT_A1R5G5B5);        // 2
        if (!fAlphaInBBOnly) {
            bbFmtVec.push_back(D3DFMT_X1R5G5B5);    // 3
            bbFmtVec.push_back(D3DFMT_R5G6B5);      // 4
        }
    }
    else {
        Z3D_ERROR1( Z3D_ERROR_PERMISSIBLE, 0, "unacceptable value passed for bpp: %d", bpp, false);
        return Z3D_D3D9HL_INVALIDCALL;
    }
    bbFmtVec.push_back(D3DFMT_UNKNOWN);

    // Доступные форматы пикселей дисплея для выбора согласно справки DX SDK
    std::vector<D3DFORMAT> dpFmtVec = bbFmtVec;
    for (size_t iFmt = 0; iFmt < dpFmtVec.size(); ++iFmt){
        if (dpFmtVec[iFmt] == D3DFMT_A8R8G8B8)
            dpFmtVec[iFmt] = D3DFMT_X8R8G8B8;
        else if (dpFmtVec[iFmt] == D3DFMT_A1R5G5B5)
            dpFmtVec[iFmt] = D3DFMT_X1R5G5B5;
    }

    // Перебор форматов с целью выбора наилучшего из поддерживаемых
    size_t iFmtFound = Z3D_D3D9HL_NOINDEX;
    D3DFORMAT dsFmt = D3DFMT_UNKNOWN;
    for (size_t iFmt = 0; iFmt < dpFmtVec.size(); ++iFmt) {
        if (dpFmtVec[iFmt] == D3DFMT_UNKNOWN)
            break;

        // Проверяем возможность работы устройства на адаптере при заданном формате
        // дисплея и заднего буфера
        HRESULT hr = d3d->CheckDeviceType(static_cast<UINT>(iAdapter),
                                     deviceType,
                                     dpFmtVec[iFmt],
                                     bbFmtVec[iFmt],
                                     fWindowed ? TRUE : FALSE);
        if (FAILED(hr)) continue;

        // Проверка совместимости формата заднего буфера и уровня мультисэмплинга
        if (multiSampleType != D3DMULTISAMPLE_NONE) {
            hr = d3d->CheckDeviceMultiSampleType(static_cast<UINT>(iAdapter),
                                             deviceType,
                                             bbFmtVec[iFmt],
                                             fWindowed ? TRUE : FALSE,
                                             multiSampleType,
                                             qualityLevels);
            if (FAILED(hr)) continue;
        }

        // Поиск подходящего формата глубины
        dsFmt = z3D_priv::D3D9HL_FindApprDepthFormat(d3d,
                                                 bpp,
                                                 dpFmtVec[iFmt],
                                                 bbFmtVec[iFmt],
                                                 multiSampleType,
                                                 qualityLevels,
                                                 fWindowed,
                                                 fStencilOnly,
                                                 iAdapter,
                                                 deviceType);
        if (dsFmt == D3DFMT_UNKNOWN) continue;

        iFmtFound = iFmt;
        break;
    }
    if (iFmtFound == Z3D_D3D9HL_NOINDEX)
        return Z3D_D3D9HL_NOTFOUND;

    // Форматы заднего буфера, дисплея и шлубины найдены,
    // производим поиск видеорежимов
    uint32_t nModes = static_cast<UINT>(d3d->GetAdapterModeCount(static_cast<UINT>(iAdapter), dpFmtVec[iFmtFound]));

    std::vector<z3DD3D9HL_VideoMode> modesFound;
    modesFound.reserve(nModes);
    for (uint32_t iMode = 0; iMode < nModes; iMode++){
        D3DDISPLAYMODE dm;
        HRESULT hr = d3d->EnumAdapterModes(static_cast<UINT>(iAdapter),
                                           dpFmtVec[iFmtFound],
                                           static_cast<UINT>(iMode),
                                           &dm);
        if (FAILED(hr)) continue;
        z3DD3D9HL_VideoMode mode;
        mode.bpp_ = bpp;
        mode.d3ddm_ = dm;
        mode.depthStencilFmt_ = dsFmt;
        mode.fAlphaInBB_ = z3D_priv::D3DFormatHasAlpha(dm.Format);
        mode.fStencil_ = z3D_priv::D3DFormatHasStencil(dsFmt);
        modesFound.push_back(mode);
    }

    // Определяем режимы дисплея с равной или наиболее близкой большей частотой развертки
    //

    // Определяем текущую частоту
    HDC hDCScreen = ::GetDC(0);
    int curRefresh = ::GetDeviceCaps(hDCScreen, VREFRESH);
    ::ReleaseDC(0, hDCScreen);

    z3D_priv::LeaveVideoModeWithClosestRefreshRates(modesFound, static_cast<uint32_t>(curRefresh));

    *numVideoModes = modesFound.size();
    if (videoModes == 0){
        return Z3D_D3D9HL_NONE;
    }
    for (uint32_t iMode = 0; iMode < modesFound.size(); ++iMode){
        videoModes[iMode] = modesFound[iMode];
    }
    return Z3D_D3D9HL_NONE;

}

z3DD3D9HL_ErrCodes D3D9HL_CreateDevice(LPDIRECT3DDEVICE9* device,
                                       D3DPRESENT_PARAMETERS* presentParams,
                                       uint32_t* pVertexProcessingType,
                                       LPDIRECT3D9 d3d,
                                       const z3DD3D9HL_VideoMode& videoMode,
                                       D3DMULTISAMPLE_TYPE multiSampleType,
                                       DWORD qualityLevel,
                                       bool fWindowed,
                                       bool fVSync,
                                       HWND hWnd,
                                       uint32_t iAdapter,
                                       D3DDEVTYPE deviceType){
    if (hWnd == 0) hWnd = ::GetActiveWindow();
    if (hWnd == 0) {
        Z3D_ERROR(Z3D_ERROR_PERMISSIBLE, hWnd == 0, "No active window", false);
        return Z3D_D3D9HL_NO_PRELIMINARY_DONE;
    }
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

    if (!fWindowed) {
        d3dpp.BackBufferWidth = static_cast<UINT>(videoMode.Width());
        d3dpp.BackBufferHeight = static_cast<UINT>(videoMode.Height());
    }
    d3dpp.BackBufferFormat = videoMode.d3ddm_.Format;
    d3dpp.BackBufferCount = 1;
    d3dpp.MultiSampleType = multiSampleType;
    d3dpp.MultiSampleQuality = qualityLevel;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Windowed = fWindowed ? TRUE : FALSE;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = videoMode.depthStencilFmt_;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = fWindowed ? 0 : static_cast<UINT>(videoMode.RefreshRate());
    d3dpp.PresentationInterval = fVSync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    D3DCAPS9 devCaps;
    DWORD  vertexProcessingType = 0;
	d3d->GetDeviceCaps( static_cast<UINT>(iAdapter), deviceType, &devCaps );
	if ( devCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ){
        vertexProcessingType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        HRESULT hr = d3d->CreateDevice(static_cast<UINT>(iAdapter),
                                     deviceType,
                                     hWnd,
                                     vertexProcessingType,
                                     &d3dpp,
                                     device);
        if (FAILED(hr)){
            vertexProcessingType = D3DCREATE_MIXED_VERTEXPROCESSING;
            hr = d3d->CreateDevice(static_cast<UINT>(iAdapter),
                                     deviceType,
                                     hWnd,
                                     vertexProcessingType,
                                     &d3dpp,
                                     device);
            if (FAILED(hr)){
                vertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
                hr = d3d->CreateDevice(static_cast<UINT>(iAdapter),
                                         deviceType,
                                         hWnd,
                                         vertexProcessingType,
                                         &d3dpp,
                                         device);
                if (FAILED(hr)) return Z3D_D3D9HL_NOTAVAILABLE;
            }
        }
    }
	else {
		vertexProcessingType |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		HRESULT hr = d3d->CreateDevice(static_cast<UINT>(iAdapter),
                                deviceType,
                                hWnd,
                                vertexProcessingType,
                                &d3dpp,
                                device);
        if (FAILED(hr)) return Z3D_D3D9HL_NOTAVAILABLE;
	}
	if (pVertexProcessingType != 0)
        *pVertexProcessingType = vertexProcessingType;
	if (presentParams != 0)
        *presentParams = d3dpp;
    return Z3D_D3D9HL_NONE;
}

namespace z3D_priv
{
struct D3D9HL_RenderState{
    bool fBegin_;
    D3D9HL_RenderState() :
        fBegin_(false){
    }
};
static D3D9HL_RenderState s_renderState;
} // end of z3D_priv
z3DD3D9HL_ErrCodes D3D9HL_BeginDeviceRender(LPDIRECT3DDEVICE9 device,
                                            D3DPRESENT_PARAMETERS* presentParams,
                                            Z3D_D3D9HL_ReleaseDeviceResourcesFunc releaseFunc,
                                            Z3D_D3D9HL_ResetDeviceResourcesFunc resetFunc) {
    Z3D_ASSERT(!z3D_priv::s_renderState.fBegin_, "It's seems BeginDeviceRender called twice", true);
    Z3D_ASSERT(device != 0, "null device passed", true);
    Z3D_ASSERT(presentParams != 0, "no present parameters passed", true);
    Z3D_ASSERT(releaseFunc != 0, "no device resource release function passed", true);
    Z3D_ASSERT(resetFunc != 0, "no device resource reset function passed", true);

    // Проверить не потеряно ли устройство
    HRESULT hr = device->TestCooperativeLevel();
    if (hr != D3D_OK){
        Z3D_INFO("Direct3D device lost");
        // Устройство потеряно - не рендерим ничего. Ждем, когда его можно будет перезагрузить
        if (hr == D3DERR_DEVICELOST){
            return Z3D_D3D9HL_DEVICE_LOST;
        }
        // Устройство потеряно, но может быть перезагружено - не рендерим в этом фрейме
        else if (hr == D3DERR_DEVICENOTRESET){
            releaseFunc();
            hr = device->Reset( presentParams );
			if (hr == D3D_OK)
                resetFunc();
            return Z3D_D3D9HL_DEVICE_NOT_RESET;
        }
    }
    hr = device->BeginScene();
    if (FAILED(hr)){
        Z3D_ERROR(Z3D_ERROR_PERMISSIBLE, FAILED(hr), "It's seems BeginDeviceRender called twice", false);
        return Z3D_D3D9HL_INVALIDCALL;
    }
    z3D_priv::s_renderState.fBegin_ = true;
    return Z3D_D3D9HL_NONE;
}

z3DD3D9HL_ErrCodes D3D9HL_EndDeviceRender(LPDIRECT3DDEVICE9 device, HWND hDestWindow){
    Z3D_ASSERT(z3D_priv::s_renderState.fBegin_, "BeginDeviceRender is not called yet", true);
    if (!z3D_priv::s_renderState.fBegin_){
        Z3D_ERROR(Z3D_ERROR_PERMISSIBLE, !z3D_priv::s_renderState.fBegin_, "BeginDeviceRender is not called yet", false);
        return Z3D_D3D9HL_INVALIDCALL;
    }
    Z3D_ASSERT(device != 0, "null device passed", true);
    device->EndScene();
    device->Present(0, 0, hDestWindow, 0);
    z3D_priv::s_renderState.fBegin_ = false;
    return Z3D_D3D9HL_NONE;
}

} // end of z3D

