/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/catalogcmd.h"
#include "wxgis/catalogui/viewscmd.h"
#include "wxgis/catalogui/gxdiscconnectionui.h"
#include "wxgis/core/globalfn.h"
#include "wxgis/framework/application.h"

#include <wx/valgen.h>

//////////////////////////////////////////////////////////////////////////////
// wxTreeViewComboPopup
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxTreeViewComboPopup, wxGxTreeViewBase)

BEGIN_EVENT_TABLE(wxTreeViewComboPopup, wxGxTreeViewBase)
    //TODO: EVT_LEFT_UP(wxTreeViewComboPopup::OnMouseClick)
    EVT_LEFT_DOWN(wxTreeViewComboPopup::OnMouseClick)
    EVT_MOTION(wxTreeViewComboPopup::OnMouseMove)
    EVT_TREE_ITEM_ACTIVATED(TREECTRLID, wxTreeViewComboPopup::OnDblClick)
	EVT_GXSELECTION_CHANGED(wxTreeViewComboPopup::OnSelectionChanged)
END_EVENT_TABLE()

bool wxTreeViewComboPopup::Create(wxWindow* parent)
{
    return wxTreeViewComboPopup::Create(parent, TREECTRLID);
}

bool wxTreeViewComboPopup::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_bClicked = false;
    return wxGxTreeViewBase::Create(parent, TREECTRLID, pos, size, wxBORDER_SIMPLE | wxTR_NO_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE, name);
}

void wxTreeViewComboPopup::Init()
{
}

void wxTreeViewComboPopup::OnPopup()
{
   m_bClicked = false;
   SelectItem(m_TreeMap[m_pSelection->GetLastSelectedObjectID()]);
   //CaptureMouse();
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
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(m_pSelection->GetLastSelectedObjectID());
        if(pGxObject)
            return pGxObject->GetName();
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
		    m_pSelection->Select(pData->m_nObjectID, false, GetId());
            m_PrewItemId = ItemId;
	    }
    }
    Dismiss();

    event.Skip();
}

void wxTreeViewComboPopup::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;

    long nSelID = m_pSelection->GetLastSelectedObjectID();
	wxTreeItemId ItemId = m_TreeMap[nSelID];
	if(ItemId.IsOk())
	{
		SelectItem(ItemId);
	}
	else
	{
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nSelID);
		IGxObject* pParentGxObj = pGxObject->GetParent();
		while(pParentGxObj)
		{
			wxTreeItemId ItemId = m_TreeMap[pParentGxObj->GetID()];
			if(ItemId.IsOk())
			{
				Expand(ItemId);
				break;
			}
			else
				pParentGxObj = pParentGxObj->GetParent();
		}
		OnSelectionChanged(event);
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
		m_pSelection->Select(pData->m_nObjectID, false, GetId());
	}
    wxComboPopup::Dismiss();
}

void wxTreeViewComboPopup::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent)
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
		pos = 0;//0 col img, 1 - col img

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject->GetID(), pos, false);

    wxString sName;
    if(m_pCatalog->GetShowExt())
        sName = pGxObject->GetName();
    else
        sName = pGxObject->GetBaseName();


	wxTreeItemId NewTreeItem = AppendItem(hParent, sName, pos, -1, pData);
	m_TreeMap[pGxObject->GetID()] = NewTreeItem;

	if(pContainer->AreChildrenViewable())
		SetItemHasChildren(NewTreeItem);

//	SortChildren(hParent);
	wxTreeCtrl::Refresh();
}

wxSize wxTreeViewComboPopup::GetAdjustedSize(int minWidth, int prefHeight, int maxHeight)
{
    return wxSize(minWidth + 190, prefHeight);
}


//////////////////////////////////////////////////////////////////////////////
// wxGxDialogContentView
//////////////////////////////////////////////////////////////////////////////

