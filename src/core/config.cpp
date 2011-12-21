/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISConfig class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/core/config.h"

#include <wx/tokenzr.h>


static wxGISAppConfigSPtr g_pConfig;

extern WXDLLIMPEXP_GIS_CORE wxGISAppConfigSPtr GetConfig(void)
{
	if(!g_pConfig)
	{
	#ifdef WXGISPORTABLE
		g_pConfig = boost::make_shared<wxGISAppConfig>(VENDOR, true);
	#else
		g_pConfig = boost::make_shared<wxGISAppConfig>(VENDOR, false);
	#endif
	}
	return g_pConfig;
}

extern WXDLLIMPEXP_GIS_CORE void ReleaseConfig(void)
{
	g_pConfig.reset();
}

//---------------------------------------------------------------
// wxGISConfig
//---------------------------------------------------------------
wxGISConfig::wxGISConfig(const wxString &sVendorName, bool bPortable)
{
 	wxStandardPaths stp;
	m_sAppExeDirPath = wxPathOnly(stp.GetExecutablePath());

    m_bPortable = bPortable;

    wxString sExeAppName = wxFileNameFromPath(stp.GetExecutablePath());
	sExeAppName = wxFileName::StripExtension(sExeAppName);

    if(bPortable)
    {
        //if potable - config path: [app.exe path\config]
        m_sGlobalConfigDirPath = m_sLocalConfigDirPath = m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxT("config");
	    if(!wxDirExists(m_sLocalConfigDirPath))
		    wxFileName::Mkdir(m_sLocalConfigDirPath, 0755, wxPATH_MKDIR_FULL);
    }
    else
    {
        m_sLocalConfigDirPath = stp.GetUserConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
        m_sGlobalConfigDirPath = stp.GetConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
        if(m_sGlobalConfigDirPath.Find(sExeAppName) != wxNOT_FOUND)
            m_sGlobalConfigDirPath.Replace(sExeAppName + wxFileName::GetPathSeparator(), wxString(wxT("")));

	    //if(!wxDirExists(m_sUserConfigDir))
		   // wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);
//	    if(!wxDirExists(m_sSysConfigDir))
//		    wxFileName::Mkdir(m_sSysConfigDir, 0775, wxPATH_MKDIR_FULL);
    }

	m_sLocalConfigDirPathNonPortable = stp.GetUserConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
}

wxGISConfig::~wxGISConfig(void)
{
	Clean();
}

void wxGISConfig::Clean(bool bInstall)
{
    wxGISEnumConfigKey CmpKey = bInstall == true ? enumGISHKLM : enumGISHKCU;
	for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
	{
        //Store only user settings. Common settings should be changed during install process
        if(m_paConfigFiles[i].Key == CmpKey)
            m_paConfigFiles[i].pXmlDoc->Save(m_paConfigFiles[i].sXmlFilePath);
		wxDELETE(m_paConfigFiles[i].pXmlDoc);
	}
	m_paConfigFiles.clear();
}

//HKLM wxGISCatalog/Frame/Views
//HKLM wxGISCatalog/PropertyPages/Page/class

wxString wxGISConfig::Read(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sDefaultValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return sDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return sDefaultValue;
	return pNode->GetAttribute(sAttributeName, sDefaultValue);
}

int wxGISConfig::ReadInt(wxGISEnumConfigKey Key, const wxString &sPath, int nDefaultValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return nDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return nDefaultValue;
	return wxAtoi(pNode->GetAttribute(sAttributeName, wxString::Format(wxT("%d"), nDefaultValue)));
}

double wxGISConfig::ReadDouble(wxGISEnumConfigKey Key, const wxString &sPath, double dDefaultValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return dDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return dDefaultValue;
	return wxAtof(pNode->GetAttribute(sAttributeName, wxString::Format(wxT("%f"), dDefaultValue)));
}

