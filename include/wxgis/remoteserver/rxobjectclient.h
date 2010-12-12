/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxRxObject class.
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

#include "wxgis/remoteserver/gxremoteserver.h"

/** \class wxRxObject gxremoteserver.h
    \brief The base class for Remote GxObjects (RxObjects).
*/
class WXDLLIMPEXP_GIS_RS wxRxObject : 
	public IRxObjectClient,
	public INetMessageReceiver
{
public:
	wxRxObject(void);
	virtual ~wxRxObject(void);
	//IRxObjectClient
	virtual bool Init(wxGxRemoteServer *pGxRemoteServer, wxXmlNode* pProperties);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
protected:
    wxGxRemoteServer* m_pGxRemoteServer;
	wxString m_sDst;
};

/** \class wxRxObjectContainer gxremoteserver.h
    \brief The base class for Remote GxObjectContainers (RxObjectContainers).
*/
class WXDLLIMPEXP_GIS_RS wxRxObjectContainer : 
	public wxRxObjectContainer
{
public:
	wxRxObjectContainer(void);
	virtual ~wxRxObjectContainer(void);
	//
	bool LoadChildren(void);
};