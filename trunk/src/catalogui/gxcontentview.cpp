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
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalog/gxdiscconnection.h"

#include "../../art/document_16.xpm"
#include "../../art/document_48.xpm"
#include "../../art/small_arrow.xpm"

#include "wx/dnd.h"
#include "wx/dataobj.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxContentView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGxContentView, wxListCtrl)
    EVT_LIST_BEGIN_LABEL_EDIT(LISTCTRLID, wxGxContentView::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LISTCTRLID, wxGxContentView::OnEndLabelEdit)
    EVT_LIST_ITEM_SELECTED(LISTCTRLID, wxGxContentView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LISTCTRLID, wxGxContentView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(LISTCTRLID, wxGxContentView::OnActivated)

    EVT_LIST_BEGIN_DRAG(LISTCTRLID, wxGxContentView::OnBeginDrag)

    EVT_LIST_COL_CLICK(LISTCTRLID, wxGxContentView::OnColClick)
    EVT_CONTEXT_MENU(wxGxContentView::OnContextMenu)
    EVT_CHAR(wxGxContentView::OnChar)
END_EVENT_TABLE()

int wxCALLBACK MyCompareFunction(long item1, long item2, long sortData)
{
	wxGxContentView::LPITEMDATA pItem1 = (wxGxContentView::LPITEMDATA)item1;
 	wxGxContentView::LPITEMDATA pItem2 = (wxGxContentView::LPITEMDATA)item2;
    LPSORTDATA psortdata = (LPSORTDATA)sortData;
    if(psortdata->currentSortCol == 0)
        return GxObjectCompareFunction(pItem1->pObject, pItem2->pObject, psortdata->bSortAsc);
    else
    {
	    IGxObjectSort* pGxObjectSort1 = dynamic_cast<IGxObjectSort*>(pItem1->pObject);
        IGxObjectSort* pGxObjectSort2 = dynamic_cast<IGxObjectSort*>(pItem2->pObject);
        if(pGxObjectSort1 && !pGxObjectSort2)
		    return psortdata->bSortAsc == 0 ? 1 : -1;
        if(!pGxObjectSort1 && pGxObjectSort2)
		    return psortdata->bSortAsc == 0 ? -1 : 1;
        if(pGxObjectSort1 && pGxObjectSort2)
        {
            bool bAlwaysTop1 = pGxObjectSort1->IsAlwaysTop();
            bool bAlwaysTop2 = pGxObjectSort2->IsAlwaysTop();
            if(bAlwaysTop1 && !bAlwaysTop2)
		        return psortdata->bSortAsc == 0 ? 1 : -1;
            if(!bAlwaysTop1 && bAlwaysTop2)
		        return psortdata->bSortAsc == 0 ? -1 : 1;
            bool bSortEnables1 = pGxObjectSort1->IsSortEnabled();
            bool bSortEnables2 = pGxObjectSort2->IsSortEnabled();
            if(!bSortEnables1 || !bSortEnables1)
                return 0;
        }

	    //bool bDiscConnection1 = dynamic_cast<wxGxDiscConnection*>(pItem1->pObject);
     //   bool bDiscConnection2 = dynamic_cast<wxGxDiscConnection*>(pItem2->pObject);
     //   if(bDiscConnection1 && !bDiscConnection2)
		   // return psortdata->bSortAsc == 0 ? 1 : -1;
     //   if(!bDiscConnection1 && bDiscConnection2)
		   // return psortdata->bSortAsc == 0 ? -1 : 1;

	    bool bContainerDst1 = dynamic_cast<IGxDataset*>(pItem1->pObject);
        bool bContainerDst2 = dynamic_cast<IGxDataset*>(pItem2->pObject);
	    bool bContainer1 = dynamic_cast<IGxObjectContainer*>(pItem1->pObject);
        bool bContainer2 = dynamic_cast<IGxObjectContainer*>(pItem2->pObject);
        if(bContainer1 && !bContainerDst1 && bContainerDst2)
	        return psortdata->bSortAsc == 0 ? 1 : -1;
        if(bContainer2 && !bContainerDst2 && bContainerDst1)
	        return psortdata->bSortAsc == 0 ? -1 : 1;
        if(bContainer1 && !bContainer2)
	        return psortdata->bSortAsc == 0 ? 1 : -1;
        if(!bContainer1 && bContainer2)
	        return psortdata->bSortAsc == 0 ? -1 : 1;

	    return pItem1->pObject->GetCategory().CmpNoCase(pItem2->pObject->GetCategory()) * (psortdata->bSortAsc == 0 ? -1 : 1);
    }
    return 0;
//        return GxObjectCompareFunction(pItem1->pObject, pItem2->pObject, psortdata->bSortAsc);
}

wxGxContentView::wxGxContentView(void)
{
}

wxGxContentView::wxGxContentView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
wxListCtrl(parent, id, pos, size, style), m_bSortAsc(true), m_current_style(enumGISCVList), m_pConnectionPointCatalog(NULL), m_ConnectionPointCatalogCookie(-1), m_pParentGxObject(NULL), m_currentSortCol(0), m_pSelection(NULL), m_bDragging(false), m_pDeleteCmd(NULL)
{
	InsertColumn(0, _("Name"),	wxLIST_FORMAT_LEFT, 150);
	InsertColumn(1, _("Type"),  wxLIST_FORMAT_LEFT, 250);

	m_ImageListSmall.Create(16, 16);
	m_ImageListLarge.Create(48, 48);

	//set default icons
	//col ico & default
    m_ImageListLarge.Add(wxBitmap(48, 48));
    m_ImageListLarge.Add(wxBitmap(48, 48));
	m_ImageListLarge.Add(wxBitmap(document_48_xpm));

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg); 
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg); 

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));
	m_ImageListSmall.Add(wxBitmap(document_16_xpm));

	SetImageList(&m_ImageListLarge, wxIMAGE_LIST_NORMAL);
	SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);
}

