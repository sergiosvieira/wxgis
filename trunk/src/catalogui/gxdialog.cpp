/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDialog class.
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

#include "wxgis/catalogui/gxdialog.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalogui/catalogcmd.h"

//////////////////////////////////////////////////////////////////////////////
// wxTreeViewComboPopup
//////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxTreeViewComboPopup, wxGxTreeViewBase)
    //EVT_LEFT_UP(wxTreeViewComboPopup::OnMouseClick)
    EVT_LEFT_DOWN(wxTreeViewComboPopup::OnMouseClick)
    EVT_MOTION(wxTreeViewComboPopup::OnMouseMove)
    EVT_TREE_ITEM_ACTIVATED(TREECTRLID, wxTreeViewComboPopup::OnDblClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTreeViewComboPopup, wxGxTreeViewBase)

bool wxTreeViewComboPopup::Create(wxWindow* parent)
{
    m_bClicked = false;
    return wxGxTreeViewBase::Create(parent, TREECTRLID, wxBORDER_SIMPLE | wxTR_NO_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE  );//wxPoint(0,0),wxDefaultSize, 
}

void wxTreeViewComboPopup::Init()
{
}

void wxTreeViewComboPopup::OnPopup()
{
   m_bClicked = false;
   GxObjectArray* pGxObjectArray = m_pSelection->GetSelectedObjects();
   if(pGxObjectArray)
       SelectItem(m_TreeMap[pGxObjectArray->at(pGxObjectArray->size() - 1)]);
}

void wxTreeViewComboPopup::OnDismiss()
{
    //ReleaseMouse();
}

void wxTreeViewComboPopup::SetStringValue(const wxString& s)
{
    //int n = wxListView::FindItem(-1,s);
    //if ( n >= 0 && n < wxListView::GetItemCount() )
    //    wxListView::Select(n);
}

// Get list selection as a string
wxString wxTreeViewComboPopup::GetStringValue() const
{
    if( m_bClicked == false )
    {
        GxObjectArray* pGxObjectArray = m_pSelection->GetSelectedObjects();
        if(pGxObjectArray)
            return 	pGxObjectArray->at(pGxObjectArray->size() - 1)->GetName();	
        return wxEmptyString;
    }

    wxTreeItemId ItemId = wxTreeCtrl::GetSelection();
    if(ItemId.IsOk())
        return GetItemText(ItemId);
    return wxEmptyString;
}

void wxTreeViewComboPopup::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();
    int flag = wxTREE_HITTEST_ONITEMINDENT;
    wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    if(ItemId.IsOk())
    {
        SelectItem(ItemId);
    }
    event.Skip();
}

void wxTreeViewComboPopup::OnMouseClick(wxMouseEvent& event)
{
    m_bClicked = true;
    wxTreeItemId ItemId = GetSelection();
	if(ItemId.IsOk())
    {
        if(m_PrewItemId.IsOk())
        {
            CollapseAllChildren(GetItemParent(m_PrewItemId));
        }
        if(ItemId != GetRootItem())
        {
            CollapseAllChildren(ItemId);
        }
        else
        {
            CollapseAll();
            Expand(GetRootItem());
        }

	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	    if(pData != NULL)
	    {
            SelectItem(ItemId);
		    m_pSelection->Select(pData->m_pObject, false, GetId());
            m_PrewItemId = ItemId;
	    }
    }
    Dismiss();

    event.Skip();
}

void wxTreeViewComboPopup::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(nInitiator == GetId())
		return;
	GxObjectArray* pGxObjectArray = m_pSelection->GetSelectedObjects();
	if(pGxObjectArray == NULL || pGxObjectArray->size() == 0)
		return;
    //
    //wxTreeCtrl::CollapseAll();
    //
	IGxObject* pGxObj = pGxObjectArray->at(pGxObjectArray->size() - 1);	
	wxTreeItemId ItemId = m_TreeMap[pGxObj];
	if(ItemId.IsOk())
	{
		SelectItem(ItemId);
	}
	else
	{
		IGxObject* pParentGxObj = pGxObj->GetParent();
		while(pParentGxObj)
		{
			wxTreeItemId ItemId = m_TreeMap[pParentGxObj];
			if(ItemId.IsOk())
			{
				Expand(ItemId);
				break;
			}
			else
				pParentGxObj = pParentGxObj->GetParent();
		}
		OnSelectionChanged(Selection, nInitiator);
	}
    //set combo text
    wxString sText = GetStringValue();
    m_combo->SetText(sText);
}

void wxTreeViewComboPopup::OnDblClick(wxTreeEvent& event)
{
    wxTreeItemId ItemId = wxTreeCtrl::GetSelection();
	if(!ItemId.IsOk())
		return;
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	if(pData != NULL)
	{
		m_pSelection->Select(pData->m_pObject, false, GetId());
	}
    wxComboPopup::Dismiss();
}

