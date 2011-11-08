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
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/framework/droptarget.h"

#include "../../art/document_16.xpm"

//////////////////////////////////////////////////////////////////////////////
// wxGxTreeViewBase
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxTreeViewBase, wxTreeCtrl)

BEGIN_EVENT_TABLE(wxGxTreeViewBase, wxTreeCtrl)
    EVT_TREE_ITEM_EXPANDING(TREECTRLID, wxGxTreeViewBase::OnItemExpanding)
    EVT_TREE_ITEM_RIGHT_CLICK(TREECTRLID, wxGxTreeViewBase::OnItemRightClick)
    EVT_CHAR(wxGxTreeViewBase::OnChar)
	EVT_GXOBJECT_REFRESHED(wxGxTreeViewBase::OnObjectRefreshed)
	EVT_GXOBJECT_ADDED(wxGxTreeViewBase::OnObjectAdded)
	EVT_GXOBJECT_DELETED(wxGxTreeViewBase::OnObjectDeleted)
	EVT_GXOBJECT_CHANGED(wxGxTreeViewBase::OnObjectChanged)
	EVT_GXSELECTION_CHANGED(wxGxTreeViewBase::OnSelectionChanged)
END_EVENT_TABLE()

wxGxTreeViewBase::wxGxTreeViewBase(void) : wxTreeCtrl(), m_pConnectionPointCatalog(NULL), m_pConnectionPointSelection(NULL), m_ConnectionPointCatalogCookie(-1), m_ConnectionPointSelectionCookie(-1)
{
}

wxGxTreeViewBase::wxGxTreeViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxTreeCtrl(parent, id, pos, size, style, wxDefaultValidator, wxT("wxGxTreeViewBase")), m_pConnectionPointCatalog(NULL), m_pConnectionPointSelection(NULL), m_ConnectionPointCatalogCookie(-1), m_ConnectionPointSelectionCookie(-1), m_pDeleteCmd(NULL)
{
	m_TreeImageList.Create(16, 16);
	SetImageList(&m_TreeImageList);
    m_TreeImageList.Add(wxIcon(document_16_xpm));
}

wxGxTreeViewBase::~wxGxTreeViewBase(void)
{
}

bool wxGxTreeViewBase::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    bool result = wxTreeCtrl::Create(parent, id, pos, size, style);
    if(result)
    {
        m_TreeImageList.Create(16, 16);
        SetImageList(&m_TreeImageList);
        m_TreeImageList.Add(wxIcon(document_16_xpm));
    }
    return result;
}


void wxGxTreeViewBase::AddRoot(IGxObject* pGxObject)
{
	if(NULL == pGxObject)
		return;
	IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
	wxIcon icon;
	if(pObjUI != NULL)
		icon = pObjUI->GetSmallImage();
	int pos(-1);
	if(icon.IsOk())
		pos = m_TreeImageList.Add(icon);

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject->GetID(), pos, false);

	wxTreeItemId wxTreeItemIdRoot = wxTreeCtrl::AddRoot(pGxObject->GetName(), pos, -1, pData);
	m_TreeMap[pGxObject->GetID()] = wxTreeItemIdRoot;

	wxTreeCtrl::SetItemHasChildren(wxTreeItemIdRoot);
	wxTreeCtrl::Expand(wxTreeItemIdRoot);

	wxTreeCtrl::SortChildren(wxTreeItemIdRoot);
	wxTreeCtrl::Refresh();
}