wxGxDialogContentView::wxGxDialogContentView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxGxContentView(parent, id, pos, size, style), m_pConnectionPointSelection(NULL), m_ConnectionPointSelectionCookie(wxNOT_FOUND), m_nFilterIndex(0), m_pFiltersArray(NULL), m_pExternalCatalog(NULL)
{
}

wxGxDialogContentView::~wxGxDialogContentView(void)
{
}

bool wxGxDialogContentView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	if(!wxGxContentView::Activate(application, pConf))
        return false;

	m_pConnectionPointSelection = dynamic_cast<wxGISConnectionPointContainer*>( m_pSelection );
	if(m_pConnectionPointSelection != NULL)
		m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	return true;
}

void wxGxDialogContentView::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND)
		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
	wxGxContentView::Deactivate();
}

void wxGxDialogContentView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	//dbl click
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

	//IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pItemData->pObject);
	//if(pGxObjectWizard != NULL)
	//	if(!pGxObjectWizard->Invoke(this))
	//		return;

    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
	if(pGxObjectContainer != NULL )
	{
		m_pSelection->Select(pItemData->nObjectID, false, IGxSelection::INIT_ALL);//GetId()
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
    if(m_pFiltersArray->size() == 0)
		return wxGxContentView::AddObject(pObject);

	if(m_nFilterIndex != m_pFiltersArray->size())
	{
		if(m_pFiltersArray->at(m_nFilterIndex)->CanDisplayObject(pObject))
			return wxGxContentView::AddObject(pObject);
	}
	else
	{
		for(size_t i = 0; i < m_pFiltersArray->size(); ++i)
			if(m_pFiltersArray->at(i)->CanDisplayObject(pObject))
				return wxGxContentView::AddObject(pObject);
	}
}

void wxGxDialogContentView::OnObjectAdded(wxGxCatalogEvent& event)
{
    wxGxContentView::OnObjectAdded(event);
    if(m_pExternalCatalog)
    {
		IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
        wxGxDiscConnectionUI* pDiscConnection = dynamic_cast<wxGxDiscConnectionUI*>(pGxObject.get());
        if(pDiscConnection)
        {
            m_pExternalCatalog->ConnectFolder(pGxObject->GetFullName(), false);
            return;
        }
        IGxObject* pParent = pGxObject->GetParent();
        if(pParent)
        {
            IGxObjectContainer* pObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pExternalCatalog);
            IGxObject* pParentObject = pObjectContainer->SearchChild(pParent->GetFullName());
            pParentObject->Refresh();
        }
    }
}

