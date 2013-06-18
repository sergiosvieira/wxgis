/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main application class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012 Bishop
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
#include "wxgis/cat_app/catalogapp.h"
#include "wxgis/core/config.h"

#include <locale.h>

//#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "gdal_priv.h"

IMPLEMENT_APP(wxGISCatalogApp)

wxGISCatalogApp::wxGISCatalogApp(void) : wxApp()
{

#if defined __WXGTK__ && !wxCHECK_VERSION(2, 9, 5)
    wxSetEnv(wxT("UBUNTU_MENUPROXY"), wxT("0"));
#endif
    m_bMainFrameCreated = false;
}

wxGISCatalogApp::~wxGISCatalogApp(void)
{
    wxGISAppConfig oConfig = GetConfig();

    SerializeLibs();

	GDALDestroyDriverManager();
	OGRCleanupAll();

	UnLoadLibs();

//the config state storing to files while destruction config class (smart pointer)
//on linux saving file in destructor produce segmentation fault
	if(oConfig.IsOk())
		oConfig.Save(enumGISHKCU);

}

bool wxGISCatalogApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;
    wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

    //create application/main frame
    m_pMainFrame = new wxGISCatalogFrame(NULL, wxID_ANY, wxString(_("wxGIS Catalog")), wxDefaultPosition, wxSize(800, 480) );

	//setup loging
	wxString sLogDir = oConfig.GetLogDir();
    if(!m_pMainFrame->SetupLog(sLogDir))
        return false;

	//setup locale
	wxString sLocale = oConfig.GetLocale();
	wxString sLocaleDir = oConfig.GetLocaleDir();
    if(!m_pMainFrame->SetupLoc(sLocale, sLocaleDir))
        return false;

   	//setup sys
    wxString sSysDir = oConfig.GetSysDir();
    if(!m_pMainFrame->SetupSys(sSysDir))
        return false;

   	//setup debug
	bool bDebugMode = oConfig.GetDebugMode();
    m_pMainFrame->SetDebugMode(bDebugMode);

    //some default GDAL
	wxString sGDALCacheMax = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), wxString(wxT("128")));
	CPLSetConfigOption ( "GDAL_CACHEMAX", sGDALCacheMax.mb_str() );
    CPLSetConfigOption ( "LIBKML_USE_DOC.KML", "no" );
    CPLSetConfigOption ( "GDAL_USE_SOURCE_OVERVIEWS", "ON" );
    //GDAL_MAX_DATASET_POOL_SIZE
    //OGR_ARC_STEPSIZE

	OGRRegisterAll();
	GDALAllRegister();

	wxLogVerbose(_("wxGISCatalogApp: Start main frame"));

	//store values

	oConfig.SetLogDir(sLogDir);
	oConfig.SetLocale(sLocale);
	oConfig.SetLocaleDir(sLocaleDir);
	oConfig.SetSysDir(sSysDir);
	oConfig.SetDebugMode(bDebugMode);

	//gdal
	oConfig.Write(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/cachemax")), sGDALCacheMax);

	oConfig.Save();

    wxString sKey(wxT("wxGISCommon/libs"));
    //load libs
	wxXmlNode* pLibsNode = oConfig.GetConfigNode(enumGISHKCU, sKey);
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = oConfig.GetConfigNode(enumGISHKLM, sKey);
	if(pLibsNode)
		LoadLibs(pLibsNode);

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    //SetTopWindow(frame);

	return true;
}

// create the file system watcher here, because it needs an active loop
void wxGISCatalogApp::OnEventLoopEnter(wxEventLoopBase* loop)
{

    if(m_pMainFrame && !m_bMainFrameCreated)
    {
        if(!m_pMainFrame->CreateApp())
            return;

        m_bMainFrameCreated = true;
        m_pMainFrame->Show(true);
    }
}
