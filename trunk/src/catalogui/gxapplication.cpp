/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication class.
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
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalogui/customizedlg.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/framework/toolbarmenu.h"

//-----------------------------------------------
// wxGxApplication
//-----------------------------------------------
wxGxApplication::wxGxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplication(parent, id, title, pos, size, style), m_pCatalog(NULL), m_pTreeView(NULL), m_pTabView(NULL)
{
}

wxGxApplication::~wxGxApplication(void)
{
	SerializeGxFramePos(true);
	//should remove toolbars from commandbar array as m_mgr manage toolbars by itself 

    m_pNewMenu->UnInit();

    IGxSelection* pSel = m_pCatalog->GetSelection();

    if(pSel->GetCount(TREECTRLID) > 0)
	{
        IGxObject* pObj = pSel->GetSelectedObjects(TREECTRLID, 0);
        if(pObj && pObj != dynamic_cast<IGxObject*>(m_pCatalog))
        {
		    wxString sLastPath = pObj->GetFullName();
		    if(sLastPath.IsEmpty())
			    sLastPath = pObj->GetName();

		    wxXmlNode* pLastLocationNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("lastpath")));
		    if(pLastLocationNode)
		    {
			    if(pLastLocationNode->HasProp(wxT("path")))
				    pLastLocationNode->DeleteProperty(wxT("path"));
		    }
		    else
		    {
			    pLastLocationNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("lastpath")), true);
		    }
		    pLastLocationNode->AddProperty(wxT("path"), sLastPath);
        }
	}

	if(m_pTreeView)
		m_pTreeView->Deactivate();
	if(m_pTabView)
		m_pTabView->Deactivate();

	////wxDELETE(m_pTreeView);
 //   m_pTreeView->Destroy();
	////wxDELETE(m_pTabView);
 //   m_pTabView->Destroy();
    for(size_t i = 0; i < m_WindowArray.size(); i++)
        //if(m_WindowArray[i])
        //    if(!m_WindowArray[i]->Destroy()) //?? the last window didn't destroy?
        wxDELETE(m_WindowArray[i]);

	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
				pToolBar->Deactivate();
		}
	}

	m_mgr.UnInit();
	wxDELETE(m_pCatalog);
}

IGxCatalog* wxGxApplication::GetCatalog(void)
{
	return m_pCatalog;
}

void wxGxApplication::SerializeGxFramePos(bool bSave)
{
	wxXmlNode *pPerspectiveXmlNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("frame/perspective")));
	if(bSave)
	{
		if(pPerspectiveXmlNode)
		{
			if(pPerspectiveXmlNode->HasProp(wxT("data")))
				pPerspectiveXmlNode->DeleteProperty(wxT("data"));
		}
		else
			pPerspectiveXmlNode = m_pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("frame/perspective")), true);
		pPerspectiveXmlNode->AddProperty(wxT("data"), m_mgr.SavePerspective());
	}
	else
	{
		if(pPerspectiveXmlNode)
		{
			wxString wxPerspective = pPerspectiveXmlNode->GetPropVal(wxT("data"), wxT(""));
			m_mgr.LoadPerspective(wxPerspective);
		}
	}
}

void wxGxApplication::Customize(void)
{
	wxGISCustomizeDlg dialog(this);
	if(dialog.ShowModal() == wxID_OK )
	{
		if(m_pGISAcceleratorTable->HasChanges())
			SetAcceleratorTable(m_pGISAcceleratorTable->GetAcceleratorTable());
		wxGISToolBarMenu* pToolBarMenu =  dynamic_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
		if(pToolBarMenu)
			pToolBarMenu->Update();
	}
}
void wxGxApplication::RemoveCommandBar(IGISCommandBar* pBar)
{
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	{
		if(m_CommandBarArray[i] == pBar)
		{
			switch(m_CommandBarArray[i]->GetType())
			{
			case enumGISCBMenubar:
				m_pMenuBar->RemoveMenu(pBar);
				break;
			case enumGISCBToolbar:
				m_mgr.DetachPane(dynamic_cast<wxWindow*>(pBar));
				m_mgr.Update();
				break;	
			case enumGISCBContextmenu:
			case enumGISCBSubMenu:
			case enumGISCBNone: 
				break;
			}			
			wxDELETE(pBar);
			m_CommandBarArray.erase(m_CommandBarArray.begin() + i);
			break;
		}
	}
}

void wxGxApplication::ShowPane(wxWindow* pWnd, bool bShow)
{
	m_mgr.GetPane(pWnd).Show(bShow);
	m_mgr.Update();
}

void wxGxApplication::ShowPane(const wxString& sName, bool bShow)
{
	m_mgr.GetPane(sName).Show(bShow);
	m_mgr.Update();
}

bool wxGxApplication::IsPaneShown(const wxString& sName)
{
	return m_mgr.GetPane(sName).IsShown();
}

bool wxGxApplication::IsApplicationWindowShown(wxWindow* pWnd)
{
	return m_mgr.GetPane(pWnd).IsShown();
}

bool wxGxApplication::AddCommandBar(IGISCommandBar* pBar)
{
	if(!pBar)
		return false;
	m_CommandBarArray.push_back(pBar);
	switch(pBar->GetType())
	{
	case enumGISCBMenubar:
		m_pMenuBar->AddMenu(pBar);
		break;
	case enumGISCBToolbar:
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(pBar);
			if(pToolBar)
			{
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(wxGetTranslation(pToolBar->GetCaption())).ToolbarPane().Top().Position(100).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()));
				m_mgr.Update();
			}
		}
		break;
	case enumGISCBContextmenu:
	case enumGISCBSubMenu:
	case enumGISCBNone: 
		break;
	}
	return true;
}