void wxGxTreeViewBase::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;
	IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
	wxIcon icon;
	if(pObjUI != NULL)
		icon = pObjUI->GetSmallImage();

	int pos(wxNOT_FOUND);
	if(icon.IsOk())
    {
        for(size_t i = 0; i < m_IconsArray.size(); ++i)
        {
            if(m_IconsArray[i].oIcon.IsSameAs(icon))
            {
                pos = m_IconsArray[i].iImageIndex;
                break;
            }
        }
        if(pos == wxNOT_FOUND)
        {
            pos = m_TreeImageList.Add(icon);
            ICONDATA myicondata = {icon, pos};
            m_IconsArray.push_back(myicondata);
        }
    }
	else
		pos = 0;//m_ImageListSmall.Add(m_ImageListSmall.GetIcon(2));//0 col img, 1 - col img

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject->GetID(), pos, false);

    wxString sName;
    if(m_pCatalog->GetShowExt())
        sName = pGxObject->GetName();
    else
        sName = pGxObject->GetBaseName();


	wxTreeItemId NewTreeItem = AppendItem(hParent, sName, pos, wxNOT_FOUND, pData);
	m_TreeMap[pGxObject->GetID()] = NewTreeItem;

	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pGxObject);
	if(pContainer != NULL)
		if(pContainer->AreChildrenViewable())
			SetItemHasChildren(NewTreeItem);

    //SortChildren(hParent);
	wxTreeCtrl::Refresh();
}

bool wxGxTreeViewBase::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pGxApplication->GetCatalog());
	//delete
    m_pDeleteCmd = application->GetCommand(wxT("wxGISCatalogMainCmd"), 4);
	//new
	m_pNewMenu = dynamic_cast<wxGISNewMenu*>(application->GetCommandBar(NEWMENUNAME));

    AddRoot(dynamic_cast<IGxObject*>(m_pCatalog));

	m_pConnectionPointCatalog = dynamic_cast<wxGISConnectionPointContainer*>( m_pGxApplication->GetCatalog() );
	if(m_pConnectionPointCatalog != NULL)
		m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

	if(m_pCatalog)
	{
		m_pSelection = m_pCatalog->GetSelection();
		m_pConnectionPointSelection = dynamic_cast<wxGISConnectionPointContainer*>( m_pSelection );
		if(m_pConnectionPointSelection != NULL)
			m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);
	}

	return true;
};

void wxGxTreeViewBase::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND)
		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
		m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	wxGxView::Deactivate();
}

void wxGxTreeViewBase::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;

    long nSelID = m_pSelection->GetLastSelectedObjectID();
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nSelID);
    if(pGxObject == NULL)
        return;

	wxTreeItemId ItemId = m_TreeMap[nSelID];
	if(ItemId.IsOk())
	{
		//granted event fireing
		if(wxTreeCtrl::GetSelection() == ItemId)
			UpdateGxSelection();
		else
			wxTreeCtrl::SelectItem(ItemId, true);
		SetFocus();
	}
	else
	{
		IGxObject* pParentGxObj = pGxObject->GetParent();
		while(pParentGxObj)
		{
			wxTreeItemId ItemId = m_TreeMap[pParentGxObj->GetID()];
			if(ItemId.IsOk())
			{
                wxTreeCtrl::SetItemHasChildren(ItemId);
				wxTreeCtrl::Expand(ItemId);
				break;
			}
			else
				pParentGxObj = pParentGxObj->GetParent();
		}
		OnSelectionChanged(event);
	}
}

void wxGxTreeViewBase::UpdateGxSelection(void)
{
    wxTreeItemId TreeItemId = GetSelection();
    m_pSelection->Clear(GetId());
    if(TreeItemId.IsOk())
    {
        wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
        if(pData != NULL)
	        m_pSelection->Select(pData->m_nObjectID, true, GetId());
    }
	if(	m_pNewMenu )
		m_pNewMenu->Update(m_pSelection);
}

int wxGxTreeViewBase::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    wxGxTreeItemData* pData1 = (wxGxTreeItemData*)GetItemData(item1);
    wxGxTreeItemData* pData2 = (wxGxTreeItemData*)GetItemData(item2);
    IGxObjectSPtr pGxObject1 = m_pCatalog->GetRegisterObject(pData1->m_nObjectID);
    IGxObjectSPtr pGxObject2 = m_pCatalog->GetRegisterObject(pData2->m_nObjectID);
    if(pGxObject1 != NULL && pGxObject2 != NULL)
    {
        return GxObjectCompareFunction(pGxObject1.get(), pGxObject2.get(), 1);
    }
    return 0;
}