bool wxGISConfig::ReadBool(wxGISEnumConfigKey Key, const wxString &sPath, bool bDefaultValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return bDefaultValue;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)
		return bDefaultValue;
	wxString sDefaultValue = bDefaultValue == true ? wxString(wxT("yes")) : wxString(wxT("no"));
	wxString sValue = pNode->GetAttribute(sAttributeName, sDefaultValue);

	return sValue.CmpNoCase(wxString(wxT("yes"))) == 0 || sValue.CmpNoCase(wxString(wxT("on"))) == 0 || sValue.CmpNoCase(wxString(wxT("1"))) == 0 || sValue.CmpNoCase(wxString(wxT("t"))) == 0 || sValue.CmpNoCase(wxString(wxT("true"))) == 0;
}

bool wxGISConfig::SplitPathToXml(const wxString &  fullpath, wxString *psFileName, wxString *psPathInXml)
{
	int nPos = fullpath.Find('/');
	if(nPos == wxNOT_FOUND)
		return false;
	*psFileName = fullpath.Left(nPos) + wxT(".xml");
	*psPathInXml = fullpath.Right(fullpath.Len() - nPos - 1);
	return true;
}

bool wxGISConfig::SplitPathToAttribute(const wxString &  fullpath, wxString *psPathToAttribute, wxString *psAttributeName)
{
	int nPos = fullpath.Find('/', true);
	if(nPos == wxNOT_FOUND)
		return false;
	*psPathToAttribute = fullpath.Left(nPos);
	*psAttributeName = fullpath.Right(fullpath.Len() - nPos - 1);
	return true;
}
wxXmlNode *wxGISConfig::GetConfigRootNode(wxGISEnumConfigKey Key, const wxString &sFileName)
{
    wxXmlDocument* pDoc(NULL);

    //search cached configs
	for(size_t i = 0; i < m_paConfigFiles.size(); ++i)
		if(m_paConfigFiles[i].sXmlFileName.CmpNoCase(sFileName) == 0 && m_paConfigFiles[i].Key == Key)
			return m_paConfigFiles[i].pXmlDoc->GetRoot();

	wxString sConfigDirPath;
	switch(Key)
	{
	case enumGISHKLM:
		sConfigDirPath = m_sGlobalConfigDirPath;
		break;
	case enumGISHKCU:
		sConfigDirPath = m_sLocalConfigDirPath;
		break;
	default:
		return NULL;
	};

	if(!wxDirExists(sConfigDirPath))
		wxFileName::Mkdir(sConfigDirPath, 0755, wxPATH_MKDIR_FULL);

	wxString sConfigFilePath = sConfigDirPath + wxFileName::GetPathSeparator() + sFileName;
	if(wxFileName::FileExists(sConfigFilePath))
	{
		pDoc = new wxXmlDocument(sConfigFilePath);
	}
	else
	{
		//Get global config if local config is not available
		if(Key == enumGISHKCU)
        {
			if(!wxDirExists(m_sGlobalConfigDirPath))
				wxFileName::Mkdir(sConfigDirPath, 0755, wxPATH_MKDIR_FULL);
			else
			{
				wxString sConfigFilePathNew = m_sGlobalConfigDirPath + wxFileName::GetPathSeparator() + sFileName;
				if(wxFileName::FileExists(sConfigFilePathNew))
					pDoc = new wxXmlDocument(sConfigFilePathNew);
			}
		}

		//last chance - load from config directory near pplication executable
		if(!pDoc)
		{
			wxString sConfigFilePathNew = m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxT("config") +  wxFileName::GetPathSeparator() + sFileName;
			if(wxFileName::FileExists(sConfigFilePathNew))
				pDoc = new wxXmlDocument(sConfigFilePathNew);
		}

		//create new config
		if(!pDoc)
		{
			pDoc = new wxXmlDocument();
			wxString sRootNodeName = sFileName.Left(sFileName.Len() - 4);//trim ".xml"
			pDoc->SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, sRootNodeName));
		}
	}

	if(!pDoc)
		return NULL;

	WXXMLCONF conf = {pDoc, Key, sFileName, sConfigFilePath};
	m_paConfigFiles.push_back(conf);

	return pDoc->GetRoot();
}


