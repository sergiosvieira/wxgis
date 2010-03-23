/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolder class.
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
#include "wxgis/catalog/gxarchfolder.h"

class WXDLLIMPEXP_GIS_CLT wxGxSpatialReferencesFolder :
	public IGxObjectUI,
	public IGxObjectContainer,
    public IGxRootObjectProperties,
    public wxObject
{
   DECLARE_DYNAMIC_CLASS(wxGxSpatialReferencesFolder)
public:
	wxGxSpatialReferencesFolder(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxSpatialReferencesFolder(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void){return wxString(_("Coordinate Systems"));};
	virtual wxString GetPath(void){return m_sPath;};
	virtual wxString GetCategory(void){return wxString(_("Coordinate Systems Folder"));};
	virtual void Refresh(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxSpatialReferencesFolder.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxSpatialReferencesFolder.NewMenu"));};
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* pConfigNode);
	//wxGxSpatialReferencesFolder
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
protected:
	wxString m_sPath;
	wxArrayString m_FileNames;
	bool m_bIsChildrenLoaded;
    wxXmlNode* m_pConfigNode;
};

/////////////////////////////////////////////////////////////////////////
// wxGxPrjFolder
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLT wxGxPrjFolder :
	public wxGxArchiveFolder
{
public:
	wxGxPrjFolder(wxString Path, wxString Name, bool bShowHidden);
	virtual ~wxGxPrjFolder(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Coordinate Systems Folder"));};
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	//wxGxFolder
	virtual void LoadChildren(void);
};
