/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISToolExecuteView class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
#include "wxgis/geoprocessingui/gptoolbox.h"
#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/catalogui/gxcatdroptarget.h"

#include "wx/tokenzr.h"

#include "../../art/small_arrow.xpm"
#include "../../art/state.xpm"


//////////////////////////////////////////////////////////////////////////////
// wxGxToolExecuteView
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGxToolExecuteView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGxToolExecuteView, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnBeginDrag)
    EVT_LIST_ITEM_SELECTED(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnActivated)
    EVT_LIST_COL_CLICK(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnColClick)
    EVT_CONTEXT_MENU(wxGxToolExecuteView::OnContextMenu)
    EVT_CHAR(wxGxToolExecuteView::OnChar)
END_EVENT_TABLE()

int wxCALLBACK TasksCompareFunction(long item1, long item2, long sortData)
{
	IGxObject* pObject1 = (IGxObject*)item1;
 	IGxObject* pObject2 = (IGxObject*)item2;

    IGxTask* pTask1 = dynamic_cast<IGxTask*>(pObject1);
    IGxTask* pTask2 = dynamic_cast<IGxTask*>(pObject2);
    if(!pTask1)
        return -1;
    if(!pTask2)
        return 1;

    int nRes = 0;
    LPSORTTASKDATA psortdata = (LPSORTTASKDATA)sortData;
    switch(psortdata->currentSortCol)
    {
    case 0:
    case 4:
        nRes = pObject1->GetName().CmpNoCase(pObject2->GetName()); 
        break;
    case 1:
        nRes = (pTask1->GetStart() > pTask2->GetStart()) == true ? 1 : -1;
        break;
    case 2:
        nRes = (pTask1->GetFinish() > pTask2->GetFinish()) == true ? 1 : -1;
        break;
    case 3:
        nRes = (pTask1->GetDonePercent() > pTask2->GetDonePercent()) == true ? 1 : -1;
        break;
    };
   return nRes * (psortdata->bSortAsc == 0 ? -1 : 1);
}

wxGxToolExecuteView::wxGxToolExecuteView(void)
{
}

wxGxToolExecuteView::wxGxToolExecuteView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
wxListCtrl(parent, id, pos, size, style), m_bSortAsc(true), m_pConnectionPointCatalog(NULL), m_ConnectionPointCatalogCookie(-1), m_pParentGxObject(NULL), m_currentSortCol(0), m_pSelection(NULL), m_bHideDone(false)
{
    Create(parent, id, pos, size, style);
}

wxGxToolExecuteView::~wxGxToolExecuteView(void)
{
	ResetContents();
}

bool wxGxToolExecuteView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    SetDropTarget(new wxGISCatalogDropTarget(static_cast<IGxViewDropTarget*>(this)));
    m_HighLightItem = wxNOT_FOUND;
    m_bSortAsc = true;
    m_bHideDone = false;
    m_pConnectionPointCatalog = NULL;
    m_ConnectionPointCatalogCookie = -1;
    m_pParentGxObject = NULL;
    m_currentSortCol = 0;
    m_pSelection = NULL;
    
    m_sViewName = wxString(_("Tool Execute View"));


    wxListCtrl::Create(parent, TOOLEXECUTECTRLID, pos, size, TOOLEXECVIEWSTYLE);
    
	InsertColumn(0, _("Priority"), wxLIST_FORMAT_LEFT, 60);      //priority
    InsertColumn(1, _("Tool name"), wxLIST_FORMAT_LEFT, 150);    //toolname
    InsertColumn(2, _("Start"), wxLIST_FORMAT_LEFT, 100);        //begin
    InsertColumn(3, _("Finish"), wxLIST_FORMAT_LEFT, 100);       //end
    InsertColumn(4, _("Done %"), wxLIST_FORMAT_CENTER, 50);      //percent
    InsertColumn(5, _("Last message"), wxLIST_FORMAT_LEFT, 350); //current message

	m_ImageList.Create(16, 16);

	//set default icons
	//col ico & default
    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg); 
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg); 

	m_ImageList.Add(wxBitmap(SmallDown));
	m_ImageList.Add(wxBitmap(SmallUp));
    m_ImageList.Add(wxBitmap(state_xpm));

	SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
    return true;
}

bool wxGxToolExecuteView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);
	Serialize(m_pXmlConf, false);

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(application->GetCatalog());

    IApplication* pApp = dynamic_cast<IApplication*>(application);
    if(pApp)
    {
		//delete
        m_pDeleteCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 4);
    }

	m_pConnectionPointCatalog = dynamic_cast<IConnectionPointContainer*>( m_pCatalog );
	if(m_pConnectionPointCatalog != NULL)
		m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

	m_pSelection = m_pCatalog->GetSelection();

	return true;
}

