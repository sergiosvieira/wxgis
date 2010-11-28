/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  toolbox classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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
#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include <wx/stdpaths.h>

#include "../../art/toolbox_16.xpm"
#include "../../art/toolbox_48.xpm"
#include "../../art/tool_16.xpm"
#include "../../art/tool_48.xpm"
#include "../../art/toolboxfavor_16.xpm"
#include "../../art/toolboxfavor_48.xpm"
#include "../../art/toolexec_16.xpm"
#include "../../art/toolexec_48.xpm"

/////////////////////////////////////////////////////////////////////////
// wxGxToolbox
/////////////////////////////////////////////////////////////////////////
wxGxToolbox::wxGxToolbox() : m_bIsChildrenLoaded(false)
{
    m_pRootToolbox = NULL;
    m_pDataNode = NULL;
}

wxGxToolbox::wxGxToolbox(wxGxRootToolbox* pRootToolbox, wxXmlNode* pDataNode, wxIcon LargeToolboxIcon, wxIcon SmallToolboxIcon, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_bIsChildrenLoaded(false)
{
    m_pRootToolbox = pRootToolbox;
    m_pDataNode = pDataNode;
    if(m_pDataNode)
        m_sName = wxGetTranslation( m_pDataNode->GetPropVal(wxT("name"), NONAME) );
    m_LargeToolboxIcon = LargeToolboxIcon;
    m_SmallToolboxIcon = SmallToolboxIcon;
    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxToolbox::~wxGxToolbox(void)
{
}

wxIcon wxGxToolbox::GetLargeImage(void)
{
	return m_LargeToolboxIcon;
}

wxIcon wxGxToolbox::GetSmallImage(void)
{
	return m_SmallToolboxIcon;
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
            wxGxToolbox* pToolbox = new wxGxToolbox(m_pRootToolbox, pChild, m_LargeToolboxIcon, m_SmallToolboxIcon, m_LargeToolIcon, m_SmallToolIcon);
            IGxObject* pGxObj = static_cast<IGxObject*>(pToolbox);
            if(!AddChild(pGxObj))
                wxDELETE(pGxObj);
        }
        else if(pChild->GetName().IsSameAs(wxT("tool"), false))
        {
            wxGxTool* pTool = new wxGxTool(m_pRootToolbox, pChild, m_LargeToolIcon, m_SmallToolIcon);
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
    m_LargeToolboxIcon = wxIcon(toolbox_48_xpm);
    m_SmallToolboxIcon = wxIcon(toolbox_16_xpm);
    m_LargeToolIcon = wxIcon(tool_48_xpm);
    m_SmallToolIcon = wxIcon(tool_16_xpm);
}

wxGxRootToolbox::~wxGxRootToolbox(void)
{
}

void wxGxRootToolbox::Detach(void)
{
	EmptyChildren();
    wxDELETE(m_pConfig);
}

void wxGxRootToolbox::Init(wxXmlNode* pConfigNode)
{
#ifdef WXGISPORTABLE
    m_pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, true);
#else
	m_pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, false);
#endif
    m_pPropNode = new wxXmlNode(*pConfigNode);
}

void wxGxRootToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxXmlNode *pToolsChild(NULL), *pToolboxesChild(NULL);
    pToolsChild = m_pConfig->GetConfigNode(wxT("tools"), false, true);
    pToolboxesChild = m_pConfig->GetConfigNode(wxT("toolboxes"), false, true);

    m_pRootToolbox = this;

    //Add task exec
    wxGxToolExecute* pToolExecute = new wxGxToolExecute(m_pRootToolbox, pToolsChild);
    IGxObject* pGxObj = static_cast<IGxObject*>(pToolExecute);

    if( !AddChild(pGxObj) )
    {
        wxDELETE(pGxObj);
    }
    else
    {
        m_pToolMngr = static_cast<wxGISGPToolManager*>(pToolExecute);
    }

    //Add favorites
    bool bShowFavorites = wxAtoi(pToolboxesChild->GetPropVal(wxT("show_favorites"), wxT("1")));
    if(bShowFavorites)
    {
        short nMax = wxAtoi(pToolboxesChild->GetPropVal(wxT("max_favorites"), wxT("10")));
        wxGxFavoritesToolbox* pToolbox = new wxGxFavoritesToolbox(m_pRootToolbox, nMax, m_LargeToolIcon, m_SmallToolIcon);
        IGxObject* pGxObj = static_cast<IGxObject*>(pToolbox);
        if(!AddChild(pGxObj))
            wxDELETE(pGxObj);
    }
    LoadChildrenFromXml(pToolboxesChild);

	m_bIsChildrenLoaded = true;
}

