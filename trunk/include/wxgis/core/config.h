/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISConfig class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#define VENDOR wxT("wxGIS")

WX_DECLARE_STRING_HASH_MAP( wxXmlNode*, wxGISConfigNodesMap );
/** \class wxGISConfig config.h
    \brief The config class
*/
class WXDLLIMPEXP_GIS_CORE wxGISConfig
{
public:
	wxGISConfig(const wxString &sVendorName = VENDOR, bool bPortable = false);
	virtual ~wxGISConfig(void);
	virtual void Clean(bool bInstall = false);
	virtual wxString Read(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sDefaultValue);
	virtual int ReadInt(wxGISEnumConfigKey Key, const wxString &sPath, int nDefaultValue);
	virtual double ReadDouble(wxGISEnumConfigKey Key, const wxString &sPath, double dDefaultValue);
	virtual bool ReadBool(wxGISEnumConfigKey Key, const wxString &sPath, bool bDefaultValue);
	virtual wxXmlNode *GetConfigNode(wxGISEnumConfigKey Key, const wxString &sPath);
	virtual bool Write(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sValue);
	virtual bool Write(wxGISEnumConfigKey Key, const wxString &sPath, bool bValue);
	virtual bool Write(wxGISEnumConfigKey Key, const wxString &sPath, int nValue);
	static void DeleteNodeChildren(wxXmlNode* pNode);
	virtual wxXmlNode *CreateConfigNode(wxGISEnumConfigKey Key, const wxString &sPath);
	virtual wxString GetLocalConfigDir(void){return m_sLocalConfigDirPath;};
	virtual wxString GetGlobalConfigDir(void){return m_sGlobalConfigDirPath;};
	//typedefs
	typedef struct wxxmlconf
	{
		wxXmlDocument* pXmlDoc;
        wxGISEnumConfigKey Key;
        wxString sXmlFileName;
		wxString sXmlFilePath;
	}WXXMLCONF;
protected:
	bool SplitPathToXml(const wxString &  fullpath, wxString *psFileName, wxString *psPathInXml);
	bool SplitPathToAttribute(const wxString &  fullpath, wxString *psPathToAttribute, wxString *psAttributeName);
	wxXmlNode* GetConfigRootNode(wxGISEnumConfigKey Key, const wxString &sFileName);
protected:
	wxString m_sLocalConfigDirPath, m_sGlobalConfigDirPath, m_sAppExeDirPath;
    bool m_bPortable;
	std::vector<WXXMLCONF> m_paConfigFiles;
	wxGISConfigNodesMap m_pmConfigNodes;
};

/** \class wxGISAppConfig config.h
    \brief The extended config class

	Added methods for Get/Set Locale, System Directory and Log Directory paths, Debug mode
*/
class WXDLLIMPEXP_GIS_CORE wxGISAppConfig : 
	public wxGISConfig
{
public:
	wxGISAppConfig(const wxString &sVendorName = VENDOR, bool bPortable = false);
	virtual ~wxGISAppConfig(void);
    virtual wxString GetLocale(void);
    virtual wxString GetLocaleDir(void);
    virtual wxString GetSysDir(void);
    virtual wxString GetLogDir(void);
    virtual bool GetDebugMode(void);
    virtual void SetLocale(const wxString &sLocale);
    virtual void SetLocaleDir(const wxString &sLocaleDir);
    virtual void SetSysDir(const wxString &sSysDir);
	virtual void SetLogDir(const wxString &sLogDir);
    virtual void SetDebugMode(bool bDebug);
};

DEFINE_SHARED_PTR(wxGISAppConfig);

/** \fn wxGISAppConfigSPtr GetConfig(void)
    \brief Global config getter.
	
	If config object is not exist it created, otherwise - AddRef for pointer
 */	
WXDLLIMPEXP_GIS_CORE wxGISAppConfigSPtr GetConfig(void);
/** \fn void ReleaseConfig(void)
    \brief Release global config pointer.
 */	
WXDLLIMPEXP_GIS_CORE void ReleaseConfig(void);


/*
class WXDLLIMPEXP_GIS_CORE wxGISConfig : 
	public IGISConfig
{
public:
	wxGISConfig(const wxString &sAppName, const wxString &sConfigDir, bool bPortable = false);
	virtual ~wxGISConfig(void);
	//IGISConfig
    virtual wxXmlNode* GetConfigNode(wxGISEnumConfigKey Key, wxString sPath);
    virtual wxXmlNode* CreateConfigNode(wxGISEnumConfigKey Key, wxString sPath, bool bUniq = true);
    virtual wxXmlNode* GetConfigNode(wxString sPath, bool bCreateInCU, bool bUniq);
    //
	static void DeleteNodeChildren(wxXmlNode* pNode);

	virtual void Clean(bool bInstall = false);
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
*/
/** \class wxGISAppConfig config.h
    \brief The extended config class

	Added methods for Get/Set Locale, System Directory and Log Directory paths, Debug mode
*/
/*
class WXDLLIMPEXP_GIS_CORE wxGISAppConfig : 
	public wxGISConfig
{
public:
	wxGISAppConfig(const wxString &sAppName, const wxString &sConfigDir, bool bPortable = false);
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

DEFINE_SHARED_PTR(wxGISAppConfig);
*/