void wxGxDialogContentView::OnObjectDeleted(wxGxCatalogEvent& event)
{
    wxGxContentView::OnObjectDeleted(event);
    if(m_pExternalCatalog)
    {
		IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
        wxGxDiscConnectionUI* pDiscConnection = dynamic_cast<wxGxDiscConnectionUI*>(pGxObject.get());
        if(pDiscConnection)
        {
            m_pExternalCatalog->DisconnectFolder(pGxObject->GetInternalName());
            return;
        }
        IGxObject* pParent = pGxObject->GetParent();
        if(pParent)
        {
            IGxObjectContainer* pObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pExternalCatalog);
            IGxObject* pParentObject = pObjectContainer->SearchChild(pParent->GetFullName());
            pParentObject->Refresh();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// wxGxObjectDialog
//////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxObjectDialog, wxDialog)
	//EVT_LIST_ITEM_SELECTED(LISTCTRLID, wxGxObjectDialog::OnItemSelected)
	//EVT_LIST_ITEM_DESELECTED(LISTCTRLID, wxGxObjectDialog::OnItemSelected)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxObjectDialog::OnCommand)
	EVT_MENU_RANGE(ID_MENUCMD, ID_MENUCMDMAX, wxGxObjectDialog::OnDropDownCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxObjectDialog::OnCommandUI)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, wxGxObjectDialog::OnToolDropDown)
    EVT_COMBOBOX(FILTERCOMBO, wxGxObjectDialog::OnFilterSelect)
    EVT_BUTTON(wxID_OK, wxGxObjectDialog::OnOK)
    EVT_UPDATE_UI(wxID_OK, wxGxObjectDialog::OnOKUI)
END_EVENT_TABLE()

wxGxObjectDialog::wxGxObjectDialog( wxWindow* parent, IGxCatalog* pExternalCatalog, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), m_pCatalog(NULL), m_pDropDownCommand(NULL), m_bAllowMultiSelect(false), m_bOverwritePrompt(false), m_nDefaultFilter(0), m_pwxGxContentView(NULL), m_PopupCtrl(NULL), m_bAllFilters(true), m_bOwnFilter(true)
{
	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    m_pExternalCatalog = pExternalCatalog; 
    m_pCatalog = new wxGxCatalogUI(true);
	m_pCatalog->Init(m_pExternalCatalog);
	m_pCatalog->SetShowExt(true);

	bMainSizer = new wxBoxSizer( wxVERTICAL );

	bHeaderSizer = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Look in:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText1->Wrap( -1 );
	bHeaderSizer->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_TreeCombo = new wxComboCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCB_READONLY);
#ifdef __WXMSW__
    m_TreeCombo->UseAltPopupWindow(true);
#else
    m_TreeCombo->UseAltPopupWindow(false);
#endif
    m_PopupCtrl = new wxTreeViewComboPopup();
    m_TreeCombo->SetPopupControl(m_PopupCtrl);
    m_TreeCombo->EnablePopupAnimation(true);
    m_PopupCtrl->Activate(this, NULL);//!!!!

	bHeaderSizer->Add( m_TreeCombo, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

	m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxAUI_TB_HORZ_LAYOUT|wxBORDER_NONE);
	m_toolBar->SetToolBitmapSize( wxSize( 16,16 ) );
	m_toolBar->SetArtProvider(new wxGxToolBarArt());

    wxGISApplication* pApp = dynamic_cast<wxGISApplication*>(GetApplication());
    if(pApp)
    {
    //	0	Up One Level
    //	1	Connect Folder
    //	2	Disconnect Folder
    //	3	Location
    //  4   Delete Item
    //  5   Back
    //  6   Forward
    //  7   Create Folder

        int IDs[8] = {5,0,wxNOT_FOUND,1,2,wxNOT_FOUND,4,7};
        ICommand *pCmd(nullptr);
        wxGISCatalogMainCmd* pwxGISCatalogMainCmd(nullptr);
        for(size_t i = 0; i < 8; ++i)
        {
            if(IDs[i] == wxNOT_FOUND)
            {
                m_toolBar->AddSeparator();
                continue;
            }
            pCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), IDs[i]);
            if(!pCmd)
                continue;
            pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
            pwxGISCatalogMainCmd->OnCreate(static_cast<IFrameApplication*>(this));
            pwxGISCatalogMainCmd->SetSubType(IDs[i]);
            pwxGISCatalogMainCmd->SetID(pCmd->GetID());

            //pwxGISCatalogMainCmd->SetID(ID_PLUGINCMD + i + 1);
            m_CommandArray.push_back(pwxGISCatalogMainCmd);
            wxGISEnumCommandKind kind = pwxGISCatalogMainCmd->GetKind();
            if(pwxGISCatalogMainCmd->GetKind() == enumGISCommandDropDown)
                kind = enumGISCommandNormal;
		    m_toolBar->AddTool( pwxGISCatalogMainCmd->GetID(), pwxGISCatalogMainCmd->GetCaption(), pwxGISCatalogMainCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogMainCmd->GetTooltip(), pwxGISCatalogMainCmd->GetMessage(), NULL );
		    if(pwxGISCatalogMainCmd->GetKind() == enumGISCommandDropDown)
			    m_toolBar->SetToolDropDown(pwxGISCatalogMainCmd->GetID(), true);
        }

        pCmd = pApp->GetCommand(wxT("wxGISCatalogViewsCmd"), 0);
        if(pCmd)
        {
            wxGISCatalogViewsCmd* pwxGISCatalogViewsCmd = new wxGISCatalogViewsCmd();
            pwxGISCatalogViewsCmd->OnCreate(static_cast<IFrameApplication*>(this));
            pwxGISCatalogViewsCmd->SetSubType(0);
            pwxGISCatalogViewsCmd->SetID(pCmd->GetID());
            //pwxGISCatalogViewsCmd->SetID(ID_PLUGINCMD + 9);
            m_CommandArray.push_back(pwxGISCatalogViewsCmd);
            wxGISEnumCommandKind kind = pwxGISCatalogViewsCmd->GetKind();
            if(pwxGISCatalogViewsCmd->GetKind() == enumGISCommandDropDown)
                kind = enumGISCommandNormal;
	        m_toolBar->AddTool( pwxGISCatalogViewsCmd->GetID(), pwxGISCatalogViewsCmd->GetCaption(), pwxGISCatalogViewsCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogViewsCmd->GetTooltip(), pwxGISCatalogViewsCmd->GetMessage(), NULL );
	        if(pwxGISCatalogViewsCmd->GetKind() == enumGISCommandDropDown)
		        m_toolBar->SetToolDropDown(pwxGISCatalogViewsCmd->GetID(), true);
        }

	    m_toolBar->Realize();

        //rename
        pCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 8);
        if(pCmd)
        {
            pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
            pwxGISCatalogMainCmd->OnCreate(static_cast<IFrameApplication*>(this));
            pwxGISCatalogMainCmd->SetID(pCmd->GetID());
            pwxGISCatalogMainCmd->SetSubType(8);
            m_CommandArray.push_back(pwxGISCatalogMainCmd);
        }

        //refresh
        pCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 9);
        if(pCmd)
        {
            pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
            pwxGISCatalogMainCmd->OnCreate(static_cast<IFrameApplication*>(this));
            pwxGISCatalogMainCmd->SetID(pCmd->GetID());
            pwxGISCatalogMainCmd->SetSubType(9);
            m_CommandArray.push_back(pwxGISCatalogMainCmd);
        }

        //load accelerators
        if(pApp->GetGISAcceleratorTable())
            SetAcceleratorTable(pApp->GetGISAcceleratorTable()->GetAcceleratorTable());
    }

	bHeaderSizer->Add( m_toolBar, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
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

wxGxObjectDialog::~wxGxObjectDialog()
{
	if(m_pwxGxContentView)
		m_pwxGxContentView->Deactivate();
	if(m_PopupCtrl)
		m_PopupCtrl->Deactivate();

    SerializeFramePos(true);

	for(size_t i = 0; i < m_CommandArray.size(); ++i)
		wxDELETE(m_CommandArray[i]);

	RemoveAllFilters();

    wxDELETE(m_pwxGxContentView);
    
    if(m_pCatalog)
    {
        m_pCatalog->Detach();
        delete m_pCatalog;
    }
}

void wxGxObjectDialog::OnCommand(wxCommandEvent& event)
{
	event.Skip();
	Command(GetCommand(event.GetId()));
}

void wxGxObjectDialog::Command(ICommand* pCmd)
{
	pCmd->OnClick();
    if(pCmd->GetSubType() == 4)
    {
        IGxSelection* pSel = m_pwxGxContentView->GetSelectedObjects();
        if(pSel)
            pSel->Clear(NOTFIRESELID/*LISTCTRLID*/);;
        //m_pCatalog->GetSelection()->Clear(NOTFIRESELID/*LISTCTRLID*/);
    }
}

void wxGxObjectDialog::OnCommandUI(wxUpdateUIEvent& event)
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

ICommand* wxGxObjectDialog::GetCommand(long CmdID)
{
	for(size_t i = 0; i < m_CommandArray.size(); ++i)
		if(m_CommandArray[i]->GetID() == CmdID)
			return m_CommandArray[i];
	return NULL;
}

ICommand* wxGxObjectDialog::GetCommand(wxString sCmdName, unsigned char nCmdSubType)
{
	for(size_t i = 0; i < m_CommandArray.size(); ++i)
	{
        wxObject* pObj = dynamic_cast<wxObject*>(m_CommandArray[i]);
		wxClassInfo * pInfo = pObj->GetClassInfo();
		wxString sCommandName = pInfo->GetClassName();
		if(sCommandName == sCmdName && m_CommandArray[i]->GetSubType() == nCmdSubType)
			return m_CommandArray[i];
	}
	return NULL;
}

void wxGxObjectDialog::OnDropDownCommand(wxCommandEvent& event)
{
	event.Skip();
	if(m_pDropDownCommand)
        m_pDropDownCommand->OnDropDownCommand(event.GetId());
}

void wxGxObjectDialog::OnToolDropDown(wxAuiToolBarEvent& event)
{
	event.Skip();
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

void wxGxObjectDialog::SetButtonCaption(wxString sOkBtLabel)
{
	m_sOkBtLabel = sOkBtLabel;
}

void wxGxObjectDialog::SetStartingLocation(wxString sStartPath)
{
	m_sStartPath = sStartPath;
}

void wxGxObjectDialog::SetName(wxString sName)
{
	m_sName = sName;
}

void wxGxObjectDialog::SetAllowMultiSelect(bool bAllowMultiSelect)
{
	m_bAllowMultiSelect = bAllowMultiSelect;
}

void wxGxObjectDialog::SetOverwritePrompt(bool bOverwritePrompt)
{
	m_bOverwritePrompt = bOverwritePrompt;
}

void wxGxObjectDialog::SetAllFilters(bool bAllFilters)
{
	m_bAllFilters = bAllFilters;
}

int wxGxObjectDialog::ShowModalOpen()
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

int wxGxObjectDialog::ShowModalSave()
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

void wxGxObjectDialog::OnInit()
{
    long nStyle = wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxBORDER_THEME;
	if(!m_bAllowMultiSelect)
		nStyle |= wxLC_SINGLE_SEL;
   	m_pwxGxContentView = new wxGxDialogContentView(this, LISTCTRLID, wxDefaultPosition, wxDefaultSize, nStyle);
    //m_pwxGxContentView->SetExternalCatalog(m_pExternalCatalog);
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(pConfig)
	{
	    wxXmlNode* pContentViewConf = pConfig->GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/views/contentsview")));
	    m_pwxGxContentView->Activate(this, pContentViewConf);
	}
	RegisterChildWindow(static_cast<wxWindow*>(m_pwxGxContentView));

	bMainSizer->Insert(1, m_pwxGxContentView, 1, wxALL|wxEXPAND, 5 );
	m_pwxGxContentView->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( wxGxObjectDialog::OnItemSelected ), NULL, this );
	m_pwxGxContentView->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( wxGxObjectDialog::OnItemSelected ), NULL, this );

	for(size_t i = 0; i < m_FilterArray.size(); ++i)
		m_WildcardCombo->AppendString(m_FilterArray[i]->GetName());
	if(m_FilterArray.size() > 1 && m_bAllFilters)
		m_WildcardCombo->AppendString(_("All listed filters"));
	if(m_FilterArray.size() == 0)
		m_WildcardCombo->AppendString(_("All items"));
	m_WildcardCombo->Select(m_nDefaultFilter);

	m_pwxGxContentView->SetCurrentFilter(m_nDefaultFilter);
	m_pwxGxContentView->SetFilters(&m_FilterArray);

	if(m_sStartPath.IsEmpty())
	{
		IGxObject* pObj = dynamic_cast<IGxObject*>(m_pCatalog);
		wxString sLastPath;
		if(pConfig)
			sLastPath = pConfig->Read(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), pObj->GetName());
		else
			sLastPath = pObj->GetName();
		m_pCatalog->SetLocation(sLastPath);
	}
	else
		m_pCatalog->SetLocation(m_sStartPath);

    m_NameTextCtrl->SetFocus();

    SerializeFramePos(false);
}

