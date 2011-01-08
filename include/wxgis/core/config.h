/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISConfig class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009 Bishop
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
#include <wx/stdpaths.h>
#include <wx/filename.h>

#define HKCU_CONFIG_NAME wxT("hkcu_config.xml")
#define HKLM_CONFIG_NAME wxT("hklm_config.xml")

//---------------------------------------------------------------
// wxGISConfig
//---------------------------------------------------------------

class WXDLLIMPEXP_GIS_CORE wxGISConfig : 
	public IGISConfig
{
public:
	wxGISConfig(wxString sAppName, wxString sConfigDir, bool bPortable = false);
	virtual ~wxGISConfig(void);
	//IGISConfig
    virtual wxXmlNode* GetConfigNode(wxGISEnumConfigKey Key, wxString sPath);
    virtual wxXmlNode* CreateConfigNode(wxGISEnumConfigKey Key, wxString sPath, bool bUniq = true);
    virtual wxXmlNode* GetConfigNode(wxString sPath, bool bCreateInCU, bool bUniq);
    //
	static void DeleteNodeChildren(wxXmlNode* pNode);

	virtual void Clean(void);
	typedef struct wxxmlconfnode
	{
		wxXmlNode* pXmlNode;
		wxString sXmlPath;
        wxGISEnumConfigKey Key;
	}WXXMLCONFNODE;

	typedef struct wxxmlconf
	{
        wxString sRootNodeName;
		wxXmlDocument* pXmlDoc;
		wxString xml_path;
        wxGISEnumConfigKey Key;
	}WXXMLCONF;
protected:
	wxString m_sUserConfigDir, m_sSysConfigDir, m_sExeDirPath;
	wxString m_sAppName;
	std::vector<WXXMLCONF> m_configs_arr;
	std::vector<WXXMLCONFNODE> m_confignodes_arr;
    bool m_bPortable;
};

//---------------------------------------------------------------
// wxGISAppConfig
//---------------------------------------------------------------

class WXDLLIMPEXP_GIS_CORE wxGISAppConfig : 
	public wxGISConfig
{
public:
	wxGISAppConfig(wxString sAppName, wxString sConfigDir, bool bPortable = false);
	virtual ~wxGISAppConfig(void);

    virtual wxString GetLocale(void);
    virtual wxString GetLocaleDir(void);
    virtual wxString GetSysDir(void);
    virtual wxString GetLogDir(void);
    virtual bool GetDebugMode(void);
    virtual void SetLocale(wxString sLocale);
    virtual void SetLocaleDir(wxString sLocaleDir);
    virtual void SetSysDir(wxString sSysDir);
	virtual void SetLogDir(wxString sLogDir);
    virtual void SetDebugMode(bool bDebug);
};