wxGxContentView::~wxGxContentView(void)
{
	ResetContents();
}

bool wxGxContentView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_bSortAsc = true;
    m_current_style = enumGISCVList;
    m_pConnectionPointCatalog = NULL;
    m_ConnectionPointCatalogCookie = -1;
    m_pParentGxObject = NULL;
    m_currentSortCol = 0;
    m_pSelection = NULL;
    m_bDragging = false;
    m_pDeleteCmd = NULL;

    wxListCtrl::Create(parent, LISTCTRLID, pos, size, wxLC_LIST | wxBORDER_NONE | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE);
    
    InsertColumn(0, _("Name"),	wxLIST_FORMAT_LEFT, 150);
	InsertColumn(1, _("Type"),  wxLIST_FORMAT_LEFT, 250);

	m_ImageListSmall.Create(16, 16);
	m_ImageListLarge.Create(48, 48);

	//set default icons
	//col ico & default
    m_ImageListLarge.Add(wxBitmap(48, 48));
    m_ImageListLarge.Add(wxBitmap(48, 48));
	m_ImageListLarge.Add(wxBitmap(document_48_xpm));

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg); 
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg); 

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));
	m_ImageListSmall.Add(wxBitmap(document_16_xpm));

	SetImageList(&m_ImageListLarge, wxIMAGE_LIST_NORMAL);
	SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

    return true;
}

bool wxGxContentView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);
	Serialize(m_pXmlConf, false);

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(application->GetCatalog());

    IApplication* pApp = dynamic_cast<IApplication*>(application);
    if(pApp)
    {
		//delete
        m_pDeleteCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 4);
		//new 
		m_pNewMenu = dynamic_cast<wxGISNewMenu*>(pApp->GetCommandBar(NEWMENUNAME));

    }

	m_pConnectionPointCatalog = dynamic_cast<IConnectionPointContainer*>( m_pCatalog );
	if(m_pConnectionPointCatalog != NULL)
		m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

	m_pSelection = m_pCatalog->GetSelection();

	return true;
}

void wxGxContentView::Deactivate(void)
{
	if(m_ConnectionPointCatalogCookie != -1)
		m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}