void wxGxObjectDialog::OnItemSelected(wxListEvent& event)
{
	event.Skip();
    //if(m_bIsSaveDlg)
    //    return;

    wxGxDialogContentView::LPITEMDATA pItemData = (wxGxDialogContentView::LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

	IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject.get());
    if(!pGxDataset)
    {
    	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
        if(pGxObjectContainer)
            return;
    }

    wxString sTempName;
	long item = wxNOT_FOUND;
	while(1)
    {
        item = m_pwxGxContentView->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == wxNOT_FOUND )
            break;
		if(sTempName.IsEmpty())
			sTempName += m_pwxGxContentView->GetItemText(item);
		else
			sTempName += wxT("; ") + m_pwxGxContentView->GetItemText(item);
    }
	if(!sTempName.IsEmpty())
        m_sName = sTempName;
	TransferDataToWindow();
}

void wxGxObjectDialog::AddFilter(IGxObjectFilter* pFilter, bool bDefault)
{
	m_FilterArray.push_back(pFilter);
	if(bDefault)
		m_nDefaultFilter = m_FilterArray.size() - 1;
}

void wxGxObjectDialog::RemoveAllFilters(void)
{
    if(m_bOwnFilter)
        for(size_t i = 0; i < m_FilterArray.size(); ++i)
            wxDELETE(m_FilterArray[i]);
}