void wxGxTreeViewBase::OnItemRightClick(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if(!item.IsOk())
		return;
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(pData != NULL)
	{
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject.get());
        if(pGxObjectUI)
        {
            wxString psContextMenu = pGxObjectUI->ContextMenu();
			pGxObject.reset();
            IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(m_pGxApplication);
            if(pApp)
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(pApp->GetCommandBar(psContextMenu));
                if(pMenu)
                {
                    PopupMenu(pMenu, event.GetPoint());
                }
            }
        }
    }
}

void wxGxTreeViewBase::OnItemExpanding(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if(!item.IsOk())
		return;

	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(pData != NULL)
	{
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
		if(pGxObjectContainer != NULL)
		{
			wxBusyCursor wait;
			if(pGxObjectContainer->HasChildren() && pGxObjectContainer->AreChildrenViewable())
			{
				if(pData->m_bExpandedOnce == false)
				{
					GxObjectArray* pArr = pGxObjectContainer->GetChildren();
					if(pArr != NULL)
					{
						if(pArr->size() != 0)
						{
							for(size_t i = 0; i < pArr->size(); ++i)
								AddTreeItem(pArr->at(i), item);//false
							pData->m_bExpandedOnce = true;
                            SetItemHasChildren(item, GetChildrenCount(item, false) > 0);
                            SortChildren(item);
							return;
						}
					}
				}
				else
					return;
			}
			else
			{
				SetItemHasChildren(item, false);
				return;
			}
		}
	}
	SetItemHasChildren(item, false);
}


void wxGxTreeViewBase::OnChar(wxKeyEvent& event)
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

void wxGxTreeViewBase::OnObjectRefreshed(wxGxCatalogEvent& event)
{
	wxTreeItemId TreeItemId = m_TreeMap[event.GetObjectID()];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(pData != NULL)
		{
			if(pData->m_bExpandedOnce)
			{
				DeleteChildren(TreeItemId);
				pData->m_bExpandedOnce = false;
				Expand(TreeItemId);
			}
            else
            {
                IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
			    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
				wxBusyCursor wait;
			    if(pGxObjectContainer && pGxObjectContainer->HasChildren() && !ItemHasChildren(TreeItemId))
                {
                    SetItemHasChildren(TreeItemId);
                }
            }
		}
	}
}

void wxGxTreeViewBase::OnObjectDeleted(wxGxCatalogEvent& event)
{
	wxTreeItemId TreeItemId = m_TreeMap[event.GetObjectID()];
	if(TreeItemId.IsOk())
		Delete(TreeItemId);
	m_TreeMap.erase(event.GetObjectID());
}

void wxGxTreeViewBase::OnObjectAdded(wxGxCatalogEvent& event)
{
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
    IGxObject* pParentObject = pGxObject->GetParent();
	wxTreeItemId TreeItemId = m_TreeMap[pParentObject->GetID()];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(pData != NULL)
		{
			if(pData->m_bExpandedOnce)
            {
				AddTreeItem(pGxObject.get(), TreeItemId);
                SortChildren(TreeItemId);
            }
			else
				SetItemHasChildren(TreeItemId, true);
		}
	}
}

void wxGxTreeViewBase::OnObjectChanged(wxGxCatalogEvent& event)
{
	wxTreeItemId TreeItemId = m_TreeMap[event.GetObjectID()];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(pData != NULL)
		{
            IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
			IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject.get());
			IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
			if(pGxObjectUI != NULL)
			{
				wxString sName = pGxObject->GetName();
                if(!m_pCatalog->GetShowExt())
                {
                    wxFileName FileName(sName);
                    FileName.SetEmptyExt();
                    sName = FileName.GetName();
                }
				wxIcon icon = pGxObjectUI->GetSmallImage();

                int pos(wxNOT_FOUND);
				if(icon.IsOk())
                {
                    for(size_t i = 0; i < m_IconsArray.size(); ++i)
                    {
                        if(m_IconsArray[i].oIcon.IsSameAs(icon))
                        {
                            pos = m_IconsArray[i].iImageIndex;
                            break;
                        }
                    }
                    if(pos == wxNOT_FOUND)
                    {
                        pos = m_TreeImageList.Add(icon);
                        ICONDATA myicondata = {icon, pos};
                        m_IconsArray.push_back(myicondata);
                    }
                    SetItemImage(TreeItemId, pos);
					//m_TreeImageList.Replace(pData->m_smallimage_index, icon);
                }
				SetItemText(TreeItemId, sName);
				if(pGxObjectContainer != NULL)
				{
					wxBusyCursor wait;
					bool bItemHasChildren = pGxObjectContainer->HasChildren();
					if(ItemHasChildren(TreeItemId) && !bItemHasChildren)
					{
						//DeleteChildren(TreeItemId);
						CollapseAndReset(TreeItemId);
						pData->m_bExpandedOnce = false;
                        UpdateGxSelection();
					}
					SetItemHasChildren(TreeItemId, bItemHasChildren && pGxObjectContainer->AreChildrenViewable());
				}
				//wxTreeCtrl::Refresh();
			}
		}
	}
}

