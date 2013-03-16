/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxobjectfactory.h"

#include <wx/xml/xml.h>

/** \class wxGxCatalog gxcatalog.h
    \brief The GxCatalog class.
*/
class WXDLLIMPEXP_GIS_CLT wxGxCatalog : 
    public wxGxCatalogBase,
	public wxGISConnectionPointContainer
{
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGxCatalog);
public:
    wxGxCatalog(wxGxObject *oParent = NULL, const wxString &soName = _("Catalog"), const CPLString &soPath = "");
    virtual ~wxGxCatalog(void);
    virtual void ObjectAdded(long nObjectID);
	virtual void ObjectChanged(long nObjectID);
	virtual void ObjectDeleted(long nObjectID);
	virtual void ObjectRefreshed(long nObjectID);
    virtual bool GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds);
    virtual void EnableRootItem(size_t nItemId, bool bEnable);
    virtual bool Destroy(void);
    virtual wxGxObject* const GetRootItemByType(const wxClassInfo * info) const;
    virtual wxGxObjectFactory* const GetObjectFactoryByClassName(const wxString &sClassName);
    virtual wxGxObjectFactory* const GetObjectFactoryByName(const wxString &sFactoryName);

    typedef struct _root_item{
        wxString sClassName;
        wxString sName;
        bool bEnabled;
        wxXmlNode* pConfig;
    } ROOTITEM;
protected:
    //wxGxCatalogBase
	virtual void LoadObjectFactories(const wxXmlNode* pNode);
	virtual void LoadObjectFactories(void);
    virtual void LoadChildren(void);    
    virtual void LoadChildren(wxXmlNode* const pNode);
	virtual void EmptyObjectFactories(void);
    virtual void SerializePlugins(wxXmlNode* pNode, bool bStore = false);
protected:
	virtual wxString GetConfigName(void) const {return wxString(wxT("wxCatalog"));};
protected:
    wxArrayString m_CatalogRootItemArray;
    wxVector<wxGxObjectFactory*> m_ObjectFactoriesArray;
    wxVector<ROOTITEM> m_staRootitems;
};

#define wxGIS_GXCATALOG_EVENT(x)  {  wxGxCatalogBase* pGxCatalog = GetGxCatalog(); \
    if(pGxCatalog) pGxCatalog->x(GetId()); }
#define wxGIS_GXCATALOG_EVENT_ID(x, id)   { wxGxCatalogBase* pGxCatalog = GetGxCatalog(); \
    if(pGxCatalog) pGxCatalog->x(id); }

/*
class WXDLLIMPEXP_GIS_CLT wxGxCatalog :
	public IGxObjectContainer,
	public IGxCatalog,
	public wxGISConnectionPointContainer
{
public:
	wxGxCatalog(void);
	virtual ~wxGxCatalog(void);
	//IGxObject
	+virtual bool Attach(IGxObject* pParent, IGxCatalog* pCatalog);
	+virtual void Detach(void);
	+virtual wxString GetName(void){return wxString(_("Catalog"));};
    +virtual wxString GetBaseName(void){return GetName();};
	+virtual wxString GetFullName(void){return wxEmptyString;}//wxString(_("Catalog"));};
    +virtual CPLString GetInternalName(void){return CPLString();};
	+virtual wxString GetCategory(void){return wxString(wxT("Root"));};
	+virtual IGxObject* const GetParent(void){return NULL;};
	+virtual void Refresh(void);
	//IGxObjectContainer
	-virtual bool AreChildrenViewable(void){return true;};
	+virtual bool HasChildren(void){return true;};
	+virtual GxObjectArray* const GetChildren(void){return &m_Children;};
    virtual IGxObject* SearchChild(wxString sPath);
	//IGxCatalog
	+virtual wxString ConstructFullName(IGxObject* pObject);
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray);
	virtual void ObjectAdded(long nObjectID);
	virtual void ObjectChanged(long nObjectID);
	virtual void ObjectDeleted(long nObjectID);
	virtual void ObjectRefreshed(long nObjectID);
	-virtual IGxObject* ConnectFolder(wxString sPath, bool bSelect = true);
	-virtual void DisconnectFolder(CPLString sPath);
	+virtual void RegisterObject(IGxObject* pObj);
	+virtual void UnRegisterObject(long nID);
	+virtual IGxObjectSPtr GetRegisterObject(long nID);
	//wxGxCatalog
	virtual void EnableRootItem(IGxObject* pRootItem, bool bEnable);
	virtual void Init(void);
protected:
	virtual wxString GetConfigName(void){return wxString(wxT("wxCatalog"));};
	virtual void LoadChildren(wxXmlNode* const pNode);
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
	virtual void EmptyDisabledChildren(void);
protected:
	bool m_bIsChildrenLoaded;
	wxArrayString m_CatalogRootItemArray;
    wxGxDiscConnections* m_pGxDiscConnections;
	long m_nGlobalID;
	wxCriticalSection m_RegCritSect;
};
*/

