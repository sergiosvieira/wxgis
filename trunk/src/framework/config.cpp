/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISConfig class.
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
#include "wxgis/framework/config.h"
#include "wxgis/framework/application.h"

#include <wx/tokenzr.h>

//---------------------------------------------------------------
// wxGISConfig
//---------------------------------------------------------------

wxGISConfig::wxGISConfig(wxString sAppName, wxString sConfigDir, bool bPortable)
{
 	wxStandardPaths stp;
	m_sAppName = sAppName;

	m_sExeDirPath = wxPathOnly(stp.GetExecutablePath());
	
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
        wxString sExeAppName;
        IApplication* pApp = ::GetApplication();
        if(pApp)
            sExeAppName = pApp->GetAppName();
        else 
            sExeAppName = sAppName;
        m_sUserConfigDir = stp.GetUserConfigDir() + wxFileName::GetPathSeparator() + sConfigDir;
        m_sSysConfigDir = stp.GetConfigDir() + wxFileName::GetPathSeparator() + sConfigDir;
        if(m_sSysConfigDir.Find(sExeAppName) != wxNOT_FOUND)
            m_sSysConfigDir.Replace(sExeAppName + wxFileName::GetPathSeparator(), wxString(wxT("")));

	    if(!wxDirExists(m_sUserConfigDir))
		    wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);
	    if(!wxDirExists(m_sSysConfigDir))
		    wxFileName::Mkdir(m_sSysConfigDir, 0775, wxPATH_MKDIR_FULL);
    }
}

wxGISConfig::~wxGISConfig(void)
{
	Clean();
}

void wxGISConfig::Clean(void)
{
	for(size_t i = 0; i < m_configs_arr.size(); i++)
	{
		m_configs_arr[i].pXmlDoc->Save(m_configs_arr[i].xml_path);
		wxDELETE(m_configs_arr[i].pXmlDoc);
	}
	m_configs_arr.clear();
}

wxXmlNode* wxGISConfig::GetConfigNode(wxGISEnumConfigKey Key, wxString sPath)
{
    //path: wxGISCatalog\Frame\Views
    for(size_t i = 0; i < m_confignodes_arr.size(); i++)
	{
        if(m_confignodes_arr[i].sXmlPath == sPath && m_confignodes_arr[i].Key == Key)
		{
            return m_confignodes_arr[i].pXmlNode;
		}
	}

    //get root name
    wxString sRootNodeName = m_sAppName;	

    wxXmlDocument* pDoc(NULL);

	for(size_t i = 0; i < m_configs_arr.size(); i++)
	{
		if(m_configs_arr[i].sRootNodeName == sRootNodeName && m_configs_arr[i].Key == Key)
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
			wxString sys_dir = m_sSysConfigDir + wxFileName::GetPathSeparator() + sRootNodeName;
			wxString sys_path = sys_dir + wxFileName::GetPathSeparator() + HKLM_CONFIG_NAME;

			if(!wxDirExists(sys_dir))
			{
				wxFileName::Mkdir(sys_dir, 0775, wxPATH_MKDIR_FULL);
			}
			sXMLDocPath = sys_path;
			break;
		}
		case enumGISHKCU:
		{
			wxString user_dir = m_sUserConfigDir + wxFileName::GetPathSeparator() + sRootNodeName;
			wxString user_path = user_dir + wxFileName::GetPathSeparator() + HKCU_CONFIG_NAME;

			if(!wxDirExists(user_dir))
			{
				wxFileName::Mkdir(user_dir, 0755, wxPATH_MKDIR_FULL);
				return NULL;
			}
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
			if(Key == enumGISHKLM)
			{
				wxString sPath = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("config") +  wxFileName::GetPathSeparator() + sRootNodeName + wxT(".xml");
				pDoc = new wxXmlDocument(sPath);
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

    wxXmlNode* pChildNode = pRoot->GetChildren();

    wxStringTokenizer tkz(sPath, wxString(wxT("/")), false );
	wxString token, sChildName;
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
		if(tkz.HasMoreTokens())
			token = token.RemoveLast();
        token.MakeLower();
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
        WXXMLCONFNODE confnode = {pChildNode, sPath, Key};
		m_confignodes_arr.push_back(confnode);
        return pChildNode;
    }
    
    return NULL;
}

wxXmlNode* wxGISConfig::CreateConfigNode(wxGISEnumConfigKey Key, wxString sPath, bool bUniq)
{
    wxString sRootNodeName = m_sAppName;	

    wxXmlDocument* pDoc(NULL);

	for(size_t i = 0; i < m_configs_arr.size(); i++)
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

	wxStringTokenizer tkz(sPath, wxString(wxT("/")), false );
	wxString token, sChildName;
	bool bCreate(true);
	while ( tkz.HasMoreTokens() )
	{
        token = tkz.GetNextToken();
 		if(tkz.HasMoreTokens())
			token = token.RemoveLast();
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

//---------------------------------------------------------------
// wxGISAppConfig
//---------------------------------------------------------------

wxGISAppConfig::wxGISAppConfig(wxString sAppName, wxString sConfigDir, bool bPortable) : wxGISConfig(sAppName, sConfigDir, bPortable)
{
}

wxGISAppConfig::~wxGISAppConfig(void)
{
}

wxString wxGISAppConfig::GetLocale(void)
{
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, wxString(wxT("loc")));
    wxString sDefaultOut(wxT("en"));
    if(!pNode)
        return sDefaultOut;
    return pNode->GetPropVal(wxT("locale"), sDefaultOut);
}

wxString wxGISAppConfig::GetLocaleDir(void)
{
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, wxString(wxT("loc")));
    
    wxString sDefaultOut = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("locale");
    if(!pNode)
        return sDefaultOut;
    return pNode->GetPropVal(wxT("path"), sDefaultOut);
}

wxString wxGISAppConfig::GetLogDir(void)
{
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, wxString(wxT("log")));
    
    wxString sDefaultOut = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("log");
    if(!pNode)
        return sDefaultOut;
    return pNode->GetPropVal(wxT("path"), sDefaultOut);
}


