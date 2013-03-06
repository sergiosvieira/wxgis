/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnections class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxxmlconnstor.h"

#define CONNCONF wxT("conn.xml")

/** \class wxGxDiscConnections gxdiscconnections.h
    \brief The Disc Connections GxRootObject.
*/

class WXDLLIMPEXP_GIS_CLT wxGxDiscConnections :
	public wxGxXMLConnectionStorage,
    public IGxRootObjectProperties
{
   DECLARE_DYNAMIC_CLASS(wxGxDiscConnections)
public:
	wxGxDiscConnections(void);
	virtual ~wxGxDiscConnections(void);
	//wxGxObject
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual bool Destroy(void);
    virtual wxString GetBaseName(void) const {return GetName();};
    virtual wxString GetFullName(void) const {return wxEmptyString;};
	virtual wxString GetCategory(void) const {return wxString(_("Folder connections"));};
	virtual void Refresh(void);
	//wxGxObjectContainer
	//virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void) const {return true;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
	//wxGxDiscConnections
    //virtual wxGxObject *CreateConnection(long nKey, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	//virtual void LoadChildren(void);
	//virtual void EmptyChildren(void);
    virtual bool ConnectFolder(const wxString &sPath);
    virtual bool DisconnectFolder(int nXmlId);
 //   virtual void StoreConnections(void);

//    typedef struct _conn_data{
//        wxString sName;
//        CPLString sPath;
//    }CONN_DATA;
protected:
    //wxGxXMLConnectionStorage
    virtual bool IsObjectExist(wxGxObject* const pObj, const wxXmlNode* pNode);
    virtual void CreateConnectionsStorage(void);
    virtual wxGxObject* CreateChildGxObject(const wxXmlNode* pNode);
    virtual int GetStorageVersion(void) const {return 2;};
protected:
	bool m_bIsChildrenLoaded;
//    std::vector<CONN_DATA> m_aConnections;
    //wxString m_sUserConfig;
    wxString m_sUserConfigDir;
    wxFileSystemWatcher *m_pWatcher;
    wxCriticalSection m_oCritSect;
private:
    DECLARE_EVENT_TABLE()
};
