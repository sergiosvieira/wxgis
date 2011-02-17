/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServer class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
#include "wxgis/networking/processor.h"

/** \class wxGxRemoteServer gxremoteserver.h
    \brief A Remote Server GxObject.
*/
class WXDLLIMPEXP_GIS_RS wxGxRemoteServer :
	public IGxObjectContainer,
	public INetCallback,
	public wxGISNetMessageProcessor,
	public INetMessageReceiver
{
public:
	wxGxRemoteServer(INetClientConnection* pNetConn);
	virtual ~wxGxRemoteServer(void);
	//IGxObject
	virtual bool Attach(IGxObject* pParent, IGxCatalog* pCatalog);
	virtual void Detach(void);
	virtual wxString GetName(void);
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void){return CPLString();};
	virtual wxString GetCategory(void){return wxString(_("Remote Server"));};
	virtual void Refresh(void);
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){/*LoadChildren();*/ return m_Children.size() > 0 ? true : false;};
    //wxGxRemoteServer
    virtual bool Connect(void);
    virtual bool Disconnect(void);
    virtual bool IsConnected(void);
    virtual wxXmlNode* GetProperties(void);
	//INetCallback
	virtual void OnConnect(void);
	virtual void OnDisconnect(void);
	virtual void PutInMessage(WXGISMSG msg);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
	//wxGxRemoteServer
	virtual void LoadChildren(void);
	virtual void EmptyChildren(void);
	virtual void PutOutMessage(WXGISMSG msg);
protected:
    INetClientConnection* m_pNetConn;
	bool m_bIsChildrenLoaded;
    bool m_bAuth;
    wxMsgInThread *m_pMsgInThread;
	int m_nChildCount;
};

/** \class IRxObjectClient gxremoteserver.h
    \brief The interface class for Remote GxObjects (RxObjects).
*/
class IRxObjectClient
{
public:
	virtual ~IRxObjectClient(void){};
	virtual bool Init(wxGxRemoteServer *pGxRemoteServer, wxXmlNode* pProperties) = 0;
};