void wxGxTreeViewBase::RefreshAll(void)
{
	DeleteAllItems();
	AddRoot(dynamic_cast<IGxObject*>(m_pGxApplication->GetCatalog()));
}

//////////////////////////////////////////////////////////////////////////////
// wxGxTreeView
//////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGxTreeView, wxGxTreeViewBase)
    EVT_TREE_BEGIN_LABEL_EDIT(TREECTRLID, wxGxTreeView::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(TREECTRLID, wxGxTreeView::OnEndLabelEdit)
    EVT_TREE_SEL_CHANGED(TREECTRLID, wxGxTreeView::OnSelChanged)
    EVT_TREE_BEGIN_DRAG(TREECTRLID, wxGxTreeView::OnBeginDrag)
    EVT_TREE_ITEM_ACTIVATED(TREECTRLID, wxGxTreeView::OnActivated)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxGxTreeView, wxGxTreeViewBase)

wxGxTreeView::wxGxTreeView(void) : wxGxTreeViewBase()
{
}

wxGxTreeView::wxGxTreeView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeViewBase(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
    SetDropTarget(new wxGISDropTarget(static_cast<IViewDropTarget*>(this)));
}

wxGxTreeView::~wxGxTreeView(void)
{
}

void wxGxTreeView::OnBeginLabelEdit(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(pData == NULL)
	{
		event.Veto();
		return;
	}
     
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pGxObject.get());
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

void wxGxTreeView::OnEndLabelEdit(wxTreeEvent& event)
{
    if ( event.GetLabel().IsEmpty() )
    {
        //wxMessageDialog(this, _("Label is too short. Please make it longer!"), _("Warning"), wxOK | wxICON_EXCLAMATION);
        event.Veto();
		return;
    }

	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
	{
		event.Veto();
		return;
	}
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(event.GetItem());
	if(pData == NULL)
	{
		event.Veto();
		return;
	}
    
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pGxObject.get());
	if(pObjEdit == NULL)
	{
		event.Veto();
		return;
	}

	if(!pObjEdit->Rename(event.GetLabel()))
	{
		event.Veto();
		wxMessageBox(_("Rename error!"), _("Error"), wxICON_ERROR | wxOK );
		return;
	}

	m_pCatalog->ObjectChanged(pGxObject->GetID());
}

void wxGxTreeView::OnSelChanged(wxTreeEvent& event)
{
    //if(!event.GetOldItem())
    //    return;
    event.Skip();

    UpdateGxSelection();

	//wxTreeItemId item = event.GetItem();
	//if(!item.IsOk())
	//	return;
	//wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	//if(pData != NULL)
	//{
	//	//wxKeyEvent kevt = event.GetKeyEvent();
	//	m_pSelection->Select(pData->m_pObject, false/*kevt.m_controlDown*/, GetId());
	//}

	//wxArrayTreeItemIds treearray;
 //   size_t count = GetSelections(treearray);
	//if(!item.IsOk() || count == 0)
	//{
	//	m_pParent->SetNewMenu(wxT(""));
	//	weViewData* pEvtData = NULL;
	//	pEvtData = new weViewData((wxWindow*)NULL);
	//	wxCommandEvent notifevtview(wxEVT_ONVIEW, m_pParent->GetView()->GetId());
	//	notifevtview.SetEventObject(pEvtData);
	//	wxPostEvent(m_pParent->GetView(), notifevtview);

	//	return;
	//}
	//UpdateSelection();
	//if(pData != NULL)
	//	OnObjectSelected(pData->m_pObject);
}

