/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/framework/toolbarmenu.h"

//-----------------------------------------------
// wxGxApplication
//-----------------------------------------------
IMPLEMENT_CLASS(wxGxApplication, wxGISApplicationEx)

wxGxApplication::wxGxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplicationEx(parent, id, title, pos, size, style), m_pCatalog(NULL), m_pTreeView(NULL), m_pTabView(NULL)
{
}

wxGxApplication::~wxGxApplication(void)
{
	//if(m_pCatalog)
	//	m_pCatalog->Detach();
	//wxDELETE(m_pCatalog);
}

IGxCatalog* const wxGxApplication::GetCatalog(void)
{
	return m_pCatalog;
}

bool wxGxApplication::Create(void)
{
	m_mgr.SetManagedWindow(this);

    m_pNewMenu = new wxGISNewMenu();
    m_pNewMenu->OnCreate(this);
    m_pNewMenu->Reference();
    m_CommandBarArray.push_back(m_pNewMenu);
    m_pNewMenu->Reference();
    m_CommandArray.push_back(m_pNewMenu);

    wxGISApplication::Create();

	wxLogMessage(_("wxGxApplication: Create and init catalog"));
	m_pCatalog = new wxGxCatalogUI();
	m_pCatalog->Init();

	wxLogMessage(_("wxGxApplication: Start. Creating main application frame..."));

	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return false;

	wxXmlNode* pViewsNode = pConfig->GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/views")));

	if(!pViewsNode)
	{
		wxLogError(_("wxGxApplication: Error finding <views> XML Node"));
		return false;
	}

    int nPaneCount(0);
    wxXmlNode* pViewsChildNode = pViewsNode->GetChildren();
    while(pViewsChildNode)
    {
        if(pViewsChildNode->GetName().CmpNoCase(wxT("treeview")) == 0)
        {
	        m_pTreeView = new wxGxTreeView(this, TREECTRLID);
	        if(m_pTreeView->Activate(this, pViewsChildNode))
	        {
		        m_mgr.AddPane(m_pTreeView, wxAuiPaneInfo().Name(wxT("tree_window")).Caption(_("Tree Pane")).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Left().Layer(1).Position(1).CloseButton(true));//.MinimizeButton(true).MaximizeButton(true)
		        RegisterChildWindow(m_pTreeView);
	        }
	        else
		        wxDELETE(m_pTreeView);
        }
        else if(pViewsChildNode->GetName().CmpNoCase(wxT("tabview")) == 0)
        {
	        m_pTabView = new wxGxTabView(this);
	        if(m_pTabView->Activate(this, pViewsChildNode))
	        {
		        m_mgr.AddPane(m_pTabView, wxAuiPaneInfo().Name(wxT("main_window")).CenterPane());//.PaneBorder(true)
		        RegisterChildWindow(m_pTabView);
	        }
	        else
		        wxDELETE(m_pTabView);
        }
        else
        {
 		    wxString sClass = pViewsChildNode->GetAttribute(wxT("class"), ERR);

		    wxObject *obj = wxCreateDynamicObject(sClass);
		    IView *pView = dynamic_cast<IView*>(obj);
            if(pView)
            {
                pView->Create(this);
			    wxWindow* pWnd = dynamic_cast<wxWindow*>(pView);
			    if(pWnd != NULL)
			    {
				    pWnd->Hide();
				    if(pView->Activate(this, pViewsChildNode))
                    {
                        nPaneCount++;
                        m_mgr.AddPane(pWnd, wxAuiPaneInfo().Name(wxString::Format(wxT("window_%d"), nPaneCount)).Caption(pView->GetViewName()).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Right().Layer(1).Position(nPaneCount).CloseButton(true));
                        RegisterChildWindow(pWnd);
                        wxLogMessage(_("wxGxApplication: View class %s initialise"), sClass.c_str());
                    }
				    else
				    {
					    wxLogError(_("wxGxApplication: Error activation IGxView class (%s)"), sClass.c_str());
					    wxDELETE(obj);
				    }
			    }
		    }
		    else
		    {
			    wxLogError(_("wxGxApplication: Error creating view %s"), sClass.c_str());
			    wxDELETE(obj);
		    }
        }

        pViewsChildNode = pViewsChildNode->GetNext();
    }

	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
			{
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(pToolBar->GetCaption()).ToolbarPane().Top().Position(i).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()).BestSize(-1,-1));
				pToolBar->Activate(this);
			}
		}
	}

	SerializeFramePosEx(false);

    //m_pNewMenu->Init();
	m_mgr.Update();


	wxString sLastPath;
	IGxObject* pObj = dynamic_cast<IGxObject*>(m_pCatalog);
	if(pObj)
		sLastPath = pObj->GetName();
	sLastPath = pConfig->Read(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), sLastPath);

	if(m_pCatalog)
	{
		if(m_pCatalog->GetOpenLastPath())
			m_pCatalog->SetLocation(sLastPath);
		else
			m_pCatalog->SetLocation(wxT(""));
	}

	wxLogMessage(_("wxGxApplication: Creation complete"));

	return true;
}

void wxGxApplication::OnClose(wxCloseEvent& event)
{
    event.Skip();

	//should remove toolbars from commandbar array as m_mgr manage toolbars by itself
    //m_pNewMenu->UnInit();

	if(m_pCatalog)
	{
		IGxSelection* pSel = m_pCatalog->GetSelection();

		if(pSel->GetCount(TREECTRLID) > 0)
		{
			IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(TREECTRLID, 0));
			if(pGxObject && pGxObject.get() != dynamic_cast<IGxObject*>(m_pCatalog))
			{
				wxString sLastPath = pGxObject->GetFullName();
				if(sLastPath.IsEmpty())
					sLastPath = pGxObject->GetName();

				wxGISAppConfigSPtr pConfig = GetConfig();
				if(pConfig)
					pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), sLastPath);
			}
		}
	}

 	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
				pToolBar->Deactivate();
		}
	}

	int nBegSize = m_WindowArray.size();
    for(size_t i = 0; i < m_WindowArray.size(); ++i)
    {
        IView* pView = dynamic_cast<IView*>(m_WindowArray[i]);
        if(pView)
            pView->Deactivate();
		//if during deactivation some windows would be unregistered and deleted from m_WindowArray
		if(nBegSize != m_WindowArray.size())
		{
			int nStayCount = nBegSize - i - m_WindowArray.size();
			i = i + nStayCount - 1;
			nBegSize = m_WindowArray.size();
		}
    }
	UnRegisterChildWindow(m_pTreeView);
	UnRegisterChildWindow(m_pTabView);

	if(m_pCatalog)
		m_pCatalog->Detach();
	wxDELETE(m_pCatalog);

    wxGISApplication::OnClose(event);

 	SerializeFramePosEx(true);
    m_mgr.UnInit();

  //  while (!m_WindowArray.empty())
  //  {
  //      wxWindow* poWnd = m_WindowArray.back();
		//if( poWnd && !poWnd->IsBeingDeleted() )
  //      {
		//	RemoveChild(poWnd);
  //          if(!poWnd->Destroy())
  //              delete poWnd;
  //      }
  //      m_WindowArray.pop_back();
  //  }
}