void wxGxObjectDialog::OnFilterSelect(wxCommandEvent& event)
{
    m_nDefaultFilter = m_WildcardCombo->GetCurrentSelection();
    m_pwxGxContentView->SetCurrentFilter(m_nDefaultFilter);
    m_pwxGxContentView->RefreshAll();
}

void wxGxObjectDialog::OnOK(wxCommandEvent& event)
{
	//TODO: if path in editbox goto path? not to Sopen
    m_nRetCode = wxID_OK;
    if ( Validate() && TransferDataFromWindow() )
    {
        int nPos = m_WildcardCombo->GetCurrentSelection();
        //fill out data
        if(m_bIsSaveDlg)
        {
            IGxSelection* pSel = m_pwxGxContentView->GetSelectedObjects();
            if(!pSel)
                return;
			long nSelID = pSel->GetSelectedObjectID(0);
			IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nSelID);
            IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
            if(pObjCont)
            {
                m_pCatalog->GetSelection()->Select(nSelID, false, IGxSelection::INIT_ALL);
                return;
            }

            if(nPos != m_FilterArray.size())
            {
                wxGISEnumSaveObjectResults Result = m_FilterArray[nPos]->CanSaveObject(GetLocation(), GetNameWithExt());
                if(!DoSaveObject(Result))
                    return;
            }
            else
            {
                for(size_t j = 0; j < m_FilterArray.size(); j++)
                {
                    wxGISEnumSaveObjectResults Result = m_FilterArray[j]->CanSaveObject(GetLocation(), GetNameWithExt());
                    if(DoSaveObject(Result))
                        break;
                }
            }
        }
        else
        {
            IGxSelection* pSel = m_pwxGxContentView->GetSelectedObjects();

 			long nSelID = pSel->GetSelectedObjectID(0);
 			IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nSelID);
            IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(pGxObject.get());
            if(pObjCont)
            {
                m_pCatalog->GetSelection()->Select(nSelID, false, IGxSelection::INIT_ALL);
                return;
            }

            for(size_t i = 0; i < pSel->GetCount(); ++i)
            {
				IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                m_ObjectArray.push_back(pGxObject.get());
            }
            for(size_t i = 0; i < m_ObjectArray.size(); ++i)
            {
                if(nPos != m_FilterArray.size())
                {
                    if(!m_FilterArray[nPos]->CanChooseObject(m_ObjectArray[i]))
                    {
                        IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(m_ObjectArray[i]);
                        if(pObjCont)
                        {
                            m_pCatalog->GetSelection()->Select(m_ObjectArray[i]->GetID(), false, IGxSelection::INIT_ALL);
                            return;
                        }
                        m_ObjectArray.erase(m_ObjectArray.begin() + i);
                        i--;
                    }
                }
                else
                {
                    for(size_t j = 0; j < m_FilterArray.size(); j++)
                    {
                        bool bCanChooseObject = false;
                        if(m_FilterArray[j]->CanChooseObject(m_ObjectArray[i]))
                        {
                            bCanChooseObject = true;
                            break;
                        }
                        if(!bCanChooseObject)
                        {
                            IGxObjectContainer* pObjCont = dynamic_cast<IGxObjectContainer*>(m_ObjectArray[i]);
                            if(pObjCont)
                            {
                                m_pCatalog->GetSelection()->Select(m_ObjectArray[i]->GetID(), false, IGxSelection::INIT_ALL);
                                return;
                            }
                            m_ObjectArray.erase(m_ObjectArray.begin() + i);
                            i--;
                        }
                    }
                }
            }
            if(m_ObjectArray.size() == 0)
            {
                wxMessageBox(_("Cannot choose item(s)"), _("Error"), wxICON_ERROR | wxOK, this);
                return;
            }
        }

        wxTreeItemId ItemId = m_PopupCtrl->GetSelection();
        if(ItemId.IsOk())
        {
            wxGxTreeItemData* pData = (wxGxTreeItemData*)m_PopupCtrl->GetItemData(ItemId);
            if(pData != NULL)
            {
		        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
				wxGISAppConfigSPtr pConfig = GetConfig();
				if(pGxObject && pConfig)
					pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), pGxObject->GetFullName());
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

