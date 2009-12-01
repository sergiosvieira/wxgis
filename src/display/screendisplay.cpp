/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISScreenDisplay class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include "wxgis/display/screendisplay.h"

//-----------------------------------
// wxRasterDrawThread
//-----------------------------------

wxRasterDrawThread::wxRasterDrawThread(const unsigned char* pOrigData, unsigned char* pDestData, int nOrigX, int nOrigY, double rOrigX, double rOrigY, int nDestX, int nDestY, double rDeltaX, double rDeltaY, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel, int nYbeg, int nYend) : wxThread(wxTHREAD_JOINABLE)
{
    m_pTrackCancel = pTrackCancel;
    m_pOrigData = pOrigData;
    m_pDestData = pDestData;
    m_nOrigX = nOrigX;
    m_nOrigY = nOrigY;
    m_rOrigX = rOrigX;
    m_rOrigY = rOrigY;
    m_nDestX = nDestX;
    m_nDestY = nDestY;
    m_rDeltaX = rDeltaX;
    m_rDeltaY = rDeltaY;
    m_Quality = Quality;
    m_nYbeg = nYbeg;
    m_nYend = nYend;
}

void *wxRasterDrawThread::Entry()
{
    double rWRatio, rHRatio;
    rWRatio = m_rOrigX / m_nDestX;
    rHRatio = m_rOrigY / m_nDestY;
    switch(m_Quality)
    {
    case enumGISQualityBilinear:
        OnBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
        break;
    case enumGISQualityHalfBilinear:
        OnHalfBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
        break;
    case enumGISQualityHalfQuadBilinear:
        OnHalfQuadBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
        break;
    case enumGISQualityBicubic:
        OnBicubicInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
        break;
    case enumGISQualityNearest:
    default:
        OnNearestNeighbourInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
        break;
    }

	return NULL;
}
void wxRasterDrawThread::OnExit()
{
}

void wxRasterDrawThread::OnNearestNeighbourInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel)
{
    for(int nDestPixY = nYbeg; nDestPixY < nYend; nDestPixY++)
    {
        int nOrigPixY = (int)(rHRatio * double(nDestPixY) + rDeltaY);
        int scan_line = nOrigPixY * nOrigWidth;
        for(int nDestPixX = 0; nDestPixX < nDestWidth; nDestPixX++)
        {
            int nOrigPixX = (int)(rWRatio * double(nDestPixX) + rDeltaX);
            int src_pixel_index = scan_line + nOrigPixX;
            src_pixel_index *= 3;
            pDestData[0] = pOrigData[src_pixel_index + 0];
            pDestData[1] = pOrigData[src_pixel_index + 1];
            pDestData[2] = pOrigData[src_pixel_index + 2];
            pDestData += 3;  
            if(pTrackCancel && !pTrackCancel->Continue())
                return;
        }
    }
}

