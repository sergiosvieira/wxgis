/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxCatalogUI main header.
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

#include "wxgis/base.h"
#include "wxgis/catalog/catalog.h"

#define TREECTRLID	1005 //wxGxCatalog tree
#define LISTCTRLID	1006 //wxGxCatalog contents view
#define TABLECTRLID	1007 //wxGxCatalog table view
#define TABCTRLID	1008 //wxGxCatalog tab with views - contents, preview & etc.
#define MAPCTRLID	1009 //wxGxCatalog map view
//1010 reserved for NOTFIRESELID from catalog.h
#define FILTERCOMBO	1011 //wxGxObjectDialog
#define WXGISHIGHEST	1200 //


class IGxApplication
{
public:
	virtual ~IGxApplication(void){};
	virtual IGxCatalog* GetCatalog(void) = 0;
};


class IGxObjectWizard
{
public:
	virtual ~IGxObjectWizard(void){};
	virtual bool Invoke(wxWindow* pParentWnd) = 0;
};


class IGxObjectUI
{
public:
	virtual ~IGxObjectUI(void){};
	virtual wxIcon GetLargeImage(void) = 0;
	virtual wxIcon GetSmallImage(void) = 0;
	virtual wxString ContextMenu(void) = 0;
	virtual wxString NewMenu(void) = 0;
};

class IGxObjectEditUI
{
public:
	virtual ~IGxObjectEditUI(void){};
	virtual void EditProperties(wxWindow *parent){};
};

class IGxSelection
{
public:
	enum wxGISEnumInitiators
	{
		INIT_ALL = -2,
		INIT_NONE = -1
	} Initiator;
	virtual ~IGxSelection(void){};
	virtual void Select( IGxObject* pObject,  bool appendToExistingSelection, long nInitiator ) = 0;
	virtual void Select( IGxObject* pObject) = 0;
	virtual void Unselect(IGxObject* pObject, long nInitiator) = 0;
	virtual void Clear(long nInitiator) = 0;
	virtual size_t GetCount(void) = 0;
	virtual size_t GetCount(long nInitiator) = 0;
	virtual IGxObject* GetSelectedObjects(size_t nIndex) = 0;
	virtual IGxObject* GetSelectedObjects(long nInitiator, size_t nIndex) = 0;
	virtual IGxObject* GetLastSelectedObject(void) = 0;
	virtual void SetInitiator(long nInitiator) = 0;
    virtual void Do(IGxObject* pObject) = 0;
    virtual bool CanRedo() = 0;
	virtual bool CanUndo() = 0;
	virtual void RemoveDo(wxString sPath) = 0;
    virtual wxString Redo(int nPos = -1) = 0;
    virtual wxString Undo(int nPos = -1) = 0;
    virtual void Reset() = 0;
    virtual size_t GetDoSize() = 0;
    virtual int GetDoPos(void) = 0;
    virtual wxString GetDoPath(size_t nIndex) = 0;
};

class IGxSelectionEvents
{
public:
	virtual ~IGxSelectionEvents(void){};
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator) = 0;
};

class IGxView
{
public:
	virtual ~IGxView(void){};
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("view")) = 0;
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf) = 0;
	virtual void Deactivate(void) = 0;
	virtual bool Applies(IGxSelection* Selection) = 0;
	virtual void Refresh(void) = 0;
	virtual wxString GetViewName(void) = 0;
	virtual wxIcon GetViewIcon(void) = 0;
	virtual void SetViewIcon(wxIcon Icon) = 0;
    virtual void BeginRename(IGxObject* pGxObject = NULL) = 0;
};

enum wxGISEnumContentsViewStyle
{
	enumGISCVReport = 0, 
	enumGISCVSmall,
	enumGISCVLarge,
	enumGISCVList
};

class IGxContentsView
{
public:
	virtual ~IGxContentsView(void){};
	virtual void SelectAll(void) = 0;
    virtual bool CanSetStyle(void) = 0;
	virtual void SetStyle(wxGISEnumContentsViewStyle style) = 0;
    virtual wxGISEnumContentsViewStyle GetStyle(void) = 0;
};
