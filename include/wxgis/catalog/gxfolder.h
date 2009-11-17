/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolder class.
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
#include <wx/dir.h>

class WXDLLIMPEXP_GIS_CLT wxGxFolder :
	public IGxObjectUI,
	public IGxObjectEdit,
	public IGxObjectContainer,
	public wxDirTraverser
{
public:
	wxGxFolder(wxString Path, wxString Name, bool bShowHidden);
	virtual ~wxGxFolder(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void){return m_sName;};
	virtual wxString GetPath(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Folder"));};
	virtual void Refresh(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxFolder.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxFolder.NewMenu"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual void EditProperties(wxWindow *parent);
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
	//wxGxFolder
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
	//wxDirTraverser
	virtual wxDirTraverseResult OnFile(const wxString& filename);
	virtual wxDirTraverseResult OnDir(const wxString& dirname);
protected:
	wxString m_sName, m_sPath;
	wxArrayString m_FileNames;
	bool m_bShowHidden;
	bool m_bIsChildrenLoaded;
};