void wxGxObjectDialog::SerializeFramePos(bool bSave)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
        return;

	if(bSave)
	{
		if( IsMaximized() )
		{
			pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), true);
		}
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);

			pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), false);
			pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/width")), w);
			pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/height")), h);
			pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/xpos")), x);
			pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/ypos")), y);
		}
	}
	else
	{
		//load
		bool bMaxi = pConfig->ReadBool(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), false);
		if(!bMaxi)
		{
			int x = pConfig->ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/xpos")), 50);
			int y = pConfig->ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/ypos")), 50);
			int w = pConfig->ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/width")), 450);
			int h = pConfig->ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/height")), 650);
			
			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}

void wxGxObjectDialog::OnOKUI(wxUpdateUIEvent& event)
{
    if(m_NameTextCtrl->GetValue().IsEmpty())
    {
        event.Enable(false);
        return;
    }
    if(m_bIsSaveDlg)
    {
        IGxObjectFilter* pFilter = GetCurrentFilter();
        if(pFilter)
        {
            wxGISEnumSaveObjectResults Result = pFilter->CanSaveObject(GetLocation(), GetNameWithExt());

            if(enumGISSaveObjectDeny == Result)
            {
                event.Enable(false);
                return;
            }
        }
    }
    event.Enable(true);
}

wxString wxGxObjectDialog::GetNameWithExt(void)
{
	if(m_bIsSaveDlg)
	{
		wxString sName = GetName();

		IGxObjectFilter* pFilter = GetCurrentFilter();
		if(pFilter)
		{
			wxString sExt;
			sExt = pFilter->GetExt();
			if(!sExt.IsEmpty())
				sName += wxT(".") + sExt;
		}
		return sName;
	}
	else
	{
		if(m_ObjectArray.size() > 0)
		{
			return m_ObjectArray[0]->GetName();
		}
	}
	return wxEmptyString;
}

