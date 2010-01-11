/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDialog filters of GxObjects to show.
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

#include "wxgis/catalogui/catalogui.h"

//------------------------------------------------------------
// wxGxObjectFilter
//------------------------------------------------------------

class wxGxObjectFilter : public IGxObjectFilter
{
public:
	wxGxObjectFilter(void);
	~wxGxObjectFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
};

//------------------------------------------------------------
// wxGxPrjFileFilter
//------------------------------------------------------------

class wxGxPrjFileFilter : public wxGxObjectFilter
{
public:
	wxGxPrjFileFilter(void);
	~wxGxPrjFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
};

//------------------------------------------------------------
// wxGxRasterDatasetFilter
//------------------------------------------------------------

class wxGxRasterDatasetFilter : public wxGxObjectFilter
{
public:
	wxGxRasterDatasetFilter(void);
	~wxGxRasterDatasetFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
};