void wxRasterDrawThread::OnBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel)
{
	int srcpixymax = nOrigHeight - 1;
	int srcpixxmax = nOrigWidth - 1;

    double srcpixy, srcpixy1, srcpixy2, dy, dy1;
    double srcpixx, srcpixx1, srcpixx2, dx, dx1;
	double r1, g1, b1;
	double r2, g2, b2;

    for(int nDestPixY = nYbeg; nDestPixY < nYend; nDestPixY++)
    {	
        // We need to calculate the source pixel to interpolate from - Y-axis
        srcpixy = double(nDestPixY) * rHRatio + rDeltaY;
        srcpixy1 = (int)(srcpixy);
        srcpixy2 = ( srcpixy1 == srcpixymax ) ? srcpixy1 : srcpixy1 + 1.0;
        dy = srcpixy - srcpixy1;
        dy1 = 1.0 - dy;

        for(int nDestPixX = 0; nDestPixX < nDestWidth; nDestPixX++)
        {
            // X-axis of pixel to interpolate from
            srcpixx = double(nDestPixX) * rWRatio + rDeltaX;
			srcpixx1 = (int)(srcpixx);
			srcpixx2 = ( srcpixx1 == srcpixxmax ) ? srcpixx1 : srcpixx1 + 1.0;
			dx = srcpixx - srcpixx1;
			dx1 = 1.0 - dx;

			int x_offset1 = srcpixx1 < 0.0 ? 0 : srcpixx1 > srcpixxmax ? srcpixxmax : (int)(srcpixx1);
			int x_offset2 = srcpixx2 < 0.0 ? 0 : srcpixx2 > srcpixxmax ? srcpixxmax : (int)(srcpixx2);
			int y_offset1 = srcpixy1 < 0.0 ? 0 : srcpixy1 > srcpixymax ? srcpixymax : (int)(srcpixy1);
			int y_offset2 = srcpixy2 < 0.0 ? 0 : srcpixy2 > srcpixymax ? srcpixymax : (int)(srcpixy2);

			int src_pixel_index00 = y_offset1 * nOrigWidth + x_offset1;
			int src_pixel_index01 = y_offset1 * nOrigWidth + x_offset2;
			int src_pixel_index10 = y_offset2 * nOrigWidth + x_offset1;
			int src_pixel_index11 = y_offset2 * nOrigWidth + x_offset2;

			//first line
			r1 = pOrigData[src_pixel_index00 * 3 + 0] * dx1 + pOrigData[src_pixel_index01 * 3 + 0] * dx;
			g1 = pOrigData[src_pixel_index00 * 3 + 1] * dx1 + pOrigData[src_pixel_index01 * 3 + 1] * dx;
			b1 = pOrigData[src_pixel_index00 * 3 + 2] * dx1 + pOrigData[src_pixel_index01 * 3 + 2] * dx;

			//second line
			r2 = pOrigData[src_pixel_index10 * 3 + 0] * dx1 + pOrigData[src_pixel_index11 * 3 + 0] * dx;
			g2 = pOrigData[src_pixel_index10 * 3 + 1] * dx1 + pOrigData[src_pixel_index11 * 3 + 1] * dx;
			b2 = pOrigData[src_pixel_index10 * 3 + 2] * dx1 + pOrigData[src_pixel_index11 * 3 + 2] * dx;

			//result lines
			//result = p1  * 0.2 + p2 * 0.8

            pDestData[0] = r1 * dy1 + r2 * dy;
            pDestData[1] = g1 * dy1 + g2 * dy;
            pDestData[2] = b1 * dy1 + b2 * dy;
            pDestData += 3;
            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
	}
}

void wxRasterDrawThread::OnHalfBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel)
{
	int srcpixymax = nOrigHeight - 1;
	int srcpixxmax = nOrigWidth - 1;

    double srcpixy, srcpixy1, srcpixy2, dy, dy1;
    double srcpixx, srcpixx1, srcpixx2, dx, dx1;
	double r1, g1, b1;
	double r2, g2, b2;
    long W = nDestWidth * 3;

    for(int nDestPixY = nYbeg; nDestPixY < nYend - 1; nDestPixY += 2)
    {	
        // We need to calculate the source pixel to interpolate from - Y-axis
        srcpixy = double(nDestPixY) * rHRatio + rDeltaY;
        srcpixy1 = (int)(srcpixy);
        srcpixy2 = ( srcpixy1 == srcpixymax ) ? srcpixy1 : srcpixy1 + 1.0;
        dy = srcpixy - srcpixy1;
        dy1 = 1.0 - dy;

        for(int nDestPixX = 0; nDestPixX < nDestWidth; nDestPixX++)
        {
            // X-axis of pixel to interpolate from
            srcpixx = double(nDestPixX) * rWRatio + rDeltaX;
			srcpixx1 = (int)(srcpixx);
			srcpixx2 = ( srcpixx1 == srcpixxmax ) ? srcpixx1 : srcpixx1 + 1.0;
			dx = srcpixx - srcpixx1;
			dx1 = 1.0 - dx;

			int x_offset1 = srcpixx1 < 0.0 ? 0 : srcpixx1 > srcpixxmax ? srcpixxmax : (int)(srcpixx1);
			int x_offset2 = srcpixx2 < 0.0 ? 0 : srcpixx2 > srcpixxmax ? srcpixxmax : (int)(srcpixx2);
			int y_offset1 = srcpixy1 < 0.0 ? 0 : srcpixy1 > srcpixymax ? srcpixymax : (int)(srcpixy1);
			int y_offset2 = srcpixy2 < 0.0 ? 0 : srcpixy2 > srcpixymax ? srcpixymax : (int)(srcpixy2);

			int src_pixel_index00 = y_offset1 * nOrigWidth + x_offset1;
			int src_pixel_index01 = y_offset1 * nOrigWidth + x_offset2;
			int src_pixel_index10 = y_offset2 * nOrigWidth + x_offset1;
			int src_pixel_index11 = y_offset2 * nOrigWidth + x_offset2;

			//first line
			r1 = pOrigData[src_pixel_index00 * 3 + 0] * dx1 + pOrigData[src_pixel_index01 * 3 + 0] * dx;
			g1 = pOrigData[src_pixel_index00 * 3 + 1] * dx1 + pOrigData[src_pixel_index01 * 3 + 1] * dx;
			b1 = pOrigData[src_pixel_index00 * 3 + 2] * dx1 + pOrigData[src_pixel_index01 * 3 + 2] * dx;

			//second line
			r2 = pOrigData[src_pixel_index10 * 3 + 0] * dx1 + pOrigData[src_pixel_index11 * 3 + 0] * dx;
			g2 = pOrigData[src_pixel_index10 * 3 + 1] * dx1 + pOrigData[src_pixel_index11 * 3 + 1] * dx;
			b2 = pOrigData[src_pixel_index10 * 3 + 2] * dx1 + pOrigData[src_pixel_index11 * 3 + 2] * dx;

			//result lines
			//result = p1  * 0.2 + p2 * 0.8

            pDestData[0] = r1 * dy1 + r2 * dy;
            pDestData[1] = g1 * dy1 + g2 * dy;
            pDestData[2] = b1 * dy1 + b2 * dy;
             
            if(nDestPixY < nYend - 1)
            {
                pDestData[0 + W] = pDestData[0];
                pDestData[1 + W] = pDestData[1];
                pDestData[2 + W] = pDestData[2];
            }

            pDestData += 3;
            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
        pDestData += W;
	}
}

void wxRasterDrawThread::OnHalfQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel)
{
	int srcpixymax = nOrigHeight - 1;
	int srcpixxmax = nOrigWidth - 1;

    double srcpixy, srcpixy1, srcpixy2, dy, dy1;
    double srcpixx, srcpixx1, srcpixx2, dx, dx1;
	double r1, g1, b1;
	double r2, g2, b2;
    long W = nDestWidth * 3;

	int ostX = nDestWidth % 2;
	//int ostY = nYend % 2;

    for(int nDestPixY = nYbeg; nDestPixY < nYend - 1; nDestPixY += 2)
    {	
        // We need to calculate the source pixel to interpolate from - Y-axis
        srcpixy = double(nDestPixY) * rHRatio + rDeltaY;
        srcpixy1 = (int)(srcpixy);
        srcpixy2 = ( srcpixy1 == srcpixymax ) ? srcpixy1 : srcpixy1 + 1.0;
        dy = srcpixy - srcpixy1;
        dy1 = 1.0 - dy;

        for(int nDestPixX = 0; nDestPixX < nDestWidth - 1; nDestPixX += 2)
        {
            // X-axis of pixel to interpolate from
            srcpixx = double(nDestPixX) * rWRatio + rDeltaX;
			srcpixx1 = (int)(srcpixx);
			srcpixx2 = ( srcpixx1 == srcpixxmax ) ? srcpixx1 : srcpixx1 + 1.0;
			dx = srcpixx - srcpixx1;
			dx1 = 1.0 - dx;

			int x_offset1 = srcpixx1 < 0.0 ? 0 : srcpixx1 > srcpixxmax ? srcpixxmax : (int)(srcpixx1);
			int x_offset2 = srcpixx2 < 0.0 ? 0 : srcpixx2 > srcpixxmax ? srcpixxmax : (int)(srcpixx2);
			int y_offset1 = srcpixy1 < 0.0 ? 0 : srcpixy1 > srcpixymax ? srcpixymax : (int)(srcpixy1);
			int y_offset2 = srcpixy2 < 0.0 ? 0 : srcpixy2 > srcpixymax ? srcpixymax : (int)(srcpixy2);

			int src_pixel_index00 = y_offset1 * nOrigWidth + x_offset1;
			int src_pixel_index01 = y_offset1 * nOrigWidth + x_offset2;
			int src_pixel_index10 = y_offset2 * nOrigWidth + x_offset1;
			int src_pixel_index11 = y_offset2 * nOrigWidth + x_offset2;

			//first line
			r1 = pOrigData[src_pixel_index00 * 3 + 0] * dx1 + pOrigData[src_pixel_index01 * 3 + 0] * dx;
			g1 = pOrigData[src_pixel_index00 * 3 + 1] * dx1 + pOrigData[src_pixel_index01 * 3 + 1] * dx;
			b1 = pOrigData[src_pixel_index00 * 3 + 2] * dx1 + pOrigData[src_pixel_index01 * 3 + 2] * dx;

			//second line
			r2 = pOrigData[src_pixel_index10 * 3 + 0] * dx1 + pOrigData[src_pixel_index11 * 3 + 0] * dx;
			g2 = pOrigData[src_pixel_index10 * 3 + 1] * dx1 + pOrigData[src_pixel_index11 * 3 + 1] * dx;
			b2 = pOrigData[src_pixel_index10 * 3 + 2] * dx1 + pOrigData[src_pixel_index11 * 3 + 2] * dx;

			//result lines
			//result = p1  * 0.2 + p2 * 0.8

            pDestData[0] = r1 * dy1 + r2 * dy;
            pDestData[1] = g1 * dy1 + g2 * dy;
            pDestData[2] = b1 * dy1 + b2 * dy;
            pDestData[3] = pDestData[0];
            pDestData[4] = pDestData[1];
            pDestData[5] = pDestData[2];
             
            if(nDestPixY < nYend - 1)
            {
                pDestData[0 + W] = pDestData[0];
                pDestData[1 + W] = pDestData[1];
                pDestData[2 + W] = pDestData[2];
                pDestData[3 + W] = pDestData[0];
                pDestData[4 + W] = pDestData[1];
                pDestData[5 + W] = pDestData[2];
            }

            pDestData += 6;
            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
		pDestData += ostX * 3;
        pDestData += W;
	}
}

void wxRasterDrawThread::OnFourQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel)
{
	int srcpixymax = nOrigHeight - 1;
	int srcpixxmax = nOrigWidth - 1;

    double srcpixy, srcpixy1, srcpixy2, dy, dy1;
    double srcpixx, srcpixx1, srcpixx2, dx, dx1;
	double r1, g1, b1;
	double r2, g2, b2;
    long W = nDestWidth * 3;

	int ostX = nDestWidth % 2;
	//int ostY = nYend % 2;


    for(int nDestPixY = nYbeg; nDestPixY < nYend - 1; nDestPixY += 3)
    {	
        // We need to calculate the source pixel to interpolate from - Y-axis
        srcpixy = double(nDestPixY) * rHRatio + rDeltaY;
        srcpixy1 = (int)(srcpixy);
        srcpixy2 = ( srcpixy1 == srcpixymax ) ? srcpixy1 : srcpixy1 + 1.0;
        dy = srcpixy - srcpixy1;
        dy1 = 1.0 - dy;

        for(int nDestPixX = 0; nDestPixX < nDestWidth - 1; nDestPixX += 2)
        {
            // X-axis of pixel to interpolate from
            srcpixx = double(nDestPixX) * rWRatio + rDeltaX;
			srcpixx1 = (int)(srcpixx);
			srcpixx2 = ( srcpixx1 == srcpixxmax ) ? srcpixx1 : srcpixx1 + 1.0;
			dx = srcpixx - srcpixx1;
			dx1 = 1.0 - dx;

			int x_offset1 = srcpixx1 < 0.0 ? 0 : srcpixx1 > srcpixxmax ? srcpixxmax : (int)(srcpixx1);
			int x_offset2 = srcpixx2 < 0.0 ? 0 : srcpixx2 > srcpixxmax ? srcpixxmax : (int)(srcpixx2);
			int y_offset1 = srcpixy1 < 0.0 ? 0 : srcpixy1 > srcpixymax ? srcpixymax : (int)(srcpixy1);
			int y_offset2 = srcpixy2 < 0.0 ? 0 : srcpixy2 > srcpixymax ? srcpixymax : (int)(srcpixy2);

			int src_pixel_index00 = y_offset1 * nOrigWidth + x_offset1;
			int src_pixel_index01 = y_offset1 * nOrigWidth + x_offset2;
			int src_pixel_index10 = y_offset2 * nOrigWidth + x_offset1;
			int src_pixel_index11 = y_offset2 * nOrigWidth + x_offset2;

			//first line
			r1 = pOrigData[src_pixel_index00 * 3 + 0] * dx1 + pOrigData[src_pixel_index01 * 3 + 0] * dx;
			g1 = pOrigData[src_pixel_index00 * 3 + 1] * dx1 + pOrigData[src_pixel_index01 * 3 + 1] * dx;
			b1 = pOrigData[src_pixel_index00 * 3 + 2] * dx1 + pOrigData[src_pixel_index01 * 3 + 2] * dx;

			//second line
			r2 = pOrigData[src_pixel_index10 * 3 + 0] * dx1 + pOrigData[src_pixel_index11 * 3 + 0] * dx;
			g2 = pOrigData[src_pixel_index10 * 3 + 1] * dx1 + pOrigData[src_pixel_index11 * 3 + 1] * dx;
			b2 = pOrigData[src_pixel_index10 * 3 + 2] * dx1 + pOrigData[src_pixel_index11 * 3 + 2] * dx;

			//result lines
			//result = p1  * 0.2 + p2 * 0.8

            pDestData[0] = r1 * dy1 + r2 * dy;
            pDestData[1] = g1 * dy1 + g2 * dy;
            pDestData[2] = b1 * dy1 + b2 * dy;
            pDestData[3] = pDestData[0];
            pDestData[4] = pDestData[1];
            pDestData[5] = pDestData[2];

            if(nDestPixY < nYend - 2)
            {
                pDestData[0 + W] = pDestData[0];
                pDestData[1 + W] = pDestData[1];
                pDestData[2 + W] = pDestData[2];
                pDestData[3 + W] = pDestData[0];
                pDestData[4 + W] = pDestData[1];
                pDestData[5 + W] = pDestData[2];

                pDestData[0 + W * 2] = pDestData[0];
                pDestData[1 + W * 2] = pDestData[1];
                pDestData[2 + W * 2] = pDestData[2];
                pDestData[3 + W * 2] = pDestData[0];
                pDestData[4 + W * 2] = pDestData[1];
                pDestData[5 + W * 2] = pDestData[2];
            }

            pDestData += 6;
            if(pTrackCancel && !pTrackCancel->Continue())
                return;
		}
		pDestData += ostX * 3;
        pDestData += W * 2;
	}
}

