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

    if(bPortable)
    {
        //if potable - config path: [app.exe path\config]
        m_sGlobalConfigDirPath = m_sLocalConfigDirPath = m_sAppExeDirPath + wxFileName::GetPathSeparator() + wxT("config");
	    if(!wxDirExists(m_sLocalConfigDirPath))
		    wxFileName::Mkdir(m_sLocalConfigDirPath, 0755, wxPATH_MKDIR_FULL);
    }
    else
    {
        wxString sExeAppName = wxFileNameFromPath(stp.GetExecutablePath());
		sExeAppName = wxFileName::StripExtension(sExeAppName);

        m_sLocalConfigDirPath = stp.GetUserConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
        m_sGlobalConfigDirPath = stp.GetConfigDir() + wxFileName::GetPathSeparator() + sVendorName;
        if(m_sGlobalConfigDirPath.Find(sExeAppName) != wxNOT_FOUND)
            m_sGlobalConfigDirPath.Replace(sExeAppName + wxFileName::GetPathSeparator(), wxString(wxT("")));

	    //if(!wxDirExists(m_sUserConfigDir))
		   // wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);
//	    if(!wxDirExists(m_sSysConfigDir))
//		    wxFileName::Mkdir(m_sSysConfigDir, 0775, wxPATH_MKDIR_FULL);
    }
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

