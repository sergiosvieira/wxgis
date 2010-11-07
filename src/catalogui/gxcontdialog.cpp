/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContainerDialog class.
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

#include "wxgis/catalogui/gxcontdialog.h"
#include "wxgis/catalogui/catalogcmd.h"

//////////////////////////////////////////////////////////////////////////////
// wxTreeContainerView
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxTreeContainerView, wxGxTreeView)

BEGIN_EVENT_TABLE(wxTreeContainerView, wxGxTreeView)
END_EVENT_TABLE()

wxTreeContainerView::wxTreeContainerView(void) : wxGxTreeView()
{
}

wxTreeContainerView::wxTreeContainerView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeView(parent, id, /*wxDefaultPosition, wxDefaultSize, */style)
{
}

wxTreeContainerView::~wxTreeContainerView(void)
{
    RemoveAllShowFilters();
}

void wxTreeContainerView::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;

    if(m_ShowFilterArray.size() > 0)
    {        
		for(size_t i = 0; i < m_ShowFilterArray.size(); i++)
			if(m_ShowFilterArray[i]->CanDisplayObject(pGxObject))
                goto ADD;
        return;
	}


ADD:
    wxGxTreeViewBase::AddTreeItem(pGxObject, hParent);
}

void wxTreeContainerView::AddShowFilter(IGxObjectFilter* pFilter)
{
	m_ShowFilterArray.push_back(pFilter);
}

void wxTreeContainerView::RemoveAllShowFilters(void)
{
	for(size_t i = 0; i < m_ShowFilterArray.size(); i++)
		wxDELETE(m_ShowFilterArray[i]);
}

bool wxTreeContainerView::CanChooseObject( IGxObject* pObject )
{
	if(NULL == pObject)
		return false;

    if(m_ShowFilterArray.size() > 0)
		for(size_t i = 0; i < m_ShowFilterArray.size(); i++)
			if(m_ShowFilterArray[i]->CanChooseObject(pObject))
                return true;
    return false;
}

//void wxTreeContainerView::OnSelChanged(wxTreeEvent& event)
//{
////    event.Skip();
//
//    wxArrayTreeItemIds treearray;
//    size_t count = GetSelections(treearray);
//    m_pSelection->Clear(GetId());
//    for(size_t i = 0; i < count; i++)
//    {
//	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
//	    if(pData != NULL)
//	    {
//		    m_pSelection->Select(pData->m_pObject, true, GetId());
//	    }
//    }
//}

////////////////////////////////////////////////////////////////////////////////
//// wxGxContainerDialog
////////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxContainerDialog, wxDialog)
    EVT_COMBOBOX(FILTERCOMBO, wxGxContainerDialog::OnFilerSelect)
    EVT_BUTTON(wxID_OK, wxGxContainerDialog::OnOK)
    EVT_BUTTON(ID_CREATE, wxGxContainerDialog::OnCreate)
    EVT_UPDATE_UI(wxID_OK, wxGxContainerDialog::OnOKUI)
    EVT_UPDATE_UI(ID_CREATE, wxGxContainerDialog::OnCreateUI)
END_EVENT_TABLE()

