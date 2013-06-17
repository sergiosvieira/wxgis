/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISConfig class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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
#include <wx/hashmap.h>
#include <wx/xml/xml.h>

#define VENDOR wxT("wxgis")

WX_DECLARE_STRING_HASH_MAP( wxXmlNode*, wxGISConfigNodesMap );

/** \class wxGISConfig config.h
    \brief The config class
*/
class WXDLLIMPEXP_GIS_CORE wxGISConfig : public wxObject
{
    DECLARE_CLASS(wxGISConfig)
public:
    wxGISConfig(void);
	wxGISConfig(const wxString &sVendorName, bool bPortable);
    void Create(const wxString &sVendorName = VENDOR, bool bPortable = false);

    bool IsOk() const { return m_refData != NULL; };

    bool operator == ( const wxGISConfig& obj ) const;
    bool operator != (const wxGISConfig& obj) const { return !(*this == obj); };

	wxString Read(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sDefaultValue);
	int ReadInt(wxGISEnumConfigKey Key, const wxString &sPath, int nDefaultValue);
	double ReadDouble(wxGISEnumConfigKey Key, const wxString &sPath, double dDefaultValue);
	bool ReadBool(wxGISEnumConfigKey Key, const wxString &sPath, bool bDefaultValue);
	bool Write(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sValue);
	bool Write(wxGISEnumConfigKey Key, const wxString &sPath, bool bValue);
	bool Write(wxGISEnumConfigKey Key, const wxString &sPath, int nValue);

	static void DeleteNodeChildren(wxXmlNode* pNode);
	wxXmlNode *GetConfigNode(wxGISEnumConfigKey Key, const wxString &sPath);
	wxXmlNode *CreateConfigNode(wxGISEnumConfigKey Key, const wxString &sPath);

	wxString GetLocalConfigDir(void) const;
	wxString GetGlobalConfigDir(void) const;
	wxString GetLocalConfigDirNonPortable(void) const;

    void Save(const wxGISEnumConfigKey Key = enumGISHKAny);
protected:
	bool SplitPathToXml(const wxString &  fullpath, wxString *psFileName, wxString *psPathInXml);
	bool SplitPathToAttribute(const wxString &  fullpath, wxString *psPathToAttribute, wxString *psAttributeName);
	wxXmlNode* GetConfigRootNode(wxGISEnumConfigKey Key, const wxString &sFileName) const;
protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
};

/** \class wxGISConfigRefData config.h
    \brief The reference data class for wxGISConfig
*/

class  wxGISConfigRefData : public wxObjectRefData
{
    friend class wxGISConfig;
    friend class wxGISAppConfig;
public:
    wxGISConfigRefData()
    {
    }

    virtual ~wxGISConfigRefData()
    {
        wxGISEnumConfigKey CmpKey = enumGISHKCU;//bInstall == true ? enumGISHKLM : enumGISHKCU;
 	    for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
	    {
//the config state storing to files while destruction config class (smart pointer)
//on linux saving file in destructor produce segmentation fault
//#ifdef __WXMSW__
//            //Store only user settings. Common settings should be changed during install process
//            if(m_paConfigFiles[i].eKey == CmpKey)
//            {
//                wxString sXmlFilePath = m_paConfigFiles[i].sXmlFilePath;
//                m_paConfigFiles[i].pXmlDoc->Save(sXmlFilePath);
//            }
//#endif
            wxDELETE(m_paConfigFiles[i].pXmlDoc);
	    }
	    m_paConfigFiles.clear();
    }

    void Save(const wxGISEnumConfigKey Key = enumGISHKAny, const wxString&  sXmlFileName = wxEmptyString)
    {
        if(sXmlFileName.IsEmpty())
        {
	        for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
            {
                if(Key == enumGISHKAny || m_paConfigFiles[i].eKey & Key)
                {
                    //if(wxFileName::IsFileWritable(m_paConfigFiles[i].sXmlFilePath))
                    wxString sXmlFilePath = m_paConfigFiles[i].sXmlFilePath;
                    m_paConfigFiles[i].pXmlDoc->Save(sXmlFilePath);
                }
	        }
        }
        else
        {
 	        for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
            {
                if(m_paConfigFiles[i].eKey & Key && m_paConfigFiles[i].sXmlFileName.CmpNoCase(sXmlFileName) == 0)
                    m_paConfigFiles[i].pXmlDoc->Save(m_paConfigFiles[i].sXmlFilePath);
	        }
        }
    }

    wxGISConfigRefData( const wxGISConfigRefData& data )
        : wxObjectRefData()
    {
        m_sLocalConfigDirPath = data.m_sLocalConfigDirPath;
        m_sGlobalConfigDirPath = data.m_sGlobalConfigDirPath;
        m_sLocalConfigDirPathNonPortable = data.m_sLocalConfigDirPathNonPortable;
        m_bPortable = data.m_bPortable;
        m_pmConfigNodes = data.m_pmConfigNodes;
        m_paConfigFiles = data.m_paConfigFiles;
    }

    bool operator == (const wxGISConfigRefData& data) const
    {
        return m_sLocalConfigDirPath == data.m_sLocalConfigDirPath &&
            m_sGlobalConfigDirPath == data.m_sGlobalConfigDirPath &&
            m_sLocalConfigDirPathNonPortable == data.m_sLocalConfigDirPathNonPortable &&
            m_bPortable == data.m_bPortable &&
            m_pmConfigNodes.size() == data.m_pmConfigNodes.size() &&
            m_paConfigFiles.size() == data.m_paConfigFiles.size();
    }

	//typedefs
	typedef struct wxxmlconf
	{
		wxXmlDocument *pXmlDoc;
        wxGISEnumConfigKey eKey;
        wxString sXmlFileName;
		wxString sXmlFilePath;
	}WXXMLCONF;

protected:
	wxString m_sLocalConfigDirPath, m_sGlobalConfigDirPath, m_sAppExeDirPath;
	wxString m_sLocalConfigDirPathNonPortable;
    bool m_bPortable;
	wxVector<WXXMLCONF> m_paConfigFiles;
	wxGISConfigNodesMap m_pmConfigNodes;
};

/** \class wxGISAppConfig config.h
    \brief The extended config class

	Added methods for Get/Set Locale, System Directory and Log Directory paths, Debug mode
*/

class WXDLLIMPEXP_GIS_CORE wxGISAppConfig : public wxGISConfig
{
    DECLARE_CLASS(wxGISAppConfig);
public:
    wxGISAppConfig(void);
	wxGISAppConfig(const wxString &sVendorName, bool bPortable);

    wxString GetLocale(void);
    wxString GetLocaleDir(void);
    wxString GetSysDir(void);
    wxString GetLogDir(void);
    bool GetDebugMode(void);
    void SetLocale(const wxString &sLocale);
    void SetLocaleDir(const wxString &sLocaleDir);
    void SetSysDir(const wxString &sSysDir);
	void SetLogDir(const wxString &sLogDir);
    void SetDebugMode(bool bDebug);
};

/** \fn wxGISAppConfig GetConfig(void)
    \brief Global config getter.

	If config object is not exist it created, otherwise - AddRef to smart pointer
 */
WXDLLIMPEXP_GIS_CORE wxGISAppConfig GetConfig(void);