wxString wxGxObjectDialog::GetName(void)
{
    wxString sName = m_NameTextCtrl->GetValue();

    IGxObjectFilter* pFilter = GetCurrentFilter();
    if(pFilter)
    {
        wxString sExt;
        sExt = pFilter->GetExt();
        if(sExt.IsEmpty())
            return sName;
    }

    wxFileName FileName(sName);
    FileName.SetEmptyExt();
    sName = FileName.GetName();

    return sName;
}

wxString wxGxObjectDialog::GetFullPath(void)
{
    wxString sPath = GetPath();
    wxString sFPath;
    if(sPath[sPath.Len() - 1] == '\\')
        sFPath = sPath + GetNameWithExt();
    else
        sFPath = sPath + wxFileName::GetPathSeparator() + GetNameWithExt();
    return sFPath;
}

wxString wxGxObjectDialog::GetPath(void)
{
    IGxObject* pObj = GetLocation();
    if(pObj)
    {
        return pObj->GetFullName();
    }
    return wxEmptyString;
}

CPLString wxGxObjectDialog::GetInternalPath(void)
{
    IGxObject* pObj = GetLocation();
    if(pObj)
    {
        return pObj->GetInternalName();
    }
    return CPLString();
}

IGxObject* wxGxObjectDialog::GetLocation(void)
{
    return m_pwxGxContentView->GetParentGxObject();
}