void wxGxToolExecuteView::Deactivate(void)
{
	if(m_ConnectionPointCatalogCookie != -1)
		m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

void wxGxToolExecuteView::Serialize(wxXmlNode* pRootNode, bool bStore)
{
	if(pRootNode == NULL)
		return;

	if(bStore)
	{
        if(pRootNode->HasProp(wxT("sort")))
            pRootNode->DeleteProperty(wxT("sort"));
        pRootNode->AddProperty(wxT("sort"), wxString::Format(wxT("%d"), m_bSortAsc));
        if(pRootNode->HasProp(wxT("sort_col")))
            pRootNode->DeleteProperty(wxT("sort_col"));
        pRootNode->AddProperty(wxT("sort_col"), wxString::Format(wxT("%d"), m_currentSortCol));

        //store col width
        wxString sCols;
        for(size_t i = 0; i < GetColumnCount(); i++)
        {
            sCols += wxString::Format(wxT("%d"), GetColumnWidth(i));
            sCols += wxT("|");
        }
        if(pRootNode->HasProp(wxT("cols_width")))
            pRootNode->DeleteProperty(wxT("cols_width"));
        pRootNode->AddProperty(wxT("cols_width"), sCols);
	}
	else
	{
		m_bSortAsc = wxAtoi(pRootNode->GetPropVal(wxT("sort"), wxT("1")));
		m_currentSortCol = wxAtoi(pRootNode->GetPropVal(wxT("sort_col"), wxT("0")));
        //load col width
        wxString sCol = pRootNode->GetPropVal(wxT("cols_width"), wxT(""));
	    wxStringTokenizer tkz(sCol, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
        int col_counter(0);
	    while ( tkz.HasMoreTokens() )
	    {
            if(col_counter >= GetColumnCount())
                break;
		    wxString token = tkz.GetNextToken();
		    //token.Replace(wxT("|"), wxT(""));	
		    int nWidth = wxAtoi(token);
            SetColumnWidth(col_counter, nWidth); 
            col_counter++;
	    }

        SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	    SortItems(TasksCompareFunction, (long)&sortdata);
	    SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
	}
}

void wxGxToolExecuteView::AddObject(IGxObject* pObject)
{
	if(pObject == NULL)
		return;
	wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pObject);
	if(pGxTask != NULL)
	{
#ifdef __WXGTK__
        if(GetColumnCount() < 6)
        {
		InsertColumn(0, _("Tool name"), wxLIST_FORMAT_LEFT, 150);    //priority
        InsertColumn(1, _("Tool name"), wxLIST_FORMAT_LEFT, 150);    //toolname
        InsertColumn(2, _("Start"), wxLIST_FORMAT_LEFT, 100);        //begin
        InsertColumn(3, _("Finish"), wxLIST_FORMAT_LEFT, 100);       //end
        InsertColumn(4, _("Done %"), wxLIST_FORMAT_CENTER, 50);      //percent
        InsertColumn(5, _("Last message"), wxLIST_FORMAT_LEFT, 350); //current message
        }
#endif
        // enumGISTaskWork = 1,    enumGISTaskDone = 2,    enumGISTaskQuered = 3,     enumGISTaskPaused = 4,     enumGISTaskError = 5
        //4 - error, 5 - warning, 6 - work, 7 - queued, 8 - done, 9 - paused, 10 - deleted
        char nIcon = -1;
        wxColor color;
        switch(pGxTask->GetState())
        {
        case enumGISTaskWork:
            color = wxColor(RGB(230,255,230));
            nIcon = 6;
            break;
        case enumGISTaskDone:
            if(m_bHideDone)
                return;
            color = wxColor(RGB(230,230,255));
            nIcon = 8;
            break;
        case enumGISTaskQuered:
            color = wxColor(RGB(255,230,255));
            nIcon = 7;
            break;
        case enumGISTaskPaused:
            color = wxColor(RGB(255,255,230));
            nIcon = 9;
            break;
        case enumGISTaskError:
            color = wxColor(RGB(255,230,230));
            nIcon = 4;
            break;
        };

		long ListItemID = InsertItem(0, wxString::Format(wxT("%d"), pGxTask->GetPriority() + 1), nIcon);
	    SetItem(ListItemID, 1, pObject->GetName());
        wxDateTime dts = pGxTask->GetStart();
        if(dts.IsValid())
            SetItem(ListItemID, 2, dts.Format(_("%d-%m-%Y %H:%M:%S")));
        wxDateTime dtf = pGxTask->GetFinish();
        if(dtf.IsValid())
            SetItem(ListItemID, 3, pGxTask->GetFinish().Format(_("%d-%m-%Y %H:%M:%S")));
        SetItem(ListItemID, 4, wxString::Format(_("%.1f%%"), pGxTask->GetDonePercent()));
        SetItem(ListItemID, 5, pGxTask->GetLastMessage());

        SetItemBackgroundColour(ListItemID, color);
        SetItemData(ListItemID, (long)pObject);

    	wxListCtrl::Refresh();
	}
}

void wxGxToolExecuteView::OnColClick(wxListEvent& event)
{
    m_currentSortCol = event.GetColumn();
	m_bSortAsc = !m_bSortAsc;

    SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(TasksCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

void wxGxToolExecuteView::OnContextMenu(wxContextMenuEvent& event)
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

void wxGxToolExecuteView::OnSelected(wxListEvent& event)
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
        IGxObject* pObject = (IGxObject*)GetItemData(nItem);
	    if(pObject == NULL)
            continue;
		nCount++;
        m_pSelection->Select(pObject, true, NOTFIRESELID);
    }
}

bool wxGxToolExecuteView::Show(bool show)
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


void wxGxToolExecuteView::OnDeselected(wxListEvent& event)
{
	//event.Skip();
    if(GetSelectedItemCount() == 0)
        m_pSelection->Select(m_pParentGxObject, false, NOTFIRESELID);

	IGxObject* pObject = (IGxObject*)event.GetData();
	if(pObject == NULL)
		return;

	m_pSelection->Unselect(pObject, NOTFIRESELID);
}

void wxGxToolExecuteView::ShowContextMenu(const wxPoint& pos)
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

	IGxObject* pObject = (IGxObject*)GetItemData(item);
	if(pObject != NULL)
	{
        bool bAdd = true;
        m_pSelection->Select(pObject, bAdd, NOTFIRESELID);

		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pObject);
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

void wxGxToolExecuteView::SetColumnImage(int col, int image)
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

void wxGxToolExecuteView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	IGxObject* pObject = (IGxObject*)event.GetData();
	if(pObject == NULL)
		return;

	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pObject);
	if(pGxObjectWizard != NULL)
		pGxObjectWizard->Invoke(this);
}

