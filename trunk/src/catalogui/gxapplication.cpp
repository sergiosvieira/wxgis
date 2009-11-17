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
wxGxApplication::wxGxApplication(IGISConfig* pConfig, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplication(pConfig, parent, id, title, pos, size, style), m_pCatalog(NULL), m_pTreeView(NULL), m_pTabView(NULL)
{
	wxLogMessage(_("wxGxApplication: Creating main application frame..."));
	
	m_mgr.SetManagedWindow(this);

	wxGxCatalog* pCatalog = new wxGxCatalog();
	pCatalog->Init();
	m_pCatalog = static_cast<IGxCatalog*>(pCatalog);

	wxXmlNode* pViewsNode = m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/views")));

	if(!pViewsNode)
	{
		wxLogError(_("wxGxApplication: Error find <views> XML Node"));
		return;
	}

	m_pTreeView = new wxGxTreeView(this, TREECTRLID);
	if(m_pTreeView->Activate(this, m_pCatalog, m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/views/treeview")))))
	{
		m_mgr.AddPane(m_pTreeView, wxAuiPaneInfo().Name(wxT("tree_window")).Caption(_("Tree Pane")).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Left().Layer(1/*2*/).Position(1).CloseButton(true));
		RegisterChildWindow(m_pTreeView);
	}
	else
		wxDELETE(m_pTreeView);

	m_pTabView = new wxGxTabView(this);
	if(m_pTabView->Activate(this, m_pCatalog, m_pConfig->GetConfigNode(enumGISHKLM, wxString(wxT("frame/views/tabview")))))
	{
		m_mgr.AddPane(m_pTabView, wxAuiPaneInfo().Name(wxT("main_window")).CenterPane());//.PaneBorder(true)
		RegisterChildWindow(m_pTabView);
	}
	else
		wxDELETE(m_pTabView);
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

	SerializeFramePos(false);

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
}

wxGxApplication::~wxGxApplication(void)
{
	GxObjectArray* pArr = m_pCatalog->GetSelection()->GetSelectedObjects(TREECTRLID);
	if(pArr)
	{
		if(pArr->size() > 0)
		{
			wxString sLastPath = pArr->at(0)->GetFullName();
			if(sLastPath.IsEmpty())
				sLastPath = pArr->at(0)->GetName();

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

	wxDELETE(m_pTreeView);
	wxDELETE(m_pTabView);

	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
				pToolBar->Deactivate();
		}
	}

	SerializeFramePos(true);
	//should remove toolbars from commandbar array as m_mgr manage toolbars by itself 
	m_mgr.UnInit();
	wxDELETE(m_pCatalog);
}

IGxCatalog* wxGxApplication::GetCatalog(void)
{
	return m_pCatalog;
}

void wxGxApplication::SerializeFramePos(bool bSave)
{
	wxGISApplication::SerializeFramePos(bSave);

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


