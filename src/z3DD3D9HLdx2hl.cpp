/* This file is a part of Zavod3D engine project.
It's licensed unser the MIT license (see "License.txt" for details).*/

/* Файл
Реализация кода преобразования значений из Direct3D9  и обратно.
*/


#include "z3DD3D9HL.h"
#include "z3DDebugSystem.h"

namespace z3D_priv
{
/* Получить число бит на пиксель для формата DirectX.
@return Если заданный формат не известен функции, возвращается Z3D_D3D9HL_NOINDEX.
*/
uint32_t D3DFormat2Bpp( D3DFORMAT d3dfmt ){
	switch ( d3dfmt )
	{
    case D3DFMT_R8G8B8:				return 24;
    case D3DFMT_A8R8G8B8:			return 32;
    case D3DFMT_X8R8G8B8:			return 32;
    case D3DFMT_R5G6B5:				return 16;
    case D3DFMT_X1R5G5B5:			return 16;
    case D3DFMT_A1R5G5B5:			return 16;
    case D3DFMT_A4R4G4B4:			return 32;
    case D3DFMT_R3G3B2:				return 8;
    case D3DFMT_A8:					return 8;
    case D3DFMT_A8R3G3B2:			return 16;
    case D3DFMT_X4R4G4B4:			return 16;
    case D3DFMT_P8:					return 8;

    case D3DFMT_D32:				return 32;
    case D3DFMT_D16:				return 16;
    case D3DFMT_D24S8:				return 32;

	case D3DFMT_A8P8:				return 16;
    case D3DFMT_L8:                 return 8;
    case D3DFMT_A8L8:               return 16;
    case D3DFMT_A4L4:               return 8;
    case D3DFMT_V8U8:               return 16;
    case D3DFMT_L6V5U5:             return 16;
    case D3DFMT_X8L8V8U8:           return 32;
    case D3DFMT_Q8W8V8U8:           return 32;
	case D3DFMT_V16U16:             return 32;


    case D3DFMT_D16_LOCKABLE:       return 16;
    case D3DFMT_D15S1:				return 16;
    case D3DFMT_D24X8:              return 32;
    case D3DFMT_D24X4S4:            return 32;

    case D3DFMT_UNKNOWN:
        Z3D_ASSERT(d3dfmt != D3DFMT_UNKNOWN, "", true);
        break;
	default:
        return Z3D_D3D9HL_NOINDEX;
	}
	return Z3D_D3D9HL_NOINDEX;
}

/* Проверить, содержит ли формат альфа-канал.
*/
bool D3DFormatHasAlpha( D3DFORMAT d3dfmt ){
	switch ( d3dfmt )
	{
    case D3DFMT_A8R8G8B8:			return true;
    case D3DFMT_X8R8G8B8:			return false;
    case D3DFMT_R5G6B5:				return false;
    case D3DFMT_X1R5G5B5:			return false;
    case D3DFMT_A1R5G5B5:			return true;
    // FIXMT: это просто заглушка от варнингов. На самом деле тут могут быть переданы форматы с альфа каналом
    default:                        return false;
	}
	return false;
}

/* Проверить, содержит ли формат биты трафарета.
*/
bool D3DFormatHasStencil( D3DFORMAT d3dfmt ){
	switch ( d3dfmt )
	{
    case D3DFMT_D32:				return false;
    case D3DFMT_D16:				return false;
    case D3DFMT_D24S8:				return true;
    case D3DFMT_D15S1:				return true;
    case D3DFMT_D24X8:              return false;
    case D3DFMT_D24X4S4:            return true;
    // FIXMT: это просто заглушка от варнингов. На самом деле тут могут быть переданы форматы с трафаретом
    default:                        return false;
	}
	return false;
}
} // end of z3D_priv