void wxGxContentView::Serialize(wxXmlNode* pRootNode, bool bStore)
{
	if(pRootNode == NULL)
		return;

	if(bStore)
	{
        if(pRootNode->HasProp(wxT("style")))
            pRootNode->DeleteProperty(wxT("style"));
        pRootNode->AddProperty(wxT("style"), wxString::Format(wxT("%d"), m_current_style));
        if(pRootNode->HasProp(wxT("sort")))
            pRootNode->DeleteProperty(wxT("sort"));
        pRootNode->AddProperty(wxT("sort"), wxString::Format(wxT("%d"), m_bSortAsc));
        if(pRootNode->HasProp(wxT("sort_col")))
            pRootNode->DeleteProperty(wxT("sort_col"));
        pRootNode->AddProperty(wxT("sort_col"), wxString::Format(wxT("%d"), m_currentSortCol));
        if(pRootNode->HasProp(wxT("name_width")))
            pRootNode->DeleteProperty(wxT("name_width"));
        pRootNode->AddProperty(wxT("name_width"), wxString::Format(wxT("%d"), GetColumnWidth(0)));
        if(pRootNode->HasProp(wxT("type_width")))
            pRootNode->DeleteProperty(wxT("type_width"));
        pRootNode->AddProperty(wxT("type_width"), wxString::Format(wxT("%d"), GetColumnWidth(1)));
	}
	else
	{
		m_bSortAsc = wxAtoi(pRootNode->GetPropVal(wxT("sort"), wxT("1")));
		m_currentSortCol = wxAtoi(pRootNode->GetPropVal(wxT("sort_col"), wxT("0")));
		wxGISEnumContentsViewStyle style = (wxGISEnumContentsViewStyle)wxAtoi(pRootNode->GetPropVal(wxT("style"), wxT("0")));
		int nw = wxAtoi(pRootNode->GetPropVal(wxT("name_width"), wxT("150")));
		if(nw == 0)
			nw = 150;
		int tw = wxAtoi(pRootNode->GetPropVal(wxT("type_width"), wxT("250")));
		if(tw == 0)
			tw = 250;
		SetColumnWidth(0, nw);
		SetColumnWidth(1, tw);

        SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
		SortItems(MyCompareFunction, (long)&sortdata);
		SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);

		SetStyle(style);
	}
}

void wxGxContentView::AddObject(IGxObject* pObject)
{
	if(pObject == NULL)
		return;

    IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pObject);
	wxIcon icon_small, icon_large;
	if(pObjUI != NULL)
	{
		icon_small = pObjUI->GetSmallImage();
		icon_large = pObjUI->GetLargeImage();
	}

	int pos = GetIconPos(icon_small, icon_large);

	LPITEMDATA pData = new _itemdata;
	pData->pObject = pObject;
	pData->iImageIndex = pos;

    wxString sName;
    if(m_pCatalog->GetShowExt())
        sName = pObject->GetName();
    else
        sName = pObject->GetBaseName();

	wxString sType = pObject->GetCategory();

#ifdef __WXGTK__
    if(GetColumnCount() < 2)
    {
        InsertColumn(0, _("Name"),	wxLIST_FORMAT_LEFT, 150);
        InsertColumn(1, _("Type"),  wxLIST_FORMAT_LEFT, 250);
    }
#endif
	long ListItemID = InsertItem(0, sName, pos);//GetItemCount()
	SetItem(ListItemID, 1, sType);
	SetItemPtrData(ListItemID, (wxUIntPtr) pData);

	wxListCtrl::Refresh();
}

void wxGxContentView::OnColClick(wxListEvent& event)
{
    //event.Skip();
	//int col = event.GetColumn();
	//if(col != 0)
	//   return;
    m_currentSortCol = event.GetColumn();
	m_bSortAsc = !m_bSortAsc;

    SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(MyCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

void wxGxContentView::OnContextMenu(wxContextMenuEvent& event)
{
    //event.Skip();
    wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1)
	{
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
	else
	{
        point = ScreenToClient(point);
    }
    ShowContextMenu(point);
}

void wxGxContentView::OnSelected(wxListEvent& event)
{
	//event.Skip();
    m_pSelection->Clear(NOTFIRESELID);
    long nItem = -1;
	size_t nCount(0);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == -1 )
            break;
        LPITEMDATA pItemData = (LPITEMDATA)GetItemData(nItem);
	    if(pItemData == NULL)
            continue;
		nCount++;
        m_pSelection->Select(pItemData->pObject, true, NOTFIRESELID);
    }
	if(	m_pNewMenu )
	{
		if(nCount > 0)
			m_pNewMenu->Update(m_pSelection);
		else
		{
			m_pSelection->SetInitiator(TREECTRLID);
			m_pNewMenu->Update(m_pSelection);
		}
	}
}

