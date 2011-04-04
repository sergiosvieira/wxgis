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
#include "wxgis/framework/application.h"

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
    m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxToolbox::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
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
		m_pCatalog->ObjectChanged(GetID());
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
            IGxObject* pGxObj = static_cast<IGxObject*>(pTool);
            if(!AddChild(pGxObj))
                wxDELETE(pGxObj);
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

void wxGxRootToolbox::Init(wxXmlNode* const pConfigNode)
{
#ifdef WXGISPORTABLE
    m_pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, true);
#else
	m_pConfig = new wxGISAppConfig(TOOLBX_NAME, CONFIG_DIR, false);
#endif
    m_pPropNode = new wxXmlNode(*pConfigNode);

    LoadChildren();
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

IGPToolSPtr wxGxRootToolbox::GetGPTool(wxString sToolName)
{
    return m_pToolMngr->GetTool(sToolName, m_pCatalog);
}

bool wxGxRootToolbox::OnPrepareTool(wxWindow* pParentWnd, IGPToolSPtr pTool, IGPCallBack* pCallBack, bool bSync)
{
    if(!pTool)
    {
        wxMessageBox(_("Wrong input tool!"), _("Error"), wxICON_ERROR | wxOK );
        return false;
    }
    //create tool config dialog
    wxGISGPToolDlg* pDlg = new wxGISGPToolDlg(this, pTool, pCallBack, bSync, pParentWnd, wxID_ANY, pTool->GetDisplayName());
    pDlg->Show(true);

    return true;
}

void wxGxRootToolbox::OnExecuteTool(wxWindow* pParentWnd, IGPToolSPtr pTool, IGPCallBack* pCallBack, bool bSync)
{
    if(bSync)
    {
        wxGxTaskExecDlg dlg(m_pToolMngr, pCallBack, pParentWnd, wxID_ANY, pTool->GetDisplayName());
        int nTaskID = m_pToolMngr->OnExecute(pTool, static_cast<ITrackCancel*>(&dlg), static_cast<IGPCallBack*>(&dlg));
        dlg.SetTaskID(nTaskID);
        dlg.ShowModal();
    }
    else
    {
        m_pToolMngr->OnExecute(pTool, NULL, pCallBack);
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
    m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxFavoritesToolbox::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
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
                IGxObject* pGxObj = static_cast<IGxObject*>(pTool);
                if(!AddChild(pGxObj))
                    wxDELETE(pGxObj);
           }
        }
    }

    m_bIsChildrenLoaded = true;
}

wxString wxGxFavoritesToolbox::GetName(void)
{
    return wxString(_("Recent"));
}

/////////////////////////////////////////////////////////////////////////
// wxGxToolExecute
/////////////////////////////////////////////////////////////////////////

wxGxToolExecute::wxGxToolExecute(wxGxRootToolbox* pRootToolbox, wxXmlNode* pToolsNode) : wxGISGPToolManager(pToolsNode)
{
    m_pRootToolbox = pRootToolbox;
    m_LargeToolIcon = wxIcon(toolexec_48_xpm);
    m_SmallToolIcon = wxIcon(toolexec_16_xpm);
}

wxGxToolExecute::~wxGxToolExecute(void)
{
}

wxIcon wxGxToolExecute::GetLargeImage(void)
{
	return m_LargeToolIcon;
}

wxIcon wxGxToolExecute::GetSmallImage(void)
{
	return m_SmallToolIcon;
}

void wxGxToolExecute::Detach(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
}

void wxGxToolExecute::Refresh(void)
{
    m_pCatalog->ObjectRefreshed(GetID());
}

wxString wxGxToolExecute::GetName(void)
{
    if(m_nRunningTasks)
        return wxString::Format(_("Executed list [%d]"), m_nRunningTasks);
    else
        return wxString(_("Executed list"));

}

