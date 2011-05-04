/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplicationEx class. Add AUI managed frames & etc.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#pragma once

#include "wxgis/framework/applicationex.h"
#include "wxgis/framework/customizedlg.h"
#include "wxgis/framework/toolbarmenu.h"

//-----------------------------------------------
// wxGISApplicationEx
//-----------------------------------------------
IMPLEMENT_CLASS(wxGISApplicationEx, wxGISApplication)

wxGISApplicationEx::wxGISApplicationEx(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplication(parent, id, title, pos, size, style)
{
}

wxGISApplicationEx::~wxGISApplicationEx(void)
{
    m_mgr.UnInit();
}

void wxGISApplicationEx::SerializeFramePosEx(bool bSave)
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

void wxGISApplicationEx::Customize(void)
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

void wxGISApplicationEx::RemoveCommandBar(IGISCommandBar* pBar)
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

void wxGISApplicationEx::ShowPane(wxWindow* pWnd, bool bShow)
{
	m_mgr.GetPane(pWnd).Show(bShow);
	m_mgr.Update();
}

void wxGISApplicationEx::ShowPane(const wxString& sName, bool bShow)
{
	m_mgr.GetPane(sName).Show(bShow);
	m_mgr.Update();
}

bool wxGISApplicationEx::IsPaneShown(const wxString& sName)
{
	return m_mgr.GetPane(sName).IsShown();
}

bool wxGISApplicationEx::IsApplicationWindowShown(wxWindow* pWnd)
{
	return m_mgr.GetPane(pWnd).IsShown();
}

bool wxGISApplicationEx::AddCommandBar(IGISCommandBar* pBar)
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
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(pToolBar->GetCaption()).ToolbarPane().Top().Position(100).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()));
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

void wxGISApplicationEx::ShowStatusBar(bool bShow)
{
	wxGISApplication::ShowStatusBar(bShow);
	m_mgr.Update();
}

void wxGISApplicationEx::ShowApplicationWindow(wxWindow* pWnd, bool bShow)
{
	ShowPane(pWnd, bShow);
}

const WINDOWARRAY* const wxGISApplicationEx::GetChildWindows(void)
{
	return &m_WindowArray;
}

void wxGISApplicationEx::RegisterChildWindow(wxWindow* pWnd)
{
    WINDOWARRAY::iterator pos = std::find(m_WindowArray.begin(), m_WindowArray.end(), pWnd);
	if(pos != m_WindowArray.end())
        return;
	m_WindowArray.push_back(pWnd);
}

void wxGISApplicationEx::UnRegisterChildWindow(wxWindow* pWnd)
{
    WINDOWARRAY::iterator pos = std::find(m_WindowArray.begin(), m_WindowArray.end(), pWnd);
	if(pos != m_WindowArray.end())
		m_WindowArray.erase(pos);
}

