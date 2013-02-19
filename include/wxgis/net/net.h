/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGIS Networking header. Network classes for remote server
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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

#include "wxgis/core/core.h"

#include "wx/socket.h"

#if wxUSE_IPV6
    typedef wxIPV6address IPaddress;
#else
    typedef wxIPV4address IPaddress;
#endif

/*
#define WIN 0
#define LIN 1

#ifdef __WXMSW__
	#define CURROS WIN
#else
	#define CURROS LIN
#endif
    */

/** \enum wxGISMessagePriority net.h
 *  \brief The message priority type.
 */
enum wxGISMessagePriority
{
	enumGISPriorityLowest   = 0, 
	enumGISPriorityLow      = 25,
	enumGISPriorityNormal   = 50,
	enumGISPriorityHigh     = 75,
    enumGISPriorityHighest  = 100
};

/** \enum wxGISNetCommand
    \brief A network command types.

    This is predefined command types to network message.
*/
enum wxGISNetCommand
{
    enumGISNetCmdUnk = 0,/**< The type is unknown */ 
    enumGISNetCmdHello,  /**< The connect to server command - usually with login and pass */ 
    enumGISNetCmdCmd,    /**< The network message is a command */ 
    enumGISNetCmdNote,   /**< The network message is a notification */ 
    enumGISNetCmdBye,    /**< The network message is a disconnect notification */ 
    enumGISNetCmdSetExitSt  /**< The network message is a exit state */
};

/** \enum wxGISNetCommand
    \brief A network command description types.

    This is command description types to network message.
*/
enum wxGISNetCommandState
{    
    enumGISNetCmdStUnk = 0,	/**< The type is unknown */
    //status
    enumGISNetCmdStOk,      /**< The indicator for any net command - OK */
	enumGISNetCmdStErr,     /**< The indicator for any net command - Error */
    enumGISNetCmdStAccept,  /**< The indicator for enumGISNetCmdHello net command - Connection accepted */
    enumGISNetCmdStRefuse,  /**< The indicator for enumGISNetCmdHello net command - Connection refused */
    //command
    enumGISCmdGetChildren,  /**< The indicator for enumGISNetCmdCmd net command - Get object children */
    enumGISCmdGetDetails,   /**< The indicator for enumGISNetCmdCmd net command - Get object details */
    enumGISCmdStAdd,        /**< The indicator for enumGISNetCmdCmd net command - Add new object */
    enumGISCmdStDel,        /**< The indicator for enumGISNetCmdCmd net command - Delete object */
    enumGISCmdStChng,       /**< The indicator for enumGISNetCmdCmd net command - Change object */  
    enumGISCmdSetParam,      /**< The indicator for enumGISNetCmdCmd net command - Set parameter */
    //exit state
    enumGISNetCmdStExit,
    enumGISNetCmdStNoExit,
    enumGISNetCmdStExitAfterExec,
    //task
    enumGISCmdStStart,      /**< The indicator for enumGISNetCmdCmd net command - Start task execution - change state? */
    enumGISCmdStStop,       /**< The indicator for enumGISNetCmdCmd net command - Stop task execution - change state? */    
    enumGISCmdStPriority,   /**< The indicator for enumGISNetCmdCmd net command - Set task priority - change state? */    
    enumGISCmdNoteMsg,      /**< The indicator for enumGISNetCmdNote net notification - New message from task*/   
    //enumGISCmdNoteVol,      /**< The indicator for enumGISNetCmdNote net notification - Task volume changed*/ 
    //enumGISCmdNotePercent,  /**< The indicator for enumGISNetCmdNote net notification - Task percent changed*/ 
    //other
    enumGISNetCmdStAuth     /**< The indicator for enumGISNetCmdHello net command - connect server */


        /*/notifications
    ,
    enumGISMsgStSnd,
    enumGISMsgStRcv*/
};

/*
#include <wx/socket.h>
#include <wx/xml/xml.h>

#include <queue>

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
*//*
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
    virtual const unsigned char* GetData(void) = 0;
    virtual const size_t GetDataLen(void) = 0;
    virtual const wxGISMessageState GetState(void) = 0;
    virtual void SetState(wxGISMessageState nState) = 0;
	virtual void SetDestination(wxString sDst) = 0;
	virtual const wxString GetDestination(void) = 0;
    virtual wxXmlNode* GetRoot(void) = 0;
};

DEFINE_SHARED_PTR(INetMessage);

typedef struct _msg
{
	INetMessageSPtr pMsg;
	long nUserID;
	bool operator< (const struct _msg& x) const { return pMsg->GetPriority() < x.pMsg->GetPriority(); }
} WXGISMSG;

typedef std::priority_queue< WXGISMSG, std::deque<WXGISMSG> > WXGISMSGQUEUE;

/** \class INetMessageReceiver networking.h
    \brief A network message class.
*//*
class INetMessageReceiver
{
public:
	virtual ~INetMessageReceiver(void){};
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode) = 0;
};

/** \class INetMessageProcessor networking.h
    \brief A network message class.
*//*
class INetMessageProcessor
{
public:
	virtual ~INetMessageProcessor(void){};
    virtual WXGISMSG GetInMessage(void) = 0;
    virtual void ProcessMessage(WXGISMSG msg) = 0;
	virtual void ClearMessageQueue(void) = 0;
	virtual void AddMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver) = 0;
	virtual void DelMessageReceiver(wxString sName, INetMessageReceiver* pNetMessageReceiver) = 0;
	virtual void ClearMessageReceiver(void) = 0;
};
*/