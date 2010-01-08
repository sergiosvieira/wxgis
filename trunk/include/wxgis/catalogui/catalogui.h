/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxCatalogUI main header.
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

#include "wxgis/base.h"
#include "wxgis/catalog/catalog.h"

#define TREECTRLID	1005 //wxGxCatalog tree
#define LISTCTRLID	1006 //wxGxCatalog contents view
#define TABLECTRLID	1007 //wxGxCatalog table view
#define TABCTRLID	1008 //wxGxCatalog tab with views - contents, preview & etc.
#define MAPCTRLID	1009 //wxGxCatalog map view
//1010 reserved for NOTFIRESELID from catalog.h

enum wxGISEnumSaveObjectResults
{
	enumGISSaveObjectNone = 0x0000, 
	enumGISSaveObjectAccept = 0x0001,
	enumGISSaveObjectExists = 0x0002,
	enumGISSaveObjectDeny = 0x0004
};

class IGxViewsFactory
{
public:
	virtual ~IGxViewsFactory(void){};
	virtual wxWindow* CreateView(wxString sName, wxWindow* parent) = 0;
};

class IGxApplication
{
public:
	virtual ~IGxApplication(void){};
	virtual IGxCatalog* GetCatalog(void) = 0;
};

class IGxView
{
public:
	virtual ~IGxView(void){};
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf) = 0;
	virtual void Deactivate(void) = 0;
	virtual bool Applies(IGxSelection* Selection) = 0;
	virtual void Refresh(void) = 0;
	virtual wxString GetName(void) = 0;
};

class IGxObjectFilter
{
public:
	virtual ~IGxObjectFilter(void){};
	virtual bool CanChooseObject( IGxObject* pObject ) = 0;//, esriDoubleClickResult* result
	virtual bool CanDisplayObject( IGxObject* pObject ) = 0;
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName ) = 0;
	virtual wxString GetName(void) = 0;
};

typedef std::vector<IGxObjectFilter*> OBJECTFILTERS, *LPOBJECTFILTERS;