/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGISSearchServerDlg class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#pragma once

#include "wxgis/remoteserverui/remoteserverui.h"

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/gauge.h>
#include <wx/dialog.h>
#include <wx/imaglist.h>
#include <wx/icon.h>
#include <wx/socket.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

/** \class wxClientUDPNotifier serversearchdlg.h
    \brief The thread listening answers from remote servers on broadcast messages.
*/
class wxGISSearchServerDlg;

class wxClientUDPNotifier : public wxThread
{
public:
	wxClientUDPNotifier(wxGISSearchServerDlg* pParent, int nAdvPort = 1977);
    virtual void *Entry();
    virtual void OnExit();
	virtual void SendBroadcastMsg(void);
private:
	wxDatagramSocket *m_socket;
	wxGISSearchServerDlg* m_pParentDlg;
	int m_nAdvPort;
};

/** \class wxGISSearchServerDlg serversearchdlg.h
    \brief The dialog to search remote servers.
*/

class wxGISSearchServerDlg : public wxDialog 
{
public:
  enum
  {
		ID_SEARCHBT = wxID_HIGHEST + 1,
		ID_STOPBT,
		ID_ACCEPT
  };
public:
	wxGISSearchServerDlg( wxWindow* parent, size_t port = 1976, wxWindowID id = wxID_ANY, const wxString& title = _("Search Server"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,300 ), long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
	~wxGISSearchServerDlg();
	void AddHost(wxString sPort, wxString sName, wxString sHost, wxString sIP, wxString sBanner);
protected:
	void OnSearch( wxCommandEvent& event );
	void OnSearchUI( wxUpdateUIEvent& event );
	void OnStop( wxCommandEvent& event );
	void OnStopUI( wxUpdateUIEvent& event );
	void OnAccept( wxCommandEvent& event );
	void OnAcceptUI( wxUpdateUIEvent& event );
	void OnClose(wxCloseEvent& event);
protected:
	wxListCtrl* m_listCtrl;
	wxButton* m_button_search;
	wxButton* m_button_stop;
	wxButton* m_button_accept;
	wxGauge* m_gauge;
	
private:
	wxString m_ipaddress;
	wxImageList m_ImageList;
	bool m_bContinueSearch;
	size_t m_port;
	wxClientUDPNotifier* m_pClientUDPNotifier;
	
	DECLARE_EVENT_TABLE()		
};
