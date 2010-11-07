/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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

/** \class wxGxDiscConnection gxdiscconnection.h
    \brief A Disc Connection GxObject.
*/
class WXDLLIMPEXP_GIS_CLT wxGxDiscConnection :
	public wxGxFolder
{
public:
	wxGxDiscConnection(wxString Path, wxString Name);
	virtual ~wxGxDiscConnection(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Folder connection"));};
	//IGxObjectEdit
	virtual bool Delete(void);
	virtual bool CanDelete(void){return false;};
	virtual bool Rename(wxString NewName);
};