double wxRasterDrawThread::BiCubicKernel(double x)
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
}

void wxRasterDrawThread::OnBicubicInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel)
{
	int srcpixymax = nOrigHeight - 1;
	int srcpixxmax = nOrigWidth - 1;

    double srcpixy, dy;
    double srcpixx, dx;
	
    for(int nDestPixY = nYbeg; nDestPixY < nYend; nDestPixY++)
    {
        // We need to calculate the source pixel to interpolate from - Y-axis
        srcpixy = double(nDestPixY) * rHRatio + rDeltaY;
        dy = srcpixy - (int)srcpixy;

        for(int nDestPixX = 0; nDestPixX < nDestWidth; nDestPixX++)
        {
            // X-axis of pixel to interpolate from
            srcpixx = double(nDestPixX) * rWRatio + rDeltaX;
            dx = srcpixx - (int)srcpixx;

            // Sums for each color channel
            double sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;

            // Here we actually determine the RGBA values for the destination pixel
            for ( int k = -1; k <= 2; k++ )
            {
                // Y offset
                int y_offset = srcpixy + k < 0.0 ? 0 : srcpixy + k >= nOrigHeight ? nOrigHeight - 1 : (int)(srcpixy + k);

                // Loop across the X axis
                for ( int i = -1; i <= 2; i++ )
                {
                    // X offset
                    int x_offset = srcpixx + i < 0.0 ? 0 : srcpixx + i >= nOrigWidth ? nOrigWidth - 1 : (int)(srcpixx + i);

                    // Calculate the exact position where the source data
                    // should be pulled from based on the x_offset and y_offset
                    int src_pixel_index = y_offset * nOrigWidth + x_offset;

                    // Calculate the weight for the specified pixel according
                    // to the bicubic b-spline kernel we're using for
                    // interpolation
                    double pixel_weight = BiCubicKernel(i - dx) * BiCubicKernel(k - dy);

                    // Create a sum of all velues for each color channel
                    // adjusted for the pixel's calculated weight
                    sum_r += pOrigData[src_pixel_index * 3 + 0] * pixel_weight;
                    sum_g += pOrigData[src_pixel_index * 3 + 1] * pixel_weight;
                    sum_b += pOrigData[src_pixel_index * 3 + 2] * pixel_weight;
                }
            }

            // Put the data into the destination image.  The summed values are
            // of double data type and are rounded here for accuracy
            pDestData[0] = (unsigned char)(sum_r + 0.5);
            pDestData[1] = (unsigned char)(sum_g + 0.5);
            pDestData[2] = (unsigned char)(sum_b + 0.5);
            pDestData += 3;
            if(pTrackCancel && !pTrackCancel->Continue())
                return;
        }
    }
}

