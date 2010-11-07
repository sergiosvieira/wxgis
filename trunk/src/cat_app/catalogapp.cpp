/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main application class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/cat_app/catalogapp.h"
#include "wxgis/cat_app/catalogframe.h"

#include <locale.h>
#include "ogrsf_frmts/ogrsf_frmts.h"
#include "ogr_api.h"

IMPLEMENT_APP(wxGISCatalogApp);

wxGISCatalogApp::wxGISCatalogApp(void)
{
}

wxGISCatalogApp::~wxGISCatalogApp(void)
{
	//wxLogMessage(_("wxGISCatalogApp is exiting..."));

	GDALDestroyDriverManager();
	OGRCleanupAll();

	wxDELETE(m_pConfig);
}

bool wxGISCatalogApp::OnInit()
{
	wxGISCatalogFrame* frame = new wxGISCatalogFrame(NULL, wxID_ANY, _("wxGIS Catalog"), wxDefaultPosition, wxSize(800, 480) );
#ifdef WXGISPORTABLE
    m_pConfig = new wxGISAppConfig(frame->GetAppName(), CONFIG_DIR, true);
#else
	m_pConfig = new wxGISAppConfig(frame->GetAppName(), CONFIG_DIR, false);
#endif

	//setup loging
	wxString sLogDir = m_pConfig->GetLogDir();
    if(!frame->SetupLog(sLogDir))
        return false;

	//setup locale
	wxString sLocale = m_pConfig->GetLocale();
	wxString sLocaleDir = m_pConfig->GetLocaleDir();
    if(!frame->SetupLoc(sLocale, sLocaleDir))
        return false;

   	//setup sys
    wxString sSysDir = m_pConfig->GetSysDir();
    if(!frame->SetupSys(sSysDir))
        return false;

   	//setup debug
	bool bDebugMode = m_pConfig->GetDebugMode();
    frame->SetDebugMode(bDebugMode);

    //some default GDAL
    CPLSetConfigOption( "GDAL_CACHEMAX", "128" );
    //GDAL_MAX_DATASET_POOL_SIZE
    //OGR_ARC_STEPSIZE

	OGRRegisterAll();
	GDALAllRegister();

	wxLogMessage(_("wxGISCatalogApp: Start main frame"));

	//store values
	m_pConfig->SetLogDir(sLogDir);
	m_pConfig->SetLocale(sLocale);
	m_pConfig->SetLocaleDir(sLocaleDir);
	m_pConfig->SetSysDir(sSysDir);
	m_pConfig->SetDebugMode(bDebugMode);

    if(!frame->Create(m_pConfig))
        return false;
    SetTopWindow(frame);
    frame->Show();

	return true;
}