void wxGxToolExecuteView::OnObjectAdded(IGxObject* pObj)
{
	if(pObj->GetParent() == m_pParentGxObject)
    {
		AddObject(pObj);
        SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	    SortItems(TasksCompareFunction, (long)&sortdata);
    }
}

void wxGxToolExecuteView::OnObjectDeleted(IGxObject* pObj)
{
	for(long i = 0; i < GetItemCount(); i++)
	{
		IGxObject* pObject = (IGxObject*)GetItemData(i);
		if(pObject == NULL)
			continue;
		if(pObject != pObj)
			continue;
        SetItemData(i, NULL);
		//delete pItemData;
		DeleteItem(i);
		//Refresh();
		return;
	}
}

void wxGxToolExecuteView::OnObjectChanged(IGxObject* pObj)
{
    if(!pObj)
        return;

    //find item
    int nItem = wxNOT_FOUND;
    for(size_t i = 0; i < GetItemCount(); i++)
    {
        IGxObject* pObject = (IGxObject*)GetItemData(i);
        if(pObject == pObj)
        {
            nItem = i;
            break;
        }
    }
    if(nItem == wxNOT_FOUND)
        return;

    wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pObj);
    if(pGxTask != NULL)
    {
        // enumGISTaskWork = 1,    enumGISTaskDone = 2,    enumGISTaskQuered = 3,     enumGISTaskPaused = 4,     enumGISTaskError = 5
        //4 - error, 5 - warning, 6 - work, 7 - queued, 8 - done, 9 - paused, 10 - deleted
        char nIcon = -1;
        wxColor color;
        switch(pGxTask->GetState())
        {
        case enumGISTaskWork:
            color = wxColor(RGB(230,255,230));
            nIcon = 6;
            break;
        case enumGISTaskDone:
            color = wxColor(RGB(230,230,255));
            nIcon = 8;
            break;
        case enumGISTaskQuered:
            color = wxColor(RGB(255,230,255));
            nIcon = 7;
            break;
        case enumGISTaskPaused:
            color = wxColor(RGB(255,255,230));
            nIcon = 9;
            break;
        case enumGISTaskError:
            color = wxColor(RGB(255,230,230));
            nIcon = 4;
            break;
        };

		SetItem(nItem, 0, wxString::Format(wxT("%d"), pGxTask->GetPriority() + 1), nIcon);

        SetItem(nItem, 1, pObj->GetName());
        wxDateTime dtb = pGxTask->GetStart();
        if(dtb.IsValid())
            SetItem(nItem, 2, dtb.Format(_("%d-%m-%Y %H:%M:%S")));
        wxDateTime dtf = pGxTask->GetFinish();
        if(dtf.IsValid())
            SetItem(nItem, 3, dtf.Format(_("%d-%m-%Y %H:%M:%S")));
        SetItem(nItem, 4, wxString::Format(_("%.1f%%"), pGxTask->GetDonePercent()));
        SetItem(nItem, 5, pGxTask->GetLastMessage());

        SetItemBackgroundColour(nItem, color);

	    wxListCtrl::Refresh();
    }
}

