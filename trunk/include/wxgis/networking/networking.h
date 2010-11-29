/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGIS Networking header. Network classes for remote server
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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

#include "wxgis/base.h"

#include "wx/socket.h"

#include <queue>

#define WIN 0
#define LIN 1

#ifdef __WXMSW__
	#define CURROS WIN
#else
	#define CURROS LIN
#endif

enum wxGISMessagePriority
{
	enumGISPriorityLowest   = 0, 
	enumGISPriorityLow      = 25,
	enumGISPriorityNormal   = 50,
	enumGISPriorityHight    = 75,
    enumGISPriorityHightest = 100
};

enum wxGISMessageDirection
{
    enumGISMsgDirUnk,
	enumGISMsgDirIn, 
	enumGISMsgDirOut
};

enum wxGISMessageState
{
    enumGISMsgStUnk,
	enumGISMsgStOk, 
	enumGISMsgStErr,
    enumGISMsgStRefuse,
    enumGISMsgStNote,
    enumGISMsgStGet,
    enumGISMsgStCmd,
    enumGISMsgStHello,
    enumGISMsgStBye,
    enumGISMsgStSnd,
    enumGISMsgStRcv
};

enum wxGISCommandState
{
    enumGISCmdStUnk,
    enumGISCmdStErr,
    enumGISCmdStAdd,
    enumGISCmdStDel,
    enumGISCmdStChng,
    enumGISCmdStStart,
    enumGISCmdStStop
};

enum wxGISUserType
{
    enumGISUserUnk,
    enumGISUserAnon,
    enumGISUserPass
};

#define FILETRANSFERBUFFSIZE 10240 
#define MSGBUFFSIZE 15000
#define SIMPLEMSGBUFFSIZE 1024

/** \class INetMessage networking.h
    \brief The network message interface class.
*/
class INetMessage
{
public:
    virtual ~INetMessage(void){};
	//pure virtual
    virtual const short GetPriority(void) = 0;
    virtual void SetPriority(short nPriority) = 0;
    virtual bool IsOk(void) = 0;
    virtual const wxGISMessageDirection GetDirection(void) = 0;
    virtual void SetDirection(wxGISMessageDirection nDirection) = 0;
};


typedef struct _msg
{
	INetMessage* pMsg;
	char nUserID;
	bool operator< (const _msg& x) const { return pMsg->GetPriority() < x.pMsg->GetPriority(); }
} WXGISMSG;

typedef std::priority_queue< MSG, std::deque<MSG> > WXGISMSGQUEUE;

/** \class INetConnection networking.h
    \brief The network connection interface class.
*/
class INetConnection
{
public:
    virtual ~INetConnection(void){};
	virtual bool Disconnect(void) = 0;
	virtual bool IsConnected(void) = 0;
	virtual const char GetUserID(void){return m_nUserID;};
	virtual GetMessage;
	virtual PutMessage;
protected:
	char m_nUserID;	
	//messages quere
	WXGISMSGQUEUE m_MsgQuere;
	//pop put msg
};
