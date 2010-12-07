/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network classes.
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
#include "wxgis/networking/tcpnetwork.h"

class wxClientTCPNetFactory;

/** \class wxClientUDPNotifier tcpnetfactory.h
    \brief The thread listening answers from remote servers on broadcast messages.
*/
class wxClientUDPNotifier : public wxThread
{
public:
	wxClientUDPNotifier(wxClientTCPNetFactory* pFactory);
    virtual void *Entry();
    virtual void OnExit();
	virtual void SendBroadcastMsg(void);
private:
	wxDatagramSocket *m_socket;
	wxClientTCPNetFactory* m_pFactory;
};

/** \class wxClientTCPNetFactory tcpnetfactory.h
    \brief The factory to create net connections.
*/
class WXDLLIMPEXP_GIS_RS wxClientTCPNetFactory : 
	public wxObject,
	public INetConnFactory
{
    DECLARE_DYNAMIC_CLASS(wxClientTCPNetFactory)
public:
	wxClientTCPNetFactory(void);
	~wxClientTCPNetFactory(void);
	//INetConnFactory
	virtual wxString GetName(void){return wxString(_("TCP/IP Network"));};
	virtual bool StartServerSearch();
	virtual bool StopServerSearch();
	virtual bool CanStopServerSearch(){return true;};
	virtual bool IsServerSearching(){return false;};
	virtual void SetCallback(INetSearchCallback* pCallback){m_pCallback = pCallback;};
	virtual INetSearchCallback* GetCallback(void){return m_pCallback;};
	virtual wxXmlNode* GetProperties(void);
	virtual void SetProperties(const wxXmlNode* pProp);
	INetClientConnection* GetConnection(wxXmlNode* pProp);
	//virtual char GetID(void){return m_nID;};
	//virtual void SetID(char nID){m_nID = nID;};
	//wxClientTCPNetFactory
	virtual unsigned short GetAdvPort(void){return m_nAdvPort;};
	virtual unsigned short GetPort(void){return m_nPort;};
protected:
    INetSearchCallback* m_pCallback;
	wxString m_sAddr;
	unsigned short m_nPort, m_nAdvPort;
	wxClientUDPNotifier* m_pClientUDPNotifier;
	//char m_nID;
};

/** \class wxClientTCPNetConnection tcpnetfactory.h
    \brief The connection to communicate with server.
*/
class WXDLLIMPEXP_GIS_RS wxClientTCPNetConnection : 
	public wxObject,
	public INetClientConnection
{
    DECLARE_DYNAMIC_CLASS(wxClientTCPNetConnection)
public:
	wxClientTCPNetConnection(void);
	~wxClientTCPNetConnection(void);
	//INetConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
	virtual wxString GetName(void){return m_sConnName;};
	//virtual WXGISMSG GetInMessage(void) = 0;
    virtual void PutInMessage(WXGISMSG msg);
	//INetClientConnection
	virtual wxXmlNode* GetProperties(void);
	virtual bool SetProperties(const wxXmlNode* pProp);
	virtual void SetCallback(INetCallback* pNetCallback){m_pCallBack = pNetCallback;};
    virtual wxString GetUser(void){return m_sUserName;};
    virtual wxString GetCryptPasswd(void){return m_sCryptPass;};
protected:
	wxString m_sConnName;
	wxString m_sUserName;
	wxString m_sCryptPass;
	wxString m_sIP;
	wxString m_sPort;
	wxSocketClient* m_pSock;
	wxNetTCPReader* m_pClientTCPReader;
	wxNetTCPWriter* m_pClientTCPWriter;
//	wxNetTCPWaitlost* m_pClientTCPWaitlost;
	INetCallback* m_pCallBack;

};