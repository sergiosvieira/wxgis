/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Initializer class for logs, locale, libs and etc.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/core/init.h"
#include "wxgis/core/config.h"

////////////////////////////////////////////////////////////////////////////////
// wxGISAppWithLibs
////////////////////////////////////////////////////////////////////////////////

wxGISAppWithLibs::wxGISAppWithLibs(void)
{
}

wxGISAppWithLibs::~wxGISAppWithLibs(void)
{
}


void wxGISAppWithLibs::LoadLibs(wxXmlNode* pRootNode)
{
	wxXmlNode *child = pRootNode->GetChildren();
	while(child)
	{
		wxString sPath = child->GetAttribute(wxT("path"), wxEmptyString);
		if(sPath.Len() > 0)
		{
			//check for doubles
			if(m_LibMap[sPath] != NULL)
			{
				child = child->GetNext();
				continue;
			}

			wxDynamicLibrary* pLib = new wxDynamicLibrary(sPath);
			if(pLib != NULL)
			{
				wxLogMessage(_("wxGISAppWithLibs: Library %s loaded"), sPath.c_str());
				m_LibMap[sPath] = pLib;
			}
			else
				wxLogError(_("wxGISAppWithLibs: Error loading library %s"), sPath.c_str());
		}
		child = child->GetNext();
	}
}

void wxGISAppWithLibs::SerializeLibs()
{
	wxGISAppConfigSPtr pConfig;
	pConfig = GetConfig();
	if(!pConfig)
		return;
	wxXmlNode* pLibsNode(NULL);
	pLibsNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		pConfig->DeleteNodeChildren(pLibsNode);
	else
		pLibsNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));

	if(!pLibsNode)
		return;

    for(LIBMAP::iterator item = m_LibMap.begin(); item != m_LibMap.end(); ++item)
	{
		wxXmlNode* pNewNode = new wxXmlNode(pLibsNode, wxXML_ELEMENT_NODE, wxString(wxT("lib")));
		pNewNode->AddAttribute(wxT("path"), item->first);

		wxFileName FName(item->first);
		wxString sName = FName.GetName();

		pNewNode->AddAttribute(wxT("name"), sName);
	}
}


void wxGISAppWithLibs::UnLoadLibs()
{
    for(LIBMAP::iterator item = m_LibMap.begin(); item != m_LibMap.end(); ++item)
		wxDELETE(item->second);
}
////////////////////////////////////////////////////////////////////////////////
// wxGISInitializer
////////////////////////////////////////////////////////////////////////////////

wxGISInitializer::wxGISInitializer(void) : m_pLocale(NULL), m_pszOldLocale(NULL)
{
}

wxGISInitializer::~wxGISInitializer(void)
{
}

bool wxGISInitializer::Initialize(const wxString &sAppName, const wxString &sLogFilePrefix, wxCmdLineParser& parser)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
        return false;

	wxString sLogDir = pConfig->GetLogDir();
	if(!SetupLog(sLogDir, sLogFilePrefix))
        return false;
	wxLogMessage(_("wxGISInitializer: %s %s is initializing..."), sAppName.c_str(), wxString(APP_VER).c_str());

	if(!SetupLoc(pConfig->GetLocale(), pConfig->GetLocaleDir()))
        return false;

	//setup sys dir
	wxString sSysDir = pConfig->GetSysDir();
	if(!SetupSys(sSysDir))
        return false;

	bool bDebugMode = pConfig->GetDebugMode();
	SetDebugMode(bDebugMode);

	//store values
	//m_pConfig->SetLogDir(sLogDir);
	//m_pConfig->SetLocale(sLocale);
	//m_pConfig->SetLocaleDir(sLocaleDir);
	//m_pConfig->SetSysDir(sSysDir);
	//m_pConfig->SetDebugMode(bDebugMode);

    //load libs
	wxXmlNode* pLibsNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	pLibsNode = pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("wxGISCommon/libs")));
	if(pLibsNode)
		LoadLibs(pLibsNode);
	return true;
}

void wxGISInitializer::Uninitialize()
{
    if(m_pszOldLocale != NULL)
		setlocale(LC_NUMERIC, m_pszOldLocale);
    wxDELETE(m_pLocale);
	wxDELETE(m_pszOldLocale);

	ReleaseConfig();

	UnLoadLibs();
}

