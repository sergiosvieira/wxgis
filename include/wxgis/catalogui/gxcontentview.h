/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContentView class.
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
#include "wx/listctrl.h"
#include "wx/imaglist.h"

typedef struct _sortdata
{
    bool bSortAsc;
    short currentSortCol;
} SORTDATA, *LPSORTDATA;


class WXDLLIMPEXP_GIS_CLU wxGxContentView :
	public wxListCtrl,
	public wxGxView,
	public IGxSelectionEvents,
	public IGxCatalogEvents
{
public:
	typedef enum _listsyle{ REPORT, SMALL, LARGE, LIST } LISTSTYLE, *LPLISTSTYLE;

	wxGxContentView(wxWindow* parent, wxWindowID id = LISTCTRLID, const wxPoint& pos = wxDefaultPosition,
						 const wxSize& size = wxDefaultSize, long style = wxLC_REPORT | wxBORDER_NONE | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE );
	virtual ~wxGxContentView(void);
	virtual void Serialize(wxXmlNode* pRootNode, bool bStore);
	virtual void AddObject(IGxObject* pObject);
	virtual void SetStyle(LISTSTYLE style);
    virtual LISTSTYLE GetStyle(void){return m_current_style;};
	virtual void ResetContents(void);
    virtual IGxObject* GetParentGxObject(void){return m_pParentGxObject;};
    virtual void SelectAll(void);
//IGxView
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(IGxSelection* Selection);
//IGxSelectionEvents
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator);
//IGxCatalogEvents
	virtual void OnObjectAdded(IGxObject* object);
	virtual void OnObjectChanged(IGxObject* object);
	virtual void OnObjectDeleted(IGxObject* object);
	virtual void OnObjectRefreshed(IGxObject* object);
	virtual void OnRefreshAll(void);
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
    virtual void OnSetFocus(wxFocusEvent& event);
    virtual void OnBeginDrag(wxListEvent& event);

	typedef struct _itemdata
	{
		IGxObject* pObject;
		int iImageIndex;
	} ITEMDATA, *LPITEMDATA;

protected:
	bool m_bSortAsc;
	short m_currentSortCol;
	LISTSTYLE m_current_style;
	wxImageList m_ImageListSmall, m_ImageListLarge;
	IConnectionPointContainer* m_pConnectionPointCatalog/*, *m_pConnectionPointSelection*/;
	long m_ConnectionPointCatalogCookie/*, m_ConnectionPointSelectionCookie*/;
	IGxSelection* m_pSelection;
    IGxCatalog* m_pCatalog;
	IGxObject* m_pParentGxObject;
	bool m_bDragging;

DECLARE_EVENT_TABLE()
};
