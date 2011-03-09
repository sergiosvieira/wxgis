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

#include "wxgissrv/srv_framework/server.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxGISServer
/////////////////////////////////////////////////////////////////////////////////////////////////////
static IServerApplication* m_pGlobalApp;
extern WXDLLIMPEXP_GIS_FRW IServerApplication* GetApplication()
{
    return m_pGlobalApp;
}

wxGISServer::wxGISServer(void) : wxGISWorkPlugin(), m_pNetService(NULL), m_pAuthService(NULL), m_pCatalogService(NULL)
{
}

wxGISServer::~wxGISServer(void)
{
}

bool wxGISServer::Start(wxString sAppName, wxString sConfigDir, wxCmdLineParser& parser)
{
	if(!wxGISWorkPlugin::Start(sAppName, sConfigDir, parser))
		return false;

	wxLogDebug(wxT("OnStartMessageThread"));
    bool bResult = OnStartMessageThread();
    if(!bResult)
        return bResult;

	//GDAL
    CPLSetConfigOption( "GDAL_CACHEMAX", "128" );

	OGRRegisterAll();
	GDALAllRegister();
	//END GDAL

	wxLogDebug(wxT("Start workers service"));
    //3. Start workers service
	//3a. Create catalog
	m_pCatalogService = new wxRxCatalog();
    bResult = m_pCatalogService->Start(this, NULL);
    if(!bResult)
        return bResult;

	wxLogDebug(wxT("Start Auth service"));
	//4. Start Auth service
    wxXmlNode* pAuthNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("auth")));
    if(pAuthNode == NULL)
        pAuthNode = m_pConfig->CreateConfigNode(enumGISHKLM, wxString(wxT("auth")));
	m_pAuthService = new wxGISAuthService();
    bResult = m_pAuthService->Start(this, pAuthNode);
    if(!bResult)
        return bResult;

    wxXmlNode* pNetworkNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("network")));
    if(pNetworkNode == NULL)
        pNetworkNode = m_pConfig->CreateConfigNode(enumGISHKLM, wxString(wxT("network")));
	wxLogDebug(wxT("Start network service"));
    //5. Start network service
    m_pNetService = new wxGISNetworkService();
    bResult = m_pNetService->Start(this, pNetworkNode);
    if(!bResult)
	{
		wxLogError(_("Start network service failed!"));
        return bResult;
	}
    m_pGlobalApp = this;

	wxLogDebug(wxT("Started..."));
	return true;
}

void wxGISServer::Stop(void)
{
    //1. Stop network service
    if(m_pNetService)
        m_pNetService->Stop();
    wxDELETE(m_pNetService);

    //2. Stop auth service
    if(m_pAuthService)
        m_pAuthService->Stop();
    wxDELETE(m_pAuthService);

	OnStopMessageThread();

    //3. Stop workers service
	//3a. Delete catalog
    if(m_pCatalogService)
        m_pCatalogService->Stop();
    wxDELETE(m_pCatalogService);

	//GDAL
	GDALDestroyDriverManager();
	OGRCleanupAll();
	//END GDAL

	wxGISWorkPlugin::Stop();
}

void wxGISServer::PutInMessage(WXGISMSG msg)
{
    m_MsgQueue.push(msg);
}

void wxGISServer::PutOutMessage(WXGISMSG msg)
{
	if(msg.pMsg->IsOk() && m_pNetService)
		m_pNetService->PutOutMessage(msg);
	else
		wsDELETE(msg.pMsg);
}

IGISConfig* wxGISServer::GetConfig(void)
{
    return m_pConfig;
}

void wxGISServer::SetAuth(AUTHRESPOND stUserInfo)
{
	if(m_pNetService)
		m_pNetService->SetAuth(stUserInfo);
}

AUTHRESPOND wxGISServer::GetAuth(long nID)
{
	AUTHRESPOND respond;
	respond.bIsValid = false;
	if(m_pNetService)
		respond = m_pNetService->GetAuth(nID);
	return respond;
}


bool wxGISServer::SetupSys(wxString sSysPath)
{
	if(!wxGISWorkPlugin::SetupSys(sSysPath))
		return false;

    CPLSetConfigOption("GDAL_DATA", wgWX2MB( (sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator()).c_str() ) );
    return true;
}

void wxGISServer::SetDebugMode(bool bDebugMode)
{
	wxGISWorkPlugin::SetDebugMode(bDebugMode);		
	CPLSetConfigOption("CPL_DEBUG", bDebugMode == true ? "ON" : "OFF");
	CPLSetConfigOption("CPL_TIMESTAMP", "ON");
	CPLSetConfigOption("CPL_LOG_ERRORS", bDebugMode == true ? "ON" : "OFF");
}

bool wxGISServer::SetupLog(wxString sLogPath, wxString sNamePrefix)
{
	if(!wxGISWorkPlugin::SetupLog(sLogPath, wxT("srv")))
		return false;

	wxString sCPLLogPath = sLogPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal_log_srv.txt"));
	CPLSetConfigOption("CPL_LOG", wgWX2MB(sCPLLogPath.c_str()) );
    return true;
}
