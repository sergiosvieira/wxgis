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
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/framework/framework.h"
#include "../../art/doc_16.xpm"

#include "wx/dnd.h"
#include "wx/dataobj.h"

//////////////////////////////////////////////////////////////////////////////
// wxGxTreeViewBase
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxTreeViewBase, wxTreeCtrl)

BEGIN_EVENT_TABLE(wxGxTreeViewBase, wxTreeCtrl)
    EVT_TREE_ITEM_EXPANDING(TREECTRLID, wxGxTreeViewBase::OnItemExpanding)
    EVT_TREE_ITEM_RIGHT_CLICK(TREECTRLID, wxGxTreeViewBase::OnItemRightClick)
END_EVENT_TABLE()

wxGxTreeViewBase::wxGxTreeViewBase(void) : wxTreeCtrl(), m_pConnectionPointCatalog(NULL), m_pConnectionPointSelection(NULL), m_ConnectionPointCatalogCookie(-1), m_ConnectionPointSelectionCookie(-1)
{
}

wxGxTreeViewBase::wxGxTreeViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxTreeCtrl(parent, id, pos, size, style, wxDefaultValidator, wxT("wxGxTreeViewBase")), m_pConnectionPointCatalog(NULL), m_pConnectionPointSelection(NULL), m_ConnectionPointCatalogCookie(-1), m_ConnectionPointSelectionCookie(-1)
{
	m_TreeImageList.Create(16, 16);
	SetImageList(&m_TreeImageList);
    m_TreeImageList.Add(wxIcon(doc_16_xpm));
}

wxGxTreeViewBase::~wxGxTreeViewBase(void)
{
}

bool wxGxTreeViewBase::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    bool result = wxTreeCtrl::Create(parent, id, pos, size, style);
    if(result)
    {
        m_TreeImageList.Create(16, 16);
        SetImageList(&m_TreeImageList);
        m_TreeImageList.Add(wxIcon(doc_16_xpm));
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

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject, pos, false);

	wxTreeItemId wxTreeItemIdRoot = wxTreeCtrl::AddRoot(pGxObject->GetName(), pos, -1, pData);
	m_TreeMap[pGxObject] = wxTreeItemIdRoot;

	wxTreeCtrl::SetItemHasChildren(wxTreeItemIdRoot);
	wxTreeCtrl::Expand(wxTreeItemIdRoot);

	wxTreeCtrl::SortChildren(wxTreeItemIdRoot);
	wxTreeCtrl::Refresh();

	//IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pGxObject);
	//if(pContainer != NULL)
	//	if(pContainer->AreChildrenViewable())
	//		SetItemHasChildren(NewTreeItem);
}

void wxGxTreeViewBase::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;
	IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
	wxIcon icon;
	if(pObjUI != NULL)
		icon = pObjUI->GetSmallImage();

	int pos(-1);
	if(icon.IsOk())
    {
        for(size_t i = 0; i < m_IconsArray.size(); i++)
        {
            if(m_IconsArray[i].oIcon.IsSameAs(icon))
            {
                pos = m_IconsArray[i].iImageIndex;
                break;
            }
        }
        if(pos == -1)
        {
            pos = m_TreeImageList.Add(icon);
            ICONDATA myicondata = {icon, pos};
            m_IconsArray.push_back(myicondata);
        }
    }
	else
		pos = 0;//m_ImageListSmall.Add(m_ImageListSmall.GetIcon(2));//0 col img, 1 - col img

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject, pos, false);

    wxString sName = pGxObject->GetName();
    if(!m_pCatalog->GetShowExt())
    {
        wxFileName FileName(sName);
        FileName.SetEmptyExt();
        sName = FileName.GetName();
    }

	wxTreeItemId NewTreeItem = AppendItem(hParent, sName, pos, -1, pData);
	m_TreeMap[pGxObject] = NewTreeItem;

	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pGxObject);
	if(pContainer != NULL)
		if(pContainer->AreChildrenViewable())
			SetItemHasChildren(NewTreeItem);

    //SortChildren(hParent);
	wxTreeCtrl::Refresh();
}

