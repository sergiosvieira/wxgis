/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxTreeView class.
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

#include "wxgis/catalogui/gxview.h"

#include <wx/treectrl.h>
//#include <wx/msgdlg.h>

#include "../../art/document.xpm"

//////////////////////////////////////////////////////////////////////////////
// wxGxTreeItemData
//////////////////////////////////////////////////////////////////////////////

class wxGxTreeItemData : public wxTreeItemData
{
public:
	wxGxTreeItemData(IGxObject* pObject, int smallimage_index, bool bExpandedOnce)
	{
		m_pObject = pObject;
		m_smallimage_index = smallimage_index;
		m_bExpandedOnce = bExpandedOnce;
	}

	~wxGxTreeItemData(void)
	{
		m_pObject = NULL;
	}

	IGxObject* m_pObject;
	int m_smallimage_index;
	bool m_bExpandedOnce;
};

//////////////////////////////////////////////////////////////////////////////
// wxGxTreeViewBase
//////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLU wxGxTreeViewBase :
	public wxTreeCtrl,
	public wxGxView,
	public IGxSelectionEvents,
	public IGxCatalogEvents
{
    DECLARE_DYNAMIC_CLASS(wxGxTreeViewBase)
public:
    wxGxTreeViewBase(void);
	wxGxTreeViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
	virtual ~wxGxTreeViewBase(void);
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
	virtual void AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent, bool sort = true);
	virtual void AddRoot(IGxObject* pGxObject);
//IGxView
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
//IGxSelectionEvents
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator);
//IGxCatalogEvents
	virtual void OnObjectAdded(IGxObject* object);
	virtual void OnObjectChanged(IGxObject* object);
	virtual void OnObjectDeleted(IGxObject* object);
	virtual void OnObjectRefreshed(IGxObject* object);
	virtual void OnRefreshAll(void);
//wxTreeCtrl
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
//
	virtual void OnItemExpanding(wxTreeEvent& event);

	typedef std::map<IGxObject*, wxTreeItemId> WETREEMAP; 
protected:
	wxImageList m_TreeImageList;
	WETREEMAP m_TreeMap;
	IConnectionPointContainer* m_pConnectionPointCatalog, *m_pConnectionPointSelection;
	long m_ConnectionPointCatalogCookie, m_ConnectionPointSelectionCookie;
	IGxSelection* m_pSelection;

    DECLARE_EVENT_TABLE()
};


//////////////////////////////////////////////////////////////////////////////
// wxGxTreeView
//////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLU wxGxTreeView :
	public wxGxTreeViewBase
{
    DECLARE_DYNAMIC_CLASS(wxGxTreeView)
public:
    wxGxTreeView(void);
	wxGxTreeView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxBORDER_NONE | wxTR_EDIT_LABELS /*| wxTR_MULTIPLE| wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT*/);
	virtual ~wxGxTreeView(void);
//
    void OnBeginLabelEdit(wxTreeEvent& event);
    void OnEndLabelEdit(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);

    DECLARE_EVENT_TABLE()
};