//-----------------------------------------------------
// wxGISDisplay
//-----------------------------------------------------
wxGISDisplay::wxGISDisplay(void) : m_bIsDerty(true)
{
	m_pDisplayTransformation = new wxGISDisplayTransformation();
}

wxGISDisplay::~wxGISDisplay(void)
{
	wxDELETE(m_pDisplayTransformation);
}

bool wxGISDisplay::IsDerty(void)
{
	return m_bIsDerty;
}

void wxGISDisplay::SetDerty(bool bIsDerty)
{
	m_bIsDerty = bIsDerty;
}

IDisplayTransformation* wxGISDisplay::GetDisplayTransformation(void)
{
	return static_cast<IDisplayTransformation*>(m_pDisplayTransformation);
}

//-----------------------------------------------------
// wxGISScreenDisplay
//-----------------------------------------------------


wxGISScreenDisplay::wxGISScreenDisplay(void) : m_bIsDerty(true), m_nLastCacheID(1)
{
	m_pDisplayTransformation = new wxGISDisplayTransformation();

	int max_x = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int max_y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	wxBitmap BackGroundBuffer(wxBitmap(max_x, max_y, 32));
	m_dc.SelectObject(BackGroundBuffer);
	m_dc.SetBackground(wxBrush(wxColour(240, 255, 255), wxSOLID));
	m_dc.Clear();
	m_dc.SelectObject(wxNullBitmap);
	CACHEDATA bk_data = {false, BackGroundBuffer};
	m_caches.push_back(bk_data);
	//geography cache
	wxBitmap Buffer(wxBitmap(max_x, max_y, 32));
	CACHEDATA geo_data = {true, Buffer};
	m_caches.push_back(geo_data);
}

