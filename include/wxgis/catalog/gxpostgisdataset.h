/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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
#pragma once

#include "wxgis/defs.h"

#ifdef wxGIS_USE_POSTGRES
/*
#include "wxgis/catalog/catalog.h"
//#include "wxgis/datasource/postgisdataset.h"

/** \class wxGxPostGISTableDataset gxpostgisdataset.h
    \brief A PostGIS Table Dataset GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxPostGISTableDataset :
	public IGxObject,
	public IGxDataset
{
public:
	//wxGxPostGISTableDataset(CPLString soPath, wxGISDatasetSPtr pwxGISDataset);
	wxGxPostGISTableDataset(CPLString szName, CPLString szSchema, wxGISPostgresDataSourceSPtr pwxGISRemoteConn);
	virtual ~wxGxPostGISTableDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void);
	virtual wxString GetCategory(void){return wxString(_("PostGIS Table"));};
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(bool bCache = true, ITrackCancel* pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void){return enumGISTableDataset;};
    virtual int GetSubType(void){return enumTablePostgres;};
	virtual bool IsCached(void){return false;};
	virtual void Cache(ITrackCancel* pTrackCancel = NULL){};
protected:
	wxString m_sName;
    CPLString m_sPath;
    CPLString m_szFullyQualifiedName;
	wxGISDatasetSPtr m_pwxGISDataset;
    wxGISPostgresDataSourceSPtr m_pwxGISRemoteConn;
};

/** \class wxGxPostGISFeatureDataset gxpostgisdataset.h
    \brief A PostGIS Feature Dataset GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxPostGISFeatureDataset :
	public IGxObject,
	public IGxDataset
{
public:
	wxGxPostGISFeatureDataset(CPLString szName, CPLString szSchema, wxGISPostgresDataSourceSPtr pwxGISRemoteConn);
	virtual ~wxGxPostGISFeatureDataset(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void);
	virtual wxString GetCategory(void){return wxString(_("PostGIS Feature Dataset"));};
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(bool bCache = true, ITrackCancel* pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void){return enumGISFeatureDataset;};
    virtual int GetSubType(void){return emumVecPostGIS;};
	virtual bool IsCached(void){return false;};
	virtual void Cache(ITrackCancel* pTrackCancel = NULL){};
protected:
	wxString m_sName;
    CPLString m_sPath;
    CPLString m_szFullyQualifiedName;
	wxGISDatasetSPtr m_pwxGISDataset;
    wxGISPostgresDataSourceSPtr m_pwxGISRemoteConn;
};
*/
#endif //wxGIS_USE_POSTGRES