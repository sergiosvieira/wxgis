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
#include "wxgis/catalogui/catalogcmd.h"
#include "wxgis/catalogui/viewscmd.h"

#include <wx/valgen.h>

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

wxGxDialogContentView::wxGxDialogContentView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxGxContentView(parent, id, pos, size, style), m_pConnectionPointSelection(NULL), m_ConnectionPointSelectionCookie(-1), m_nFilterIndex(0)
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
		return;
	}

	wxCommandEvent butevent( wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK );
    GetParent()->GetEventHandler()->ProcessEvent( butevent );

}

void wxGxDialogContentView::SetCurrentFilter(size_t nFilterIndex)
{
	m_nFilterIndex = nFilterIndex;
}

void wxGxDialogContentView::AddObject(IGxObject* pObject)
{
	if(m_nFilterIndex != m_pFiltersArray->size())
	{
		if(m_pFiltersArray->at(m_nFilterIndex)->CanDisplayObject(pObject))
			return wxGxContentView::AddObject(pObject);
	}
	else
	{
		for(size_t i = 0; i < m_pFiltersArray->size(); i++)
			if(m_pFiltersArray->at(i)->CanDisplayObject(pObject))
				return wxGxContentView::AddObject(pObject);
	}
}

//////////////////////////////////////////////////////////////////////////////
// wxGxDialog
//////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxDialog, wxDialog)
	EVT_LIST_ITEM_SELECTED(LISTCTRLID, wxGxDialog::OnItemSelected)
	EVT_LIST_ITEM_DESELECTED(LISTCTRLID, wxGxDialog::OnItemSelected)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMD + 10, wxGxDialog::OnCommand)
	EVT_MENU_RANGE(ID_MENUCMD, ID_MENUCMD + 128, wxGxDialog::OnDropDownCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMD + 10, wxGxDialog::OnCommandUI)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, wxGxDialog::OnToolDropDown)
    EVT_COMBOBOX(FILTERCOMBO, wxGxDialog::OnFilerSelect)
    EVT_BUTTON(wxID_OK, wxGxDialog::OnOK)
    EVT_UPDATE_UI(wxID_OK, wxGxDialog::OnOKUI)
END_EVENT_TABLE()