bool wxGxTreeViewBase::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;

    m_pCatalog = application->GetCatalog();
    AddRoot(dynamic_cast<IGxObject*>(m_pCatalog));

	m_pConnectionPointCatalog = dynamic_cast<IConnectionPointContainer*>( application->GetCatalog() );
	if(m_pConnectionPointCatalog != NULL)
		m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

	m_pSelection = application->GetCatalog()->GetSelection();
	m_pConnectionPointSelection = dynamic_cast<IConnectionPointContainer*>( m_pSelection );
	if(m_pConnectionPointSelection != NULL)
		m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	return true;
};

void wxGxTreeViewBase::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != -1)
		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
	if(m_ConnectionPointCatalogCookie != -1)
		m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	wxGxView::Deactivate();
}

void wxGxTreeViewBase::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;

	IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();
    if(pGxObj == NULL)
        return;

	wxTreeItemId ItemId = m_TreeMap[pGxObj];
	if(ItemId.IsOk())
	{
		wxTreeCtrl::SelectItem(ItemId/*, false*/);
		SetFocus();
	}
	else
	{
		IGxObject* pParentGxObj = pGxObj->GetParent();
		while(pParentGxObj)
		{
			wxTreeItemId ItemId = m_TreeMap[pParentGxObj];
			if(ItemId.IsOk())
			{
				wxTreeCtrl::Expand(ItemId);
				break;
			}
			else
				pParentGxObj = pParentGxObj->GetParent();
		}
		OnSelectionChanged(Selection, nInitiator);
	}
}

void wxGxTreeViewBase::OnObjectAdded(IGxObject* object)
{
	if(object == NULL)
		return;
	IGxObject* pParentObject = object->GetParent();
	wxTreeItemId TreeItemId = m_TreeMap[pParentObject];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(pData != NULL)
		{
			if(pData->m_bExpandedOnce)
            {
				AddTreeItem(object,TreeItemId);
                SortChildren(TreeItemId);
            }
			else
				SetItemHasChildren(TreeItemId, true);
		}
	}
}

void wxGxTreeViewBase::OnObjectChanged(IGxObject* object)
{
	if(object == NULL)
		return;

	wxTreeItemId TreeItemId = m_TreeMap[object];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(pData != NULL)
		{
			IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pData->m_pObject);
			IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pData->m_pObject);
			if(pGxObjectUI != NULL)
			{
				wxString sName = pData->m_pObject->GetName();
                if(!m_pCatalog->GetShowExt())
                {
                    wxFileName FileName(sName);
                    FileName.SetEmptyExt();
                    sName = FileName.GetName();
                }
				wxIcon icon = pGxObjectUI->GetSmallImage();
//TODO: Check if icon of this item is same to other items
				if(icon.IsOk())
					m_TreeImageList.Replace(pData->m_smallimage_index, icon);
				SetItemText(TreeItemId, sName);
				if(pGxObjectContainer != NULL)
				{
					bool bItemHasChildren = pGxObjectContainer->HasChildren();
					if(ItemHasChildren(TreeItemId) && !bItemHasChildren)
						DeleteChildren(TreeItemId);
					SetItemHasChildren(TreeItemId, bItemHasChildren && pGxObjectContainer->AreChildrenViewable());
				}
				wxTreeCtrl::Refresh();
			}
		}
	}
}

void wxGxTreeViewBase::OnObjectDeleted(IGxObject* object)
{
	if(object == NULL)
		return;
	wxTreeItemId TreeItemId = m_TreeMap[object];
	if(TreeItemId.IsOk())
		Delete(TreeItemId);
	m_TreeMap.erase(object);
}

void wxGxTreeViewBase::OnObjectRefreshed(IGxObject* object)
{
	if(object == NULL)
		return;
	wxTreeItemId TreeItemId = m_TreeMap[object];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(pData != NULL)
		{
			if(pData->m_bExpandedOnce)
			{
                //store current sel
                Unselect();
				DeleteChildren(TreeItemId);
				pData->m_bExpandedOnce = false;
				Expand(TreeItemId);
                //restore current sel
			}
            else
            {
			    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(object);
			    if(pGxObjectContainer && pGxObjectContainer->HasChildren() && !ItemHasChildren(TreeItemId))
                {
                    SetItemHasChildren(TreeItemId);
                    wxTreeCtrl::Refresh();
                }
            }
		}
	}
}

