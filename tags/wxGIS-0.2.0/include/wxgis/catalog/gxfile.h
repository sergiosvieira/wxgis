/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
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
#include "wxgis/carto/carto.h"

//--------------------------------------------------------------
//class wxGxFile
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFile :
	public IGxObject,
	public IGxObjectUI,
    public IGxFile, 
    public IGxObjectEdit
{
public:
	wxGxFile(wxString Path, wxString Name);
	virtual ~wxGxFile(void);
    virtual wxString GetPath(void){return m_sPath;};
	//IGxObject
	virtual wxString GetName(void){return m_sName;};
protected:
	wxString m_sName, m_sPath;
};

//--------------------------------------------------------------
//class wxGxPrjFile
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxPrjFile :
    public wxGxFile
{
public:
	wxGxPrjFile(wxString Path, wxString Name, wxGISEnumPrjFileType Type, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxPrjFile(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Coordinate System"));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxPrjFile.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxPrjFile.NewMenu"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual void EditProperties(wxWindow *parent);
	//wxGxPrjFile
	virtual OGRSpatialReference* GetSpatialReference(void);
protected:
    wxGISEnumPrjFileType m_Type;
	OGRSpatialReference m_OGRSpatialReference;
    wxIcon m_oLargeIcon;
    wxIcon m_oSmallIcon;
};

//--------------------------------------------------------------
//class wxGxTextFile
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxTextFile :
    public wxGxFile
{
public:
	wxGxTextFile(wxString Path, wxString Name, wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxTextFile(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Text file"));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxTextFile.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxTextFile.NewMenu"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_oLargeIcon;
    wxIcon m_oSmallIcon;
};