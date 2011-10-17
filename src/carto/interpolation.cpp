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

void NearestNeighbourInterpolation(void *pInputData, int nInputXSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY , GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = dInputXSize / nOutXSize;
	double dYRatio = dInputYSize / nOutYSize;
    double dfR, dfG, dfB, dfA;
    int nIndex;
    wxGISColorTable mColorTable = pRasterRenderer->GetColorTable();

    for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
    {
        int nOrigPixY = (int)(dYRatio * double(nDestPixY) + dDeltaY);
        int scan_line = nOrigPixY * nInputXSize;
        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            int nOrigPixX = (int)(dXRatio * double(nDestPixX) + dDeltaX);
            int src_pixel_index = scan_line + nOrigPixX;

            src_pixel_index *= nBandCount;

            switch(pRasterRenderer->GetDataType())
            {
            case enumGISRenderTypeIndexed:
                {
                    nIndex = (int)SRCVAL(pInputData, eSrcType, src_pixel_index);
                    wxColor Color = mColorTable[nIndex];
                    dfR = Color.Red();
                    dfG = Color.Green();
                    dfB = Color.Blue();
                    dfA = Color.Alpha();
                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
            case enumGISRenderTypeRGBA:
			    switch(nBandCount)
			    {
			    case 1:
				    {
                        dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					    pRasterRenderer->FillPixel(pOutputData, &dfR, NULL, NULL, NULL);
				    }
				    break;
			    case 3:			
				    {
					    dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					    dfG = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1);
					    dfB = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2);
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, NULL);
				    }
				    break;
			    case 4:
				    {
					    dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					    dfG = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1);
					    dfB = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2);
					    dfA = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 3);
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
				    }
				    break;
			    default:
				    break;	
			    }
                break;
            case enumGISRenderTypePackedRGBA:
                {
                    wxColor Color(SRCVAL(pInputData, eSrcType, src_pixel_index));
                    dfR = Color.Red();
                    dfG = Color.Green();
                    dfB = Color.Blue();
                    dfA = Color.Alpha();
                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
            };

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
	}
}


void BilinearInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY , GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = dInputXSize / nOutXSize;
	double dYRatio = dInputYSize / nOutYSize;

	int srcpixymax = nInputYSize - 1;
	int srcpixxmax = nInputXSize - 1;

    double srcpixy, srcpixy2, dy, dy1;
    double srcpixx, srcpixx2, dx, dx1;
    int srcpixy1, srcpixx1;
	double r1, g1, b1, a1;
	double r2, g2, b2, a2;
    int x_offset1, x_offset2, y_offset1, y_offset2;
    int src_pixel_index00, src_pixel_index01, src_pixel_index10, src_pixel_index11;
    double dfR, dfG, dfB, dfA;
    long nIndex1, nIndex2, nIndex3, nIndex4;
    wxGISColorTable mColorTable = pRasterRenderer->GetColorTable();

     for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
     {
        srcpixy = dYRatio * double(nDestPixY) + dDeltaY;
        srcpixy1 = (int)(srcpixy);
        srcpixy2 = ( srcpixy1 == srcpixymax ) ? srcpixy1 : srcpixy1 + 1.0;
        dy = srcpixy - srcpixy1;
        dy1 = 1.0 - dy;

        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            // X-axis of pixel to interpolate from
            srcpixx = dXRatio * double(nDestPixX) + dDeltaX;
			srcpixx1 = (int)(srcpixx);
			srcpixx2 = ( srcpixx1 == srcpixxmax ) ? srcpixx1 : srcpixx1 + 1.0;
			dx = srcpixx - srcpixx1;
			dx1 = 1.0 - dx;

			x_offset1 = srcpixx1 < 0 ? 0 : srcpixx1 > srcpixxmax ? srcpixxmax : srcpixx1;
			x_offset2 = srcpixx2 < 0 ? 0 : srcpixx2 > srcpixxmax ? srcpixxmax : srcpixx2;
			y_offset1 = srcpixy1 < 0 ? 0 : srcpixy1 > srcpixymax ? srcpixymax : srcpixy1;
			y_offset2 = srcpixy2 < 0 ? 0 : srcpixy2 > srcpixymax ? srcpixymax : srcpixy2;

			src_pixel_index00 = (y_offset1 * nInputXSize + x_offset1) * nBandCount;
			src_pixel_index01 = (y_offset1 * nInputXSize + x_offset2) * nBandCount;
			src_pixel_index10 = (y_offset2 * nInputXSize + x_offset1) * nBandCount;
			src_pixel_index11 = (y_offset2 * nInputXSize + x_offset2) * nBandCount;

            switch(pRasterRenderer->GetDataType())
            {
            case enumGISRenderTypeIndexed:
                {
                    nIndex1 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index00);
                    nIndex2 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index01);
                    if(nIndex1 == nIndex2)
                    {
                        wxColor Color00 = mColorTable[nIndex1];
                        //first line
                        r1 = Color00.Red() * dx1 + Color00.Red() * dx;
                        g1 = Color00.Green() * dx1 + Color00.Green() * dx;
                        b1 = Color00.Blue() * dx1 + Color00.Blue() * dx;
                        a1 = Color00.Alpha() * dx1 + Color00.Alpha() * dx;
                    }
                    else
                    {
                        wxColor Color00 = mColorTable[nIndex1];
                        wxColor Color01 = mColorTable[nIndex2];
                        //first line
                        r1 = Color00.Red() * dx1 + Color01.Red() * dx;
                        g1 = Color00.Green() * dx1 + Color01.Green() * dx;
                        b1 = Color00.Blue() * dx1 + Color01.Blue() * dx;
                        a1 = Color00.Alpha() * dx1 + Color01.Alpha() * dx;
                    }

                    nIndex3 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index10);
                    nIndex4 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index11);
                    if(nIndex1 == nIndex3 && nIndex2 == nIndex4)
                    {
                        r2 = r1;
                        g2 = g1;
                        b2 = b1;
                        a2 = a1;
                    }
                    else if(nIndex3 == nIndex4)
                    {
                        wxColor Color10 = mColorTable[nIndex3];
			            //second line
                        r2 = Color10.Red() * dx1 + Color10.Red() * dx;
                        g2 = Color10.Green() * dx1 + Color10.Green() * dx;
                        b2 = Color10.Blue() * dx1 + Color10.Blue() * dx;
                        a2 = Color10.Alpha() * dx1 + Color10.Alpha() * dx;
                    }
                    else
                    {
                        wxColor Color10 = mColorTable[nIndex3];
                        wxColor Color11 = mColorTable[nIndex4];
			            //second line
                        r2 = Color10.Red() * dx1 + Color11.Red() * dx;
                        g2 = Color10.Green() * dx1 + Color11.Green() * dx;
                        b2 = Color10.Blue() * dx1 + Color11.Blue() * dx;
                        a2 = Color10.Alpha() * dx1 + Color11.Alpha() * dx;
                    }

                    if(IsDoubleEquil(r1, r2))
                        dfR = r1;
                    else
                        dfR = r1 * dy1 + r2 * dy;
                    if(IsDoubleEquil(g1, g2))
                        dfG = g1;
                    else
                        dfG = g1 * dy1 + g2 * dy;
                    if(IsDoubleEquil(b1, b2))
                        dfB = b1;
                    else
                        dfB = b1 * dy1 + b2 * dy;
                    if(IsDoubleEquil(a1, a2))
                        dfA = a1;
                    else
                        dfA = a1 * dy1 + a2 * dy;

                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
            case enumGISRenderTypeRGBA:
			    switch(nBandCount)
			    {
			    case 1:
				    {
                        //first line
                        r1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01) * dx;

                        //second line
                        r2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11) * dx;


                        if(IsDoubleEquil(r1, r2))
                            dfR = r1;
                        else
                            dfR = r1 * dy1 + r2 * dy;

					    pRasterRenderer->FillPixel(pOutputData, &dfR, NULL, NULL, NULL);
				    }
				    break;
			    case 3:			
				    {
                        //first line
                        r1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01) * dx;
                        g1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 1) * dx;
                        b1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 2) * dx;

                        //second line
                        r2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11) * dx;
                        g2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 1) * dx;
                        b2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 2) * dx;

 
                        if(IsDoubleEquil(r1, r2))
                            dfR = r1;
                        else
                            dfR = r1 * dy1 + r2 * dy;
                        if(IsDoubleEquil(g1, g2))
                            dfG = g1;
                        else
                            dfG = g1 * dy1 + g2 * dy;
                        if(IsDoubleEquil(b1, b2))
                            dfB = b1;
                        else
                            dfB = b1 * dy1 + b2 * dy;
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, NULL);
				    }
				    break;
			    case 4:
				    {
                        //first line
                        r1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01) * dx;
                        g1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 1) * dx;
                        b1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 2) * dx;
                        a1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 3) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 3) * dx;

                        //second line
                        r2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11) * dx;
                        g2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 1) * dx;
                        b2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 2) * dx;
                        a2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 3) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 3) * dx;


                        if(IsDoubleEquil(r1, r2))
                            dfR = r1;
                        else
                            dfR = r1 * dy1 + r2 * dy;
                        if(IsDoubleEquil(g1, g2))
                            dfG = g1;
                        else
                            dfG = g1 * dy1 + g2 * dy;
                        if(IsDoubleEquil(b1, b2))
                            dfB = b1;
                        else
                            dfB = b1 * dy1 + b2 * dy;
                        if(IsDoubleEquil(a1, a2))
                            dfA = a1;
                        else
                            dfA = a1 * dy1 + a2 * dy;
                    
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
				    }
				    break;
			    default:
				    break;	
			    }
                break;
             case enumGISRenderTypePackedRGBA:
                {
                    wxColor Color00(SRCVAL(pInputData, eSrcType, src_pixel_index00));
                    wxColor Color01(SRCVAL(pInputData, eSrcType, src_pixel_index01));
                    wxColor Color10(SRCVAL(pInputData, eSrcType, src_pixel_index10));
                    wxColor Color11(SRCVAL(pInputData, eSrcType, src_pixel_index11));

                    //first line
                    r1 = Color00.Red() * dx1 + Color01.Red() * dx;
                    g1 = Color00.Green() * dx1 + Color01.Green() * dx;
                    b1 = Color00.Blue() * dx1 + Color01.Blue() * dx;
                    a1 = Color00.Alpha() * dx1 + Color01.Alpha() * dx;
			        //second line
                    r2 = Color10.Red() * dx1 + Color11.Red() * dx;
                    g2 = Color10.Green() * dx1 + Color11.Green() * dx;
                    b2 = Color10.Blue() * dx1 + Color11.Blue() * dx;
                    a2 = Color10.Alpha() * dx1 + Color11.Alpha() * dx;

                    dfR = r1 * dy1 + r2 * dy;
                    dfG = g1 * dy1 + g2 * dy;
                    dfB = b1 * dy1 + b2 * dy;
                    dfA = a1 * dy1 + a2 * dy;

                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
           };

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
	}
}

void BicubicInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY , GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, IRasterRenderer *pRasterRenderer, ITrackCancel *pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = dInputXSize / nOutXSize;
	double dYRatio = dInputYSize / nOutYSize;

	int srcpixymax = nInputYSize - 1;
	int srcpixxmax = nInputXSize - 1;

    double srcpixy, dy;
    double srcpixx, dx;
    wxGISColorTable mColorTable = pRasterRenderer->GetColorTable();

    double sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;

     for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
     {
        srcpixy = dYRatio * double(nDestPixY) + dDeltaY;
        dy = srcpixy - (int)srcpixy;

        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            // X-axis of pixel to interpolate from
            srcpixx = dXRatio * double(nDestPixX) + dDeltaX;
			dx = srcpixx - (int)srcpixx;

            sum_r = sum_g = sum_b = sum_a = 0;

            // Here we actually determine the RGBA values for the destination pixel
            for ( int k = -1; k <= 2; k++ )
            {
                // Y offset
                int y_offset = srcpixy + k < 0.0 ? 0 : srcpixy + k >= nInputYSize ? nInputYSize - 1 : (int)(srcpixy + k);

                // Loop across the X axis
                for ( int i = -1; i <= 2; ++i )
                {
                    // X offset
                    int x_offset = srcpixx + i < 0.0 ? 0 : srcpixx + i >= nInputXSize ? nInputXSize - 1 : (int)(srcpixx + i);

                    // Calculate the exact position where the source data
                    // should be pulled from based on the x_offset and y_offset
                    int src_pixel_index = (y_offset * nInputXSize + x_offset) * nBandCount;

                    // Calculate the weight for the specified pixel according
                    // to the bicubic b-spline kernel we're using for
                    // interpolation
                    double pixel_weight = BiCubicKernel(i - dx) * BiCubicKernel(k - dy);

                    // Create a sum of all velues for each color channel
                    // adjusted for the pixel's calculated weight

                    switch(pRasterRenderer->GetDataType())
                    {
                    case enumGISRenderTypeIndexed:
                        {
                            wxColor Color = mColorTable[SRCVAL(pInputData, eSrcType, src_pixel_index)];
                            sum_r += Color.Red() * pixel_weight;
                            sum_g += Color.Green() * pixel_weight;
                            sum_b += Color.Blue() * pixel_weight;
                            sum_a += Color.Alpha() * pixel_weight;
                        }
                        break;
                    case enumGISRenderTypeRGBA:
			            switch(nBandCount)
			            {
			            case 1:
                            sum_r += (double)SRCVAL(pInputData, eSrcType, src_pixel_index) * pixel_weight;
				            break;
			            case 3:			
                            sum_r += (double)SRCVAL(pInputData, eSrcType, src_pixel_index) * pixel_weight;
                            sum_g += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1) * pixel_weight;
                            sum_b += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2) * pixel_weight;
				            break;
			            case 4:
                            sum_r += (double)SRCVAL(pInputData, eSrcType, src_pixel_index) * pixel_weight;
                            sum_g += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1) * pixel_weight;
                            sum_b += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2) * pixel_weight;
                            sum_a += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 3) * pixel_weight;
				            break;
			            default:
				            break;	
			            }
                        break;
                     case enumGISRenderTypePackedRGBA:
                        {
                            wxColor Color(SRCVAL(pInputData, eSrcType, src_pixel_index));
                            sum_r += Color.Red() * pixel_weight;
                            sum_g += Color.Green() * pixel_weight;
                            sum_b += Color.Blue() * pixel_weight;
                            sum_a += Color.Alpha() * pixel_weight;
                       }
                        break;
                    };
                }
            }

            switch(pRasterRenderer->GetDataType())
            {
            case enumGISRenderTypeIndexed:
                pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, &sum_a);
                break;
            case enumGISRenderTypeRGBA:
			    switch(nBandCount)
			    {
			    case 1:
                    pRasterRenderer->FillPixel(pOutputData, &sum_r, NULL, NULL, NULL);
				    break;
			    case 3:			
                    pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, NULL);
				    break;
			    case 4:
                    pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, &sum_a);
				    break;
			    default:
				    break;	
			    }
                break;
            case enumGISRenderTypePackedRGBA:
                pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, &sum_a);
                break;
            };

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
	}
}