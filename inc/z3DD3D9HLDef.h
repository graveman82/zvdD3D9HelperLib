/* This file is a part of Zavod3D engine project.
It's licensed unser the MIT license (see "License.txt" for details).*/

#ifndef Z3DD3D9HLDEF_H
#define Z3DD3D9HLDEF_H

/** @file z3DD3D9HLDef.h*/

/* Файл
Определения типов библиотеки Zavod3D D3D9 Helper Lib.
*/

#include <d3d9.h>

#ifdef __GNUC__
#include <stdint.h>
#elif _MSC_VER
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;

typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif
/// Коды ошибок
enum z3DD3D9HL_ErrCodes{
    Z3D_D3D9HL_NONE,                ///< ошибок нет
    Z3D_D3D9HL_INVALIDCALL,         ///< недопустимый аргумент
    Z3D_D3D9HL_NOTFOUND,            ///< запрошенная возвожность не найдена или не поддерживается
    Z3D_D3D9HL_NOTAVAILABLE,        ///< запрошенная функция не поддерживается
    Z3D_D3D9HL_NO_PRELIMINARY_DONE, ///< какие-то предварительные операции или условия не выполнены
    Z3D_D3D9HL_DEVICE_LOST,         ///< устройство потеряно
    Z3D_D3D9HL_DEVICE_NOT_RESET
};

/// Неопределенное значение индекса или порядкового номера
#define Z3D_D3D9HL_NOINDEX 0xFFFFFFFF

/// Число бит на пиксель
enum z3DD3D9HL_Bpp{
    Z3D_D3D9HL_BPP16 = 16,          ///< 16 бит на пиксель
    Z3D_D3D9HL_BPP32 = 32           ///< 32 бит на пиксель
};

/// Видеорежим
struct z3DD3D9HL_VideoMode{
    D3DDISPLAYMODE d3ddm_;          ///< режим дисплея

    uint8_t bpp_;                   ///< число бит на пиксель в экранном буфере
    bool fAlphaInBB_;               ///< присутствие альфа-канала в формате пикселей заднего буфера
    bool fStencil_;                 ///< присутствие битов трафарета в формате пикселей буфера глубины
    D3DFORMAT depthStencilFmt_;     ///< формат пикселей буфера глубины и трафарета

    /// Получить разрешение по ширине
    uint32_t Width() const { return static_cast<uint32_t>(d3ddm_.Width); }
    /// Получить разрешение по высоте
    uint32_t Height() const { return static_cast<uint32_t>(d3ddm_.Height); }
    /// Получить частоту развертки
    uint32_t RefreshRate() const { return static_cast<uint32_t>(d3ddm_.RefreshRate); }

    /** @brief Возвращает true, если заданные ширина и высота совпадают с теми,
    которые есть у этого видеорежима. */
    bool MatchedTo(uint32_t width, uint32_t height) {
        if (d3ddm_.Width != static_cast<UINT>(width))
            return false;
        if (d3ddm_.Height != static_cast<UINT>(height))
            return false;
        return true;
    }
};

/** Прототип функции для освобождения ресурсов устройства.
В функция должен быть реализован пробег по всем ресурсам, созданныи при помощи D3DPOOL_DEFAULT,
и их освобождение через вызов метода Release(). Более подробную тнформацию можно получить из справки DX SDK.
*/
typedef bool (*Z3D_D3D9HL_ReleaseDeviceResourcesFunc)();

/// Прототип функции для перезагрузки ресурсов устройства
typedef bool (*Z3D_D3D9HL_ResetDeviceResourcesFunc)();

#endif // Z3DD3D9HLDEF_H