bool wxGxContentView::Show(bool show)
{
    bool res = wxListCtrl::Show(show);
    if(show)
    {
        //deselect all items
        long nItem = -1;
        for ( ;; )
        {
            nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if ( nItem == -1 )
                break;
            SetItemState(nItem, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
        }
    }
    return res;
}


void wxGxContentView::OnDeselected(wxListEvent& event)
{
	//event.Skip();
    if(GetSelectedItemCount() == 0)
        m_pSelection->Select(m_pParentGxObject, false, NOTFIRESELID);

	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

	m_pSelection->Unselect(pItemData->pObject, NOTFIRESELID);
	if(	m_pNewMenu )
	{
		if(GetSelectedItemCount() > 0)
			m_pNewMenu->Update(m_pSelection);
		else
		{
			m_pSelection->SetInitiator(TREECTRLID);
			m_pNewMenu->Update(m_pSelection);
		}
	}
}

void wxGxContentView::ShowContextMenu(const wxPoint& pos)
{
	long item = -1;
    item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if(item == -1)
	{
		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(m_pParentGxObject);
        if(pGxObjectUI)
        {
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            IApplication* pApp = dynamic_cast<IApplication*>(m_pApplication);
            if(pApp)
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(pApp->GetCommandBar(psContextMenu));
                if(pMenu)
                {
                    PopupMenu(pMenu, pos.x, pos.y);
                }
            }
        }
		return;
	}

	LPITEMDATA pItemData = (LPITEMDATA)GetItemData(item);
	if(pItemData != NULL)
	{
        bool bAdd = true;
        m_pSelection->Select(pItemData->pObject, bAdd, NOTFIRESELID);

		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pItemData->pObject);
		if(pGxObjectUI != NULL)
		{
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            IApplication* pApp = dynamic_cast<IApplication*>(m_pApplication);
            if(pApp)
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(pApp->GetCommandBar(psContextMenu));
                if(pMenu)
                {
                    PopupMenu(pMenu, pos.x, pos.y);
                }
            }
		}
	}
}

void wxGxContentView::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    //reset image
    item.SetImage(-1);
    for(size_t i = 0; i < GetColumnCount(); i++)
        SetColumn(i, item);

    item.SetImage(image);
    SetColumn(col, item);
}

void wxGxContentView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pItemData->pObject);
	if(pGxObjectWizard != NULL)
		if(!pGxObjectWizard->Invoke(this))
			return;

	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pItemData->pObject);
	if(pGxObjectContainer != NULL )//&& pGxObjectContainer->HasChildren() )
	{
    //  m_pApplication->GetCatalog()->SetLocation(pItemData->pObject->GetFullName());
		m_pSelection->Select(pItemData->pObject, false, GetId());
	//	m_pCatalog->OnSelectObject(pItemData->pObject);
	}
}

void wxGxContentView::SetStyle(wxGISEnumContentsViewStyle style)
{
    if(m_current_style == style)
        return;
	switch(style)
	{
	case enumGISCVReport:
        SetSingleStyle(wxLC_REPORT);
		//SetWindowStyleFlag(m_style | wxLC_REPORT);
		break;
	case enumGISCVSmall:
        SetSingleStyle(wxLC_SMALL_ICON);
		//SetWindowStyleFlag(m_style | wxLC_SMALL_ICON );
		break;
	case enumGISCVLarge:
        SetSingleStyle(wxLC_ICON);
		//SetWindowStyleFlag(m_style | wxLC_ICON );
		break;
	case enumGISCVList:
        SetSingleStyle(wxLC_LIST);
		//SetWindowStyleFlag(m_style | wxLC_LIST );
		break;
	}

    switch(m_current_style)
	{
	case enumGISCVReport:
        SetSingleStyle(wxLC_REPORT, false);
		break;
	case enumGISCVSmall:
        SetSingleStyle(wxLC_SMALL_ICON, false);
		break;
	case enumGISCVLarge:
        SetSingleStyle(wxLC_ICON, false);
		break;
	case enumGISCVList:
        SetSingleStyle(wxLC_LIST, false);
		break;
	}
	m_current_style = style;
#ifdef __WXGTK__
    OnRefreshAll();
#endif
}