wxGISScreenDisplay::~wxGISScreenDisplay(void)
{
	wxDELETE(m_pDisplayTransformation);
}

void wxGISScreenDisplay::OnDraw(wxDC &dc, wxCoord x, wxCoord y, bool bClearBackground)
{	
	wxRect DevRect = m_pDisplayTransformation->GetDeviceFrame();
	if(bClearBackground)
		dc.DrawBitmap(m_caches[0].bmp.GetSubBitmap(DevRect), 0, 0);
	dc.DrawBitmap(m_caches[m_caches.size() - 1].bmp.GetSubBitmap(DevRect), x, y);	//, true
    m_InvalidRectArray.clear();
}

void wxGISScreenDisplay::OnPanDraw(wxDC &dc, wxCoord x, wxCoord y)
{
	wxRect DispRect = m_pDisplayTransformation->GetDeviceFrame();

	if(abs(x) > DispRect.GetHeight() &&  abs(y) > DispRect.GetWidth())
	{
		dc.DrawBitmap(m_caches[0].bmp.GetSubBitmap(DispRect), 0, 0);	//, true
        AddInvalidRect(DispRect);
		return;
	}	
	if(x < 0)
	{
		wxRect Rect(0, 0, -x, DispRect.GetHeight());
		dc.DrawBitmap(m_caches[0].bmp.GetSubBitmap(Rect), 0, 0);
        AddInvalidRect(Rect);
	}
	if(y < 0)
	{
		wxRect Rect(0, 0, DispRect.GetWidth(), -y);
		dc.DrawBitmap(m_caches[0].bmp.GetSubBitmap(Rect), 0, 0);
        AddInvalidRect(Rect);
	}
	if(x > 0)
	{
		wxRect Rect(0, 0, x, DispRect.GetHeight());
		dc.DrawBitmap(m_caches[0].bmp.GetSubBitmap(Rect), DispRect.GetWidth() - x, 0);
        Rect.Offset(DispRect.GetWidth() - x, 0);
        AddInvalidRect(Rect);
	}
	if(y > 0)
	{
		wxRect Rect(0, 0, DispRect.GetWidth(), y);
		dc.DrawBitmap(m_caches[0].bmp.GetSubBitmap(Rect), 0, DispRect.GetHeight() - y);
        Rect.Offset(0, DispRect.GetHeight() - y);
        AddInvalidRect(Rect);
	}

	dc.DrawBitmap(m_caches[m_caches.size() - 1].bmp.GetSubBitmap(DispRect), -x, -y);
}

