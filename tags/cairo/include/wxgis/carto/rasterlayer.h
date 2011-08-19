/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
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
#include "wxgis/datasource/rasterdataset.h"

/** \class wxGISRasterLayer rasterlayer.h
    \brief The class represent raster datasource in map.
*/
class WXDLLIMPEXP_GIS_CRT wxGISRasterLayer :
	public wxGISLayer
{
public:
	wxGISRasterLayer(wxGISDatasetSPtr pwxGISDataset);
	virtual ~wxGISRasterLayer(void);
//wxGISLayer
	virtual OGRSpatialReferenceSPtr GetSpatialReference(void);
	virtual void SetSpatialReference(OGRSpatialReferenceSPtr pSpatialReference);
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
	virtual OGREnvelope GetEnvelope(void);
	virtual bool IsValid(void);
//wxGISRasterLayer
	virtual IRasterRendererSPtr GetRenderer(void){return m_pRasterRenderer;};
	virtual void SetRenderer(IRasterRendererSPtr pRasterRenderer){m_pRasterRenderer = pRasterRenderer;};
	virtual void GetSubRaster(OGREnvelope& Envelope, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
protected:
	wxGISRasterDatasetSPtr m_pwxGISRasterDataset;
    OGRSpatialReferenceSPtr m_pSpatialReference;

	OGREnvelope m_FullEnvelope;
	OGREnvelope m_PreviousEnvelope;

	IRasterRendererSPtr m_pRasterRenderer;
};

