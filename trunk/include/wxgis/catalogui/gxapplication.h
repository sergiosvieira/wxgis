/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication main header.
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
#pragma once
#include "wxgis/framework/application.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/catalogui/gxtabview.h"
#include "wxgis/catalogui/newmenu.h"

#include "wx/aui/aui.h"
#include "wx/artprov.h"

/** \class wxGxApplication gxapplication.h
 *   \brief A catalog application framework class.
 */
class WXDLLIMPEXP_GIS_CLU wxGxApplication :
	public wxGISApplication,
    public IGxApplication
{
    DECLARE_CLASS(wxGxApplication)
public:	
	wxGxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	virtual ~wxGxApplication(void);
	virtual wxAuiManager* GetAuiManager(void){return &m_mgr;};
	virtual void ShowPane(wxWindow* pWnd, bool bShow = true);
	virtual void ShowPane(const wxString& sName, bool bShow = true);
	virtual bool IsPaneShown(const wxString& sName);
    //IGxApplication
	virtual IGxCatalog* GetCatalog(void);
	//wxGISApplication
	virtual void RemoveCommandBar(IGISCommandBar* pBar);
	virtual bool AddCommandBar(IGISCommandBar* pBar);
    //events 
    virtual void OnClose(wxCloseEvent& event);
	//IApplication
	virtual void Customize(void);
	virtual void ShowStatusBar(bool bShow);
	virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true);
	virtual bool IsApplicationWindowShown(wxWindow* pWnd);
	virtual const WINDOWARRAY* GetChildWindows(void);
	virtual void RegisterChildWindow(wxWindow* pWnd);
	virtual void UnRegisterChildWindow(wxWindow* pWnd);
    virtual wxString GetAppName(void){return wxString(wxT("wxGISCatalog"));};
    virtual wxString GetAppVersionString(void){return wxString(APP_VER);};
    virtual bool Create(IGISConfig* pConfig);
    virtual bool SetupSys(wxString sSysPath);
    virtual void SetDebugMode(bool bDebugMode);
    virtual bool SetupLog(wxString sLogPath);
protected:
    //wxGxApplication
	virtual void SerializeGxFramePos(bool bSave = false);
protected:
	wxAuiManager m_mgr;
	wxGxTreeView* m_pTreeView;
	wxGxTabView* m_pTabView;
	wxGxCatalogUI* m_pCatalog;
	WINDOWARRAY m_WindowArray;
    wxGISNewMenu* m_pNewMenu;
};
