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
	public INetMessageReceiver,
	public IGxObject
{
public:
	wxRxObject(void);
	virtual ~wxRxObject(void);
	//IGxObject
	virtual wxString GetCategory(void){return wxString(_("Remote item"));};
	virtual void Detach(void);
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
	//IRxObjectClient
	virtual bool Init(wxGxRemoteServer *pGxRemoteServer, wxXmlNode* pProperties);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode) = 0;
protected:
    wxGxRemoteServer* m_pGxRemoteServer;
	wxString m_sDst, m_sName;
};

/** \class wxRxObjectContainer gxremoteserver.h
    \brief The base class for Remote GxObjectContainers (RxObjectContainers).
*/
class WXDLLIMPEXP_GIS_RS wxRxObjectContainer : 
	public IRxObjectClient,
	public INetMessageReceiver,
	public IGxObjectContainer
{
public:
	wxRxObjectContainer(void);
	virtual ~wxRxObjectContainer(void);
	//IGxObject
	virtual void Detach(void);
	virtual wxString GetName(void){return m_sName;};
    virtual wxString GetBaseName(void){return GetName();};
	virtual wxString GetCategory(void){return wxString(_("Remote container"));};
	virtual void Refresh(void);
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){LoadChildren(); return m_Children.size() > 0 ? true : false;};
	//IRxObjectClient
	virtual bool Init(wxGxRemoteServer *pGxRemoteServer, wxXmlNode* pProperties);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
	//wxRxMonitoringDatabases
	virtual void EmptyChildren(void);
	virtual bool LoadChildren(void);
protected:
    wxGxRemoteServer* m_pGxRemoteServer;
	wxString m_sDst, m_sName;
	bool m_bIsChildrenLoaded;
	int m_nChildCount;
	wxArrayString m_sDstArray;
};