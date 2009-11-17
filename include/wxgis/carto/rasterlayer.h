/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterLayer header.
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
#pragma once

#include "wxgis/carto/carto.h"
#include "wxgis/carto/rasterdataset.h"

class WXDLLIMPEXP_GIS_CRT wxGISRasterLayer :
	public wxGISLayer
{
public:
	wxGISRasterLayer(wxGISDataset* pwxGISDataset);
	virtual ~wxGISRasterLayer(void);
//IwxGISLayer
	virtual void Draw(wxGISEnumDrawPhase DrawPhase, ICachedDisplay* pDisplay, ITrackCancel* pTrackCancel);
	virtual OGRSpatialReference* GetSpatialReference(void);
	virtual OGREnvelope* GetEnvelope(void);
	virtual bool IsValid(void);
//wxGISRasterLayer
	virtual IRasterRenderer* GetRenderer(void){return m_pRasterRenderer;};
	virtual void SetRenderer(IRasterRenderer* pRasterRenderer){m_pRasterRenderer = pRasterRenderer;};
protected:
	wxGISRasterDataset* m_pwxGISRasterDataset;
	IRasterRenderer* m_pRasterRenderer;
	OGREnvelope m_pPreviousDisplayEnv;
};