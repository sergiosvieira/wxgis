/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PostresDataSource class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/datasource/filter.h"

/** \class wxGISPostgresDataSource postgisdataset.h
    \brief The PostGIS DataSource class.
*/
class WXDLLIMPEXP_GIS_DS wxGISPostgresDataSource :
	public wxGISDataset
{
public:
	wxGISPostgresDataSource(wxString sName, wxString sPass, wxString sPort = wxT("5432"), wxString sAddres = wxT("localhost"), wxString sDBName = wxT("postgres"), bool bIsBinaryCursor = false);
	virtual ~wxGISPostgresDataSource(void);
	//wxGISDataset
    virtual size_t GetSubsetsCount(void);
    virtual wxGISDatasetSPtr GetSubset(size_t nIndex);
    virtual wxString GetName(void);
	virtual void Close(void);
	virtual bool IsCached(void){return true;};
	virtual void Cache(ITrackCancel* pTrackCancel = NULL){};
	//wxGISPostGISDataset
    virtual wxGISDatasetSPtr GetSubset(wxString sTablename);
	virtual OGRDataSource* GetDataSource(void);
	virtual bool Open(void);
	virtual wxGISDatasetSPtr ExecuteSQL(wxString sStatement, wxGISSpatialFilter* pSpatialFilter = NULL, wxString sDialect = wxEmptyString);
protected:
	virtual wxGISDatasetSPtr GetDatasetFromOGRLayer(CPLString &sPath, OGRLayer* poLayer, OGRDataSource *poDS);
protected:
	OGRDataSource *m_poDS;
    wxString m_sName, m_sPass, m_sPort, m_sAddres, m_sDBName;
	bool m_bIsBinaryCursor;
    wxCriticalSection m_CritSect;
};

DEFINE_SHARED_PTR(wxGISPostgresDataSource);

