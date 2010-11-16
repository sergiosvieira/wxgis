//#include "weCreateNetworkConnDlg.h"
//
/////////////////////////////////////////////////////////////////////////////
//BEGIN_EVENT_TABLE( weCreateNetworkConnDlg, wxDialog )
//	EVT_BUTTON( wxID_SAVE, weCreateNetworkConnDlg::OnSaveBtnClick )
//END_EVENT_TABLE()
//
//weCreateNetworkConnDlg::weCreateNetworkConnDlg( wxWindow* parent, wxIcon Icon, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), m_pConnObj(NULL)
//{
//	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
//	
//	bSizer = new wxBoxSizer( wxVERTICAL );
//	
//	m_choicebook = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT|wxCLIP_CHILDREN );
//	m_choicebook->SetMinSize( wxSize( 300,300 ) );
//	
//	bSizer->Add( m_choicebook, 1, wxEXPAND|wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
//	
//	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
//	bSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
//
//	m_sdbSizer = new wxStdDialogButtonSizer();
//	m_sdbSizerSave = new wxButton( this, wxID_SAVE );
//	m_sdbSizer->AddButton( m_sdbSizerSave );
//	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
//	m_sdbSizer->AddButton( m_sdbSizerCancel );
//	m_sdbSizer->Realize();
//	bSizer->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );
//	
//	this->SetSizer( bSizer );
//	this->Layout();
//	
//	this->Centre( wxBOTH );
//
//    // set frame icon
//	SetIcon(Icon);
//}
//
//weCreateNetworkConnDlg::~weCreateNetworkConnDlg()
//{
//}
//
//void weCreateNetworkConnDlg::OnSaveBtnClick( wxCommandEvent& evt )
//{
//	wxBookCtrlBase *currBook = m_choicebook;
//	wePropertyPage* pPPage = static_cast<wePropertyPage*>(currBook->GetCurrentPage());
//	if(pPPage != NULL)
//	{
//		m_pConnObj = pPPage->OnSave();
//	//SetReturnCode
//		if(m_pConnObj != NULL)
//			EndModal(wxID_SAVE);
//		else
//			wxLogError(_("weMonClient: Where is no connection object!"));
//	}
////	Close();
//}
//
//bool weCreateNetworkConnDlg::OnInit(NETWORKPLUGINS* pNetPluginsArr)
//{
//	if(pNetPluginsArr == NULL)
//		return false;
//	wxBookCtrlBase *currBook = m_choicebook;
//	for(size_t i = 0; i < pNetPluginsArr->size(); i++)
//	{
//		IweNetworkClientPlugin* plugin = pNetPluginsArr->at(i);
//		wxString sName = plugin->GetName();
//		wePropertyPage* pPPage = plugin->GetPropertyPage(currBook);
//		currBook->AddPage(pPPage, sName);
//	}
//	return true;
//}
