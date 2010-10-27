/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnections class.
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
#include "wxgis/catalog/gxdiscconnections.h"
#include "../../art/folder_conns_16.xpm"
#include "../../art/folder_conns_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnections, wxObject)

wxGxDiscConnections::wxGxDiscConnections(void) : m_bIsChildrenLoaded(false)
{
}

wxGxDiscConnections::~wxGxDiscConnections(void)
{
}

wxIcon wxGxDiscConnections::GetLargeImage(void)
{
	return wxIcon(folder_conns_48_xpm);
}

wxIcon wxGxDiscConnections::GetSmallImage(void)
{
	return wxIcon(folder_conns_16_xpm);
}

void wxGxDiscConnections::Detach(void)
{
	EmptyChildren();
}

void wxGxDiscConnections::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(this);
}
 
void wxGxDiscConnections::Init(wxXmlNode* pConfigNode)
{
    //m_sPath = pConfigNode->GetPropVal(wxT("path"), NON);
    //if(m_sPath.IsEmpty() || m_sPath == wxString(NON))
    //{
    //    ///vsizip/c:/wxGIS/sys/cs.zip/cs
    //    wxStandardPaths stp;
    //    wxString sExeDirPath = wxPathOnly(stp.GetExecutablePath());
    //    m_sPath = wxT("/vsizip/") + sExeDirPath + wxT("/sys/cs.zip/cs");
    //    m_sPath.Replace(wxT("\\"), wxT("/"));
    //}
    //else
    //    m_sPath.Replace(wxT("\\"), wxT("/"));
    //wxLogMessage(_("wxGxSpatialReferencesFolder: The path is set to '%s'"), m_sPath.c_str());
    //CPLSetConfigOption("wxGxSpatialReferencesFolder", wgWX2MB(m_sPath));
}

wxXmlNode* wxGxDiscConnections::GetProperties(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("rootitem"));
    wxClassInfo* pInfo = GetClassInfo();
    if(pInfo)
        pNode->AddProperty(wxT("name"), pInfo->GetClassName());
    pNode->AddProperty(wxT("is_enabled"), wxT("1"));    
//#ifndef WXGISPORTABLE
//    if(pNode->HasProp(wxT("path")))
//        pNode->DeleteProperty(wxT("path"));
//    pNode->AddProperty(wxT("path"), m_sPath);
//#endif  
    return pNode;
}

void wxGxDiscConnections::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        if(m_pCatalog)
        {
            IGxSelection* pSel = m_pCatalog->GetSelection();
            if(pSel)
                m_pCatalog->GetSelection()->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxGxDiscConnections::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(this);
	return true;
}

void wxGxDiscConnections::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;	

 //   wxBusyCursor wait;
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
	//				wxGxPrjFolder* pFolder = new wxGxPrjFolder(sFolderPath, wxGetTranslation(sFileName), m_pCatalog->GetShowHidden());
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
	//m_bIsChildrenLoaded = true;
}