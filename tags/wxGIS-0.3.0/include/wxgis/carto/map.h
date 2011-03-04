/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISMap class.
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

class WXDLLIMPEXP_GIS_CRT wxGISMap
{
public:
	wxGISMap(void);
	virtual ~wxGISMap(void);
    virtual bool Create(void);
	virtual void SetName(wxString sName){m_sMapName = sName;};
	virtual wxString GetName(void){return m_sMapName;};
	virtual void AddLayer(wxGISLayer* pLayer);
	virtual void ClearLayers(void);
	virtual size_t GetLayerCount(void){return m_Layers.size();};
	virtual wxString GetDescription(void){return m_sDescription;};
	virtual void SetDescription(wxString sDescription){m_sDescription = sDescription;};
	virtual OGREnvelope GetFullExtent(void);
	virtual void SetSpatialReference(OGRSpatialReference* pSpatialReference);
	virtual OGRSpatialReference* GetSpatialReference(void);
protected:
	wxString m_sMapName, m_sDescription;
	std::vector<wxGISLayer*> m_Layers;
	OGRSpatialReference* m_pSpatialReference;
};
