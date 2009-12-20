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


//--------------------------------------------------------------
//class wxGxFile
//--------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFile :
	public IGxObject,
	public IGxObjectUI,
    public IGxFile
{
public:
	wxGxFile(wxString Path, wxString Name);
	virtual ~wxGxFile(void);
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
	wxGxPrjFile(wxString Path, wxString Name, wxGISEnumPrjFileType Type);
	virtual ~wxGxPrjFile(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Coordinate System"));};
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxPrjFile.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxPrjFile.NewMenu"));};
private:
    wxGISEnumPrjFileType m_Type;
};

//--------------------------------------------------------------
//class wxGxTextFile
//--------------------------------------------------------------
