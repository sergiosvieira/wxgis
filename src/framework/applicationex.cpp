/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplicationEx class. Add AUI managed frames & etc.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/framework/applicationex.h"
#include "wxgis/framework/customizedlg.h"
#include "wxgis/framework/toolbarmenu.h"

#ifdef HAVE_PROJ
    #include "proj_api.h"
#endif

//-----------------------------------------------
// wxGISApplicationEx
//-----------------------------------------------
IMPLEMENT_CLASS(wxGISApplicationEx, wxGISApplication)

wxGISApplicationEx::wxGISApplicationEx(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplication(parent, id, title, pos, size, style)
{
}

wxGISApplicationEx::~wxGISApplicationEx(void)
{
}

void wxGISApplicationEx::SerializeFramePosEx(bool bSave)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return;

	wxXmlNode *pPerspectiveXmlNode = pConfig->GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/perspective")));
	if(bSave)
		pConfig->Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/perspective/data")), m_mgr.SavePerspective());
	else
	{
		wxString wxPerspective = pConfig->Read(enumGISHKCU, GetAppName() + wxString(wxT("/frame/perspective/data")), wxEmptyString);
		m_mgr.LoadPerspective(wxPerspective);
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
	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
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

bool wxGISApplicationEx::Create(void)
{
	m_mgr.SetManagedWindow(this);

    wxGISApplication::Create();

	wxLogMessage(_("wxGISApplicationEx: Start. Creating main application frame..."));


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

	m_mgr.Update();

	wxLogMessage(_("wxGISApplicationEx: Creation complete"));

    return true;
}

void wxGISApplicationEx::OnClose(wxCloseEvent& event)
{
    event.Skip();

 	for(size_t i = 0; i < m_CommandBarArray.size(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = dynamic_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
				pToolBar->Deactivate();
		}
	}

    for(size_t i = 0; i < m_WindowArray.size(); ++i)
    {
        IView* pView = dynamic_cast<IView*>(m_WindowArray[i]);
        if(pView)
            pView->Deactivate();
    }


	wxGISApplication::OnClose(event);

 	SerializeFramePosEx(true);
    m_mgr.UnInit();

  //  while (!m_WindowArray.empty())
  //  {
  //      wxWindow* poWnd = m_WindowArray.back();
		//if( poWnd && !poWnd->IsBeingDeleted() )
  //      {
  //          if(!poWnd->Destroy())
  //              delete poWnd;
  //      }
  //      m_WindowArray.pop_back();
  //  }
}

bool wxGISApplicationEx::SetupSys(const wxString &sSysPath)
{
    if(!wxGISApplication::SetupSys(sSysPath))
        return false;
	wxString sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator();
	CPLSetConfigOption("GDAL_DATA", sGdalDataDir.mb_str(wxConvUTF8) );
#ifdef HAVE_PROJ
	sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("proj")) + wxFileName::GetPathSeparator();
	//CPLSetConfigOption("PROJ_LIB", sGdalDataDir.mb_str(wxConvUTF8) );
    CPLString pszPROJ_LIB(sGdalDataDir.mb_str(wxConvUTF8));
    const char *path = pszPROJ_LIB.c_str();
    pj_set_searchpath(1, &path);
#endif
    CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);
    CPLSetConfigOption("DXF_ENCODING", CPL_ENC_ASCII);
    return true;
}

void wxGISApplicationEx::SetDebugMode(bool bDebugMode)
{
	CPLSetConfigOption("CPL_DEBUG", bDebugMode == true ? "ON" : "OFF");
	CPLSetConfigOption("CPL_TIMESTAMP", "ON");
	CPLSetConfigOption("CPL_LOG_ERRORS", bDebugMode == true ? "ON" : "OFF");
}

bool wxGISApplicationEx::SetupLog(const wxString &sLogPath)
{
    if(!wxGISApplication::SetupLog(sLogPath))
        return false;
	wxString sCPLLogPath = sLogPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal_log_cat.txt"));
	CPLString szCPLLogPath(sCPLLogPath.mb_str(wxConvUTF8));
	CPLSetConfigOption("CPL_LOG", szCPLLogPath );
    return true;
}

