//#include "weMonTCPClientPanel.h"
//#include "weServerSearchDlg.h"
//#include "wxRegExValidator.h"
//#include "weTCPConnectionObject.h"
//
/////////////////////////////////////////////////////////////////////////////
//BEGIN_EVENT_TABLE( weTCPClientPanel, wePropertyPage )
//	EVT_BUTTON( ID_SEARCHBT, weTCPClientPanel::OnSearchBtnClick )
//END_EVENT_TABLE()
//
//weTCPClientPanel::weTCPClientPanel( wxWindow* parent, wxXmlNode *conf_root, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wePropertyPage( parent, conf_root, id, pos, size, style )
//{
//	m_conf_root = conf_root;
//	m_sPort = wxString(wxT("1976"));
//    m_IPAddress = wxString(wxT("0.0.0.0"));
//	m_sName = wxString(_("New connection"));
//
//	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
//
//	wxFlexGridSizer* fgSizerMain;
//	fgSizerMain = new wxFlexGridSizer( 1, 2, 0, 0 );
//	fgSizerMain->SetFlexibleDirection( wxBOTH );
//	fgSizerMain->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
//	
//	wxFlexGridSizer* fgSizer1;
//	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
//	fgSizer1->SetFlexibleDirection( wxBOTH );
//	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
//	
//	m_staticText01 = new wxStaticText( this, wxID_ANY, _("Connection name"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText01->Wrap( -1 );
//	fgSizer1->Add( m_staticText01, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
//
//	m_textCtr_name = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &m_sName));
//	fgSizer1->Add( m_textCtr_name, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
//
//	m_staticText11 = new wxStaticText( this, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText11->Wrap( -1 );
//	fgSizer1->Add( m_staticText11, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
//	
//	m_textCtr_addr = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxRegExValidator(wxT("[0-9]{1,3}(.[0-9]{1,3}){3,3}"), &m_IPAddress) );
//	fgSizer1->Add( m_textCtr_addr, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
//	
//	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText12->Wrap( -1 );
//	fgSizer1->Add( m_staticText12, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
//	
//	m_textCtr_port = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_sPort));
//
//	fgSizer1->Add( m_textCtr_port, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
//	
//	fgSizerMain->Add( fgSizer1, 1, wxEXPAND, 5 );
//	
//	m_button_search = new wxButton( this, ID_SEARCHBT, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
//	fgSizerMain->Add( m_button_search, 0, wxALL, 5 );
//	
//	wxStaticBoxSizer* sbSizer2;
//	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Authentication") ), wxVERTICAL );
//	
//	wxFlexGridSizer* fgSubSizer;
//	fgSubSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
//	fgSubSizer->SetFlexibleDirection( wxBOTH );
//	fgSubSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
//	
//	m_staticText13 = new wxStaticText( this, wxID_ANY, _("User"), wxDefaultPosition, wxDefaultSize, 0 );
//	m_staticText13->Wrap( -1 );
//	fgSubSizer->Add( m_staticText13, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
//	
//	m_textCtrl_user = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxEXPAND, wxGenericValidator(&m_UserName) );
//	fgSubSizer->Add( m_textCtrl_user, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
//	
//	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Password"), wxDefaultPosition, wxDefaultSize, 0);
//	m_staticText14->Wrap( -1 );
//	fgSubSizer->Add( m_staticText14, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
//	
//	m_textCtrl_pass = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxEXPAND, wxGenericValidator(&m_Password)  );
//	fgSubSizer->Add( m_textCtrl_pass, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
//	
//	sbSizer2->Add( fgSubSizer, 1, wxEXPAND, 5 );
//	
//	fgSizerMain->Add( sbSizer2, 1, wxEXPAND, 5 );
//	
//	this->SetSizer( fgSizerMain );
//	this->Layout();
//	TransferDataToWindow();
//}
//
//weTCPClientPanel::~weTCPClientPanel()
//{
//}
//
//void weTCPClientPanel::OnSearchBtnClick( wxCommandEvent& evt )
//{
//	TransferDataFromWindow();
//	weSearchServerDlg dlg(this, wxAtoi(m_sPort));
//	if(dlg.ShowModal() == weSearchServerDlg::ID_ACCEPT)
//	{
//		m_IPAddress = dlg.m_ipaddress;
//		TransferDataToWindow();
//	}
//	evt.Skip();
//}
//
//IweObject* weTCPClientPanel::OnSave(void)
//{
//	if(!TransferDataFromWindow())
//	{
//		wxMessageBox(_("Some input values are not correct!"), _("Error"), wxOK | wxICON_ERROR);
//		return NULL;
//	}
//	wxXmlNode* pConnectionsNode = m_conf_root->GetChildren();
//	if(pConnectionsNode == NULL)
//		pConnectionsNode = new wxXmlNode(m_conf_root, wxXML_ELEMENT_NODE, wxT("Connections"));
//	wxXmlNode* pConnectionNode = new wxXmlNode(pConnectionsNode, wxXML_ELEMENT_NODE, wxT("Connection"));
//	wxXmlProperty* pPasswordProp = new wxXmlProperty(wxT("pass"), m_Password, NULL);
//	wxXmlProperty* pUserNameProp = new wxXmlProperty(wxT("user"), m_UserName, pPasswordProp);
//	wxXmlProperty* pPortProp = new wxXmlProperty(wxT("port"), m_sPort, pUserNameProp);
//	wxXmlProperty* pIPAddressProp = new wxXmlProperty(wxT("address"), m_IPAddress, pPortProp);
//	wxXmlProperty* pNameProp = new wxXmlProperty(wxT("name"), m_sName, pIPAddressProp);
//	pConnectionNode->SetProperties(pNameProp);
//	//pConnectionsNode->AddChild(pConnectionNode);
//	//create IweObject
//	IweObject* pObj = new weTCPConnectionObject(m_sName, m_IPAddress, m_sPort, m_UserName, m_Password, pConnectionNode);
//	return pObj;
//}