bool wxGxObjectDialog::DoSaveObject(wxGISEnumSaveObjectResults Result)
{
    if(Result == enumGISSaveObjectDeny)
    {
        wxMessageBox(_("Cannot save item!"), _("Error"), wxICON_ERROR | wxOK, this);
        return false;
    }
    else if(Result == enumGISSaveObjectExists && m_bOverwritePrompt)
    {
        int nRes = wxMessageBox(_("Item exists! Overwrite?"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL, this);
        if(nRes == wxCANCEL)
            return false;
        else if(nRes == wxNO)
            m_nRetCode = wxID_CANCEL;
    //    else
    //    {
    //        IGxObjectEdit* pObjEd = dynamic_cast<IGxObjectEdit*>(m_pCatalog->SearchChild(GetFullPath()));
    //        //Delete item
    //        if(!pObjEd)
    //        {
    //            wxMessageBox(_("Cannot delete item!"), _("Error"), wxICON_ERROR | wxOK, this);
    //            return false;
    //        }

    //        if(!pObjEd->CanDelete())
    //        {
    //            wxMessageBox(_("Cannot delete item!"), _("Error"), wxICON_ERROR | wxOK, this);
    //            return false;
    //        }

    //        if(!pObjEd->Delete())
    //        {
    //            wxMessageBox(_("Cannot delete item!"), _("Error"), wxICON_ERROR | wxOK, this);
    //            return false;
    //        }
    //    }
    }
    return true;
}

const WINDOWARRAY* const wxGxObjectDialog::GetChildWindows(void)
{
	return &m_WindowArray;
}

void wxGxObjectDialog::RegisterChildWindow(wxWindow* pWnd)
{
	for(size_t i = 0; i < m_WindowArray.size(); ++i)
		if(m_WindowArray[i] == pWnd)
			return;
	m_WindowArray.push_back(pWnd);
}

void wxGxObjectDialog::UnRegisterChildWindow(wxWindow* pWnd)
{
    WINDOWARRAY::iterator pos = std::find(m_WindowArray.begin(), m_WindowArray.end(), pWnd);
	if(pos != m_WindowArray.end())
		m_WindowArray.erase(pos);
}

IGxObjectFilter* wxGxObjectDialog::GetCurrentFilter(void)
{
    if(m_FilterArray.size() == 0)
        return NULL;
    return m_FilterArray[m_nDefaultFilter];
}

size_t wxGxObjectDialog::GetCurrentFilterId(void)
{
    return m_nDefaultFilter;
}
