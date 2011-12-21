/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCSVFileFactory class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

class WXDLLIMPEXP_GIS_CLT wxGxCSVFileFactory :
	public IGxObjectFactory,
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxGxCSVFileFactory)
public:
	wxGxCSVFileFactory(void);
	virtual ~wxGxCSVFileFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray);
    virtual void Serialize(wxXmlNode* const pConfig, bool bStore);
    virtual wxString GetClassName(void){return GetClassInfo()->GetClassName();};
    virtual wxString GetName(void){return wxString(_("CSV Files"));};
    //wxGxFileFactory
    virtual IGxObject* GetGxObject(CPLString path, wxString name);
};
