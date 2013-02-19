/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISCarto main header.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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

#include "wxgis/display/gisdisplay.h"
#include "wxgis/datasource/quadtree.h"
#include "wxgis/datasource/rasterdataset.h"

/** \enum wxGISEnumRendererType rasterrenderer.h
    \brief A renderer color interpretation type
*/
enum wxGISEnumRendererType
{
	enumGISRenderTypeNone = 0,
	enumGISRenderTypeVector,
	enumGISRenderTypeRaster
};

WX_DECLARE_HASH_MAP( int, wxColor, wxIntegerHash, wxIntegerEqual, wxGISColorTable );


/** \class IFeatureRenderer carto.h
    \brief The base class for map vector layer renderer
*//*
class IFeatureRenderer : public IRenderer
{
public:
	virtual ~IFeatureRenderer(void){};
	virtual void Draw(wxGISQuadTreeCursorSPtr pCursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL) = 0;
};
DEFINE_SHARED_PTR(IFeatureRenderer);


/** \class IRasterRenderer carto.h
    \brief The base class for map raster layer renderer
*//*
class IRasterRenderer : public IRenderer
{
public:
	virtual ~IRasterRenderer(void){};
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster) = 0;
	virtual int *GetBandsCombination(int *pnBandCount) = 0;
	virtual void Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL) = 0;
	//transform pixel from raster value to display unsigned char and store it in pOutputData (ARGB32)
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA) = 0;
    virtual wxGISEnumRendererType GetDataType(void) = 0;
    virtual wxGISColorTable GetColorTable(void) = 0;
};
DEFINE_SHARED_PTR(IRasterRenderer);

*/