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

inline double BiCubicKernel(double x)
{
	if ( x > 2.0 )
		return 0.0;

	double a, b, c, d;
	double xm1 = x - 1.0;
	double xp1 = x + 1.0;
	double xp2 = x + 2.0;

	a = ( xp2 <= 0.0 ) ? 0.0 : xp2 * xp2 * xp2;
	b = ( xp1 <= 0.0 ) ? 0.0 : xp1 * xp1 * xp1;
	c = ( x   <= 0.0 ) ? 0.0 : x * x * x;
	d = ( xm1 <= 0.0 ) ? 0.0 : xm1 * xm1 * xm1;

	return ( 0.16666666666666666667 * ( a - ( 4.0 * b ) + ( 6.0 * c ) - ( 4.0 * d ) ) );
};

void WXDLLIMPEXP_GIS_CRT NearestNeighbourInterpolation(void *pInputData, int nInputXSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel = NULL);

void WXDLLIMPEXP_GIS_CRT BilinearInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel = NULL);

void WXDLLIMPEXP_GIS_CRT BicubicInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel = NULL);

// static void OnHalfBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
// static void OnHalfQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
// static void OnFourQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);


