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
#include "wxgis/carto/interpolation.h"

void NearestNeighbourInterpolation(void *pInputData, int nInputXSize, int nInputYSize, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = (double)nInputXSize / nOutXSize;
	double dYRatio = (double)nInputYSize / nOutYSize;

    for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
    {
        int nOrigPixY = (int)(dYRatio * nDestPixY);
        int scan_line = nOrigPixY * nInputXSize;
        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            int nOrigPixX = (int)(dXRatio * nDestPixX);
            int src_pixel_index = scan_line + nOrigPixX;

            src_pixel_index *= nBandCount;

			switch(nBandCount)
			{
			case 1:
				{
					void* pPixColor = GetValuePointer(pInputData, src_pixel_index, eSrcType);
					pRasterRenderer->FillPixel(pOutputData, pPixColor, NULL, NULL, NULL);
				}
				break;
			case 3:			
				{
					void* pPixColorR = GetValuePointer(pInputData, src_pixel_index, eSrcType);
					void* pPixColorG = GetValuePointer(pInputData, src_pixel_index + 1, eSrcType);
					void* pPixColorB = GetValuePointer(pInputData, src_pixel_index + 2, eSrcType);
					pRasterRenderer->FillPixel(pOutputData, pPixColorR, pPixColorG, pPixColorB, NULL);
				}
				break;
			case 4:
				{
					void* pPixColorR = GetValuePointer(pInputData, src_pixel_index, eSrcType);
					void* pPixColorG = GetValuePointer(pInputData, src_pixel_index + 1, eSrcType);
					void* pPixColorB = GetValuePointer(pInputData, src_pixel_index + 2, eSrcType);
					void* pPixColorA = GetValuePointer(pInputData, src_pixel_index + 3, eSrcType);
					pRasterRenderer->FillPixel(pOutputData, pPixColorR, pPixColorG, pPixColorB, pPixColorA);
				}
				break;
			default:
				break;	
			}

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
	}
}