void wxGISScreenDisplay::OnPanStop(wxDC &dc)
{
    StartDrawing(m_caches.size() - 1);
	wxRect rect = m_pDisplayTransformation->GetDeviceFrame();
    bool bTest = m_dc.Blit(0, 0, rect.GetWidth(), rect.GetHeight(), &dc, 0, 0);
}

void wxGISScreenDisplay::OnStretchDraw(wxDC &dc, wxCoord nDestWidth, wxCoord nDestHeight, wxCoord x, wxCoord y, bool bClearBackground, wxGISEnumDrawQuality quality )
{	
    wxCriticalSectionLocker locker(m_CritSect);

    wxRect Rect = m_pDisplayTransformation->GetDeviceFrame();
	wxImage Img = m_caches[m_caches.size() - 1].bmp.GetSubBitmap(Rect).ConvertToImage();
	Img = Img.Scale(nDestWidth, nDestHeight, quality);
    //Img = Scale(Img, nDestWidth, nDestHeight, quality);
	if(bClearBackground)
		dc.DrawBitmap(m_caches[0].bmp, 0, 0);
	dc.DrawBitmap(Img, x, y);
}

void wxGISScreenDisplay::OnStretchDraw2(wxDC &dc, wxRect Rect, bool bClearBackground, wxGISEnumDrawQuality quality )
{	
    wxCriticalSectionLocker locker(m_CritSect);

    wxRect DevRect = m_pDisplayTransformation->GetDeviceFrame();
	wxImage Img = m_caches[m_caches.size() - 1].bmp.GetSubBitmap(Rect).ConvertToImage();
	//Img = Img.Scale(DevRect.width, DevRect.height, quality);
	Img = Scale(Img, DevRect.width, DevRect.height, quality);
	if(bClearBackground)
		dc.DrawBitmap(m_caches[0].bmp, 0, 0);
	dc.DrawBitmap(Img, DevRect.x, DevRect.y);
}

void wxGISScreenDisplay::DrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fill_style)
{
	m_dc.DrawPolygon(n, points, xoffset, yoffset, fill_style);
}

void wxGISScreenDisplay::DrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fill_style)
{
	m_dc.DrawPolyPolygon(n, count, points, xoffset, yoffset, fill_style);
}

void wxGISScreenDisplay::SetDerty(bool bIsDerty)
{
	m_bIsDerty = bIsDerty;

	for(size_t i = 0; i < m_caches.size(); i++)
		m_caches[i].IsDerty = bIsDerty;
    if(!bIsDerty)
        m_InvalidRectArray.clear();
}