void wxGxToolExecuteView::OnObjectRefreshed(IGxObject* pObj)
{
    if(m_pParentGxObject == pObj)
        OnRefreshAll();
    if(pObj->GetParent() == m_pParentGxObject)
    {
	    OnObjectChanged(pObj);
    }
}

void wxGxToolExecuteView::OnRefreshAll(void)
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

    SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(TasksCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

void wxGxToolExecuteView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;
	IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();
	//if(m_pParentGxObject == pGxObj)
	//	return;

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

    SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(TasksCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

bool wxGxToolExecuteView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); i++)
	{
		wxGxToolExecute* pGxToolExecute = dynamic_cast<wxGxToolExecute*>( Selection->GetSelectedObjects(i) );
		if(pGxToolExecute != NULL)
			return true;
	}
	return false;
}

void wxGxToolExecuteView::ResetContents(void)
{
	DeleteAllItems();
}

void wxGxToolExecuteView::SelectAll(void)
{
	for(long item = 0; item < GetItemCount(); item++)
        SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxGxToolExecuteView::OnChar(wxKeyEvent& event)
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

void wxGxToolExecuteView::HideDone(bool bHide)
{
    m_bHideDone = bHide;
    OnRefreshAll();
}

void wxGxToolExecuteView::OnBeginDrag(wxListEvent& event)
{
    long nItem = -1;
	wxArrayString saArr;
	FillDataArray(saArr);
	if(saArr.GetCount() > 0)
	{
		wxFileDataObject *pMyData = new wxFileDataObject();
		for(size_t i = 0; i < saArr.GetCount(); i++)
			pMyData->AddFile(saArr[i]);

	    wxDropSource dragSource( this );
		dragSource.SetData( *pMyData );
		wxDragResult result = dragSource.DoDragDrop( TRUE );  
		wxDELETE(pMyData)
	}
}

wxDragResult wxGxToolExecuteView::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{    
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
    wxPoint pt(x, y);
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	{ 
        wxSize sz = GetClientSize();
        if(DNDSCROLL > y)//scroll up
            ScrollLines(-1);
        else if((sz.GetHeight() - DNDSCROLL) < y)//scroll down
            ScrollLines(1);

        SetItemState(m_HighLightItem, wxLIST_STATE_DROPHILITED, wxLIST_STATE_DROPHILITED);
        m_HighLightItem = nItemId;

        return def;
    }
    return wxDragNone;
}

bool wxGxToolExecuteView::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
 //   wxPoint pt(x, y);
	//unsigned long nFlags(0);
	//long nItemId = HitTest(pt, (int &)nFlags);
	//if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	//{ 
 //       SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
 //       m_HighLightItem = nItemId;
 //       SetItemState(m_HighLightItem, wxLIST_STATE_DROPHILITED, wxLIST_STATE_DROPHILITED);

 //       LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(nItemId);
 //       pdata->bChanged = !pdata->bChanged;
 //       pdata->nCheckState = !pdata->nCheckState;
 //       bool bCheck = pdata->nCheckState;
 //       SetItemImage(nItemId, bCheck == true ? 1 : 0, bCheck == true ? 1 : 0);
	//}


    //int flag = wxTREE_HITTEST_ONITEMINDENT;
    //wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    //if(ItemId.IsOk())
    //{
    //    SetItemDropHighlight(ItemId, false);

	   // wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	   // if(pData == NULL)
		  //  return wxDragNone;
    //    IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pData->m_pObject);
    //    if(pTarget == NULL)
		  //  return false;
    //    return pTarget->Drop(filenames);
    //}
    return false;
}

void wxGxToolExecuteView::OnLeave()
{
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
}

void wxGxToolExecuteView::FillDataArray(wxArrayString &saDataArr)
{
	int nItem(-1);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == -1 )
            break;
        IGxObject* pObject = (IGxObject*)GetItemData(nItem);
        wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pObject);
	    if(pGxTask == NULL)
            continue;
        saDataArr.Add(wxString::Format(wxT("TaskID: %d"), pGxTask->GetTaskID()));
    }	
}