/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxToolExecuteView class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

/** \class wxGISToolExecuteView gxtoolexecview.h
    \brief The tasks execution view class.
*/

/*
class WXDLLIMPEXP_GIS_GPU wxGISToolExecuteView :
	public wxListCtrl,
	public IGxSelectionEvents,
	public IGxCatalogEvents
    {
    DECLARE_DYNAMIC_CLASS(wxGISToolExecuteView)
public:
	enum
	{
		TOOLEXECUTECTRLID = WXGISHIGHEST + 1
    };

    wxGISToolExecuteView(void);
	wxGISToolExecuteView(wxWindow* parent, wxWindowID id = TOOLEXECUTECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_REPORT | wxBORDER_NONE | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE);
	virtual ~wxGISToolExecuteView(void);
	virtual void Serialize(wxXmlNode* pRootNode, bool bStore);
	virtual void AddObject(IGxObject* pObject);
	virtual void ResetContents(void);
    virtual IGxObject* GetParentGxObject(void){return m_pParentGxObject;};
    virtual void SelectAll(void);
    virtual bool Show(bool show = true);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_REPORT | wxBORDER_NONE | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE, const wxString& name = wxT("ToolExecuteView"));
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
	virtual void OnSelected(wxListEvent& event);
	virtual void OnDeselected(wxListEvent& event);

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
	bool m_bSortAsc;
	short m_currentSortCol;
	wxImageList m_ImageList;
	IConnectionPointContainer* m_pConnectionPointCatalog;
	long m_ConnectionPointCatalogCookie;
	IGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
	IGxObject* m_pParentGxObject;
    std::vector<ICONDATA> m_IconsArray;

DECLARE_EVENT_TABLE()
};
*/

/** \class wxGxToolExecuteView gxtoolexecview.h
    \brief The tasks execution gxcatalog view class.
*/

/*
class WXDLLIMPEXP_GIS_GPU wxGxToolExecuteView :
	public wxGxView,
	public IGxSelectionEvents
{
    DECLARE_DYNAMIC_CLASS(wxGxToolExecuteView)
public:
    wxGxToolExecuteView(void);
	wxGxToolExecuteView(wxWindow* parent, wxWindowID id = TABLECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxGxToolExecuteView(void);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("TableView"));
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(IGxSelection* Selection);
private:
	IGxSelection* m_pSelection;
	IGxObject* m_pParentGxObject;
};
*/

