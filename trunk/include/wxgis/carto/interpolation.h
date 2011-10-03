/******************************************************************************
 * Project:  wxGIS
 * Purpose:  raster interpolation operations.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#pragma once

#include "wxgis/carto/carto.h"


inline void* GetValuePointer(void* pData, int nIndex, GDALDataType eSrcType)
{
	unsigned char* pDataByte = (unsigned char*)pData;//uintptr_t
	switch(eSrcType)
	{
	case GDT_Byte:
		return (void*)(pDataByte + nIndex * sizeof(GByte));
	case GDT_Float32:
		return (void*)(pDataByte + nIndex * sizeof(float));
	case GDT_Float64:
		return (void*)(pDataByte + nIndex * sizeof(double));
	case GDT_Int32:
		return (void*)(pDataByte + nIndex * sizeof(GInt32));
	case GDT_UInt16:
		return (void*)(pDataByte + nIndex * sizeof(GUInt16));
	case GDT_Int16:
		return (void*)(pDataByte + nIndex * sizeof(GInt16));
	case GDT_UInt32:
		return (void*)(pDataByte + nIndex * sizeof(GUInt32));
	case GDT_CInt16:
		return (void*)(pDataByte + nIndex * sizeof(GInt16) * 2);
	case GDT_CInt32:
		return (void*)(pDataByte + nIndex * sizeof(GInt32) * 2);
	case GDT_CFloat32:
		return (void*)(pDataByte + nIndex * sizeof(float) * 2);
	case GDT_CFloat64:
		return (void*)(pDataByte + nIndex * sizeof(double) * 2);
	default:
		return 0;
	}
	return 0;
}

void WXDLLIMPEXP_GIS_CRT NearestNeighbourInterpolation(void *pInputData, int nInputXSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel = NULL);

