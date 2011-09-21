/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRasterRGBARenderer classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/
class wxGISRasterRGBARenderer :
	public IRasterRenderer
{
public:
	wxGISRasterRGBARenderer(void);
	~wxGISRasterRGBARenderer(void);
//IRasterRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void PutRaster(wxGISRasterDatasetSPtr pRaster);
	virtual int *GetBandsCombination(void);
	virtual void Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
/**
    \brief Proceed two dim array multithreaded.
*/
protected:
	virtual bool OnPixelProceed(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, void *pTransformData );

//	virtual void Draw(wxGISDatasetSPtr pRasterDataset, wxGISEnumDrawPhase DrawPhase, IDisplay* pDisplay, ITrackCancel* pTrackCancel);
////
//	virtual OGREnvelope TransformEnvelope(const OGREnvelope* pEnvelope, OGRSpatialReference* pSrsSpatialReference, OGRSpatialReference* pDstSpatialReference);
//    virtual wxImage Scale(unsigned char* pData, int nOrigX, int nOrigY, double rOrigX, double rOrigY, int nDestX, int nDestY, double rDeltaX, double rDeltaY, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel);
protected:
	int m_nRedBand, m_nGreenBand, m_nBlueBand, m_nAlphaBand;
	wxColour m_oBkColorGet, m_oBkColorSet, m_oNoDataColor; 
	//stretch - none, custom, standard derivations, histogram equalize, min-max, histogram specification, percent clip
	//statistics - current display extent, each raster dataset, custom settings
	wxGISRasterDatasetSPtr m_pwxGISRasterDataset;
};