wxXmlNode* wxGISConfig::GetConfigNode(wxGISEnumConfigKey Key, const wxString &sPath)
{
	wxString sFullPath;
	switch(Key)
	{
	case enumGISHKLM:
		sFullPath = wxString(wxT("HKLM/"));
		break;
	case enumGISHKCU:
		sFullPath = wxString(wxT("HKCU/"));
		break;
	default:
		return  NULL;
	};

	sFullPath += sPath;

    //search cached configs nodes
	wxXmlNode* pOutputNode = m_pmConfigNodes[sFullPath];
	if(pOutputNode)
		return pOutputNode;

	//split path
	wxString sFileName, sPathInFile;
	if(!SplitPathToXml(sPath, &sFileName, &sPathInFile))
		return NULL;


	//get config root
	wxXmlNode* pRoot = GetConfigRootNode(Key, sFileName);
    if(!pRoot)
        return NULL;

	if(sPathInFile.IsEmpty())
		return pRoot;

    wxXmlNode* pChildNode = pRoot->GetChildren();

    wxStringTokenizer tkz(sPathInFile, wxString(wxT("/")), wxTOKEN_RET_EMPTY );
	wxString token, sChildName;
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
        while(pChildNode)
        {
			sChildName = pChildNode->GetName();
			if(token.CmpNoCase(sChildName) == 0)
			{
				if(tkz.HasMoreTokens())
					pChildNode = pChildNode->GetChildren();
                break;
			}
            pChildNode = pChildNode->GetNext();
        }
    }

	if(token.CmpNoCase(sChildName) == 0)
    {
		//store pointer for speed find
		m_pmConfigNodes[sFullPath] = pChildNode;
        return pChildNode;
    }
    return NULL;
}

wxXmlNode* wxGISConfig::CreateConfigNode(wxGISEnumConfigKey Key, const wxString &sPath)
{
	//split path
	wxString sFileName, sPathInFile;
	if(!SplitPathToXml(sPath, &sFileName, &sPathInFile))
		return NULL;


	//get config root
	wxXmlNode* pRoot = GetConfigRootNode(Key, sFileName);
    if(!pRoot)
        return NULL;

	wxXmlNode* pChildNode = pRoot->GetChildren();

	wxStringTokenizer tkz(sPathInFile, wxString(wxT("/")), wxTOKEN_RET_EMPTY );
	wxString token, sChildName;
	bool bCreate(true);
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
		bCreate = true;
        while(pChildNode)
        {
			sChildName = pChildNode->GetName();
			if(token.CmpNoCase(sChildName) == 0)
			{
				bCreate = false;
				if(tkz.HasMoreTokens())
				{
					pRoot = pChildNode;
					pChildNode = pChildNode->GetChildren();
				}
                break;
			}
            pChildNode = pChildNode->GetNext();
        }
		if(bCreate)
		{
			pChildNode = new wxXmlNode(pChildNode == 0 ? pRoot : pChildNode, wxXML_ELEMENT_NODE, token);
			if(tkz.HasMoreTokens())
			{
				pRoot = pChildNode;
				pChildNode = pChildNode->GetChildren();
			}
		}
	}
	return pChildNode;
}

void wxGISConfig::DeleteNodeChildren(wxXmlNode* pNode)
{
	wxXmlNode* pChild = pNode->GetChildren();
	while(pChild)
	{
		wxXmlNode* pDelChild = pChild;
		pChild = pChild->GetNext();
		if(pNode->RemoveChild(pDelChild))
			delete pDelChild;
	}
}

bool wxGISConfig::Write(wxGISEnumConfigKey Key, const wxString &sPath, const wxString &sValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return false;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)//create it
	{
		pNode = CreateConfigNode(Key, sPathToAttribute);
		if(!pNode)
			return false;
	}

	if(pNode->HasAttribute(sAttributeName))
		pNode->DeleteAttribute(sAttributeName);
	pNode->AddAttribute(sAttributeName, sValue);

	//send write event
	return true;
}

