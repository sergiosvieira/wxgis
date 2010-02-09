/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  FeatureDataset class.
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

#include "wxgis/datasource.h"
#include "cpl_quad_tree.h"

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

class WXDLLIMPEXP_GIS_CRT wxGISFeatureDataset :
	public wxGISDataset
{
public:
	wxGISFeatureDataset(wxString sPath, wxFontEncoding Encoding = wxFONTENCODING_DEFAULT);
	virtual ~wxGISFeatureDataset(void);
//wxGISDataset
	virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
//wxGISFeatureDataset
	virtual bool Open(int iLayer = 0);
	virtual OGRSpatialReference* GetSpatialReference(void);
	virtual OGREnvelope* GetEnvelope(void);
	virtual void SetSpatialFilter(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY);
	virtual OGRFeature* GetAt(long nIndex);
	virtual OGRFeature* operator [](long nIndex);
	virtual wxString GetAsString(long row, int col);
	virtual wxGISFeatureSet* GetFeatureSet(IQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
	virtual size_t GetSize(void){return m_poLayer->GetFeatureCount(false);};
	virtual OGRLayer* GetLayer(int iLayer = 0);
//
protected:
	OGRDataSource *m_poDS;
	OGREnvelope* m_psExtent;
	OGRLayer* m_poLayer;
	bool m_bIsOpened;
    bool m_bOLCStringsAsUTF8;
    wxFontEncoding m_Encoding;
    //
    bool m_bIsFeaturesLoaded;
    std::map<long nFID, OGRFeature*> m_FeaturesMap;
    CPLQuadTree* m_pQuadTree;
};