/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISServer class. Main server class
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2011 Bishop
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

#include "wxgissrv/srv_framework/network.h"
#include "wxgissrv/srv_framework/auth.h"
#include "wxgissrv/srv_framework/catalog.h"
#include "wxgissrv/srv_framework/plugin.h"
#include "wxgis/networking/processor.h"

/** \class wxGISServer server.h
    \brief The main Server class.
*/
WXDLLIMPEXP_GIS_FRW IServerApplication* GetApplication();

class WXDLLIMPEXP_GIS_FRW wxGISServer : 
	public IServerApplication,
	public wxGISNetMessageProcessor,
	public wxGISWorkPlugin
{
public:
    wxGISServer(void);
    virtual ~wxGISServer(void);
    virtual bool Start(wxString sAppName = wxT("wxGISServer"), wxString sConfigDir = CONFIG_DIR, int argc = 0, char** argv = NULL);
    virtual void Stop(void);
    // IServerApplication
    virtual IGISConfig* GetConfig(void);
    virtual void PutInMessage(WXGISMSG msg);
    virtual void PutOutMessage(WXGISMSG msg);
	virtual void SetAuth(AUTHRESPOND stUserInfo);
	virtual AUTHRESPOND GetAuth(long nID);
protected:
    virtual bool SetupSys(wxString sSysPath);
    virtual void SetDebugMode(bool bDebugMode);
    virtual bool SetupLog(wxString sLogPath, wxString sNamePrefix);
protected:
    wxGISNetworkService* m_pNetService;
	wxGISAuthService* m_pAuthService;
	wxRxCatalog* m_pCatalogService;//catalog of server objects
};