wxGxContainerDialog::wxGxContainerDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), m_pCatalog(NULL), m_pTree(NULL), m_pConfig(NULL), m_bShowCreateButton(false), m_bAllFilters(true), m_nDefaultFilter(0), m_bShowExportFormats(false)/*m_bAllowMultiSelect(false)*/
{
	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    m_pCatalog = new wxGxCatalogUI();
	m_pCatalog->Init();

	bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_staticDescriptionText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_staticDescriptionText->Wrap( -1 );
	bMainSizer->Add( m_staticDescriptionText, 0, wxALL|wxEXPAND, 8 );//|wxALIGN_CENTER_VERTICAL

    m_pTree = new wxTreeContainerView( this, TREECTRLID);
 //   m_pTree->Activate(this, NULL);//!!!!
	//bMainSizer->Add( m_pTree, 1, wxALL|wxEXPAND, 8 );

	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticWildText = new wxStaticText( this, wxID_ANY, _("Select export format"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticWildText->Wrap( -1 );
    m_staticWildText->Show(false);
	bSizer->Add( m_staticWildText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_WildcardCombo = new wxComboBox( this, FILTERCOMBO, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY ); 
    m_WildcardCombo->Show(false);
	bSizer->Add( m_WildcardCombo, 1, wxALL|wxEXPAND, 8 );
	
	bMainSizer->Add( bSizer, 0, wxEXPAND, 5 );
	

	wxFlexGridSizer* bFooterSizer = new wxFlexGridSizer( 1, 4, 0, 0 );
	bFooterSizer->AddGrowableCol( 1 );
	bFooterSizer->SetFlexibleDirection( wxBOTH );
	bFooterSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	m_CreateButton = new wxButton( this, ID_CREATE , _("Create container"), wxDefaultPosition, wxDefaultSize, 0 );
    m_CreateButton->Show(false);
	bFooterSizer->Add( m_CreateButton, 0, wxALL, 8 );

    wxStaticText* m_static = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	bFooterSizer->Add( m_static, 1, wxALL, 8 );

	m_OkButton = new wxButton( this, wxID_OK , _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OkButton->SetDefault(); 
	bFooterSizer->Add( m_OkButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 8 );
	
	m_CancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bFooterSizer->Add( m_CancelButton, 0, wxALL, 8 );
	
	bMainSizer->Add( bFooterSizer, 0, wxEXPAND, 8 );
	
	this->SetSizer( bMainSizer );
	this->Layout();

    wxGISCatalogMainCmd* pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
    pwxGISCatalogMainCmd->OnCreate(static_cast<IApplication*>(this));
    pwxGISCatalogMainCmd->SetSubType(7);//create folder
    m_pCreateCmd = static_cast<ICommand*>(pwxGISCatalogMainCmd);
}

wxGxContainerDialog::~wxGxContainerDialog()
{
	if(m_pTree)
		m_pTree->Deactivate();

    SerializeFramePos(true);

	RemoveAllFilters();

    m_pCatalog->Detach();
    wxDELETE(m_pCatalog);
    wxDELETE(m_pConfig);
}

void wxGxContainerDialog::SetButtonCaption(wxString sOkBtLabel)
{
	m_sOkBtLabel = sOkBtLabel;
}

void wxGxContainerDialog::SetStartingLocation(wxString sStartPath)
{
	m_sStartPath = sStartPath;
}

void wxGxContainerDialog::SetDescriptionText(wxString sText)
{
    m_staticDescriptionText->SetLabel(sText);
}

void wxGxContainerDialog::ShowCreateButton(bool bShow)
{
    m_bShowCreateButton = bShow;
    m_CreateButton->Show(m_bShowCreateButton);
}

void wxGxContainerDialog::AddFilter(IGxObjectFilter* pFilter, bool bDefault)
{
	m_FilterArray.push_back(pFilter);
	if(bDefault)
		m_nDefaultFilter = m_FilterArray.size() - 1;
}

void wxGxContainerDialog::RemoveAllFilters(void)
{
	for(size_t i = 0; i < m_FilterArray.size(); i++)
		wxDELETE(m_FilterArray[i]);
}

void wxGxContainerDialog::AddShowFilter(IGxObjectFilter* pFilter)
{
	m_pTree->AddShowFilter(pFilter);
}

void wxGxContainerDialog::RemoveAllShowFilters(void)
{
    m_pTree->RemoveAllShowFilters();
}

int wxGxContainerDialog::ShowModal(void)
{
	OnInit();
	return wxDialog::ShowModal();
}

void wxGxContainerDialog::OnInit()
{
#ifdef WXGISPORTABLE
    m_pConfig = new wxGISAppConfig(CONTDLG_NAME, CONFIG_DIR, true);
#else
	m_pConfig = new wxGISAppConfig(CONTDLG_NAME, CONFIG_DIR);
#endif

    m_pTree->Activate(this, NULL);//!!!!
	bMainSizer->Insert(1, m_pTree, 1, wxALL|wxEXPAND, 8 );

    if(m_bShowExportFormats)
    {
        m_staticWildText->Show(true);
        m_WildcardCombo->Show(true);
    }
    else
    {
        m_staticWildText->Show(false);
        m_WildcardCombo->Show(false);
    }

    
 //   long nStyle = wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxBORDER_THEME;
	//if(!m_bAllowMultiSelect)
	//	nStyle |= wxLC_SINGLE_SEL;

	for(size_t i = 0; i < m_FilterArray.size(); i++)
		m_WildcardCombo->AppendString(m_FilterArray[i]->GetName());
	if(m_FilterArray.size() > 1 && m_bAllFilters)
		m_WildcardCombo->AppendString(_("All listed filters"));
	if(m_FilterArray.size() == 0)
		m_WildcardCombo->AppendString(_("All items"));
	m_WildcardCombo->Select(m_nDefaultFilter);

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

void wxGxContainerDialog::SerializeFramePos(bool bSave)
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
			int w = wxAtoi(pFrameXmlNode->GetPropVal(wxT("Width"), wxT("450")));
			int h = wxAtoi(pFrameXmlNode->GetPropVal(wxT("Height"), wxT("650")));
			
			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}

void wxGxContainerDialog::SetAllFilters(bool bAllFilters)
{
	m_bAllFilters = bAllFilters;
}

void wxGxContainerDialog::ShowExportFormats(bool bShow)
{
    m_bShowExportFormats = bShow;
}

IGxObjectFilter* wxGxContainerDialog::GetCurrentFilter(void)
{
    if(m_FilterArray.size() == 0)
        return NULL;
    return m_FilterArray[m_nDefaultFilter];
}

IGxObject* wxGxContainerDialog::GetLocation(void)
{
    return m_pCatalog->GetSelection()->GetLastSelectedObject();
}

wxString wxGxContainerDialog::GetPath(void)
{
    IGxObject* pObj = GetLocation();
    if(pObj)
    {
        return pObj->GetFullName();
    }
    return wxEmptyString;
}

void wxGxContainerDialog::OnOKUI(wxUpdateUIEvent& event)
{
    IGxObject* pObj = GetLocation();
    bool bEnable = false;
    if(m_pTree) 
        bEnable = m_pTree->CanChooseObject(pObj);
    event.Enable(bEnable);
}

void wxGxContainerDialog::OnFilerSelect(wxCommandEvent& event)
{
    m_nDefaultFilter = m_WildcardCombo->GetCurrentSelection();
}

void wxGxContainerDialog::OnOK(wxCommandEvent& event)
{
    m_nRetCode = wxID_OK;
    if ( Validate() && TransferDataFromWindow() )
    {
        int nPos = m_WildcardCombo->GetCurrentSelection();
        //fill out data
        IGxSelection* pSel = m_pCatalog->GetSelection();
        for(size_t i = 0; i < pSel->GetCount(); i++)
        {
            m_ObjectArray.push_back(pSel->GetSelectedObjects(i));
        }

        wxString sLastPath = GetPath();
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

        if ( IsModal() )
            EndModal(m_nRetCode);
        else
        {
            SetReturnCode(m_nRetCode);
            this->Show(false);
        }
    }
}

void wxGxContainerDialog::OnCreate(wxCommandEvent& event)
{
    //focus tree view
    m_pTree->SetFocus();
    if(m_pCreateCmd)
        m_pCreateCmd->OnClick();
}

void wxGxContainerDialog::OnCreateUI(wxUpdateUIEvent& event)
{
    if(!m_pCreateCmd)
    {
        event.Enable(false);
        return;
    }
    event.Enable(m_pCreateCmd->GetEnabled());
}