//void wxGxContentView::UpdateSelection(void)
//{
//	//m_critsect_sel.Enter();
//	//m_Selection.clear();
//	//long item = -1;
// //   while(1)
// //   {
// //       item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
//	//	if(item == -1)
//	//		break;
//	//	LPITEMDATA pData = (LPITEMDATA)GetItemData(item);
//	//	if(pData != NULL && pData->pObject != NULL)
//	//		m_Selection.push_back(pData->pObject);
// //   }
//
//	//if(m_Selection.size() == 0)
//	//	m_Selection.push_back(m_pParentObject);
//	//m_critsect_sel.Leave();
//}

//void wxGxContentView::RenameSelection(void)
//{
//	if(m_Selection.size() == 0)
//		return;
//	long item;// = m_ListMap[m_Selection[0]];
//	for(long i = 0; i < GetItemCount(); i++)
//	{
//		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
//		if(pItemData == NULL)
//			continue;
//		if(pItemData->pObject != m_Selection[0])
//			continue;
//		EditLabel(i);
//		return;
//	}
//}

void wxGxContentView::OnBeginLabelEdit(wxListEvent& event)
{
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
	{
		event.Veto();
		return;
	}
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pItemData->pObject);
	if(pObjEdit == NULL)
	{
		event.Veto();
		return;
	}
	if(!pObjEdit->CanRename())
	{
		event.Veto();
		return;
	}
}

void wxGxContentView::OnEndLabelEdit(wxListEvent& event)
{
    if ( event.GetLabel().Len() == 0 )
    {
        event.Veto();
    }
	else
	{
		LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
		if(pItemData == NULL)
		{
			event.Veto();
			return;
		}
		IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pItemData->pObject);
		if(pObjEdit == NULL)
		{
			event.Veto();
			return;
		}
		if(!pObjEdit->Rename(event.GetLabel()))
		{
			event.Veto();
			wxMessageBox(_("Rename error!"), _("Error"), wxICON_ERROR | wxOK );
		}
	}
}

void wxGxContentView::OnObjectAdded(IGxObject* pObj)
{
	if(pObj->GetParent() == m_pParentGxObject)
    {
		AddObject(pObj);
        SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	    SortItems(MyCompareFunction, (long)&sortdata);
	    //SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
    }
}

void wxGxContentView::OnObjectDeleted(IGxObject* pObj)
{
    //wxCriticalSectionLocker locker(m_CritSectCont);
	for(long i = 0; i < GetItemCount(); i++)
	{
		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
		if(pItemData == NULL)
			continue;
		if(pItemData->pObject != pObj)
			continue;
        SetItemData(i, NULL);
		//delete pItemData;
		DeleteItem(i);
		//Refresh();
        wxListCtrl::Refresh();
        return;
	}
}

void wxGxContentView::OnObjectChanged(IGxObject* pObj)
{
    if(pObj == m_pParentGxObject)
	{
		IGxObjectContainer* pObjectContainer = dynamic_cast<IGxObjectContainer*>(pObj);
		if(pObjectContainer != NULL)
		{
			if(GetItemCount() > 0 && !pObjectContainer->HasChildren())
				ResetContents();
			else if(GetItemCount() == 0 && pObjectContainer->HasChildren())
			{
				OnRefreshAll();
				return;
			}
		}
	}

    //wxCriticalSectionLocker locker(m_CritSectCont);
	for(long i = 0; i < GetItemCount(); i++)
	{
		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
		if(pItemData == NULL)
			continue;
		if(pItemData->pObject != pObj)
			continue;
	
		IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pItemData->pObject);
		wxIcon icon_small, icon_large;
		if(pObjUI != NULL)
		{
			icon_small = pObjUI->GetSmallImage();
			icon_large = pObjUI->GetLargeImage();
		}

        int pos = GetIconPos(icon_small, icon_large);
		pItemData->iImageIndex = pos;

		wxString sName;
		if(m_pCatalog->GetShowExt())
			sName = pItemData->pObject->GetName();
		else
			sName = pItemData->pObject->GetBaseName();

		wxString sType = pItemData->pObject->GetCategory();

		if(i < GetItemCount())
		{
			SetItem(i, 0, sName, pos);
			SetItem(i, 1, sType);
		}
	}
	wxListCtrl::Refresh();
}

void wxGxContentView::OnObjectRefreshed(IGxObject* pObj)
{
    if(m_pParentGxObject == pObj)
        OnRefreshAll();
}

