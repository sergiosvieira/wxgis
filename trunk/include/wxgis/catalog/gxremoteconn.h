/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnection class.
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

//#include "ogrsf_frmts/pg/ogr_pg.h"
#include "gdal/ogr_pg.h"

//class wxGxRemoteDBSchema;

/** \class wxGxRemoteConnection gxremoteconn.h
    \brief A Remote Connection GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
	public IGxObjectContainer,
    public IGxObjectEdit,
	public IGxDataset
{
     typedef struct _pgtabledata{
        bool bHasGeometry;
        CPLString sTableName;
        CPLString sTableSchema;
    }PGTABLEDATA;
public:
	wxGxRemoteConnection(CPLString soPath, wxString Name);
	virtual ~wxGxRemoteConnection(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void);
    virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Remote Database Connection"));};
    virtual void Detach(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual bool Copy(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual bool CanCopy(CPLString szDestPath){return true;};
	virtual bool Move(CPLString szDestPath, ITrackCancel* pTrackCancel);
	virtual bool CanMove(CPLString szDestPath){return CanCopy(szDestPath) & CanDelete();};
	//IGxDataset
	virtual wxGISDatasetSPtr GetDataset(bool bCache = true, ITrackCancel* pTrackCancel = NULL);
	virtual wxGISEnumDatasetType GetType(void){return enumGISContainer;};
    virtual int GetSubType(void){return m_eType;};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
protected:
	//wxGxRemoteConnection
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(const CPLString &szName, wxGISPostgresDataSourceSPtr pwxGISRemoteCon);
protected:
    wxGISPostgresDataSourceSPtr m_pwxGISRemoteConn;
    wxGISDatasetSPtr m_pwxGISDataset;
	wxGISEnumContainerType m_eType;
    bool m_bIsChildrenLoaded;
	wxString m_sName;
    CPLString m_sPath;
};
*/
/** \class wxGxRemoteDBSchema gxremoteconn.h
    \brief A Remote Database schema GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchema :
	public IGxObjectContainer
{
    friend class wxGxRemoteConnection;
public:
	wxGxRemoteDBSchema(const wxString &sName, wxGISPostgresDataSourceSPtr pwxGISRemoteConn);
	virtual ~wxGxRemoteDBSchema(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return m_sName;};
    virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Remote Database Schema"));};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){return m_Children.size() > 0 ? true : false;};
protected:
    //wxGxRemoteDBSchema
    virtual void AddTable(CPLString &szName, CPLString &szSchema, bool bHasGeometry);
protected:
    wxGISPostgresDataSourceSPtr m_pwxGISRemoteConn;
	wxString m_sName;
    CPLString m_sPath;
};
*/
#endif //wxGIS_USE_POSTGRES