void wxGxTreeView::OnItemRightClick(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    SelectItem(item);
    wxGxTreeViewBase::OnItemRightClick(event);
}

void wxGxTreeView::OnBeginDrag(wxTreeEvent& event)
{
    //event.Skip();
	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    SelectItem(item);

    wxFileDataObject my_data;

    wxArrayTreeItemIds treearray;
    size_t count = GetSelections(treearray);
    if(count == 0)
        return;
    IGxObject* pParentGxObject(NULL);
    for(size_t i = 0; i < count; ++i)
    {
	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
	    if(pData == NULL)
            continue;

        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        pParentGxObject = pGxObject->GetParent();
        wxString sSystemPath(pGxObject->GetInternalName(), wxConvUTF8);
		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject.get());
		if(pGxObjectUI)
		{
			wxDataFormat frm = pGxObjectUI->GetDataFormat();
			if(frm.GetType() != wxDF_INVALID)
				my_data.SetFormat(frm);
		}
        my_data.AddFile(sSystemPath);
    }
    wxDropSource dragSource( this );
	dragSource.SetData( my_data );
	wxDragResult result = dragSource.DoDragDrop( TRUE );  
    if(result == wxDragMove)
        if(pParentGxObject)
            pParentGxObject->Refresh();
}

void wxGxTreeView::OnActivated(wxTreeEvent& event)
{
    event.Skip();

	wxTreeItemId item = event.GetItem();

	if(!item.IsOk())
		return;

	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(pData == NULL)
		return;

    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pGxObject.get());
	if(pGxObjectWizard != NULL)
		if(!pGxObjectWizard->Invoke(this))
			return;

}

void wxGxTreeView::BeginRename(long nObjectID)
{
	wxTreeItemId ItemId = m_TreeMap[nObjectID];
	if(!ItemId.IsOk())
    {
        wxTreeCtrl::SetItemHasChildren(GetSelection());
        wxTreeCtrl::Expand(GetSelection());  
        ItemId = m_TreeMap[nObjectID];
    }
	if(ItemId.IsOk())
	{
		wxTreeCtrl::SelectItem(ItemId/*, false*/);
		SetFocus();
        m_pSelection->Select(nObjectID, true, GetId());
	}
    EditLabel(GetSelection());
}

wxDragResult wxGxTreeView::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    if(m_HighLightItemId.IsOk())
        SetItemDropHighlight(m_HighLightItemId, false);
    wxPoint pt(x, y);
    int flag = wxTREE_HITTEST_ONITEMINDENT;
    wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    if(ItemId.IsOk())
    {
        wxSize sz = GetClientSize();
        if(DNDSCROLL > y)//scroll up
            ScrollLines(-1);
        else if((sz.GetHeight() - DNDSCROLL) < y)//scroll down
            ScrollLines(1);

        SetItemDropHighlight(ItemId);
        m_HighLightItemId = ItemId;

	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	    if(pData == NULL)
		    return wxDragNone;
        //check drop capability
        
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pGxObject.get());
        if(pTarget == NULL)
		    return wxDragNone;
        return pTarget->CanDrop(def);
    }
    return def;
}

bool wxGxTreeView::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    bool bMove = !wxGetKeyState(WXK_CONTROL);

    wxPoint pt(x, y);
    int flag = wxTREE_HITTEST_ONITEMINDENT;
    wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    if(ItemId.IsOk())
    {
        SetItemDropHighlight(ItemId, false);

	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	    if(pData == NULL)
		    return wxDragNone;
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pGxObject.get());
        if(pTarget == NULL)
		    return false;
        return pTarget->Drop(filenames, bMove);
    }
    return false;
}

void wxGxTreeView::OnLeave()
{
    if(m_HighLightItemId.IsOk())
        SetItemDropHighlight(m_HighLightItemId, false);
}

