/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
#include "wxgis/catalog/gxdiscconnections.h"

/** \class wxGxCatalog gxcatalog.h
    \brief The Gx Catalog class.
*/

class WXDLLIMPEXP_GIS_CLT wxGxCatalog :
	public IGxObjectContainer,
	public IGxCatalog,
	public IConnectionPointContainer
{
public:
	wxGxCatalog(void);
	virtual ~wxGxCatalog(void);
	//IGxObject
	virtual bool Attach(IGxObject* pParent, IGxCatalog* pCatalog){m_pParent = NULL;	m_pCatalog = NULL; return true;};
	virtual void Detach(void);
	virtual wxString GetName(void){return wxString(_("Catalog"));};
    virtual wxString GetBaseName(void){return GetName();};
	virtual wxString GetFullName(void){return wxEmptyString;}//wxString(_("Catalog"));};
    virtual CPLString GetInternalName(void){return CPLString();};
	virtual wxString GetCategory(void){return wxString(wxT("Root"));};
	virtual IGxObject* const GetParent(void){return NULL;};
	virtual void Refresh(void);
	//IGxObjectContainer
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){return true;};
	virtual GxObjectArray* const GetChildren(void){return &m_Children;};
    virtual IGxObject* SearchChild(wxString sPath);
	//IGxCatalog
	virtual wxString ConstructFullName(IGxObject* pObject);
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray);
	virtual void ObjectAdded(long nObjectID);
	virtual void ObjectChanged(long nObjectID);
	virtual void ObjectDeleted(long nObjectID);
	virtual void ObjectRefreshed(long nObjectID);
	virtual IGxObject* ConnectFolder(wxString sPath, bool bSelect = true);
	virtual void DisconnectFolder(CPLString sPath);
	virtual void RegisterObject(IGxObject* pObj);
	virtual void UnRegisterObject(long nID);
	virtual IGxObjectSPtr GetRegisterObject(long nID);
	//IConnectionPointContainer
	virtual long Advise(wxObject* pObject);
	//wxGxCatalog
	virtual void LoadChildren(wxXmlNode* const pNode);
	virtual void EmptyChildren(void);
	virtual void EmptyDisabledChildren(void);
	virtual void LoadObjectFactories(wxXmlNode* pNode);
	virtual void EmptyObjectFactories(void);
	virtual void Init(void);
    virtual void SerializePlugins(wxXmlNode* const pNode, bool bStore = false);
	virtual void EnableRootItem(IGxObject* pRootItem, bool bEnable);
protected:
	bool m_bIsChildrenLoaded;
	wxArrayString m_CatalogRootItemArray;
    wxGxDiscConnections* m_pGxDiscConnections;
	long m_nGlobalID;
	wxCriticalSection m_RegCritSect;
};

//WXDLL_ENTRY_FUNCTION();