wxGxDialog::wxGxDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), m_pCatalog(NULL), m_pDropDownCommand(NULL), m_bAllowMultiSelect(false), m_bOverwritePrompt(false), m_nDefaultFilter(0), m_pConfig(NULL), m_pObjectArray(NULL), m_pwxGxContentView(NULL), m_PopupCtrl(NULL)
{
	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    m_pCatalog = new wxGxCatalog();
	m_pCatalog->Init();

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

	m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxAUI_TB_HORZ_LAYOUT|wxBORDER_NONE);
	m_toolBar->SetToolBitmapSize( wxSize( 16,16 ) );
	m_toolBar->SetArtProvider(new wxGxToolBarArt());

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
        pwxGISCatalogMainCmd->SetSubType(IDs[i]);
        m_CommandArray.push_back(pwxGISCatalogMainCmd);
        wxGISEnumCommandKind kind = pwxGISCatalogMainCmd->GetKind();
        if(pwxGISCatalogMainCmd->GetKind() == enumGISCommandDropDown)
            kind = enumGISCommandNormal;
		m_toolBar->AddTool( pwxGISCatalogMainCmd->GetID(), pwxGISCatalogMainCmd->GetCaption(), pwxGISCatalogMainCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogMainCmd->GetTooltip(), pwxGISCatalogMainCmd->GetMessage(), NULL );
		if(pwxGISCatalogMainCmd->GetKind() == enumGISCommandDropDown)
			m_toolBar->SetToolDropDown(pwxGISCatalogMainCmd->GetID(), true);
    }

    wxGISCatalogViewsCmd* pwxGISCatalogViewsCmd = new wxGISCatalogViewsCmd();
    pwxGISCatalogViewsCmd->OnCreate(static_cast<IApplication*>(this));
    pwxGISCatalogViewsCmd->SetID(ID_PLUGINCMD + 9);
    pwxGISCatalogViewsCmd->SetSubType(0);
    m_CommandArray.push_back(pwxGISCatalogViewsCmd);
    wxGISEnumCommandKind kind = pwxGISCatalogViewsCmd->GetKind();
    if(pwxGISCatalogViewsCmd->GetKind() == enumGISCommandDropDown)
        kind = enumGISCommandNormal;
	m_toolBar->AddTool( pwxGISCatalogViewsCmd->GetID(), pwxGISCatalogViewsCmd->GetCaption(), pwxGISCatalogViewsCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogViewsCmd->GetTooltip(), pwxGISCatalogViewsCmd->GetMessage(), NULL );
	if(pwxGISCatalogViewsCmd->GetKind() == enumGISCommandDropDown)
		m_toolBar->SetToolDropDown(pwxGISCatalogViewsCmd->GetID(), true);

	m_toolBar->Realize();
	
	bHeaderSizer->Add( m_toolBar, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText2->Wrap( -1 );
	bHeaderSizer->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bMainSizer->Add( bHeaderSizer, 0, wxEXPAND, 5 );	
//
//
	fgCeilSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgCeilSizer->AddGrowableCol( 1 );
	fgCeilSizer->SetFlexibleDirection( wxBOTH );
	fgCeilSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText4->Wrap( -1 );
	fgCeilSizer->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_NameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL, wxGenericValidator(&m_sName) );
	fgCeilSizer->Add( m_NameTextCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_OkButton = new wxButton( this, wxID_OK , _("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OkButton->SetDefault(); 
	fgCeilSizer->Add( m_OkButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Show of type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText6->Wrap( -1 );
	fgCeilSizer->Add( m_staticText6, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_WildcardCombo = new wxComboBox( this, FILTERCOMBO, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY/*|wxCB_SORT*/ ); 
	fgCeilSizer->Add( m_WildcardCombo, 0, wxALL|wxEXPAND, 5 );
	
	m_CancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgCeilSizer->Add( m_CancelButton, 0, wxALL, 5 );
	
	bMainSizer->Add( fgCeilSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();
}

wxGxDialog::~wxGxDialog()
{
	if(m_pwxGxContentView)
		m_pwxGxContentView->Deactivate();
	if(m_PopupCtrl)
		m_PopupCtrl->Deactivate();
    
    SerializeFramePos(true);

	for(size_t i = 0; i < m_CommandArray.size(); i++)
		wxDELETE(m_CommandArray[i]);

	RemoveAllFilters();

    wxDELETE(m_pwxGxContentView);
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

void wxGxDialog::OnDropDownCommand(wxCommandEvent& event)
{
    if(m_pDropDownCommand)
        m_pDropDownCommand->OnDropDownCommand(event.GetId());
}

void wxGxDialog::OnToolDropDown(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked())
    {
        ICommand* pCmd = GetCommand(event.GetToolId());
        m_pDropDownCommand = dynamic_cast<IDropDownCommand*>(pCmd);
        if(m_pDropDownCommand)
        {
            wxMenu* pMenu = m_pDropDownCommand->GetDropDownMenu();
            if(pMenu)
            {
				m_toolBar->PopupMenu(pMenu, event.GetItemRect().GetBottomLeft());
            }
            wxDELETE(pMenu);
        }
    }
}

void wxGxDialog::SetButtonCaption(wxString sOkBtLabel)
{
	m_sOkBtLabel = sOkBtLabel;
}

void wxGxDialog::SetStartingLocation(wxString sStartPath)
{
	m_sStartPath = sStartPath;
}

void wxGxDialog::SetName(wxString sName)
{
	m_sName = sName;
}

void wxGxDialog::SetAllowMultiSelect(bool bAllowMultiSelect)
{
	m_bAllowMultiSelect = bAllowMultiSelect;
}

void wxGxDialog::SetOverwritePrompt(bool bOverwritePrompt)
{
	m_bOverwritePrompt = bOverwritePrompt;
}

int wxGxDialog::ShowModalOpen()
{
    m_bIsSaveDlg = false;
	if(m_OkButton)
	{
		if(m_sOkBtLabel.IsEmpty())
			m_OkButton->SetLabel(_("Open"));
		else
			m_OkButton->SetLabel(m_sOkBtLabel);
	}
	OnInit();

	return wxDialog::ShowModal();
}

int wxGxDialog::ShowModalSave()
{
    m_bIsSaveDlg = true;
	if(m_OkButton)
	{
		if(m_sOkBtLabel.IsEmpty())
			m_OkButton->SetLabel(_("Save"));
		else
			m_OkButton->SetLabel(m_sOkBtLabel);
	}
	m_bAllowMultiSelect = false;
	OnInit();

	return wxDialog::ShowModal();
}

void wxGxDialog::OnInit()
{
    m_pConfig = new wxGISAppConfig(DLG_NAME, CONFIG_DIR);

	long nStyle = wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxBORDER_THEME;
	if(!m_bAllowMultiSelect)
		nStyle |= wxLC_SINGLE_SEL;
   	m_pwxGxContentView = new wxGxDialogContentView(this, LISTCTRLID, wxDefaultPosition, wxDefaultSize, nStyle);
    wxXmlNode* pContentViewConf = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/views/contentsview")));
    if(!pContentViewConf)
        pContentViewConf = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame/views/contentsview")));

    m_pwxGxContentView->Activate(this, pContentViewConf);
    RegisterChildWindow(static_cast<wxWindow*>(m_pwxGxContentView));

	bMainSizer->Insert(1, m_pwxGxContentView, 1, wxALL|wxEXPAND, 5 );

	for(size_t i = 0; i < m_FilterArray.size(); i++)
		m_WildcardCombo->AppendString(m_FilterArray[i]->GetName());
	if(m_FilterArray.size() > 1)
		m_WildcardCombo->AppendString(_("All listed filters"));
	if(m_FilterArray.size() == 0)
		m_WildcardCombo->AppendString(_("All items"));
	m_WildcardCombo->Select(m_nDefaultFilter);

	m_pwxGxContentView->SetCurrentFilter(m_nDefaultFilter);
	m_pwxGxContentView->SetFilters(&m_FilterArray);

	if(m_sStartPath.IsEmpty())
	{
		wxXmlNode* pLastLocationNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("lastpath")));
		IGxObject* pObj = dynamic_cast<IGxObject*>(m_pCatalog);
		wxString sLastPath;
		if(pLastLocationNode)
			sLastPath = pLastLocationNode->GetPropVal(wxT("path"), pObj->GetName());
		else
			sLastPath = pObj->GetName();
		m_pCatalog->SetLocation(sLastPath);
	}
	else
		m_pCatalog->SetLocation(m_sStartPath);

    SerializeFramePos(false);
}

void wxGxDialog::OnItemSelected(wxListEvent& event)
{
	m_sName.Empty();
	long item = -1;
	while(1)
    {
        item = m_pwxGxContentView->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
		if(m_sName.IsEmpty())
			m_sName += m_pwxGxContentView->GetItemText(item);
		else
			m_sName += wxT("; ") + m_pwxGxContentView->GetItemText(item);
    }
	TransferDataToWindow();
}

void wxGxDialog::AddFilter(IGxObjectFilter* pFilter, bool bDefault)
{
	m_FilterArray.push_back(pFilter);
	if(bDefault)
		m_nDefaultFilter = m_FilterArray.size() - 1;
}

void wxGxDialog::RemoveAllFilters(void)
{
	for(size_t i = 0; i < m_FilterArray.size(); i++)
		wxDELETE(m_FilterArray[i]);
}

void wxGxDialog::OnFilerSelect(wxCommandEvent& event)
{
    int nPos = m_WildcardCombo->GetCurrentSelection();
    m_pwxGxContentView->SetCurrentFilter(nPos);
    m_pwxGxContentView->OnRefreshAll();
}

void wxGxDialog::OnOK(wxCommandEvent& event)
{
    m_nRetCode = wxID_OK;
    if ( Validate() && TransferDataFromWindow() )
    {
        int nPos = m_WildcardCombo->GetCurrentSelection();
        //fill out data
        if(m_bIsSaveDlg)
        {
            if(nPos != m_FilterArray.size())
            {
                wxGISEnumSaveObjectResults Result = m_FilterArray[nPos]->CanSaveObject(GetLocation(), GetName());
                if(!DoSaveObject(Result))
                    return;
            }
            else
            {
                for(size_t j = 0; j < m_FilterArray.size(); j++)
                {
                    wxGISEnumSaveObjectResults Result = m_FilterArray[j]->CanSaveObject(GetLocation(), GetName());
                    if(DoSaveObject(Result))
                        break;
                }
            }
        }
        else
        {
            m_pObjectArray = m_pwxGxContentView->GetSelectedObjects();
            if(!m_pObjectArray)
                return;
            for(size_t i = 0; i < m_pObjectArray->size(); i++)
            {
                if(nPos != m_FilterArray.size())
                {
                    if(!m_FilterArray[nPos]->CanChooseObject(m_pObjectArray->at(i)))
                    {
                        IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(m_pObjectArray->at(i));
                        if(pObjCont)
                        {
                            m_pCatalog->GetSelection()->Select(m_pObjectArray->at(i), false, IGxSelection::INIT_ALL);
                            return;
                        }
                        m_pObjectArray->erase(m_pObjectArray->begin() + i);
                        i--;
                    }
                }
                else
                {
                    for(size_t j = 0; j < m_FilterArray.size(); j++)
                    {
                        bool bCanChooseObject = false;
                        if(m_FilterArray[j]->CanChooseObject(m_pObjectArray->at(i)))
                        {
                            bCanChooseObject = true;
                            break;
                        }
                        if(!bCanChooseObject)
                        {
                            IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(m_pObjectArray->at(i));
                            if(pObjCont)
                            {
                                m_pCatalog->GetSelection()->Select(m_pObjectArray->at(i), false, IGxSelection::INIT_ALL);
                                return;
                            }
                            m_pObjectArray->erase(m_pObjectArray->begin() + i);
                            i--;
                        }
                    }
                }
            }
            if(m_pObjectArray->size() == 0)
            {
                wxMessageBox(_("The item(s) cannot be choosen!"), _("Error"), wxICON_ERROR | wxOK, this);
                return;
            }
        }

        wxTreeItemId ItemId = m_PopupCtrl->GetSelection();
        if(ItemId.IsOk())
        {
            wxGxTreeItemData* pData = (wxGxTreeItemData*)m_PopupCtrl->GetItemData(ItemId);
            if(pData != NULL && pData->m_pObject)
            {
                wxString sLastPath = pData->m_pObject->GetFullName();
                wxXmlNode* pLastLocationNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("lastpath")));
                if(pLastLocationNode)
                {
                    if(pLastLocationNode->HasProp(wxT("path")))
                        pLastLocationNode->DeleteProperty(wxT("path"));
                }
                else
                {
                    pLastLocationNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("lastpath")), true);
                }
                pLastLocationNode->AddProperty(wxT("path"), sLastPath);
            }
        }

        if ( IsModal() )
            EndModal(m_nRetCode);
        else
        {
            SetReturnCode(m_nRetCode);
            this->Show(false);
        }
    }
}

