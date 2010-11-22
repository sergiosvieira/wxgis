/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISToolExecuteView class.
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

#include "wxgis/geoprocessingui/gxtoolexecview.h"

/*
//////////////////////////////////////////////////////////////////////////////
// wxGISToolExecuteView
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISToolExecuteView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGISToolExecuteView, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnActivated)
    EVT_LIST_COL_CLICK(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnColClick)
    EVT_CONTEXT_MENU(wxGISToolExecuteView::OnContextMenu)
END_EVENT_TABLE()

int wxCALLBACK TasksCompareFunction(long item1, long item2, long sortData)
{
	//wxGxContentView::LPITEMDATA pItem1 = (wxGxContentView::LPITEMDATA)item1;
 //	wxGxContentView::LPITEMDATA pItem2 = (wxGxContentView::LPITEMDATA)item2;
 //   LPSORTDATA psortdata = (LPSORTDATA)sortData;
 //   if(psortdata->currentSortCol == 0)
 //       return GxObjectCompareFunction(pItem1->pObject, pItem2->pObject, psortdata->bSortAsc);
 //   else
 //   {
	//    IGxObjectSort* pGxObjectSort1 = dynamic_cast<IGxObjectSort*>(pItem1->pObject);
 //       IGxObjectSort* pGxObjectSort2 = dynamic_cast<IGxObjectSort*>(pItem2->pObject);
 //       if(pGxObjectSort1 && !pGxObjectSort2)
	//	    return psortdata->bSortAsc == 0 ? 1 : -1;
 //       if(!pGxObjectSort1 && pGxObjectSort2)
	//	    return psortdata->bSortAsc == 0 ? -1 : 1;
 //       if(pGxObjectSort1 && pGxObjectSort2)
 //       {
 //           bool bAlwaysTop1 = pGxObjectSort1->IsAlwaysTop();
 //           bool bAlwaysTop2 = pGxObjectSort2->IsAlwaysTop();
 //           if(bAlwaysTop1 && !bAlwaysTop2)
	//	        return psortdata->bSortAsc == 0 ? 1 : -1;
 //           if(!bAlwaysTop1 && bAlwaysTop2)
	//	        return psortdata->bSortAsc == 0 ? -1 : 1;
 //           bool bSortEnables1 = pGxObjectSort1->IsSortEnabled();
 //           bool bSortEnables2 = pGxObjectSort2->IsSortEnabled();
 //           if(!bSortEnables1 || !bSortEnables1)
 //               return 0;
 //       }

	//    bool bContainerDst1 = dynamic_cast<IGxDataset*>(pItem1->pObject);
 //       bool bContainerDst2 = dynamic_cast<IGxDataset*>(pItem2->pObject);
	//    bool bContainer1 = dynamic_cast<IGxObjectContainer*>(pItem1->pObject);
 //       bool bContainer2 = dynamic_cast<IGxObjectContainer*>(pItem2->pObject);
 //       if(bContainer1 && !bContainerDst1 && bContainerDst2)
	//        return psortdata->bSortAsc == 0 ? 1 : -1;
 //       if(bContainer2 && !bContainerDst2 && bContainerDst1)
	//        return psortdata->bSortAsc == 0 ? -1 : 1;
 //       if(bContainer1 && !bContainer2)
	//        return psortdata->bSortAsc == 0 ? 1 : -1;
 //       if(!bContainer1 && bContainer2)
	//        return psortdata->bSortAsc == 0 ? -1 : 1;

	//    return pItem1->pObject->GetCategory().CmpNoCase(pItem2->pObject->GetCategory()) * (psortdata->bSortAsc == 0 ? -1 : 1);
 //   }
 //   return 0;
}

wxGISToolExecuteView::wxGISToolExecuteView(void)
{
}

wxGISToolExecuteView::wxGISToolExecuteView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
wxListCtrl(parent, id, pos, size, style), m_bSortAsc(true), m_pConnectionPointCatalog(NULL), m_ConnectionPointCatalogCookie(-1), m_pParentGxObject(NULL), m_currentSortCol(0), m_pSelection(NULL)
{
    Create(parent, id, pos, size, style);
}

wxGISToolExecuteView::~wxGISToolExecuteView(void)
{
	ResetContents();
}

bool wxGISToolExecuteView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_bSortAsc = true;
    m_pConnectionPointCatalog = NULL;
    m_ConnectionPointCatalogCookie = -1;
    m_pParentGxObject = NULL;
    m_currentSortCol = 0;
    m_pSelection = NULL;

    wxListCtrl::Create(parent, LISTCTRLID, pos, size, wxLC_REPORT | wxBORDER_NONE | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE);
    
    InsertColumn(0, _("Name"),	wxLIST_FORMAT_LEFT, 150);
	InsertColumn(1, _("Type"),  wxLIST_FORMAT_LEFT, 250);

	m_ImageListSmall.Create(16, 16);
	m_ImageListLarge.Create(48, 48);

	//set default icons
	//col ico & default
    m_ImageListLarge.Add(wxBitmap(48, 48));
    m_ImageListLarge.Add(wxBitmap(48, 48));

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg); 
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg); 

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));

	SetImageList(&m_ImageListLarge, wxIMAGE_LIST_NORMAL);
	SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

    return true;
}

bool wxGISToolExecuteView::Activate(IGxApplication* application, wxXmlNode* pConf)
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

void wxGISToolExecuteView::Deactivate(void)
{
	if(m_ConnectionPointCatalogCookie != -1)
		m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}


void wxGISToolExecuteView::Serialize(wxXmlNode* pRootNode, bool bStore)
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
		LISTSTYLE style = (LISTSTYLE)wxAtoi(pRootNode->GetPropVal(wxT("style"), wxT("0")));
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

void wxGISToolExecuteView::AddObject(IGxObject* pObject)
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

            pos = m_ImageListLarge.Add(icon_large);
            ICONDATA myicondata1 = {icon_large, pos, true};
            m_IconsArray.push_back(myicondata1);
        }
    }
	else
		pos = 2;//m_ImageListSmall.Add(m_ImageListSmall.GetIcon(2));//0 col img, 1 - col img

	//if(icon_large.IsOk())
 //   {
 //       for(size_t i = 0; i < m_IconsArray.size(); i++)
 //       {
 //           if(!m_IconsArray[i].bLarge)
 //               continue;
 //           if(m_IconsArray[i].oIcon.IsSameAs(icon_large))
 //           {
 //               pos = m_IconsArray[i].iImageIndex;
 //               break;
 //           }
 //       }
 //   }
	//else
	//	pos = 2;//m_ImageListLarge.Add(m_ImageListLarge.GetIcon(2));


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

void wxGISToolExecuteView::OnColClick(wxListEvent& event)
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

void wxGISToolExecuteView::OnContextMenu(wxContextMenuEvent& event)
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

void wxGISToolExecuteView::OnSelected(wxListEvent& event)
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

bool wxGISToolExecuteView::Show(bool show)
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


void wxGISToolExecuteView::OnDeselected(wxListEvent& event)
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

void wxGISToolExecuteView::ShowContextMenu(const wxPoint& pos)
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

void wxGISToolExecuteView::SetColumnImage(int col, int image)
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

void wxGISToolExecuteView::OnActivated(wxListEvent& event)
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

void wxGISToolExecuteView::OnObjectAdded(IGxObject* pObj)
{
	if(pObj->GetParent() == m_pParentGxObject)
    {
		AddObject(pObj);
        SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	    SortItems(MyCompareFunction, (long)&sortdata);
	    //SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
    }
}

void wxGISToolExecuteView::OnObjectDeleted(IGxObject* pObj)
{
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
		return;
	}
}

void wxGISToolExecuteView::OnObjectChanged(IGxObject* pObj)
{
//	if(pObj == m_pParentObject)
//	{
//		IweObjectContainer* pweObjectContainer = dynamic_cast<IweObjectContainer*>(pObj);
//		if(pweObjectContainer != NULL)
//			if(GetItemCount() > 0 && !pweObjectContainer->HasChildren())
//				DeleteAllItems();
//	}
//	for(long i = 0; i < GetItemCount(); i++)
//	{
//		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
//		if(pItemData == NULL)
//			continue;
//		if(pItemData->pObject != pObj)
//			continue;
//////////////////////////////////////////////////////////////////////
//		IweObjectUI* pObjUI =  dynamic_cast<IweObjectUI*>(pObj);
//		wxIcon icon_small, icon_large;
//		if(pObjUI != NULL)
//		{
//			icon_small = pObjUI->GetSmallImage();
//			icon_large = pObjUI->GetLargeImage();
//		}
//
//		if(icon_small.IsOk())
//			m_pSmallImgList.Replace(pItemData->image_index, icon_small);
//		if(icon_large.IsOk())
//			m_pLargeImgList.Replace(pItemData->image_index, icon_large);
//
//		wxString name = pObj->GetName();
//		wxString type = pObj->GetCategory();
//
//		SetItem(i, 0, name);
//		SetItem(i, 1, type);
//
//		Refresh();
//
//////////////////////////////////////////////////////////////////////
//		return;
//	}
}

void wxGISToolExecuteView::OnObjectRefreshed(IGxObject* pObj)
{
    if(m_pParentGxObject == pObj)
        OnRefreshAll();
}

void wxGISToolExecuteView::OnRefreshAll(void)
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
}

void wxGISToolExecuteView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
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

bool wxGISToolExecuteView::Applies(IGxSelection* Selection)
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

void wxGISToolExecuteView::ResetContents(void)
{
	for(long i = 0; i < GetItemCount(); i++)
		delete (LPITEMDATA)GetItemData(i);
	DeleteAllItems();
}

void wxGISToolExecuteView::SelectAll(void)
{
	for(long item = 0; item < GetItemCount(); item++)
        SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
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

//////////////////////////////////////////////////////////////////////////////
// wxGxToolExecuteView
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGxToolExecuteView, wxGISToolExecuteView)

wxGxToolExecuteView::wxGxToolExecuteView(void)
{
}

wxGxToolExecuteView::wxGxToolExecuteView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISToolExecuteView(parent, id, pos, size)
{
	m_sViewName = wxString(_("Tool Execute View"));
}

wxGxToolExecuteView::~wxGxToolExecuteView(void)
{
}

bool wxGxToolExecuteView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_sViewName = wxString(_("Tool Execute View"));
    wxGISToolExecuteView::Create(parent, id, pos, size, style, name);
    return true;
}

bool wxGxTableView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);

    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(application->GetCatalog());
	m_pSelection = pGxCatalogUI->GetSelection();
	return true;
}

void wxGxTableView::Deactivate(void)
{
	wxGxView::Deactivate();
}

bool wxGxTableView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); i++)
	{
	    wxGxToolExecute* pGxToolExecute = dynamic_cast<wxGxToolExecute*>( Selection->GetSelectedObjects(i) );
	    if(pGxToolExecute)
		    return true;
    }
	return false;
}

void wxGxTableView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;

	if(m_pSelection->GetCount() == 0)
		return;
	IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();	
	if(m_pParentGxObject == pGxObj)
		return;

	wxGxToolExecute* pGxToolExecute = dynamic_cast<wxGxToolExecute*>(pGxObj);
	if(!pGxToolExecute)
		return;

    //fill data

	m_pParentGxObject = pGxObj;
//	wxWindow::Refresh();
}
*/

