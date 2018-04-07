/* This file is a part of Zavod3D engine project.
It's licensed unser the MIT license (see "License.txt" for details).*/

#ifndef Z3DD3D9HL_H
#define Z3DD3D9HL_H

/* Подключить этот файл к проекту, использующему библиотеку Zavod3D D3D9 Helper Lib.
*/

/** @page Desc Описание библиотеки

Библиотека Zavod3D D3D9 Helper Lib предназначена для упрощения использования библиотеки
Direct3D9. Она берет на себя создание сложных объектов библиотеки Direct3D9, предоставляя
пользователю более упрощенный интерфейс.
Таким образом, она не скрывает полностью взаимодействие с DirectX, а только служит
инструментом, облегчающим его использование наподобие того как это делает библиотека
D3DX, встроенная в DirectX.
*/

#include <d3d9.h>
#include <d3dx9.h>


#include "z3DD3D9HLDef.h"

/** @file z3DD3D9HL.h */

/** @page VideomodesAndDevice Поиск видеорежимов и создание графического устройства.
*/

namespace z3D
{
/** Получить информацию о доступных видеорежимах.

    Практический подход состоит в том, чтобы определить вектор типа z3DD3D9HL_VideoMode,
    вызвать этот метод с нулем в первом параметре для получения числа поддерживаемых видеорежимов,
    а затем еще раз вызвать этот метод, передав адрес первого элемента вектора в первом параметре.
    @code
    std::vector<z3DD3D9HL_VideoMode> videoModes;
    uint16_t n;
    z3D::D3D9HL_FindVideoModes(0, &n, d3d, 32, 0, 0, true);
    if (n > 0){
        videoModes.resize(n);
        D3D9HL_FindVideoModes(&videoModes[0], &n, d3d, 32, 0, 0, true);
    }
    @endcode
    @param [out] videoModes массив, который заполняется информацией об обнаруженных видеорежимах ( @see z3DD3D9HL_VideoMode ).
    Можно передать в этом параметре нуль.
    @param numVideoModes указатель на переменную, в которую будет сохранено число найденных видеорежимов.
    @param d3d указатель на объект главного интерфейса Direct3D9.
    @param bpp число бит на пиксель в экранном буфере.
    @param multiSampleType уровень мультисэмплинга (см. справку DX SDK).
    @param qualityLevels (см. справку DX SDK).
    @param fWindowed true(false) оконный (полноэкранный)режим работы приложения.
    @param fAlphaInBBOnly учитывать только форматы заднего буфера, содержащие альфа-канал.
    @param fStencilOnly учитывать толкьо форматы буфера глубины, содержащие трафарет.
    @param iAdapter номер видеоадаптера, по отношению к которому проверяются доступные видеорежимы.
    Номер видеоадаптера может варьироваться от нуля до числа, меньшего на единицу числа видеоадаптеров,
    установленных в системе ( см. справку DX SDK ).
    @param deviceType тип устройства Direct3D9 ( см. справку DX SDK ).
    @return код ошибки ( @see z3DD3D9HL_ErrCodes ).
*/
z3DD3D9HL_ErrCodes D3D9HL_FindVideoModes(z3DD3D9HL_VideoMode* videoModes,
                                         uint16_t* numVideoModes,
                                         LPDIRECT3D9 d3d,
                                         uint8_t bpp,
                                         D3DMULTISAMPLE_TYPE multiSampleType = D3DMULTISAMPLE_NONE,
                                         DWORD *qualityLevels = 0,
                                         bool fWindowed = false,
                                         bool fAlphaInBBOnly = false,
                                         bool fStencilOnly = false,
                                         uint32_t iAdapter = D3DADAPTER_DEFAULT,
                                         D3DDEVTYPE deviceType = D3DDEVTYPE_HAL);

/** Создание устройства Direct3D9.
    @param [out] device указатель на указатель на устройство для сохранения адреса объекта созданного устройства.
    @param [out] presentParams для сохранения параметров презентации.
    @param [out] pVertexProcessingType для сохранения флага обработки вершин.
    @param d3d указатель на объект главного интерфейса Direct3D9.
    @param videoMode видеорежим из массива, который д.б. заполнен функцией D3D9HL_FindVideoModes.
    @param multiSampleType уровень мультисэмплинга (см. справку DX SDK).
    @param qualityLevel (см. справку DX SDK).
    @param fWindowed true(false) оконный (полноэкранный)режим работы приложения.
    @param fVsync (false)true - (не)использовать вертикальную синхронизацию.
    @param hWnd дескриптор окна, в котором будет производиться визуализация при помощи создаваемого
        графического устройсва. Если 0, будет использовано окно с активным фокусом.
    @param iAdapter номер видеоадаптера, по отношению к которому проверяются доступные видеорежимы.
        Номер видеоадаптера может варьироваться от нуля до числа, меньшего на единицу числа видеоадаптеров,
        установленных в системе ( см. справку DX SDK ).
    @param deviceType тип устройства Direct3D9 ( см. справку DX SDK ).
    @return Код ошибки ( @see z3DD3D9HL_ErrCodes ).
*/
z3DD3D9HL_ErrCodes D3D9HL_CreateDevice(LPDIRECT3DDEVICE9* device,
                                       D3DPRESENT_PARAMETERS* presentParams,
                                       uint32_t* pVertexProcessingType,
                                       LPDIRECT3D9 d3d,
                                       const z3DD3D9HL_VideoMode& videoMode,
                                       D3DMULTISAMPLE_TYPE multiSampleType = D3DMULTISAMPLE_NONE,
                                       DWORD qualityLevel = 0,
                                       bool fWindowed = false,
                                       bool fVSync = false,
                                       HWND hWnd = 0,
                                       uint32_t iAdapter = D3DADAPTER_DEFAULT,
                                       D3DDEVTYPE deviceType = D3DDEVTYPE_HAL);

/** Запуск рендера на устройстве Direct3D9.

Производится автоматическая проверка поетри устройства и его восстановления.
    @param device указатель на устройство.
    @param presentParams параметры презентации на случай потери устройства, можно взять параметры,
    возвращенные из функции D3D9HL_CreateDevice().
    @param указатель на функцию освобождения ресурсов, связанных с устройством Direct3D9 (см. справку DX SDK ).
    @param указатель на функцию восстановления ресурсов, связанных с устройством Direct3D9.
    @return Код ошибки ( @see z3DD3D9HL_ErrCodes ).
*/
z3DD3D9HL_ErrCodes D3D9HL_BeginDeviceRender(LPDIRECT3DDEVICE9 device,
                                            D3DPRESENT_PARAMETERS* presentParams,
                                            Z3D_D3D9HL_ReleaseDeviceResourcesFunc releaseFunc,
                                            Z3D_D3D9HL_ResetDeviceResourcesFunc resetFunc);

/** Останов рендера на устройстве Direct3D9.

    Производится автоматическое переклюяение заднего и переднего буферов.
    Если с рендером связано только одно окно, параметр hDestWindow можно не задавать.
    @param device указатель на устройство.
    @param hDestWindow окно, клиентская область которого будет целевым объектом рендера.
    @return Код ошибки ( @see z3DD3D9HL_ErrCodes ).
*/
z3DD3D9HL_ErrCodes D3D9HL_EndDeviceRender(LPDIRECT3DDEVICE9 device, HWND hDestWindow = 0);

//-----------------------------------------------------------------------------



} // end of z3D
#endif // Z3DD3D9HL_H