int wxGxToolExecute::OnExecute(IGPToolSPtr pTool, ITrackCancel* pTrackCancel, IGPCallBack* pCallBack)
{
	int nTaskID = wxNOT_FOUND;
	if(pTrackCancel)
	{
		nTaskID = wxGISGPToolManager::OnExecute(pTool, pTrackCancel, pCallBack);
	}
	else
	{
		wxWindow* pWnd = dynamic_cast<wxWindow*>(GetApplication());
		wxGxTaskObject *pGxTaskObject = new wxGxTaskObject(this, pTool->GetDisplayName(), pCallBack, m_LargeToolIcon, m_SmallToolIcon);
		nTaskID = wxGISGPToolManager::OnExecute(pTool, static_cast<ITrackCancel*>(pGxTaskObject), static_cast<IGPCallBack*>(pGxTaskObject));
		pGxTaskObject->SetTaskID(nTaskID);
		//add gxobj
        IGxObject* pGxObject = static_cast<IGxObject*>(pGxTaskObject);
		if(!AddChild(pGxObject))
		{
			wxDELETE(pGxTaskObject)
			return wxNOT_FOUND;
		}
        else
        {
            if(m_pCatalog)
                m_pCatalog->ObjectAdded(pGxObject->GetID());
        }

		if(m_pCatalog)
			m_pCatalog->ObjectChanged(GetID());
	}
	return nTaskID;
}

void wxGxToolExecute::StartProcess(size_t nIndex)
{
    wxGISGPToolManager::StartProcess(nIndex);

    if(!m_pCatalog)
        return;
    for(size_t i = 0; i < m_Children.size(); i++)
    {
        wxGxTaskObject* pGxTaskObject = dynamic_cast<wxGxTaskObject*>(m_Children[i]);
        if(pGxTaskObject && pGxTaskObject->GetTaskID() == nIndex && m_pCatalog)
            m_pCatalog->ObjectChanged(m_Children[i]->GetID());
    }
    m_pCatalog->ObjectChanged(GetID());
}

void wxGxToolExecute::OnFinish(IProcess* pProcess, bool bHasErrors)
{
    wxGISGPToolManager::OnFinish(pProcess, bHasErrors);
    size_t nIndex;
    for(nIndex = 0; nIndex < m_ProcessArray.size(); nIndex++)
        if(pProcess == m_ProcessArray[nIndex].pProcess)
            break;

    if(!m_pCatalog)
        return;

    for(size_t i = 0; i < m_Children.size(); i++)
    {
        wxGxTaskObject* pGxTaskObject = dynamic_cast<wxGxTaskObject*>(m_Children[i]);
        if(pGxTaskObject && pGxTaskObject->GetTaskID() == nIndex && m_pCatalog)
            m_pCatalog->ObjectChanged(m_Children[i]->GetID());
    }
    m_pCatalog->ObjectChanged(GetID());
}

bool wxGxToolExecute::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}

/////////////////////////////////////////////////////////////////////////
// wxGxTool
/////////////////////////////////////////////////////////////////////////

wxGxTool::wxGxTool(wxGxRootToolbox* pRootToolbox, wxXmlNode* pDataNode, wxIcon LargeToolIcon, wxIcon SmallToolIcon)
{
    m_pDataNode = pDataNode;
    m_pRootToolbox = pRootToolbox;
    if(m_pDataNode && m_pRootToolbox)
        m_sInternalName = m_pDataNode->GetPropVal(wxT("name"), NONAME);

    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
}

wxGxTool::wxGxTool(wxGxRootToolbox* pRootToolbox, wxString sInternalName, wxIcon LargeToolIcon, wxIcon SmallToolIcon)
{
    m_pDataNode = NULL;
    m_pRootToolbox = pRootToolbox;
    m_sInternalName = sInternalName;

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
    //callback create/destroy gxtask
    //m_pRootToolbox->OnPrepareTool(pParentWnd, m_sInternalName, wxEmptyString, NULL/*???*/, false);
    //return false;

	IGPToolSPtr pTool = m_pRootToolbox->GetGPTool(m_sInternalName);
	if(!pTool)
	{
        //error msg
		wxMessageBox(wxString::Format(_("Error find %s tool!\nCannnot continue."), m_sInternalName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return false; 
	}
	//TODO: Callback to view?
	m_pRootToolbox->OnPrepareTool(pParentWnd, pTool, NULL, false);
    return true;

}

bool wxGxTool::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
    if(!IGxObject::Attach(pParent, pCatalog))
        return false;
    wxGISGPToolManager* pGPToolManager = m_pRootToolbox->GetGPToolManager();
    IGPToolSPtr pTool = pGPToolManager->GetTool(m_sInternalName, m_pCatalog);
    if(pTool)
    {
        m_sName = pTool->GetDisplayName();
//        wxDELETE(pTool);
        return true;
    }
    else
        return false;
}
