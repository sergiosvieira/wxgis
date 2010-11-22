/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network plugin class.
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

#include "wxgis/remoteserver/remoteserver.h"

/** \class wxClientUDPNotifier tcpnetplugin.h
    \brief The thread listening answers from remote servers on broadcast messages.
*/
//class wxClientUDPNotifier : public wxThread
//{
//public:
//	wxClientUDPNotifier(INetSearch* pPlugin);
//    virtual void *Entry();
//    virtual void OnExit();
//	virtual void SendBroadcastMsg(void);
//private:
//	wxDatagramSocket *m_socket;
//	INetSearch* m_pPlugin;
//	int m_nAdvPort;
//};

///** \class wxClientTCPNetConnectionFactory tcpnetplugin.h
//    \brief The thread listening answers from remote servers on broadcast messages.
//*/
//class wxClientTCPNetConnectionFactory : 
//	public wxObject,
//	public INetSearch
//{
//    DECLARE_DYNAMIC_CLASS(wxClientTCPNetConnectionFactory)
//public:
//	wxClientTCPNetConnectionFactory(void);
//	~wxClientTCPNetConnectionFactory(void);
//	//INetSearch
//	virtual wxString GetName(void){return wxString(_("TCP/IP Network"));};
//	virtual bool StartServerSearch();
//	virtual bool StopServerSearch(){return false;};
//	virtual bool CanStopServerSearch(){return false;};
//	virtual void SetCallback(INetSearchCallback* pCallback);
//	virtual INetSearchCallback* GetCallback(void);
//protected:
//    INetSearchCallback* m_pCallback;
//    wxXmlNode* m_pProperties;
//};
//
///** \class wxClientTCPNetConnection tcpnetplugin.h
//    \brief The connection to communicate with server.
//*/
//class wxClientTCPNetConnection : 
//	public wxObject,
//	public INetConnection
//{
//    DECLARE_DYNAMIC_CLASS(wxClientTCPNetConnection)
//public:
//	wxClientTCPNetConnection(void);
//	~wxClientTCPNetConnection(void);
//	//INetConnection
//	virtual wxXmlNode* GetProperties(void);
//	virtual void SetProperties(wxXmlNode* pProp);
//protected:
//    wxXmlNode* m_pProperties;
//};