bool wxGISApplicationEx::Create(IGISConfig* pConfig)
{
    wxGISApplication::Create(pConfig);

	wxLogMessage(_("wxGISApplicationEx: Start. Creating main application frame..."));

	m_mgr.SetManagedWindow(this);

	//wxXmlNode* pViewsNode = m_pConfig->GetConfigNode(wxString(wxT("frame/views")), true, true);

	//if(!pViewsNode)
	//{
	//	wxLogError(_("wxGISApplicationEx: Error finding <views> XML Node"));
	//	return false;
	//}

 //   int nPaneCount(0);
 //   wxXmlNode* pViewsChildNode = pViewsNode->GetChildren();
 //   while(pViewsChildNode)
 //   {
      //  if(pViewsChildNode->GetName().CmpNoCase(wxT("treeview")) == 0)
      //  {
	     //   m_pTreeView = new wxGxTreeView(this, TREECTRLID);
	     //   if(m_pTreeView->Activate(this, pViewsChildNode))
	     //   {
		    //    m_mgr.AddPane(m_pTreeView, wxAuiPaneInfo().Name(wxT("tree_window")).Caption(_("Tree Pane")).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Left().Layer(1/*2*/).Position(1).CloseButton(true));
		    //    RegisterChildWindow(m_pTreeView);
	     //   }
	     //   else
		    //    wxDELETE(m_pTreeView);
      //  }
      //  else if(pViewsChildNode->GetName().CmpNoCase(wxT("tabview")) == 0)
      //  {
	     //   m_pTabView = new wxGxTabView(this);
	     //   if(m_pTabView->Activate(this, pViewsChildNode))
	     //   {
		    //    m_mgr.AddPane(m_pTabView, wxAuiPaneInfo().Name(wxT("main_window")).CenterPane());//.PaneBorder(true)
		    //    RegisterChildWindow(m_pTabView);
	     //   }
	     //   else
		    //    wxDELETE(m_pTabView);
      //  }
      //  else
      //  {
 		   // wxString sClass = pViewsChildNode->GetPropVal(wxT("class"), ERR);

		    //wxObject *obj = wxCreateDynamicObject(sClass);
		    //IGxView *pGxView = dynamic_cast<IGxView*>(obj);
      //      if(pGxView)
      //      {
      //          pGxView->Create(this);
			   // wxWindow* pWnd = dynamic_cast<wxWindow*>(pGxView);
			   // if(pWnd != NULL)
			   // {
				  //  pWnd->Hide();
				  //  if(pGxView->Activate(this, pViewsChildNode))
      //              {
      //                  nPaneCount++;
      //                  m_mgr.AddPane(pWnd, wxAuiPaneInfo().Name(wxString::Format(wxT("window_%d"), nPaneCount)).Caption(pGxView->GetViewName()).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Right().Layer(1).Position(nPaneCount).CloseButton(true));
      //                  RegisterChildWindow(pWnd);
      //                  wxLogMessage(_("wxGxApplication: View class %s initialise"), sClass.c_str());
      //              }
				  //  else
				  //  {
					 //   wxLogError(_("wxGxApplication: Error activation IGxView class (%s)"), sClass.c_str());
					 //   wxDELETE(pGxView);
				  //  }
			   // }
		    //}
		    //else
		    //{
			   // wxLogError(_("wxGxApplication: Error creating view %s"), sClass.c_str());
			   // wxDELETE(pGxView);
		    //}
      //  }

    //    pViewsChildNode = pViewsChildNode->GetNext();
    //}
	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
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

	m_mgr.Update();

	wxLogMessage(_("wxGISApplicationEx: Creation complete"));

    return true;
}

void wxGISApplicationEx::OnClose(wxCloseEvent& event)
{
    event.Skip();

 	for(size_t i = 0; i < m_CommandBarArray.size(); i++)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
				pToolBar->Deactivate();
		}
	}

    for(size_t i = 0; i < m_WindowArray.size(); i++)
    {
        IView* pView = dynamic_cast<IView*>(m_WindowArray[i]);
        if(pView)
            pView->Deactivate();
    }

    wxGISApplication::OnClose(event);

    while (!m_WindowArray.empty())
    {
        wxWindow* poWnd = m_WindowArray.back();
        if(poWnd)
        {
            if(!poWnd->Destroy())
                delete poWnd;
        }
        m_WindowArray.pop_back();
    }

 	SerializeFramePosEx(true);
}

bool wxGISApplicationEx::SetupSys(wxString sSysPath)
{
    if(!wxGISApplication::SetupSys(sSysPath))
        return false;
    CPLSetConfigOption("GDAL_DATA", wgWX2MB( (sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator()).c_str() ) );
    return true;
}

void wxGISApplicationEx::SetDebugMode(bool bDebugMode)
{
	CPLSetConfigOption("CPL_DEBUG", bDebugMode == true ? "ON" : "OFF");
	CPLSetConfigOption("CPL_TIMESTAMP", "ON");
	CPLSetConfigOption("CPL_LOG_ERRORS", bDebugMode == true ? "ON" : "OFF");
}

bool wxGISApplicationEx::SetupLog(wxString sLogPath)
{
    if(!wxGISApplication::SetupLog(sLogPath))
        return false;
	wxString sCPLLogPath = sLogPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal_log_cat.txt"));
	CPLSetConfigOption("CPL_LOG", wgWX2MB(sCPLLogPath.c_str()) );
    return true;
}

