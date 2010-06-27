/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapInfoFactory class.
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

class wxGxMapInfoFactory :
	public IGxObjectFactory,
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxGxMapInfoFactory)
public:
	wxGxMapInfoFactory(void);
	virtual ~wxGxMapInfoFactory(void);
	//IGxObjectFactory
	virtual bool GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray);
    virtual void Serialize(wxXmlNode* pConfig, bool bStore);
	virtual wxString GetName(void){return GetClassInfo()->GetClassName();};
    typedef struct _data{
		wxString path;
		int bHasTab, bHasMap, bHasInd, bHasID, bHasDat, bHasMif, bHasMid;
	}DATA;
};
