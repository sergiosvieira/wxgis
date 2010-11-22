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

//#define ANONIM 0
//#define PASS 1

#define FILETRANSFERBUFFSIZE 10240 
#define MSGBUFFSIZE 15000
#define SIMPLEMSGBUFFSIZE 1024

class INetMessage
{
public:
    virtual ~INetMessage(void){};
	//pure virtual
    virtual short GetPriority(void) = 0;
    virtual void SetPriority(short nPriority) = 0;
    //virtual bool operator< (const INetMessage& msg) const = 0;
    //virtual INetMessage& operator= (const INetMessage& oSource) = 0;
    virtual bool IsOk(void) = 0;
    virtual wxGISMessageDirection GetDirection(void) = 0;
    virtual void SetDirection(wxGISMessageDirection nDirection) = 0;
};

class INetConnection
{
    virtual ~INetConnection(void){};
	//pure virtual
    /** \fn wxXmlNode* GetProperties(void)
     *  \brief Get Properties of plugin.
     *  \return The properties of the plugin
	 *
	 *  It should be the new wxXmlNode (not a copy of setted properties)
     */	 	
	virtual wxXmlNode* GetProperties(void) = 0;
    /** \fn void SetProperties(wxXmlNode* pProp)
     *  \brief Set Properties of plugin.
     *  \param pProp The properties of the plugin
	 *
	 *  Executed while LoadPlugins (after flugin created). 
     */	  
	virtual void SetProperties(wxXmlNode* pProp) = 0;
	//virtual wxString GetName(void) = 0;
	//messages quere
};

//typedef std::vector<INetConnection*> NETCONNARRAY;