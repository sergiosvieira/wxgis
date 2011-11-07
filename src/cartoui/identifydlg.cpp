/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISIdentifyDlg class - dialog/dock window with the results of identify.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/cartoui/identifydlg.h"

//-------------------------------------------------------------------
// wxGISIdentifyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISIdentifyDlg, wxPanel)

wxGISIdentifyDlg::wxGISIdentifyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, pos, size, style);
}

wxGISIdentifyDlg::wxGISIdentifyDlg(void)
{
}

bool wxGISIdentifyDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_fgTopSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	m_fgTopSizer->AddGrowableCol( 1 );
	m_fgTopSizer->SetFlexibleDirection( wxBOTH );
	m_fgTopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Identify from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_fgTopSizer->Add( m_staticText1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_LayerChoiceChoices;
	m_LayerChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_LayerChoiceChoices, 0 );
	m_LayerChoice->SetSelection( 0 );
	m_fgTopSizer->Add( m_LayerChoice, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_bpSplitButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_fgTopSizer->Add( m_bpSplitButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bMainSizer->Add( m_fgTopSizer, 0, wxEXPAND, 5 );
	
	//m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	//m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
	//
	//m_bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( m_bMainSizer );
	this->Layout();

    return true;
}

wxGISIdentifyDlg::~wxGISIdentifyDlg()
{
}

//-------------------------------------------------------------------
// wxAxToolboxView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxIdentifyView, wxGISIdentifyDlg)

wxAxIdentifyView::wxAxIdentifyView(void)
{
}

wxAxIdentifyView::wxAxIdentifyView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISIdentifyDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
    Create(parent, id, pos, size);
}

wxAxIdentifyView::~wxAxIdentifyView(void)
{
}

bool wxAxIdentifyView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_sViewName = wxString(_("Identify"));
    return wxGISIdentifyDlg::Create(parent, id, pos, size, wxNO_BORDER, name);
}

bool wxAxIdentifyView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
    m_pApp = application;

    ////get config xml
    //wxXmlNode *pTaskExecConf(NULL), *pToolboxTreeConf(NULL);
    //wxXmlNode* pChildConf = pConf->GetChildren();
    //while(pChildConf)
    //{
    //    if(pChildConf->GetName() == wxString(wxT("wxGxToolExecuteView")))
    //        pTaskExecConf = pChildConf;
    //    if(pChildConf->GetName() == wxString(wxT("wxGxToolboxTreeView")))
    //        pToolboxTreeConf = pChildConf;

    //    pChildConf = pChildConf->GetNext();
    //}
    //if(!pTaskExecConf)
    //{
    //    pTaskExecConf = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("wxGxToolExecuteView")));
    //    pConf->AddChild(pTaskExecConf);
    //}
    //if(!pToolboxTreeConf)
    //{
    //    pToolboxTreeConf = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("wxGxToolboxTreeView")));
    //    pConf->AddChild(pToolboxTreeConf);
    //}

    //m_pGxToolboxView = new wxGxToolboxTreeView(this, TREECTRLID);
    //AddPage(m_pGxToolboxView, m_pGxToolboxView->GetViewName(), true, m_pGxToolboxView->GetViewIcon());
    //m_pGxToolboxView->Activate(application, pToolboxTreeConf);
    //m_pApp->RegisterChildWindow(m_pGxToolboxView);


    //wxGxToolExecute* pGxToolExecute(NULL);
    //IGxObjectContainer* pRootContainer = dynamic_cast<IGxObjectContainer*>(m_pGxApplication->GetCatalog());
    //if(pRootContainer)
    //{
    //    IGxObjectContainer* pGxToolboxes = dynamic_cast<IGxObjectContainer*>(pRootContainer->SearchChild(wxString(_("Toolboxes"))));
    //    if(pGxToolboxes)
    //    {
    //        GxObjectArray* pArr = pGxToolboxes->GetChildren();
    //        if(pArr)
    //        {
    //            for(size_t i = 0; i < pArr->size(); ++i)
    //            {
    //                pGxToolExecute = dynamic_cast<wxGxToolExecute*>(pArr->operator[](i));
    //                if(pGxToolExecute)
    //                    break;
    //            }
    //        }
    //    }
    //}

    //if(pGxToolExecute)
    //{        
    //    m_pGxToolExecuteView = new wxGxToolExecuteView(this, TOOLEXECUTECTRLID);
    //    AddPage(m_pGxToolExecuteView, m_pGxToolExecuteView->GetViewName(), false, m_pGxToolExecuteView->GetViewIcon());
    //    m_pGxToolExecuteView->Activate(application, pTaskExecConf);
    //    m_pGxToolExecuteView->SetGxToolExecute(pGxToolExecute);
    //    m_pApp->RegisterChildWindow(m_pGxToolExecuteView);
    //}

	return true;
}

void wxAxIdentifyView::Deactivate(void)
{
    //m_pGxToolExecuteView->Deactivate();
    //m_pApp->UnRegisterChildWindow(m_pGxToolExecuteView);

    //m_pGxToolboxView->Deactivate();
    //m_pApp->UnRegisterChildWindow(m_pGxToolboxView);
}