/*
wxGISConfig::wxGISConfig(const wxString &sAppName, const wxString &sConfigDir, bool bPortable)
{
 	wxStandardPaths stp;
	m_sAppName = sAppName;

	m_sExeDirPath = wxPathOnly(stp.GetExecutablePath());

    m_bPortable = bPortable;

    if(bPortable)
    {
        //if potable - config path: [app.exe path\config]
        m_sUserConfigDir = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("config");
        m_sSysConfigDir = m_sUserConfigDir;
	    if(!wxDirExists(m_sUserConfigDir))
		    wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);
    }
    else
    {
        wxString sExeAppName = wxFileNameFromPath(stp.GetExecutablePath());
		wxFileName::StripExtension(sExeAppName);
        //sExeAppName = sAppName;

        m_sUserConfigDir = stp.GetUserConfigDir() + wxFileName::GetPathSeparator() + sConfigDir;
        m_sSysConfigDir = stp.GetConfigDir() + wxFileName::GetPathSeparator() + sConfigDir;
        if(m_sSysConfigDir.Find(sExeAppName) != wxNOT_FOUND)
            m_sSysConfigDir.Replace(sExeAppName + wxFileName::GetPathSeparator(), wxString(wxT("")));

	    //if(!wxDirExists(m_sUserConfigDir))
		   // wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);
//	    if(!wxDirExists(m_sSysConfigDir))
//		    wxFileName::Mkdir(m_sSysConfigDir, 0775, wxPATH_MKDIR_FULL);
    }
}

wxGISConfig::~wxGISConfig(void)
{
	Clean();
}

void wxGISConfig::Clean(bool bInstall)
{
    wxGISEnumConfigKey CmpKey = bInstall == true ? enumGISHKLM : enumGISHKCU;
	for(size_t i = 0; i < m_configs_arr.size(); ++i)
	{
        //Store only user settings. Common settings should be changed during install process
        if(m_configs_arr[i].Key == CmpKey)
            m_configs_arr[i].pXmlDoc->Save(m_configs_arr[i].xml_path);
		wxDELETE(m_configs_arr[i].pXmlDoc);
	}
	m_configs_arr.clear();
}

wxXmlNode* wxGISConfig::GetConfigNode(wxGISEnumConfigKey Key, wxString sPath)
{
    //search cached configs nodes
    for(size_t i = 0; i < m_confignodes_arr.size(); ++i)
	{
        if(m_confignodes_arr[i].sXmlPath.CmpNoCase(sPath) == 0 && m_confignodes_arr[i].Key == Key)
		{
            return m_confignodes_arr[i].pXmlNode;
		}
	}

    //get root name
    wxString sRootNodeName = m_sAppName;

    wxXmlDocument* pDoc(NULL);

    //search cached configs
	for(size_t i = 0; i < m_configs_arr.size(); ++i)
	{
		if(m_configs_arr[i].sRootNodeName.CmpNoCase(sRootNodeName) == 0 && m_configs_arr[i].Key == Key)
		{
			pDoc = m_configs_arr[i].pXmlDoc;
			break;
		}
	}

	if(!pDoc)
	{
		wxString sXMLDocPath;
		switch(Key)
		{
		case enumGISHKLM:
		{
			wxString sys_dir = m_sSysConfigDir;// + wxFileName::GetPathSeparator() + sRootNodeName;
			wxString sys_path = sys_dir + wxFileName::GetPathSeparator() + sRootNodeName + wxT(".xml");// + HKLM_CONFIG_NAME;

			if(!wxDirExists(sys_dir))
				wxFileName::Mkdir(sys_dir, 0755, wxPATH_MKDIR_FULL);

			sXMLDocPath = sys_path;
			break;
		}
		case enumGISHKCU:
		{
			wxString user_dir = m_sUserConfigDir;// + wxFileName::GetPathSeparator() + sRootNodeName;
			wxString user_path = user_dir + wxFileName::GetPathSeparator() + sRootNodeName + wxT(".xml");// + HKCU_CONFIG_NAME;

			if(!wxDirExists(user_dir))
				wxFileName::Mkdir(user_dir, 0755, wxPATH_MKDIR_FULL);

			sXMLDocPath = user_path;
			break;
		}
		case enumGISHKCC:
		case enumGISHKCR:
		default:
			return NULL;
		}

		if(wxFileName::FileExists(sXMLDocPath))
			pDoc = new wxXmlDocument(sXMLDocPath);
		else
		{
			//if(Key == enumGISHKLM)
			//{
			//	wxString sXMLPath = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("config") +  wxFileName::GetPathSeparator() + sRootNodeName + wxT(".xml");
            //    if(wxFileName::FileExists(sXMLPath))
            //        pDoc = new wxXmlDocument(sXMLPath);
			//}

			//get hklm confic if hkcu is not available
			if(Key == enumGISHKCU)
            {
                wxString sys_dir = m_sSysConfigDir;// + wxFileName::GetPathSeparator() + sRootNodeName;
                wxString sys_path = sys_dir + wxFileName::GetPathSeparator() + sRootNodeName + wxT(".xml");// + HKCU_CONFIG_NAME;
                pDoc = new wxXmlDocument(sys_path);
                if(pDoc && !pDoc->IsOk())
                    wxDELETE(pDoc);
			}

			//last chance
			if(!pDoc)
			{
				wxString sXMLPath = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("config") +  wxFileName::GetPathSeparator() + sRootNodeName + wxT(".xml");
                if(wxFileName::FileExists(sXMLPath))
                    pDoc = new wxXmlDocument(sXMLPath);
                if(pDoc && !pDoc->IsOk())
                    wxDELETE(pDoc);
			}

			if(!pDoc)
			{
				pDoc = new wxXmlDocument();
				pDoc->SetRoot(new wxXmlNode(wxXML_ELEMENT_NODE, sRootNodeName));
			}
		}

		if(!pDoc)
			return NULL;

		WXXMLCONF conf = {sRootNodeName, pDoc, sXMLDocPath, Key};
		m_configs_arr.push_back(conf);
	}

	wxXmlNode* pRoot = pDoc->GetRoot();
    if(!pRoot)
        return NULL;

	if(sPath.IsEmpty())
		return pRoot;

    wxXmlNode* pChildNode = pRoot->GetChildren();

    wxStringTokenizer tkz(sPath.Lower(), wxString(wxT("/")), wxTOKEN_RET_EMPTY );
	wxString token, sChildName;
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
		//if(tkz.HasMoreTokens())
		//	token = token.RemoveLast();
        //token.MakeLower();
        while(pChildNode)
        {
			sChildName = pChildNode->GetName();
			sChildName = sChildName.MakeLower();
            if(token == sChildName)
			{
				if(tkz.HasMoreTokens())
					pChildNode = pChildNode->GetChildren();
                break;
			}
            pChildNode = pChildNode->GetNext();
        }
    }
    if(token == sChildName)
    {
        WXXMLCONFNODE confnode = {pChildNode, sPath.Lower(), Key};
		m_confignodes_arr.push_back(confnode);
        return pChildNode;
    }

    return NULL;
}

wxXmlNode* wxGISConfig::CreateConfigNode(wxGISEnumConfigKey Key, wxString sPath, bool bUniq)
{
    wxString sRootNodeName = m_sAppName;

    wxXmlDocument* pDoc(NULL);

	for(size_t i = 0; i < m_configs_arr.size(); ++i)
	{
		if(m_configs_arr[i].sRootNodeName == sRootNodeName && m_configs_arr[i].Key == Key)
		{
			pDoc = m_configs_arr[i].pXmlDoc;
			break;
		}
	}

    if(!pDoc)
		return NULL;

	wxXmlNode* pRoot(NULL);
	pRoot = pDoc->GetRoot();
    if(!pRoot)
        return NULL;

	wxXmlNode* pChildNode = pRoot->GetChildren();

	wxStringTokenizer tkz(sPath, wxString(wxT("/")), wxTOKEN_RET_EMPTY );
	wxString token, sChildName;
	bool bCreate(true);
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
 		//if(tkz.HasMoreTokens())
			//token = token.RemoveLast();
        token.MakeLower();
		bCreate = true;
        while(pChildNode)
        {
			sChildName = pChildNode->GetName();
			sChildName = sChildName.MakeLower();
            if(token == sChildName)
			{
				bCreate = false;
				if(tkz.HasMoreTokens())
				{
					pRoot = pChildNode;
					pChildNode = pChildNode->GetChildren();
				}
				else
				{
					if(!bUniq)
					{
						bCreate = true;
						pChildNode = pChildNode->GetParent();
					}
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
		pNode->RemoveChild(pDelChild);
		wxDELETE(pDelChild);
	}
}


wxXmlNode* wxGISConfig::GetConfigNode(wxString sPath, bool bCreateInCU, bool bUniq)
{
	wxXmlNode* pConfXmlNode = GetConfigNode(enumGISHKCU, sPath);
	if(pConfXmlNode)
        return pConfXmlNode;

    pConfXmlNode = GetConfigNode(enumGISHKLM, sPath);
    if(bCreateInCU)
    {
        if(pConfXmlNode)
        {
            wxXmlNode* pNewConfXmlNode = CreateConfigNode(enumGISHKCU, sPath, bUniq);
            pNewConfXmlNode->operator=(*pConfXmlNode);
            return pNewConfXmlNode;
        }
        else
            pConfXmlNode = CreateConfigNode(enumGISHKCU, sPath, bUniq);
    }
    return pConfXmlNode;
}

//---------------------------------------------------------------
// wxGISAppConfig
//---------------------------------------------------------------

wxGISAppConfig::wxGISAppConfig(const wxString &sAppName, const wxString &sConfigDir, bool bPortable) : wxGISConfig(sAppName, sConfigDir, bPortable)
{
}

wxGISAppConfig::~wxGISAppConfig(void)
{
}

wxString wxGISAppConfig::GetLocale(void)
{
    wxXmlNode* pNode = GetConfigNode(wxString(wxT("loc")), false, true);
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
    if(!pNode)
        return sDefaultOut;
    return pNode->GetAttribute(wxT("locale"), sDefaultOut);
}

wxString wxGISAppConfig::GetLocaleDir(void)
{
    wxXmlNode* pNode = GetConfigNode(wxString(wxT("loc")), false, true);

    wxString sDefaultOut = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("locale");
    if(!pNode || m_bPortable)
        return sDefaultOut;
    return pNode->GetAttribute(wxT("path"), sDefaultOut);
}

wxString wxGISAppConfig::GetLogDir(void)
{
	wxLogNull noLog;
    wxXmlNode* pNode = GetConfigNode(wxString(wxT("log")), false, true);

    wxString sDefaultOut = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("log");
    if(!pNode)
        return sDefaultOut;
    return pNode->GetAttribute(wxT("path"), sDefaultOut);
}


wxString wxGISAppConfig::GetSysDir(void)
{
    wxXmlNode* pNode = GetConfigNode(wxString(wxT("sys")), false, true);

    wxString sDefaultOut = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("sys");
    if(!pNode || m_bPortable)
        return sDefaultOut;
    return pNode->GetAttribute(wxT("path"), sDefaultOut);
}

bool wxGISAppConfig::GetDebugMode(void)
{
    wxXmlNode* pNode = GetConfigNode(wxString(wxT("debug")), false, true);

    bool bDefaultOut = false;
    if(!pNode)
        return bDefaultOut;
    return wxString(wxT("on")) == pNode->GetAttribute(wxT("mode"), wxT("on")) ? true : false;
}

void wxGISAppConfig::SetLocale(wxString sLocale)
{
	wxString sPropPath(wxT("loc"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasAttribute(wxT("locale")))
		pNode->DeleteAttribute(wxT("locale"));
	pNode->AddAttribute(wxT("locale"), sLocale);

}

void wxGISAppConfig::SetLocaleDir(wxString sLocaleDir)
{
    if(m_bPortable)
        return;
	wxString sPropPath(wxT("loc"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasAttribute(wxT("path")))
		pNode->DeleteAttribute(wxT("path"));
	pNode->AddAttribute(wxT("path"), sLocaleDir);
}

void wxGISAppConfig::SetSysDir(wxString sSysDir)
{
    if(m_bPortable)
        return;
	wxString sPropPath(wxT("sys"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasAttribute(wxT("path")))
		pNode->DeleteAttribute(wxT("path"));
	pNode->AddAttribute(wxT("path"), sSysDir);
}

void wxGISAppConfig::SetLogDir(wxString sLogDir)
{
    if(m_bPortable)
        return;
	wxString sPropPath(wxT("log"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasAttribute(wxT("path")))
		pNode->DeleteAttribute(wxT("path"));
	pNode->AddAttribute(wxT("path"), sLogDir);
}

void wxGISAppConfig::SetDebugMode(bool bDebug)
{
	wxString sPropPath = wxString(wxT("debug"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasAttribute(wxT("mode")))
		pNode->DeleteAttribute(wxT("mode"));
	pNode->AddAttribute(wxT("mode"), bDebug == true ? wxT("on") : wxT("off"));
}
*/
