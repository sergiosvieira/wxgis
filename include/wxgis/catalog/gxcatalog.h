/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
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

#include "wxgis/catalog/catalog.h"
#include "../../art/mainframe.xpm"
#include "wxgis/catalog/gxselection.h"
// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------
#define DISCCONNCAT wxT("wxGxDiscConnection")

class WXDLLIMPEXP_GIS_CLT wxGxCatalog :
	public IGxObjectUI,
	public IGxObjectEdit,
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
	virtual wxString GetFullName(void){return wxString(_("Catalog"));};
	virtual wxString GetCategory(void){return wxString(wxT("Root"));};
	virtual IGxObject* GetParent(void){return NULL;};
	virtual void Refresh(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void){return wxNullIcon;};
	virtual wxIcon GetSmallImage(void){return wxIcon(mainframe_xpm);};
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxCatalog.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxCatalog.NewMenu"));};
	//IGxObjectEdit
	virtual bool Delete(void){return false;};
	virtual bool CanDelete(void){return false;};
	virtual bool Rename(wxString NewName){return false;};
	virtual bool CanRename(void){return false;};
	virtual void EditProperties(wxWindow *parent);
	//IGxObjectContainer
	virtual bool AddChild(IGxObject* pChild);
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){return true;};
	virtual GxObjectArray* GetChildren(void){return &m_Children;};
    virtual IGxObject* SearchChild(wxString sPath);
	//IGxCatalog
	virtual wxString ConstructFullName(IGxObject* pObject);
	virtual bool GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray);
	virtual void SetShowHidden(bool bShowHidden);
	virtual void SetShowExt(bool bShowExt);
	virtual void ObjectAdded(IGxObject* pObject);
	virtual void ObjectChanged(IGxObject* pObject);
	virtual void ObjectDeleted(IGxObject* pObject);
	virtual void ObjectRefreshed(IGxObject* pObject);
	virtual IGxObject* ConnectFolder(wxString sPath, bool bSelect = true);
	virtual void DisconnectFolder(wxString sPath);
	virtual void SetLocation(wxString sPath);
    virtual void Undo(int nPos = -1);
    virtual void Redo(int nPos = -1);
	//IConnectionPointContainer
	virtual long Advise(wxObject* pObject);
	//wxGxCatalog
	virtual void LoadChildren(wxXmlNode* pNode);
	virtual void EmptyChildren(void);
	virtual void LoadObjectFactories(wxXmlNode* pNode);
	virtual void EmptyObjectFactories(void);
	virtual void Init(void);
	virtual void SerializeDiscConnections(wxXmlNode* pNode, bool bStore = false);
    virtual void SerializePlugins(wxXmlNode* pNode, bool bStore = false);

private:
	bool m_bIsChildrenLoaded;
	std::vector<IGxObjectFactory*> m_ObjectFactoriesArray;
	std::map<wxString, IGxObject*> m_DiscConnections;
	wxArrayString m_CatalogRootItemArray;
};

//WXDLL_ENTRY_FUNCTION();