bool wxGISScreenDisplay::IsDerty(void)
{
	return m_bIsDerty;
}

IDisplayTransformation* wxGISScreenDisplay::GetDisplayTransformation(void)
{
	return static_cast<IDisplayTransformation*>(m_pDisplayTransformation);
}

void wxGISScreenDisplay::ClearCaches(void)
{
	m_caches.erase(m_caches.begin() + 2, m_caches.end());
	MergeCaches(0,1);
    m_InvalidRectArray.clear();
	m_bIsDerty = true;
}

void wxGISScreenDisplay::DrawPoint(wxCoord x, wxCoord y)
{
	m_dc.DrawPoint(x, y);
}

void wxGISScreenDisplay::DrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
	m_dc.DrawLines(n, points, xoffset, yoffset);
}

void wxGISScreenDisplay::DrawCircle(wxCoord x, wxCoord y, wxCoord radius)
{
	m_dc.DrawCircle(x, y, radius);
}

void wxGISScreenDisplay::SetBrush(wxBrush& Brush)
{
	m_dc.SetBrush(Brush);
}

void wxGISScreenDisplay::SetPen(wxPen& Pen)
{
	m_dc.SetPen(Pen);
}

void wxGISScreenDisplay::SetFont(wxFont& Font)
{
	m_dc.SetFont(Font);
}

bool wxGISScreenDisplay::IsCacheDerty(size_t cache_id)
{
	return m_caches[cache_id].IsDerty;
}

void wxGISScreenDisplay::SetCacheDerty(size_t cache_id, bool bIsDerty)
{
	m_caches[cache_id].IsDerty = bIsDerty;
}

size_t wxGISScreenDisplay::AddCache(void)
{
	int max_x = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int max_y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	wxBitmap Buffer(wxBitmap(max_x, max_y));
	CACHEDATA data={true, Buffer};
	m_caches.push_back(data);
	m_nLastCacheID = m_caches.size() - 1;
	return m_nLastCacheID;
}

void wxGISScreenDisplay::MergeCaches(size_t SrcCacheID, size_t DstCacheID)
{
    wxCriticalSectionLocker locker(m_CritSect);

	m_dc.SelectObject(m_caches[DstCacheID].bmp);
    if(m_InvalidRectArray.size() == 0)
        m_dc.DrawBitmap(m_caches[SrcCacheID].bmp, 0, 0);
    else
        for(size_t i = 0; i < m_InvalidRectArray.size(); i++)
            m_dc.DrawBitmap(m_caches[SrcCacheID].bmp.GetSubBitmap(m_InvalidRectArray[i]), m_InvalidRectArray[i].x, m_InvalidRectArray[i].y);
	m_dc.SelectObject(wxNullBitmap);
}

void wxGISScreenDisplay::StartDrawing(size_t CacheID)
{
    wxCriticalSectionLocker locker(m_CritSect);

	m_dc.SelectObject(m_caches[CacheID].bmp);
}

void wxGISScreenDisplay::FinishDrawing(void)
{
    wxCriticalSectionLocker locker(m_CritSect);

    m_dc.SelectObject(wxNullBitmap);
}

size_t wxGISScreenDisplay::GetLastCacheID(void)
{
	return m_nLastCacheID;
}

void wxGISScreenDisplay::DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	m_dc.DrawRectangle(x, y, width, height);
}

void wxGISScreenDisplay::DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent)
{
	m_dc.DrawBitmap(bitmap, x, y, transparent);
}

// Transparency
//void CWKSP_Map_DC::Draw_DC(CWKSP_Map_DC &dc_Source, double Transparency)
//{
//	if( 1 )
//	{
//		BYTE		*src_rgb, *dst_rgb;
//		int			i, n;
//		double		d;
//		wxImage		src_img, dst_img;
//		wxBitmap	bmp;
//		wxMemoryDC	mdc;
//
//		//-------------------------------------------------
//		n	= 3 * m_rDC.GetHeight() * m_rDC.GetWidth();
//		d	= 1.0 - Transparency;
//
//		bmp.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
//
//		mdc.SelectObject(bmp);
//		mdc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc, 0, 0);
//		mdc.SelectObject(wxNullBitmap);
//		dst_img	= bmp.ConvertToImage();
//		dst_rgb	= dst_img.GetData();
//
//		mdc.SelectObject(bmp);
//		mdc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc_Source.dc, 0, 0);
//		mdc.SelectObject(wxNullBitmap);
//		src_img	= bmp.ConvertToImage();
//		src_rgb	= src_img.GetData();
//
//		//-------------------------------------------------
//		for(i=0; i<n; i+=3)
//		{
//			if( src_rgb[i + 0] < 255 || src_rgb[i + 1] < 255 || src_rgb[i + 2] < 255 )
//			{
//				dst_rgb[i + 0]	= (int)(d * src_rgb[i + 0] + Transparency * dst_rgb[i + 0]);
//				dst_rgb[i + 1]	= (int)(d * src_rgb[i + 1] + Transparency * dst_rgb[i + 1]);
//				dst_rgb[i + 2]	= (int)(d * src_rgb[i + 2] + Transparency * dst_rgb[i + 2]);
//			}
//		}
//
//		//-------------------------------------------------
//		dc.DrawBitmap(wxBitmap(dst_img), 0, 0, true);
//	}
//}

