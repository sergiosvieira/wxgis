/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxToolboxView class.
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

#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/geoprocessingui/gptoolbox.h"
#include "wxgis/framework/droptarget.h"

//-------------------------------------------------------------------
// wxGxToolboxView
//-------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxToolboxView, wxAuiNotebook)

wxGxToolboxView::wxGxToolboxView(void)
{
}

wxGxToolboxView::wxGxToolboxView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxAuiNotebook(parent, id, pos, size, wxAUI_NB_BOTTOM | wxNO_BORDER | wxAUI_NB_TAB_MOVE)
{
    m_sViewName = wxString(_("wxGISToolbox"));
}

wxGxToolboxView::~wxGxToolboxView(void)
{
}

bool wxGxToolboxView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_sViewName = wxString(_("wxGISToolbox"));
    return wxAuiNotebook::Create(parent, id, pos, size, wxAUI_NB_BOTTOM | wxNO_BORDER | wxAUI_NB_TAB_MOVE);
}

bool wxGxToolboxView::Activate(IApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;

    m_pApp = application;

    //get config xml
    wxXmlNode *pTaskExecConf(NULL), *pToolboxTreeConf(NULL);
    wxXmlNode* pChildConf = pConf->GetChildren();
    while(pChildConf)
    {
        if(pChildConf->GetName() == wxString(wxT("wxGxToolExecuteView")))
            pTaskExecConf = pChildConf;
        if(pChildConf->GetName() == wxString(wxT("wxGxToolboxTreeView")))
            pToolboxTreeConf = pChildConf;

        pChildConf = pChildConf->GetNext();
    }
    if(!pTaskExecConf)
    {
        pTaskExecConf = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("wxGxToolExecuteView")));
        pConf->AddChild(pTaskExecConf);
    }
    if(!pToolboxTreeConf)
    {
        pToolboxTreeConf = new wxXmlNode(wxXML_ELEMENT_NODE, wxString(wxT("wxGxToolboxTreeView")));
        pConf->AddChild(pToolboxTreeConf);
    }

    m_pGxToolboxView = new wxGxToolboxTreeView(this, TREECTRLID);
    AddPage(m_pGxToolboxView, m_pGxToolboxView->GetViewName(), true, m_pGxToolboxView->GetViewIcon());
    m_pGxToolboxView->Activate(application, pToolboxTreeConf);
    m_pApp->RegisterChildWindow(m_pGxToolboxView);


    wxGxToolExecute* pGxToolExecute(NULL);
    IGxObjectContainer* pRootContainer = dynamic_cast<IGxObjectContainer*>(m_pGxApplication->GetCatalog());
    if(pRootContainer)
    {
        IGxObjectContainer* pGxToolboxes = dynamic_cast<IGxObjectContainer*>(pRootContainer->SearchChild(wxString(_("Toolboxes"))));
        if(pGxToolboxes)
        {
            GxObjectArray* pArr = pGxToolboxes->GetChildren();
            if(pArr)
            {
                for(size_t i = 0; i < pArr->size(); ++i)
                {
                    pGxToolExecute = dynamic_cast<wxGxToolExecute*>(pArr->operator[](i));
                    if(pGxToolExecute)
                        break;
                }
            }
        }
    }

    if(pGxToolExecute)
    {        
        m_pGxToolExecuteView = new wxGxToolExecuteView(this, TOOLEXECUTECTRLID);
        AddPage(m_pGxToolExecuteView, m_pGxToolExecuteView->GetViewName(), false, m_pGxToolExecuteView->GetViewIcon());
        m_pGxToolExecuteView->Activate(application, pTaskExecConf);
        m_pGxToolExecuteView->SetGxToolExecute(pGxToolExecute);
        m_pApp->RegisterChildWindow(m_pGxToolExecuteView);
    }

	return true;
}

void wxGxToolboxView::Deactivate(void)
{
    m_pGxToolExecuteView->Deactivate();
    m_pApp->UnRegisterChildWindow(m_pGxToolExecuteView);

    m_pGxToolboxView->Deactivate();
    m_pApp->UnRegisterChildWindow(m_pGxToolboxView);
}

wxWindow* wxGxToolboxView::GetCurrentWnd(void)
{
	//int nSelTab = GetSelection();
	//wxASSERT(nSelTab >= 0 && nSelTab < m_Tabs.size());

	//wxGxTab* pCurrTab = m_Tabs[nSelTab];
	//if(pCurrTab)
	//	return pCurrTab->GetCurrentWindow();
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////
// wxGxToolsExecView
////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGxToolboxTreeView, wxGxTreeView)

wxGxToolboxTreeView::wxGxToolboxTreeView(void) : wxGxTreeView()
{
}

wxGxToolboxTreeView::wxGxToolboxTreeView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeView(parent, id, style)
{
    SetDropTarget(new wxGISDropTarget(static_cast<IViewDropTarget*>(this)));
    m_sViewName = wxString(_("Toolboxes"));
}

wxGxToolboxTreeView::~wxGxToolboxTreeView(void)
{
}

bool wxGxToolboxTreeView::Activate(IApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pGxApplication->GetCatalog());
	//delete
    m_pDeleteCmd = application->GetCommand(wxT("wxGISCatalogMainCmd"), 4);
	//new
	m_pNewMenu = dynamic_cast<wxGISNewMenu*>(application->GetCommandBar(NEWMENUNAME));

    IGxObject* pGxToolboxes = m_pCatalog->SearchChild(wxString(_("Toolboxes")));
    AddRoot(dynamic_cast<IGxObject*>(pGxToolboxes));

	m_pConnectionPointCatalog = dynamic_cast<IConnectionPointContainer*>( m_pGxApplication->GetCatalog() );
	if(m_pConnectionPointCatalog != NULL)
		m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

	m_pSelection = m_pCatalog->GetSelection();
	//m_pConnectionPointSelection = dynamic_cast<IConnectionPointContainer*>( m_pSelection );
	//if(m_pConnectionPointSelection != NULL)
	//	m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(this);

	return true;
};

void wxGxToolboxTreeView::UpdateGxSelection(void)
{
    wxTreeItemId TreeItemId = GetSelection();
    m_pSelection->Clear(GetId());
    //if(TreeItemId.IsOk())
    //{
    //    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
    //    if(pData != NULL)
	   //     m_pSelection->Select(pData->m_pObject, true, GetId());
    //}
	if(	m_pNewMenu )
		m_pNewMenu->Update(m_pSelection);
}

void wxGxToolboxTreeView::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent)
{
    wxGxToolExecute* pGxToolExecute = dynamic_cast<wxGxToolExecute*>(pGxObject);
    if(pGxToolExecute)
        return;
    wxGxTreeView::AddTreeItem(pGxObject, hParent);
}
