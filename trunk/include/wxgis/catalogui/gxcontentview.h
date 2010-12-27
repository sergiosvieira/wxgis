/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContentView class.
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/newmenu.h"

#include "wx/listctrl.h"
#include "wx/imaglist.h"

//TODO: Fix mouse selection dragging linux

typedef struct _sortdata
{
    bool bSortAsc;
    short currentSortCol;
} SORTDATA, *LPSORTDATA;


class WXDLLIMPEXP_GIS_CLU wxGxContentView :
	public wxListCtrl,
	public wxGxView,
	public IGxSelectionEvents,
	public IGxCatalogEvents,
    public IGxContentsView
{
    DECLARE_DYNAMIC_CLASS(wxGxContentView)
public:
    wxGxContentView(void);
	wxGxContentView(wxWindow* parent, wxWindowID id = LISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_LIST | wxBORDER_NONE | wxLC_EDIT_LABELS |wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE);//
	virtual ~wxGxContentView(void);
	virtual void Serialize(wxXmlNode* pRootNode, bool bStore);
	virtual void AddObject(IGxObject* pObject);
	virtual void ResetContents(void);
    virtual IGxObject* GetParentGxObject(void){return m_pParentGxObject;};
    virtual void SelectAll(void);
    virtual bool Show(bool show = true);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = LISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_LIST | wxBORDER_NONE | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE, const wxString& name = wxT("ContentView"));
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(IGxSelection* Selection);
    virtual void BeginRename(IGxObject* pGxObject = NULL);
//IGxSelectionEvents
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator);
//IGxCatalogEvents
	virtual void OnObjectAdded(IGxObject* object);
	virtual void OnObjectChanged(IGxObject* object);
	virtual void OnObjectDeleted(IGxObject* object);
	virtual void OnObjectRefreshed(IGxObject* object);
	virtual void OnRefreshAll(void);
// IGxContentsView
	virtual void SetStyle(wxGISEnumContentsViewStyle style);
    virtual wxGISEnumContentsViewStyle GetStyle(void){return m_current_style;};
    virtual bool CanSetStyle(void){return true;};
//events
	virtual void OnColClick(wxListEvent& event);
    virtual void OnContextMenu(wxContextMenuEvent& event);
	virtual void ShowContextMenu(const wxPoint& pos);
	virtual void SetColumnImage(int col, int image);
    virtual void OnActivated(wxListEvent& event);
    virtual void OnBeginLabelEdit(wxListEvent& event);
	virtual void OnEndLabelEdit(wxListEvent& event);
	virtual void OnSelected(wxListEvent& event);
	virtual void OnDeselected(wxListEvent& event);
    virtual void OnBeginDrag(wxListEvent& event);
    virtual void OnChar(wxKeyEvent& event);

	typedef struct _itemdata
	{
		IGxObject* pObject;
		int iImageIndex;
	} ITEMDATA, *LPITEMDATA;
	typedef struct _icondata
	{
		wxIcon oIcon;
		int iImageIndex;
        bool bLarge;
	} ICONDATA;
protected:
    int GetIconPos(wxIcon icon_small, wxIcon icon_large);
protected:
	bool m_bSortAsc;
	short m_currentSortCol;
	wxGISEnumContentsViewStyle m_current_style;
	wxImageList m_ImageListSmall, m_ImageListLarge;
	IConnectionPointContainer* m_pConnectionPointCatalog;
	long m_ConnectionPointCatalogCookie;
	IGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    ICommand* m_pDeleteCmd;
	wxGISNewMenu* m_pNewMenu;
	IGxObject* m_pParentGxObject;
	bool m_bDragging;
    std::vector<ICONDATA> m_IconsArray;
    wxCriticalSection m_CritSect;
    wxCriticalSection m_CritSectCont;

    DECLARE_EVENT_TABLE()
};