void wxGxDialog::SerializeFramePos(bool bSave)
{
    if(!m_pConfig)
        return;
	wxXmlNode* pFrameXmlNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame")));

	if(bSave)
	{
		if(!pFrameXmlNode)
			pFrameXmlNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame")), true);

		if( IsMaximized() )
		{
			if(pFrameXmlNode->HasProp(wxT("maxi")))
				pFrameXmlNode->DeleteProperty(wxT("maxi"));
			pFrameXmlNode->AddProperty(wxT("maxi"), wxT("1"));
		}
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);

			wxXmlProperty* prop = pFrameXmlNode->GetProperties();
			while(prop)
			{
				wxXmlProperty* prev_prop = prop;
				prop = prop->GetNext();
				wxDELETE(prev_prop);
			}
			wxXmlProperty* pHProp = new wxXmlProperty(wxT("Height"), wxString::Format(wxT("%u"), h), NULL);
			wxXmlProperty* pWProp = new wxXmlProperty(wxT("Width"), wxString::Format(wxT("%u"), w), pHProp);
			wxXmlProperty* pYProp = new wxXmlProperty(wxT("YPos"), wxString::Format(wxT("%d"), y), pWProp);
			wxXmlProperty* pXProp = new wxXmlProperty(wxT("XPos"), wxString::Format(wxT("%d"), x), pYProp);
			wxXmlProperty* pMaxi = new wxXmlProperty(wxT("maxi"), wxT("0"), pXProp);

			pFrameXmlNode->SetProperties(pMaxi);
		}
	}
	else
	{
		//load
		if(!pFrameXmlNode)
			pFrameXmlNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame")));

		if(pFrameXmlNode == NULL)
			return;

		bool bMaxi = wxAtoi(pFrameXmlNode->GetPropVal(wxT("maxi"), wxT("0")));
		if(!bMaxi)
		{
			int x = wxAtoi(pFrameXmlNode->GetPropVal(wxT("XPos"), wxT("50")));
			int y = wxAtoi(pFrameXmlNode->GetPropVal(wxT("YPos"), wxT("50")));
			int w = wxAtoi(pFrameXmlNode->GetPropVal(wxT("Width"), wxT("850")));
			int h = wxAtoi(pFrameXmlNode->GetPropVal(wxT("Height"), wxT("530")));
			
			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}

void wxGxDialog::OnOKUI(wxUpdateUIEvent& event)
{
    if(m_NameTextCtrl->GetValue().IsEmpty())
        event.Enable(false);
    else
        event.Enable(true);
}

wxString wxGxDialog::GetName(void)
{
//TODO: extension
    return m_NameTextCtrl->GetValue();
}

wxString wxGxDialog::GetFullPath(void)
{
    IGxObject* pObj = GetLocation();
    if(pObj)
    {
        return pObj->GetFullName() + wxFileName::GetPathSeparator() + GetName();
    }
    return wxEmptyString;
}

IGxObject* wxGxDialog::GetLocation(void)
{
    GxObjectArray* pArr = m_PopupCtrl->GetSelectedObjects();
    if(pArr)
        if(pArr->size() > 0)
            return pArr->at(0);
    return NULL;
}

bool wxGxDialog::DoSaveObject(wxGISEnumSaveObjectResults Result)
{
    if(Result == enumGISSaveObjectDeny)
    {
        wxMessageBox(_("The item cannot be save!"), _("Error"), wxICON_ERROR | wxOK, this);
        return false;
    }
    else if(Result == enumGISSaveObjectExists && m_bOverwritePrompt)
    {
        int nRes = wxMessageBox(_("The item is exist! Overwrite?"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL, this);
        if(nRes == wxID_CANCEL)
            return false;
        else if(nRes == wxID_NO)
            m_nRetCode = wxID_CANCEL;
        else
        {
            IGxObjectEdit* pObjEd = dynamic_cast<IGxObjectEdit*>(m_pCatalog->SearchChild(GetFullPath()));
            //Delete item
            if(!pObjEd)
            {
                wxMessageBox(_("The item cannot be deleted!"), _("Error"), wxICON_ERROR | wxOK, this);
                return false;
            }

            if(!pObjEd->CanDelete())
            {
                wxMessageBox(_("The item cannot be deleted!"), _("Error"), wxICON_ERROR | wxOK, this);
                return false;
            }

            if(!pObjEd->Delete())
            {
                wxMessageBox(_("The item cannot be deleted!"), _("Error"), wxICON_ERROR | wxOK, this);
                return false;
            }
        }
    }
    return true;
}

WINDOWARRAY* wxGxDialog::GetChildWindows(void)
{
	return &m_WindowArray;
}

void wxGxDialog::RegisterChildWindow(wxWindow* pWnd)
{
	for(size_t i = 0; i < m_WindowArray.size(); i++)
		if(m_WindowArray[i] == pWnd)
			return;
	m_WindowArray.push_back(pWnd);
}