void wxGxTreeViewBase::OnRefreshAll(void)
{
	DeleteAllItems();
	AddRoot(dynamic_cast<IGxObject*>(m_pApplication->GetCatalog()));
}

int wxGxTreeViewBase::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    wxGxTreeItemData* pData1 = (wxGxTreeItemData*)GetItemData(item1);
    wxGxTreeItemData* pData2 = (wxGxTreeItemData*)GetItemData(item2);
    if(pData1 != NULL && pData1 != NULL)
    {
        return GxObjectCompareFunction(pData1->m_pObject, pData2->m_pObject, 1);
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
        IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pData->m_pObject);
        if(pGxObjectUI)
        {
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            IApplication* pApp = dynamic_cast<IApplication*>(m_pApplication);
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
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pData->m_pObject);
		if(pGxObjectContainer != NULL)
		{
			if(pGxObjectContainer->HasChildren() && pGxObjectContainer->AreChildrenViewable())
			{
				if(pData->m_bExpandedOnce == false)
				{
					GxObjectArray* pArr = pGxObjectContainer->GetChildren();
					if(pArr != NULL)
					{
						if(pArr->size() != 0)
						{
							for(size_t i = 0; i < pArr->size(); i++)
								AddTreeItem(pArr->at(i), item);//false
							pData->m_bExpandedOnce = true;
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
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pData->m_pObject);
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
    }
	else
	{
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
		IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pData->m_pObject);
		if(pObjEdit == NULL)
		{
			event.Veto();
			return;
		}
		pObjEdit->Rename(event.GetLabel());
	}
}

//void wxGxTreeView::OnObjectChanged(IGxObject* pObject)
//{
//	if(pObject == NULL)
//		return;
//
//	wxTreeItemId TreeItemId = m_TreeMap[pObject];
//	if(TreeItemId.IsOk())
//	{
//		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
//		if(pData != NULL)
//		{
//			IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pData->m_pObject);
//			IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pData->m_pObject);
//			if(pGxObjectUI != NULL)
//			{
//				wxString sName = pData->m_pObject->GetName();
//				wxIcon icon = pGxObjectUI->GetSmallImage();
//
//				if(icon.IsOk())
//					m_TreeImageList.Replace(pData->m_smallimage_index, icon);
//				SetItemText(TreeItemId, sName);
//				if(pGxObjectContainer != NULL)
//				{
//					bool bItemHasChildren = pGxObjectContainer->HasChildren();
//					if(ItemHasChildren(TreeItemId) && !bItemHasChildren)
//						DeleteChildren(TreeItemId);
//					SetItemHasChildren(TreeItemId, bItemHasChildren && pGxObjectContainer->AreChildrenViewable());
//				}
//				wxTreeCtrl::Refresh();
//			}
//		}
//	}
//}

void wxGxTreeView::OnSelChanged(wxTreeEvent& event)
{
    //if(!event.GetOldItem())
    //    return;
    //event.Skip();

    wxArrayTreeItemIds treearray;
    size_t count = GetSelections(treearray);
    m_pSelection->Clear(GetId());
    for(size_t i = 0; i < count; i++)
    {
	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
	    if(pData != NULL)
	    {
		    m_pSelection->Select(pData->m_pObject, true, GetId());
	    }
    }

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
    for(size_t i = 0; i < count; i++)
    {
	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
	    if(pData == NULL)
            continue;
        if(!pData->m_pObject)
            continue;
        IGxDataset* pDSet = dynamic_cast<IGxDataset*>(pData->m_pObject);
        if(pDSet)
            my_data.AddFile(pDSet->GetPath());
    }
    wxDropSource dragSource( this );
	dragSource.SetData( my_data );
	wxDragResult result = dragSource.DoDragDrop( TRUE );
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

	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pData->m_pObject);
	if(pGxObjectWizard != NULL)
		if(!pGxObjectWizard->Invoke(this))
			return;

}

