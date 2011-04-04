/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxTreeView class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/newmenu.h"

#include "wx/treectrl.h"
#include "wx/imaglist.h"

//#include <wx/msgdlg.h>

//////////////////////////////////////////////////////////////////////////////
// wxGxTreeItemData
//////////////////////////////////////////////////////////////////////////////

class wxGxTreeItemData : public wxTreeItemData
{
public:
	wxGxTreeItemData(long nObjectID, int smallimage_index, bool bExpandedOnce)
	{
		m_nObjectID = nObjectID;
		m_smallimage_index = smallimage_index;
		m_bExpandedOnce = bExpandedOnce;
	}

	~wxGxTreeItemData(void)
	{
		m_pObject = NULL;
	}

	long m_nObjectID;
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
	wxGxTreeViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS );
	virtual ~wxGxTreeViewBase(void);
	virtual void AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent);
	virtual void AddRoot(IGxObject* pGxObject);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxString& name = wxT("TreeView"));
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
//IGxSelectionEvents
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator);
//IGxCatalogEvents
	virtual void OnObjectAdded(long nObjectID);
	virtual void OnObjectChanged(long nObjectID);
	virtual void OnObjectDeleted(long nObjectID);
	virtual void OnObjectRefreshed(long nObjectID);
	virtual void OnRefreshAll(void);
//wxTreeCtrl
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

	typedef std::map<long, wxTreeItemId> WETREEMAP;
	typedef struct _icondata
	{
		wxIcon oIcon;
		int iImageIndex;
	} ICONDATA;
protected:
//events
	virtual void OnItemExpanding(wxTreeEvent& event);
	virtual void OnItemRightClick(wxTreeEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    //
    virtual void UpdateGxSelection(void);
protected:
	wxImageList m_TreeImageList;
	WETREEMAP m_TreeMap;
	IConnectionPointContainer* m_pConnectionPointCatalog, *m_pConnectionPointSelection;
	long m_ConnectionPointCatalogCookie, m_ConnectionPointSelectionCookie;
	IGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    ICommand* m_pDeleteCmd;
    std::vector<ICONDATA> m_IconsArray;
	wxGISNewMenu* m_pNewMenu;

    DECLARE_EVENT_TABLE()
};


//////////////////////////////////////////////////////////////////////////////
// wxGxTreeView
//////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLU wxGxTreeView :
	public wxGxTreeViewBase,
    public IGxViewDropTarget
{
    DECLARE_DYNAMIC_CLASS(wxGxTreeView)
public:
    wxGxTreeView(void);
	wxGxTreeView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxBORDER_NONE | wxTR_EDIT_LABELS /*| wxTR_MULTIPLE| wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT*/);
	virtual ~wxGxTreeView(void);
//IGxDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
    virtual void OnLeave();
//events
    virtual void OnBeginLabelEdit(wxTreeEvent& event);
    virtual void OnEndLabelEdit(wxTreeEvent& event);
    virtual void OnSelChanged(wxTreeEvent& event);
	virtual void OnItemRightClick(wxTreeEvent& event);
	virtual void OnBeginDrag(wxTreeEvent& event);
	virtual void OnActivated(wxTreeEvent& event);
//wxGxTreeView
    virtual void BeginRename(long nObjectID = wxNOT_FOUND);
protected:
    wxTreeItemId m_HighLightItemId;

    DECLARE_EVENT_TABLE()
};