void wxGxApplication::ShowStatusBar(bool bShow)
{
	wxGISApplication::ShowStatusBar(bShow);
	m_mgr.Update();
//	if(bShow)
//	{
//		wxFrame::GetStatusBar()->SetSize(GetSize().GetWidth(), wxDefaultCoord);
////		wxFrame::GetStatusBar()->Update();
////		wxFrame::Update();
//	}
}

void wxGxApplication::ShowApplicationWindow(wxWindow* pWnd, bool bShow)
{
	ShowPane(pWnd, bShow);
}

WINDOWARRAY* wxGxApplication::GetChildWindows(void)
{
	return &m_WindowArray;
}

void wxGxApplication::RegisterChildWindow(wxWindow* pWnd)
{
	for(size_t i = 0; i < m_WindowArray.size(); i++)
		if(m_WindowArray[i] == pWnd)
			return;
	m_WindowArray.push_back(pWnd);
}

void wxGxApplication::UnRegisterChildWindow(wxWindow* pWnd)
{
	for(size_t i = 0; i < m_WindowArray.size(); i++)
    {
		if(m_WindowArray[i] == pWnd)
        {
			m_WindowArray.erase(m_WindowArray.begin() + i);
            return;
        }
    }
}

bool wxGxApplication::Create(IGISConfig* pConfig)
{
    m_pNewMenu = new wxGISNewMenu();
    m_pNewMenu->OnCreate(this);
    m_pNewMenu->Reference();
    m_CommandBarArray.push_back(m_pNewMenu);
    m_pNewMenu->Reference();
    m_CommandArray.push_back(m_pNewMenu);

    wxGISApplication::Create(pConfig);
    
	wxLogMessage(_("wxGxApplication: Creating main application frame..."));

	m_mgr.SetManagedWindow(this);

	wxGxCatalog* pCatalog = new wxGxCatalog();
	pCatalog->Init();
	m_pCatalog = static_cast<IGxCatalog*>(pCatalog);

	wxXmlNode* pViewsNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/views")));

	if(!pViewsNode)
	{
		wxLogError(_("wxGxApplication: Error find <views> XML Node"));
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
		        m_mgr.AddPane(m_pTreeView, wxAuiPaneInfo().Name(wxT("tree_window")).Caption(_("Tree Pane")).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Left().Layer(1/*2*/).Position(1).CloseButton(true));
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
 		    wxString sClass = pViewsChildNode->GetPropVal(wxT("class"), ERR);
		    wxString sName = pViewsChildNode->GetPropVal(wxT("name"), NONAME);

		    wxObject *obj = wxCreateDynamicObject(sClass);
		    IGxViewsFactory *pFactory = dynamic_cast<IGxViewsFactory*>(obj);
		    if(pFactory != NULL)
		    {
			    wxWindow* pWnd = pFactory->CreateView(sName, this);
			    if(pWnd != NULL)
			    {
				    pWnd->Hide();

				    wxGxView* pView = dynamic_cast<wxGxView*>(pWnd);
				    if(pView != NULL)
				    {
					    if(pView->Activate(this, pViewsChildNode))
                        {
                            nPaneCount++;
                            m_mgr.AddPane(pWnd, wxAuiPaneInfo().Name(wxString::Format(wxT("window_%d"), nPaneCount)).Caption(pView->GetViewName()).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Right().Layer(1).Position(nPaneCount).CloseButton(true));
                            RegisterChildWindow(pWnd);
                            wxLogMessage(_("wxGxApplication: View class %s.%s initialise"), sClass.c_str(), sName.c_str());
                        }
				    }
				    else
				    {
					    wxLogError(_("wxGxApplication: This is not inherited IGxView class (%s.%s)"), sClass.c_str(), sName.c_str());
					    wxDELETE(pWnd);
				    }
			    }
			    else
			    {
				    wxLogError(_("wxGxApplication: Error creating view %s.%s"), sClass.c_str(), sName.c_str());
				    wxDELETE(pFactory);
			    }
		    }
		    else
		    {
			    wxLogError(_("wxGxApplication: Error initializing ViewsFactory %s"), sClass.c_str());
			    wxDELETE(obj);
		    }

		    wxDELETE(pFactory);
        }

        pViewsChildNode = pViewsChildNode->GetNext();
    }
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
			{
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(wxGetTranslation(pToolBar->GetCaption())).ToolbarPane().Top().Position(i).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()).BestSize(-1,-1));
				pToolBar->Activate(this);
			}
		}
	}

	SerializeGxFramePos(false);

    m_pNewMenu->Init();
	m_mgr.Update();

	wxXmlNode* pLastLocationNode = m_pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("lastpath")));
	IGxObject* pObj = dynamic_cast<IGxObject*>(m_pCatalog);
	wxString sLastPath;
	if(pLastLocationNode)
		sLastPath = pLastLocationNode->GetPropVal(wxT("path"), pObj->GetName());
	else
		sLastPath = pObj->GetName();
	m_pCatalog->SetLocation(sLastPath);

	wxLogMessage(_("wxGxApplication: Creation complete"));

    return true;
}

