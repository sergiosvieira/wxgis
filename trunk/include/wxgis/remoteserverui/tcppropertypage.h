///******************************************************************************
// * Project:  WebEye Monitoring Utility
// * Purpose:  TCP client panel class.
// * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
// ******************************************************************************
//*   Copyright (C) 2008  Bishop
//*
//*    This program is free software: you can redistribute it and/or modify
//*    it under the terms of the GNU General Public License as published by
//*    the Free Software Foundation, either version 3 of the License, or
//*    (at your option) any later version.
//*
//*    This program is distributed in the hope that it will be useful,
//*    but WITHOUT ANY WARRANTY; without even the implied warranty of
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//*    GNU General Public License for more details.
//*
//*    You should have received a copy of the GNU General Public License
//*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ****************************************************************************/
//#ifndef __weMonTCPClientPanel__
//#define __weMonTCPClientPanel__
//
//#include <wx/string.h>
//#include <wx/stattext.h>
//#include <wx/gdicmn.h>
//#include <wx/font.h>
//#include <wx/colour.h>
//#include <wx/settings.h>
//#include <wx/textctrl.h>
//#include <wx/sizer.h>
//#include <wx/button.h>
//#include <wx/statbox.h>
//#include <wx/panel.h>
//#include "interfacesui.h"
//
//
/////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////
///// Class TCPClientPanel
/////////////////////////////////////////////////////////////////////////////////
//class weTCPClientPanel : public wePropertyPage 
//{
//public:
//  enum
//  {
//		ID_SEARCHBT = wxID_HIGHEST + 1,
//  };
//private:
//    wxString m_sPort;
//    wxString m_UserName;
//    wxString m_Password;
//    wxString m_IPAddress;
//    wxString m_sName;
//	wxXmlNode *m_conf_root;
//protected:
//	wxStaticText* m_staticText01;
//	wxTextCtrl* m_textCtr_name;
//	wxStaticText* m_staticText11;
//	wxTextCtrl* m_textCtr_addr;
//	wxStaticText* m_staticText12;
//	wxTextCtrl* m_textCtr_port;
//	wxButton* m_button_search;
//	wxStaticText* m_staticText13;
//	wxTextCtrl* m_textCtrl_user;
//	wxStaticText* m_staticText14;
//	wxTextCtrl* m_textCtrl_pass;
//
//public:
//	weTCPClientPanel( wxWindow* parent, wxXmlNode *conf_root = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 269,159 ), long style = wxCLIP_CHILDREN|wxNO_BORDER|wxTAB_TRAVERSAL );
//	~weTCPClientPanel();
//	virtual IweObject* OnSave(void);	
//	void OnSearchBtnClick( wxCommandEvent& evt );
//
//	DECLARE_EVENT_TABLE()	
//};
//
//#endif //__weMonTCPClientPanel__
