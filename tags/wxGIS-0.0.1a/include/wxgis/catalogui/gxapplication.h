/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication main header.
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
#pragma once
#include "wxgis/catalog/catalog.h"
#include "wxgis/framework/application.h"
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/catalogui/gxtabview.h"

#include "wx/aui/aui.h"
#include "wx/artprov.h"

//-----------------------------------------------
// wxGxApplication
//-----------------------------------------------

class WXDLLIMPEXP_GIS_CLU wxGxApplication :
	public wxGISApplication,
    public IGxApplication
{
public:	
	wxGxApplication(IGISConfig* pConfig, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
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
	//IApplication
	virtual void Customize(void);
	virtual void ShowStatusBar(bool bShow);
	virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true);
	virtual WINDOWARRAY* GetChildWindows(void);
	virtual void RegisterChildWindow(wxWindow* pWnd);
    virtual wxString GetAppName(void){return wxString(APP_NAME);};
protected:
	virtual void SerializeFramePos(bool bSave = false);
protected:
	wxAuiManager m_mgr;
	wxGxTreeView* m_pTreeView;
	wxGxTabView* m_pTabView;
	IGxCatalog* m_pCatalog;
	WINDOWARRAY m_WindowArray;
};