wxImage wxGISScreenDisplay::Scale(wxImage SourceImage, int nDestWidth, int nDestHeight, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel)
{
    const unsigned char* pData = SourceImage.GetData();
    int nSourceWidth = SourceImage.GetWidth();
    int nSourceHeight = SourceImage.GetHeight();
    return Scale(pData, nSourceWidth, nSourceHeight, nSourceWidth, nSourceHeight, nDestWidth, nDestHeight, 0, 0, Quality, pTrackCancel);
}

wxImage wxGISScreenDisplay::Scale(const unsigned char* pData, int nOrigX, int nOrigY, double rOrigX, double rOrigY, int nDestX, int nDestY, double rDeltaX, double rDeltaY, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel)
{
    wxImage ResultImage(nDestX, nDestY, false);
    unsigned char* pDestData = ResultImage.GetData();
    
    double rWRatio, rHRatio;
    rWRatio = rOrigX / nDestX;
    rHRatio = rOrigY / nDestY;

    switch(Quality)
    {
    case enumGISQualityBilinear:
        wxRasterDrawThread::OnBilinearInterpolation(pData, pDestData, 0, nDestY, nOrigX, nOrigY, nDestX,  rWRatio, rHRatio, 0, 0, NULL);
        break;
    case enumGISQualityHalfBilinear:
        wxRasterDrawThread::OnHalfBilinearInterpolation(pData, pDestData, 0, nDestY, nOrigX, nOrigY, nDestX,  rWRatio, rHRatio, 0, 0, NULL);
        break;
    case enumGISQualityHalfQuadBilinear:
        wxRasterDrawThread::OnHalfQuadBilinearInterpolation(pData, pDestData, 0, nDestY, nOrigX, nOrigY, nDestX,  rWRatio, rHRatio, 0, 0, NULL);
        break;
    case enumGISQualityFourQuadBilinear:
        wxRasterDrawThread::OnFourQuadBilinearInterpolation(pData, pDestData, 0, nDestY, nOrigX, nOrigY, nDestX,  rWRatio, rHRatio, 0, 0, NULL);
        break;
    case enumGISQualityBicubic:
        wxRasterDrawThread::OnBicubicInterpolation(pData, pDestData, 0, nDestY, nOrigX, nOrigY, nDestX,  rWRatio, rHRatio, 0, 0, NULL);
        break;
    case enumGISQualityNearest:
    default:
        wxRasterDrawThread::OnNearestNeighbourInterpolation(pData, pDestData, 0, nDestY, nOrigX, nDestX, rWRatio, rHRatio, 0, 0, NULL);
        break;
    }

    ////multithreaded
    //int CPUCount(1); //check cpu count
    //std::vector<wxRasterDrawThread*> threadarray;
    //int nPartSize = nDestY / CPUCount;
    //int nBegY(0), nEndY;
    //for(int i = 0; i < CPUCount; i++)
    //{        
    //    if(i == CPUCount - 1)
    //        nEndY = nDestY;
    //    else
    //        nEndY = nPartSize * (i + 1);

    //    unsigned char* pDestInputData = pDestData  + (nDestX * nBegY * 3);
    //    wxRasterDrawThread *thread = new wxRasterDrawThread(pData, pDestInputData, nOrigX, nOrigY, rOrigX, rOrigY, nDestX, nDestY, rDeltaX, rDeltaY, Quality, pTrackCancel, nBegY, nEndY);
    //    thread->Create();
    //    thread->Run();
    //    threadarray.push_back(thread);
    //    nBegY = nEndY/* + 1*/;
    //}


    //for(size_t i = 0; i < threadarray.size(); i++)
    //{
    //    wgDELETE(threadarray[i], Wait());
    //}
    return ResultImage;
}

RECTARARRAY* wxGISScreenDisplay::GetInvalidRect(void)
{
    return &m_InvalidRectArray;
}

void wxGISScreenDisplay::AddInvalidRect(wxRect Rect)
{
    for(size_t i = 0; i < m_InvalidRectArray.size(); i++)
    {
        if(m_InvalidRectArray[i].Contains(Rect))
            return;
        if(Rect.Contains(m_InvalidRectArray[i]))
        {
            m_InvalidRectArray[i] = Rect;
            return;
        }
    }
    m_InvalidRectArray.push_back(Rect);
}