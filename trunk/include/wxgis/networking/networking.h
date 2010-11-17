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