void wxTreeViewComboPopup::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent, bool sort)
{
	if(NULL == pGxObject)
		return;
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pGxObject);
	if(NULL == pContainer)
		return;

	IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
	wxIcon icon;
	if(pObjUI != NULL)
		icon = pObjUI->GetSmallImage();
	int pos(-1);
	if(icon.IsOk())
		pos = m_TreeImageList.Add(icon);

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject, pos, false);

	wxTreeItemId NewTreeItem = AppendItem(hParent, pGxObject->GetName(), pos, -1, pData);
	m_TreeMap[pGxObject] = NewTreeItem;

	if(pContainer->AreChildrenViewable())
		SetItemHasChildren(NewTreeItem);

	if(sort)
		SortChildren(hParent);
	wxTreeCtrl::Refresh();
}


//////////////////////////////////////////////////////////////////////////////
// wxGxDialogContentView
//////////////////////////////////////////////////////////////////////////////

wxGxDialogContentView::wxGxDialogContentView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : 
wxGxContentView(parent, id, pos, size, style), m_pConnectionPointSelection(NULL), m_ConnectionPointSelectionCookie(-1)
{
}

wxGxDialogContentView::~wxGxDialogContentView(void)
{
}

bool wxGxDialogContentView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	if(!wxGxContentView::Activate(application, pConf))
        return false;

	m_pConnectionPointSelection = dynamic_cast<IConnectionPointContainer*>( m_pSelection );
	if(m_pConnectionPointSelection != NULL)
		m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	return true;
}

void wxGxDialogContentView::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != -1)
		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
	wxGxContentView::Deactivate();
}

void wxGxDialogContentView::OnActivated(wxListEvent& event)
{
	event.Skip();
	//dbl click
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

	//IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pItemData->pObject);
	//if(pGxObjectWizard != NULL)
	//	if(!pGxObjectWizard->Invoke(this))
	//		return;

	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pItemData->pObject);
	if(pGxObjectContainer != NULL )
	{
		m_pSelection->Select(pItemData->pObject, false, IGxSelection::INIT_ALL);//GetId()
	}
}

//////////////////////////////////////////////////////////////////////////////
// wxGxDialog
//////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxDialog, wxDialog)
	EVT_MENU_RANGE(wxGxDialog::ID_PLUGINCMD, ID_PLUGINCMD + 10, wxGxDialog::OnCommand)
	EVT_UPDATE_UI_RANGE(wxGxDialog::ID_PLUGINCMD, ID_PLUGINCMD + 10, wxGxDialog::OnCommandUI)
END_EVENT_TABLE()