wxXmlNode* wxGxRootToolbox::GetProperties(void)
{
    if(m_pPropNode->HasProp(wxT("is_enabled")))
        m_pPropNode->DeleteProperty(wxT("is_enabled"));
    m_pPropNode->AddProperty(wxT("is_enabled"), m_bEnabled == true ? wxT("1") : wxT("0"));    
    return m_pPropNode;
}

wxGISGPToolManager* wxGxRootToolbox::GetGPToolManager(void)
{
    return m_pToolMngr;
}

bool wxGxRootToolbox::OnPrepareTool(wxWindow* pParentWnd, wxString sToolName, wxString sInputPath, IGPCallBack* pCallBack, bool bSync)
{
    //get tool
    if(!m_pToolMngr)
        return false;
    IGPTool* pTool = m_pToolMngr->GetTool(sToolName, m_pCatalog);
    if(!pTool)
    {
        wxMessageBox(wxString::Format(_("Error get tool (internal name '%s')!"), sToolName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return false;
    }
    if(!sInputPath.IsEmpty())
    {
        GPParameters* pParams = pTool->GetParameterInfo();
        if(pParams->size() == 0)
            return false;
        IGPParameter* pParam = pParams->operator[](0);
        //TODO: check is input
        pParam->SetValue(wxVariant(sInputPath));
    }
    //create tool config dialog
    wxGISGPToolDlg* pDlg = new wxGISGPToolDlg(this, pTool, pCallBack, bSync, GetProperties(), pParentWnd);
    pDlg->Show(true);

    return true;
}

void wxGxRootToolbox::OnExecuteTool(wxWindow* pParentWnd, IGPTool* pTool, IGPCallBack* pCallBack, bool bSync)
{
    if(bSync)
    {
        wxGxTaskExecDlg dlg(m_pToolMngr, pCallBack, pParentWnd);
        int nTaskID = m_pToolMngr->OnExecute(pTool, static_cast<ITrackCancel*>(&dlg), static_cast<IGPCallBack*>(&dlg));
        dlg.SetTaskID(nTaskID);
        dlg.ShowModal();
    }
    else
    {
    }
}

/////////////////////////////////////////////////////////////////////////
// wxGxFavoritesToolbox
/////////////////////////////////////////////////////////////////////////

wxGxFavoritesToolbox::wxGxFavoritesToolbox(wxGxRootToolbox* pRootToolbox, short nMaxCount, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_bIsChildrenLoaded(false)
{
    m_pRootToolbox = pRootToolbox;
    m_nMaxCount = nMaxCount;
    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxFavoritesToolbox::~wxGxFavoritesToolbox(void)
{
}

wxIcon wxGxFavoritesToolbox::GetLargeImage(void)
{
	return wxIcon(toolboxfavor_48_xpm);
}

wxIcon wxGxFavoritesToolbox::GetSmallImage(void)
{
	return wxIcon(toolboxfavor_16_xpm);
}

void wxGxFavoritesToolbox::Detach(void)
{
	EmptyChildren();
}

void wxGxFavoritesToolbox::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(this);
}

void wxGxFavoritesToolbox::EmptyChildren(void)
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

bool wxGxFavoritesToolbox::DeleteChild(IGxObject* pChild)
{
	//bool bHasChildren = m_Children.size() > 0 ? true : false;
	//if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	//if(bHasChildren != m_Children.size() > 0 ? true : false)
	//	m_pCatalog->ObjectChanged(this);
	//return true;
}

void wxGxFavoritesToolbox::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    if(m_pRootToolbox)
    {
        wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
        if(pGPToolManager)
        {
            int nCount = MIN(pGPToolManager->GetToolCount(), m_nMaxCount);
            for(size_t i = 0; i < nCount; i++)
            {
                wxGxTool* pTool = new wxGxTool(m_pRootToolbox, pGPToolManager->GetPopularTool(i), m_LargeToolIcon, m_SmallToolIcon);
                if(pTool->IsOk())
                {
                    IGxObject* pGxObj = static_cast<IGxObject*>(pTool);
                    if(!AddChild(pGxObj))
                        wxDELETE(pGxObj);
                }
                else
                    wxDELETE(pTool);
           }
        }
    }

    m_bIsChildrenLoaded = true;
}

wxString wxGxFavoritesToolbox::GetName(void)
{
    return wxString(_("Favorites"));
}

/////////////////////////////////////////////////////////////////////////
// wxGxToolExecute
/////////////////////////////////////////////////////////////////////////

wxGxToolExecute::wxGxToolExecute(wxGxRootToolbox* pRootToolbox, wxXmlNode* pToolsNode) : wxGISGPToolManager(pToolsNode)
{
    m_pRootToolbox = pRootToolbox;
}

wxGxToolExecute::~wxGxToolExecute(void)
{
}

wxIcon wxGxToolExecute::GetLargeImage(void)
{
	return wxIcon(toolexec_48_xpm);
}

wxIcon wxGxToolExecute::GetSmallImage(void)
{
	return wxIcon(toolexec_16_xpm);
}

void wxGxToolExecute::Detach(void)
{
}

void wxGxToolExecute::Refresh(void)
{
    m_pCatalog->ObjectRefreshed(this);
}

wxString wxGxToolExecute::GetName(void)
{
    if(m_nRunningTasks)
        return wxString::Format(_("Executed list [%d]"), m_nRunningTasks);
    else
        return wxString(_("Executed list"));

}

/////////////////////////////////////////////////////////////////////////
// wxGxTool
/////////////////////////////////////////////////////////////////////////

wxGxTool::wxGxTool(wxGxRootToolbox* pRootToolbox, wxXmlNode* pDataNode, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_bIsOk(true)
{
    m_pDataNode = pDataNode;
    m_pRootToolbox = pRootToolbox;
    if(m_pDataNode && m_pRootToolbox)
    {
        m_sInternalName = m_pDataNode->GetPropVal(wxT("name"), NONAME);
        wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
        IGPTool* pTool = pGPToolManager->GetTool(m_sInternalName, m_pCatalog);
        if(pTool)
        {
            m_sName = pTool->GetDisplayName();
        }
        else
            m_bIsOk = false;
    }
    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxTool::wxGxTool(wxGxRootToolbox* pRootToolbox, wxString sInternalName, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_bIsOk(true)
{
    m_pDataNode = NULL;
    m_pRootToolbox = pRootToolbox;
    m_sInternalName = sInternalName;
    wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
    IGPTool* pTool = pGPToolManager->GetTool(m_sInternalName, m_pCatalog);
    if(pTool)
    {
        m_sName = pTool->GetDisplayName();
    }
    else
        m_bIsOk = false;

    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxTool::~wxGxTool(void)
{
}

wxIcon wxGxTool::GetLargeImage(void)
{
	return m_LargeToolIcon;
}

wxIcon wxGxTool::GetSmallImage(void)
{
	return m_SmallToolIcon;
}

bool wxGxTool::Invoke(wxWindow* pParentWnd)
{
    //wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
    //IGPTool* pTool = pGPToolManager->GetTool(m_sInternalName, m_pCatalog);
    //if(pTool)
    //{
    //    wxGISGPToolDlg* pDlg = new wxGISGPToolDlg(pTool, m_pRootToolbox->GetProperties(), pParentWnd, wxID_ANY, pTool->GetDisplayName());
    //    pDlg->Show(true);//dlg.ShowModal();
    //}
    //callback create/destroy gxtask
    m_pRootToolbox->OnPrepareTool(pParentWnd, m_sInternalName, wxEmptyString, NULL/*???*/, false);
    return false;
}