bool wxGISInitializer::SetupSys(const wxString &sSysPath)
{
	if(!wxDirExists(sSysPath))
	{
		wxLogError(wxString::Format(_("wxGISInitializer: System dir is absent! Lookup path '%s'"), sSysPath.c_str()));
		return false;
	}
    return true;
}

void wxGISInitializer::SetDebugMode(bool bDebugMode)
{
}

bool wxGISInitializer::SetupLog(const wxString &sLogPath, const wxString &sNamePrefix)
{
	if(sLogPath.IsEmpty())
	{
		wxLogError(_("wxGISInitializer: Failed to get log dir"));
        return false;
	}
	if(!wxDirExists(sLogPath))
		wxFileName::Mkdir(sLogPath, 0777, wxPATH_MKDIR_FULL);

	wxDateTime dt(wxDateTime::Now());
    wxString logfilename = sLogPath + wxFileName::GetPathSeparator() + wxString::Format(wxT("%slog_%.4d%.2d%.2d.log"),sNamePrefix.c_str(), dt.GetYear(), dt.GetMonth() + 1, dt.GetDay());

	if(!m_LogFile.Open(logfilename.GetData(), wxT("a+")))
		wxLogError(_("wxGISInitializer: Failed to open log file %s"), logfilename.c_str());

	wxLog::SetActiveTarget(new wxLogStderr(m_LogFile.fp()));

#ifdef WXGISPORTABLE
	wxLogMessage(wxT("Portable"));
#endif
	wxLogMessage(wxT(" "));
	wxLogMessage(wxT("####################################################################"));
	wxLogMessage(wxT("##                    %s                    ##"),wxNow().c_str());
	wxLogMessage(wxT("####################################################################"));
	long dFreeMem =  wxMemorySize(wxGetFreeMemory() / 1048576).ToLong();
	wxLogMessage(_("HOST '%s': OS desc - %s, free memory - %u Mb"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), dFreeMem);
	wxLogMessage(_("wxGISInitializer: Log file: %s"), logfilename.c_str());

    return true;
}

bool wxGISInitializer::SetupLoc(const wxString &sLoc, const wxString &sLocPath)
{
    wxLogMessage(_("wxGISInitializer: Initialize locale"));

    if(m_pszOldLocale != NULL)
		setlocale(LC_NUMERIC, m_pszOldLocale);
	wxDELETE(m_pszOldLocale);
    wxDELETE(m_pLocale);

	//init locale
    if ( !sLoc.IsEmpty() )
    {
		int iLocale(0);
		const wxLanguageInfo* loc_info = wxLocale::FindLanguageInfo(sLoc);
		if(loc_info != NULL)
		{
			iLocale = loc_info->Language;
			wxLogMessage(_("wxGISInitializer: Language is set to %s"), loc_info->Description.c_str());
		}

        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog

        m_pLocale = new wxLocale();
        if ( !m_pLocale->Init(iLocale) )
        {
            wxLogError(wxT("wxGISInitializer: This language is not supported by the system."));
            return false;
        }
    }

	//m_locale.Init(wxLANGUAGE_DEFAULT);

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
	wxString sLocalePath = sLocPath + wxFileName::GetPathSeparator() + sLoc;
	if(wxDirExists(sLocalePath))
	{
		wxLocale::AddCatalogLookupPathPrefix(sLocalePath);

		// Initialize the catalogs we'll be using
		//load multicat from locale
		wxArrayString trans_arr;
		wxDir::GetAllFiles(sLocalePath, &trans_arr, wxT("*.mo"));

		for(size_t i = 0; i < trans_arr.size(); ++i)
		{
			wxFileName name(trans_arr[i]);
			m_pLocale->AddCatalog(name.GetName());
		}

		// this catalog is installed in standard location on Linux systems and
		// shows that you may make use of the standard message catalogs as well
		//
		// if it's not installed on your system, it is just silently ignored
	#ifdef __LINUX__
		{
			wxLogNull noLog;
			m_pLocale->AddCatalog(_T("fileutils"));
		}
	#endif
	}

	//support of dot in doubles and floats
	m_pszOldLocale = strdup(setlocale(LC_NUMERIC, NULL));
    if( setlocale(LC_NUMERIC,"C") == NULL )
        m_pszOldLocale = NULL;

    return true;
}