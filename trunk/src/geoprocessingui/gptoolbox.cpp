/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  toolbox classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/geoprocessingui/gptoolbox.h"
#include "wxgis/geoprocessingui/gptooldlg.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include <wx/stdpaths.h>

#include "../../art/toolbox_16.xpm"
#include "../../art/toolbox_48.xpm"
#include "../../art/tool_16.xpm"
#include "../../art/tool_48.xpm"

/////////////////////////////////////////////////////////////////////////
// wxGxToolbox
/////////////////////////////////////////////////////////////////////////

wxGxToolbox::wxGxToolbox(wxGISGPToolManager* pToolMngr, wxXmlNode* pPropNode) : m_bIsChildrenLoaded(false), m_pToolMngr(NULL), m_pPropNode(NULL)
{
    m_pToolMngr = pToolMngr;
    m_pPropNode = pPropNode;
}

wxGxToolbox::wxGxToolbox(wxXmlNode* pDataNode, wxGISGPToolManager* pToolMngr, wxXmlNode* pPropNode) : m_bIsChildrenLoaded(false), m_pToolMngr(NULL), m_pPropNode(NULL)
{
    m_pToolMngr = pToolMngr;
    m_pPropNode = pPropNode;
    m_pDataNode = pDataNode;
    if(m_pDataNode)
        m_sName = wxGetTranslation( m_pDataNode->GetPropVal(wxT("name"), NONAME) );
}

wxGxToolbox::~wxGxToolbox(void)
{
}

wxIcon wxGxToolbox::GetLargeImage(void)
{
	return wxIcon(toolbox_48_xpm);
}

wxIcon wxGxToolbox::GetSmallImage(void)
{
	return wxIcon(toolbox_16_xpm);
}

void wxGxToolbox::Detach(void)
{
	EmptyChildren();
}

void wxGxToolbox::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(this);
}

void wxGxToolbox::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pCatalog);
        if(pCatalog)
        {
            IGxSelection* pSel = pCatalog->GetSelection();
            if(pSel)
                pSel->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxGxToolbox::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(this);
	return true;
}

void wxGxToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;
    LoadChildrenFromXml(m_pDataNode);
    m_bIsChildrenLoaded = true;
}

wxString wxGxToolbox::GetName(void)
{
    return m_sName;
}

void wxGxToolbox::LoadChildrenFromXml(wxXmlNode* pNode)
{
    wxXmlNode *pChild = pNode->GetChildren();
    while (pChild)
    {
        if(pChild->GetName().IsSameAs(wxT("toolbox"), false))
        {
            wxGxToolbox* pToolbox = new wxGxToolbox(pChild, m_pToolMngr, m_pPropNode);
            IGxObject* pGxObj = static_cast<IGxObject*>(pToolbox);
            if(!AddChild(pGxObj))
                wxDELETE(pGxObj);
        }
        else if(pChild->GetName().IsSameAs(wxT("tool"), false))
        {
            wxGxTool* pTool = new wxGxTool(pChild, m_pToolMngr, m_pPropNode);
            if(pTool->IsOk())
            {
                IGxObject* pGxObj = static_cast<IGxObject*>(pTool);
                if(!AddChild(pGxObj))
                    wxDELETE(pGxObj);
            }
            else
                wxDELETE(pTool);
        }
        pChild = pChild->GetNext();
    }
}

/////////////////////////////////////////////////////////////////////////
// wxGxRootToolbox
/////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxRootToolbox, wxObject)

wxGxRootToolbox::wxGxRootToolbox(void) : m_bIsChildrenLoaded(false), m_pConfig(NULL)
{
    m_pToolMngr = NULL;
    m_pPropNode = NULL;
}

wxGxRootToolbox::~wxGxRootToolbox(void)
{
}

void wxGxRootToolbox::Detach(void)
{
	EmptyChildren();
    wxDELETE(m_pToolMngr);

    wxDELETE(m_pConfig);

//    if(m_XmlDoc.IsOk())
//    {
//        m_XmlDoc.Save(m_sPath);
//    }
}

void wxGxRootToolbox::Init(wxXmlNode* pConfigNode)
{
#ifdef WXGISPORTABLE
    m_pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, true);
#else
	m_pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, false);
#endif


//    m_sPath = pConfigNode->GetPropVal(wxT("path"), NON);
//    if(m_sPath.IsEmpty() || m_sPath == wxString(NON))
//    {
//        wxStandardPaths stp;
//        wxString sExeDirPath = wxPathOnly(stp.GetExecutablePath());
//        m_sPath = sExeDirPath + wxT("/sys/toolbox.xml");
//        m_sPath.Replace(wxT("\\"), wxT("/"));
//    }
//    wxLogMessage(_("wxGxRootToolbox: The path is set to '%s'"), m_sPath.c_str());
//
    m_pPropNode = new wxXmlNode(*pConfigNode);
//    //m_pPropNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("rootitem"));
//    //wxClassInfo* pInfo = GetClassInfo();
//    //if(pInfo)
//    //    m_pPropNode->AddProperty(wxT("name"), pInfo->GetClassName());
//    //m_pPropNode->AddProperty(wxT("is_enabled"), wxT("1"));
//#ifdef WXGISPORTABLE
//    if(m_pPropNode->HasProp(wxT("path")))
//        m_pPropNode->DeleteProperty(wxT("path"));
//    m_pPropNode->AddProperty(wxT("path"), wxEmptyString);
//#endif
}

void wxGxRootToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;
//    if( !m_XmlDoc.Load(m_sPath) )
//        return;
//    if (m_XmlDoc.GetRoot()->GetName() != wxT("wxToolboxes"))
//        return;

    wxXmlNode *pToolsChild(NULL), *pToolboxesChild(NULL);
    pToolsChild = m_pConfig->GetConfigNode(wxT("tools"), false, true);
    pToolboxesChild = m_pConfig->GetConfigNode(wxT("toolboxes"), false, true);

//    wxXmlNode* pChild = m_XmlDoc.GetRoot()->GetChildren();
//    while(pChild)
//    {
//        if(pChild->GetName().CmpNoCase(wxT("toolboxes")) == 0)
//            pToolboxesChild = pChild;
//        if(pChild->GetName().CmpNoCase(wxT("tools")) == 0)
//            pToolsChild = pChild;
//        pChild = pChild->GetNext();
//    }

    m_pToolMngr = new wxGISGPToolManager(pToolsChild, m_pCatalog);
    LoadChildrenFromXml(pToolboxesChild);
    //load children from local machine node exluding dupes

 //   //VSIFilesystemHandler *poFSHandler = VSIFileManager::GetHandler( wgWX2MB(m_sPath) );
 //   //char **res = poFSHandler->ReadDir(wgWX2MB(m_sPath));

 //   char **papszFileList = VSIReadDir(wgWX2MB(m_sPath));

 //   if( CSLCount(papszFileList) == 0 )
 //   {
 //       wxLogMessage(wxT( "wxGxSpatialReferencesFolder: no files or directories" ));
 //   }
 //   else
 //   {
 //       //wxLogDebug(wxT("Files: %s"), wgMB2WX(papszFileList[0]) );
 //      	//wxArrayString FileNames;
 //       for(int i = 0; papszFileList[i] != NULL; i++ )
	//	{
	//		wxString sFileName = wgMB2WX(papszFileList[i]);
 //           //if(i > 0)
 //           //    wxLogDebug( wxT("       %s"), sFileName.c_str() );
 //           VSIStatBufL BufL;
	//		wxString sFolderPath = m_sPath + wxT("/") + sFileName;
 //           int ret = VSIStatL(wgWX2MB(sFolderPath), &BufL);
 //           if(ret == 0)
 //           {
 //               //int x = 0;
 //               if(VSI_ISDIR(BufL.st_mode))
 //               {
	//				wxGxPrjFolder* pFolder = new wxGxPrjFolder(sFolderPath, sFileName, m_pCatalog->GetShowHidden());
	//				IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
	//				bool ret_code = AddChild(pGxObj);
 //               }
 //               else
 //               {
 //                   m_FileNames.Add(sFolderPath);
 //               }
 //           }
	//	}
 //   }
 //   CSLDestroy( papszFileList );

	////load names
	//GxObjectArray Array;
	//if(m_pCatalog->GetChildren(m_sPath, &m_FileNames, &Array))
	//{
	//	for(size_t i = 0; i < Array.size(); i++)
	//	{
	//		bool ret_code = AddChild(Array[i]);
	//		if(!ret_code)
	//			wxDELETE(Array[i]);
	//	}
	//}
	m_bIsChildrenLoaded = true;
}


wxXmlNode* wxGxRootToolbox::GetProperties(void)
{
    if(m_pPropNode->HasProp(wxT("is_enabled")))
        m_pPropNode->DeleteProperty(wxT("is_enabled"));
    m_pPropNode->AddProperty(wxT("is_enabled"), m_bEnabled == true ? wxT("1") : wxT("0"));    
    return m_pPropNode;
}

/////////////////////////////////////////////////////////////////////////
// wxGxTool
/////////////////////////////////////////////////////////////////////////
wxGxTool::wxGxTool(wxGISGPToolManager* pToolMngr, wxXmlNode* pPropNode) : m_pToolMngr(NULL), m_pPropNode(NULL), m_bIsOk(true)
{
    m_sName = NONAME;
    m_pToolMngr = pToolMngr;
    m_pPropNode = pPropNode;
}

wxGxTool::wxGxTool(wxXmlNode* pDataNode, wxGISGPToolManager* pToolMngr, wxXmlNode* pPropNode) : m_pToolMngr(NULL), m_pPropNode(NULL), m_bIsOk(true)
{
    m_pDataNode = pDataNode;
    m_pToolMngr = pToolMngr;
    m_pPropNode = pPropNode;
    if(m_pDataNode && m_pToolMngr)
    {
        m_sInternalName = m_pDataNode->GetPropVal(wxT("name"), NONAME);
        IGPTool* pTool = m_pToolMngr->GetTool(m_sInternalName);
        if(pTool)
        {
            m_sName = pTool->GetDisplayName();
            wxDELETE(pTool);
        }
        else
            m_bIsOk = false;
    }
}

wxGxTool::~wxGxTool(void)
{
}

wxIcon wxGxTool::GetLargeImage(void)
{
	return wxIcon(tool_48_xpm);
}

wxIcon wxGxTool::GetSmallImage(void)
{
	return wxIcon(tool_16_xpm);
}

bool wxGxTool::Invoke(wxWindow* pParentWnd)
{
    IGPTool* pTool = m_pToolMngr->GetTool(m_sInternalName);
    if(pTool)
    {
        wxGISGPToolDlg* pDlg = new wxGISGPToolDlg(pTool, m_pToolMngr, m_pPropNode, pParentWnd, wxID_ANY, pTool->GetDisplayName());//);//
        pDlg->Show(true);
        //pDlg->SetParent(pParentWnd);
    //dlg.ShowModal();//(true);
    }
    return false;
}
