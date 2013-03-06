/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnection class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/postgisdataset.h"

//class wxGxRemoteDBSchema;

/** \class wxGxRemoteConnection gxremoteconn.h
    \brief A Remote Connection GxObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxRemoteConnection :
	public wxGxObjectContainer,
    public IGxObjectEdit,
    public IGxRemoteConnection
{
    DECLARE_CLASS(wxGxRemoteConnection)
    typedef struct _pgtabledata{
        bool bHasGeometry;
        CPLString sTableName;
        CPLString sTableSchema;
    }PGTABLEDATA;
public:
	wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxGxRemoteConnection(void);
	//wxGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Database Connection"));};
    //IGxRemoteConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(const wxString& NewName);
	virtual bool CanRename(void){return true;};
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanCopy(const CPLString &szDestPath){return true;};
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel);
	virtual bool CanMove(const CPLString &szDestPath){return CanCopy(szDestPath) & CanDelete();};
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
	virtual bool HasChildren(void);
protected:
	//wxGxRemoteConnection
	//virtual void LoadChildren(void);
	//virtual void EmptyChildren(void);
//    virtual wxGxRemoteDBSchema* GetNewRemoteDBSchema(const CPLString &szName, wxGISPostgresDataSourceSPtr pwxGISRemoteCon);
    //create wxGISDataset without openning it
    virtual wxGISDataset* const GetDatasetFast(void);
protected:
 //   wxGISPostgresDataSourceSPtr m_pwxGISRemoteConn;
    wxGISDataset* m_pwxGISDataset;
    bool m_bIsChildrenLoaded;
};

/** \class wxGxRemoteDBSchema gxremoteconn.h
    \brief A Remote Database schema GxObject.
*/
/*
class WXDLLIMPEXP_GIS_CLT wxGxRemoteDBSchema :	
	public wxGxObjectContainer,
    public IGxObjectEdit
{
    DECLARE_CLASS(wxGxRemoteDBSchema)
    friend class wxGxRemoteConnection;
public:
	wxGxRemoteDBSchema(const wxString &sName, wxGISPostgresDataSource* pwxGISRemoteConn);
	virtual ~wxGxRemoteDBSchema(void);
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return m_sName;};
    virtual CPLString GetInternalName(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Remote Database Schema"));};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void) const {return true;};
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