wxGxDialog::wxGxDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), m_pCatalog(NULL)
{
	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    wxGxCatalog* pCatalog = new wxGxCatalog();
	pCatalog->Init();
	m_pCatalog = static_cast<IGxCatalog*>(pCatalog);

	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	bHeaderSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Look in:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText1->Wrap( -1 );
	bHeaderSizer->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_TreeCombo = new wxComboCtrl( this, wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, wxCB_READONLY); 
    m_PopupCtrl = new wxTreeViewComboPopup();
    m_TreeCombo->UseAltPopupWindow(true);
    m_TreeCombo->SetPopupControl(m_PopupCtrl);
    m_TreeCombo->EnablePopupAnimation(true);
    m_PopupCtrl->Activate(this, NULL);//!!!!

	bHeaderSizer->Add( m_TreeCombo, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

	m_toolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolBar->SetToolBitmapSize( wxSize( 16,16 ) );

//	0	Up One Level
//	1	Connect Folder
//	2	Disconnect Folder
//	3	Location
//  4   Delete Item
//  5   Back
//  6   Forward
//  7   Create Folder

    int IDs[8] = {5,0,-1,1,2,-1,4,7};
    for(size_t i = 0; i < 8; i++)
    {
        if(IDs[i] == -1)
        {
            m_toolBar->AddSeparator();
            continue;
        }
        wxGISCatalogMainCmd* pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
        pwxGISCatalogMainCmd->OnCreate(static_cast<IApplication*>(this));
        pwxGISCatalogMainCmd->SetID(ID_PLUGINCMD + i + 1);
        pwxGISCatalogMainCmd->SetSubType(IDs[i]);//1,2
        m_CommandArray.push_back(pwxGISCatalogMainCmd);
        wxGISEnumCommandKind kind;
        if(pwxGISCatalogMainCmd->GetKind() == enumGISCommandDropDown)
            kind = enumGISCommandNormal;
        else
            kind = pwxGISCatalogMainCmd->GetKind();
        m_toolBar->AddTool( pwxGISCatalogMainCmd->GetID(), pwxGISCatalogMainCmd->GetCaption(), pwxGISCatalogMainCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogMainCmd->GetTooltip(), pwxGISCatalogMainCmd->GetMessage() );
    }	
	m_toolBar->Realize();
	
	bHeaderSizer->Add( m_toolBar, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("       "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText2->Wrap( -1 );
	bHeaderSizer->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bMainSizer->Add( bHeaderSizer, 0, wxEXPAND, 5 );
	
   	m_pwxGxContentView = new wxGxDialogContentView(this);
    m_pwxGxContentView->Activate(this, NULL);//!!!!

	bMainSizer->Add( m_pwxGxContentView, 1, wxALL|wxEXPAND, 5 );
	
	fgCeilSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgCeilSizer->AddGrowableCol( 1 );
	fgCeilSizer->SetFlexibleDirection( wxBOTH );
	fgCeilSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText4->Wrap( -1 );
	fgCeilSizer->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_NameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL );
	fgCeilSizer->Add( m_NameTextCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_OkButton = new wxButton( this, wxID_OK , _("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OkButton->SetDefault(); 
	fgCeilSizer->Add( m_OkButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Show of type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText6->Wrap( -1 );
	fgCeilSizer->Add( m_staticText6, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_WildcardCombo = new wxComboBox( this, wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT ); 
	fgCeilSizer->Add( m_WildcardCombo, 0, wxALL|wxEXPAND, 5 );
	
	m_CancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgCeilSizer->Add( m_CancelButton, 0, wxALL, 5 );
	
	bMainSizer->Add( fgCeilSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();

    m_pConfig = new wxGISAppConfig(DLG_NAME, CONFIG_DIR);
    wxXmlNode* pLastLocationNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("lastpath")));
	IGxObject* pObj = dynamic_cast<IGxObject*>(m_pCatalog);
	wxString sLastPath;
	if(pLastLocationNode)
		sLastPath = pLastLocationNode->GetPropVal(wxT("path"), pObj->GetName());
	else
		sLastPath = pObj->GetName();
	m_pCatalog->SetLocation(sLastPath);
}

wxGxDialog::~wxGxDialog()
{
    //move to OnOk
 //   wxTreeItemId ItemId = m_PopupCtrl->GetSelection();
	//if(ItemId.IsOk())
 //   {
	//    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	//    if(pData != NULL && pData->m_pObject)
 //           {
	//		    wxString sLastPath = pData->m_pObject->GetFullName();
	//		    wxXmlNode* pLastLocationNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("lastpath")));
	//		    if(pLastLocationNode)
	//		    {
	//			    if(pLastLocationNode->HasProp(wxT("path")))
	//				    pLastLocationNode->DeleteProperty(wxT("path"));
	//		    }
	//		    else
	//		    {
	//			    pLastLocationNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("lastpath")), true);
	//		    }
	//		    pLastLocationNode->AddProperty(wxT("path"), sLastPath);
 //           }
	//}

	if(m_pwxGxContentView)
		m_pwxGxContentView->Deactivate();
	if(m_PopupCtrl)
		m_PopupCtrl->Deactivate();
    
    //SerializeFramePos(true);

	for(size_t i = 0; i < m_CommandArray.size(); i++)
		wxDELETE(m_CommandArray[i]);


    wxDELETE(m_pCatalog);
    wxDELETE(m_pConfig);
}

void wxGxDialog::OnCommand(wxCommandEvent& event)
{
	OnCommand(GetCommand(event.GetId()));
}

void wxGxDialog::OnCommand(ICommand* pCmd)
{
	pCmd->OnClick();
}

void wxGxDialog::OnCommandUI(wxUpdateUIEvent& event)
{
	ICommand* pCmd = GetCommand(event.GetId());
	if(pCmd)
	{
		if(pCmd->GetKind() == enumGISCommandCheck)
			event.Check(pCmd->GetChecked());
		event.Enable(pCmd->GetEnabled());
        m_toolBar->SetToolShortHelp(event.GetId(), pCmd->GetTooltip());
    }
}

ICommand* wxGxDialog::GetCommand(long CmdID)
{
	for(size_t i = 0; i < m_CommandArray.size(); i++)
		if(m_CommandArray[i]->GetID() == CmdID)
			return m_CommandArray[i];
	return NULL;
}

ICommand* wxGxDialog::GetCommand(wxString sCmdName, unsigned char nCmdSubType)
{
	for(size_t i = 0; i < m_CommandArray.size(); i++)
	{
        wxObject* pObj = dynamic_cast<wxObject*>(m_CommandArray[i]);
		wxClassInfo * pInfo = pObj->GetClassInfo();
		wxString sCommandName = pInfo->GetClassName();
		if(sCommandName == sCmdName && m_CommandArray[i]->GetSubType() == nCmdSubType)
			return m_CommandArray[i];								
	}
	return NULL;
}