void wxGxContentView::OnRefreshAll(void)
{
    ResetContents();
	IGxObjectContainer* pObjContainer =  dynamic_cast<IGxObjectContainer*>(m_pParentGxObject);
	if(pObjContainer == NULL || !pObjContainer->HasChildren())
		return;
	GxObjectArray* pArr = pObjContainer->GetChildren();
	for(size_t i = 0; i < pArr->size(); i++)
	{
		AddObject(pArr->at(i));
	}

    SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(MyCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);

    wxListCtrl::Refresh();
}

void wxGxContentView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;
	IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();
	if(m_pParentGxObject == pGxObj)
		return;

	//reset
	ResetContents();
	m_pParentGxObject = pGxObj;

	IGxObjectContainer* pObjContainer =  dynamic_cast<IGxObjectContainer*>(pGxObj);
	if(pObjContainer == NULL || !pObjContainer->HasChildren())
		return;

	GxObjectArray* pArr = pObjContainer->GetChildren();
	for(size_t i = 0; i < pArr->size(); i++)
	{
		AddObject(pArr->at(i));
	}

    SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(MyCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

bool wxGxContentView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); i++)
	{
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>( Selection->GetSelectedObjects(i) );
		if(pGxObjectContainer != NULL)
			return true;
	}
	return false;
}

void wxGxContentView::ResetContents(void)
{
	for(long i = 0; i < GetItemCount(); i++)
		delete (LPITEMDATA)GetItemData(i);
	DeleteAllItems();
}

void wxGxContentView::OnBeginDrag(wxListEvent& event)
{
    wxFileDataObject my_data;
    long nItem = -1;
    int nCount(0);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == -1 )
            break;
        LPITEMDATA pItemData = (LPITEMDATA)GetItemData(nItem);
	    if(pItemData == NULL)
            continue;
        if(!pItemData->pObject)
            continue;
        my_data.AddFile(pItemData->pObject->GetInternalName());
        nCount++;
    }
    if(nCount == 0)
        return;

    wxDropSource dragSource( this );
	dragSource.SetData( my_data );
	wxDragResult result = dragSource.DoDragDrop( TRUE );
}

void wxGxContentView::SelectAll(void)
{
	for(long item = 0; item < GetItemCount(); item++)
        SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxGxContentView::OnChar(wxKeyEvent& event)
{
	if(event.GetModifiers() & wxMOD_ALT)
		return;
	if(event.GetModifiers() & wxMOD_CONTROL)
		return;
	if(event.GetModifiers() & wxMOD_SHIFT)
		return;
    switch(event.GetKeyCode())
    {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
        if(m_pDeleteCmd)
            m_pDeleteCmd->OnClick();
        break;
    default:
        break;
    }
}


void wxGxContentView::BeginRename(IGxObject* pGxObject)
{
	for(long i = 0; i < GetItemCount(); i++)
	{
		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
		if(pItemData == NULL)
			continue;
		if(pItemData->pObject == pGxObject)
        {
            SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            m_pSelection->Select(pGxObject, true, NOTFIRESELID);
            break;
        }
    }

    int nItem = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if ( nItem == -1 )
        return;
    EditLabel(nItem);
}

int wxGxContentView::GetIconPos(wxIcon icon_small, wxIcon icon_large)
{
    wxCriticalSectionLocker locker(m_CritSect);
	int pos(0);
	if(icon_small.IsOk())
	{
		for(size_t i = 0; i < m_IconsArray.size(); i++)
		{
			if(m_IconsArray[i].bLarge)
				continue;
			if(m_IconsArray[i].oIcon.IsSameAs(icon_small))
			{
				pos = m_IconsArray[i].iImageIndex;
				break;
			}
		}
		if(pos == 0)
		{
			pos = m_ImageListSmall.Add(icon_small);
			ICONDATA myicondata = {icon_small, pos, false};
			m_IconsArray.push_back(myicondata);

            wxIcon temp_large_icon(document_48_xpm);
            if(!icon_large.IsOk())
                icon_large = temp_large_icon;

            pos = m_ImageListLarge.Add(icon_large);
			ICONDATA myicondata1 = {icon_large, pos, true};
			m_IconsArray.push_back(myicondata1);
		}
	}
	else
		pos = 2;//0 col img, 1 - col img

    return pos;
}