wxString wxGISAppConfig::GetSysDir(void)
{
    wxXmlNode* pNode = GetConfigNode(enumGISHKLM, wxString(wxT("sys")));
    
    wxString sDefaultOut = m_sExeDirPath + wxFileName::GetPathSeparator() + wxT("sys");
    if(!pNode)
        return sDefaultOut;
    return pNode->GetPropVal(wxT("path"), sDefaultOut);
}

bool wxGISAppConfig::GetDebugMode(void)
{
    wxXmlNode* pNode = GetConfigNode(enumGISHKLM, wxString(wxT("debug")));
    
    bool bDefaultOut = false;
    if(!pNode)
        return bDefaultOut;
    return wxString(wxT("on")) == pNode->GetPropVal(wxT("mode"), wxT("on")) ? true : false;
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
	if(pNode->HasProp(wxT("locale")))
		pNode->DeleteProperty(wxT("locale"));
	pNode->AddProperty(wxT("locale"), sLocale);

}

void wxGISAppConfig::SetLocaleDir(wxString sLocaleDir)
{
	wxString sPropPath(wxT("loc"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasProp(wxT("path")))
		pNode->DeleteProperty(wxT("path"));
	pNode->AddProperty(wxT("path"), sLocaleDir);
}

void wxGISAppConfig::SetSysDir(wxString sSysDir)
{
	wxString sPropPath(wxT("sys"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKLM, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKLM, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasProp(wxT("path")))
		pNode->DeleteProperty(wxT("path"));
	pNode->AddProperty(wxT("path"), sSysDir);
}

void wxGISAppConfig::SetLogDir(wxString sLogDir)
{
	wxString sPropPath(wxT("log"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKCU, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKCU, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasProp(wxT("path")))
		pNode->DeleteProperty(wxT("path"));
	pNode->AddProperty(wxT("path"), sLogDir);
}

void wxGISAppConfig::SetDebugMode(bool bDebug)
{
	wxString sPropPath = wxString(wxT("debug"));
    wxXmlNode* pNode = GetConfigNode(enumGISHKLM, sPropPath);
    if(!pNode)
	{
		pNode = CreateConfigNode(enumGISHKLM, sPropPath);
		if(!pNode)
			return;
	}
	if(pNode->HasProp(wxT("mode")))
		pNode->DeleteProperty(wxT("mode"));
	pNode->AddProperty(wxT("mode"), bDebug == true ? wxT("on") : wxT("off"));
}