bool wxGISConfig::Write(wxGISEnumConfigKey Key, const wxString &sPath, bool bValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return false;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)//create it
	{
		pNode = CreateConfigNode(Key, sPathToAttribute);
		if(!pNode)
			return false;
	}

	if(pNode->HasAttribute(sAttributeName))
		pNode->DeleteAttribute(sAttributeName);
	wxString sValue = bValue == true ?  wxString(wxT("yes")) : wxString(wxT("no"));
	pNode->AddAttribute(sAttributeName, sValue);

	//send write event
	return true;
}

bool wxGISConfig::Write(wxGISEnumConfigKey Key, const wxString &sPath, int nValue)
{
	//split path
	wxString sPathToAttribute, sAttributeName;
	if(!SplitPathToAttribute(sPath, &sPathToAttribute, &sAttributeName))
		return false;

	wxXmlNode* pNode = GetConfigNode(Key, sPathToAttribute);
	if(!pNode)//create it
	{
		pNode = CreateConfigNode(Key, sPathToAttribute);
		if(!pNode)
			return false;
	}

	if(pNode->HasAttribute(sAttributeName))
		pNode->DeleteAttribute(sAttributeName);
	wxString sValue = wxString::Format(wxT("%d"), nValue);
	pNode->AddAttribute(sAttributeName, sValue);

	//send write event
	return true;
}

//---------------------------------------------------------------
// wxGISAppConfig
//---------------------------------------------------------------

wxGISAppConfig::wxGISAppConfig(const wxString &sVendorName, bool bPortable) : wxGISConfig(sVendorName, bPortable)
{
}

wxGISAppConfig::~wxGISAppConfig(void)
{
}

wxString wxGISAppConfig::GetLocale(void)
{
    const wxLanguageInfo* loc_info = wxLocale::GetLanguageInfo(wxLocale::GetSystemLanguage());
    wxString sDefaultOut = loc_info->CanonicalName;
    if(sDefaultOut.IsEmpty())
        sDefaultOut = wxString(wxT("en"));
    else
    {
        //remove duplicated part of name
        int pos = sDefaultOut.Find(wxT("_"));
        if(pos != wxNOT_FOUND)
        {
            wxString sPart1 = sDefaultOut.Left(pos);
            wxString sPart2 = sDefaultOut.Right(sDefaultOut.Len() - pos - 1);
            if(sPart1.CmpNoCase(sPart2) == 0)
                sDefaultOut = sPart1;
        }
    }

	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/loc/locale")), sDefaultOut);
}

wxString wxGISAppConfig::GetLocaleDir(void)
{
    wxString sDefaultOut = m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxT("locale");
    if(m_bPortable)
        return sDefaultOut;
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/loc/path")), sDefaultOut);
}

wxString wxGISAppConfig::GetLogDir(void)
{
	wxLogNull noLog;
    wxString sDefaultOut = m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxT("log");
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/log/path")), sDefaultOut);
}


wxString wxGISAppConfig::GetSysDir(void)
{
    wxString sDefaultOut = m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxT("sys");
    if(m_bPortable)
        return sDefaultOut;
	return Read(enumGISHKCU, wxString(wxT("wxGISCommon/sys/path")), sDefaultOut);
}

bool wxGISAppConfig::GetDebugMode(void)
{
    bool bDefaultOut = false;
	return ReadBool(enumGISHKCU, wxString(wxT("wxGISCommon/debug/mode")), bDefaultOut);
}

void wxGISAppConfig::SetLocale(const wxString &sLocale)
{
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/loc/locale")), sLocale);
}

void wxGISAppConfig::SetLocaleDir(const wxString &sLocaleDir)
{
    if(m_bPortable)
        return;
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/loc/path")), sLocaleDir);
}

void wxGISAppConfig::SetSysDir(const wxString &sSysDir)
{
    if(m_bPortable)
        return;
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/sys/path")), sSysDir);
}

void wxGISAppConfig::SetLogDir(const wxString &sLogDir)
{
    if(m_bPortable)
        return;
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/log/path")), sLogDir);
}

void wxGISAppConfig::SetDebugMode(bool bDebug)
{
	Write(enumGISHKCU, wxString(wxT("wxGISCommon/debug/mode")), bDebug);
}
