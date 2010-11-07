/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
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
#include "wxgis/catalog/gxfolder.h"

/////////////////////////////////////////////////////////////////////////
// wxGxArchive
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLT wxGxArchive :
	public wxGxFolder
{
public:
	wxGxArchive(wxString Path, wxString Name, wxString sType);
	virtual ~wxGxArchive(void);
	//IGxObject
    virtual wxString GetBaseName(void);
	virtual wxString GetCategory(void){return wxString(_("Archive"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return true;};
	virtual bool Rename(wxString NewName);
	virtual bool CanRename(void){return true;};
	//wxGxFolder
	virtual void LoadChildren(void);
protected:
    wxString m_sType;
};

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveFolder
/////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLT wxGxArchiveFolder :
	public wxGxFolder
{
public:
	wxGxArchiveFolder(wxString Path, wxString Name);
	virtual ~wxGxArchiveFolder(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Archive folder"));};
	//wxGxFolder
	virtual void LoadChildren(void);
	//IGxObjectEdit unsupported yet
	virtual bool CanDelete(void){return false;};
	virtual bool CanRename(void){return false;};
};

