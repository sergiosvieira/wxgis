/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommandBar class, and diferent implementation - wxGISMneu, wxGISToolBar 
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/framework/framework.h"

#include <wx/aui/aui.h>
#include <wx/menu.h> 

#define STANDARDSTR _("Standard")
#define LOCATIONSTR _("Location")
#define GEOGRAPHYOSTR _("Geography")

//----------------------------------------------------------------------
// wxGISCommandBar
//----------------------------------------------------------------------

class WXDLLIMPEXP_GIS_FRW wxGISCommandBar :
	public IGISCommandBar
{
public:
	wxGISCommandBar(const wxString& sName = NONAME, const wxString& sCaption = _("No Caption"), wxGISEnumCommandBars type = enumGISCBNone);
	virtual ~wxGISCommandBar(void);
	virtual void SetName(const wxString& sName);
	virtual wxString GetName(void);
	virtual void SetCaption(const wxString& sCaption);
	virtual wxString GetCaption(void);
	virtual void SetType(wxGISEnumCommandBars type);
	virtual wxGISEnumCommandBars GetType(void);
	virtual void AddCommand(ICommand* pCmd);
	virtual void AddMenu(wxMenu* pMenu, wxString sName) = 0;
	virtual void RemoveCommand(size_t nIndex);
	virtual void MoveCommandLeft(size_t nIndex);
	virtual void MoveCommandRight(size_t nIndex);
	virtual size_t GetCommandCount(void);
	virtual ICommand* GetCommand(size_t nIndex);
	virtual void Serialize(IFrameApplication* pApp, wxXmlNode* pNode, bool bStore = false);
protected:
	COMMANDARRAY m_CommandArray;
	wxString m_sName;
	wxString m_sCaption;
	wxGISEnumCommandBars m_type;
};


//----------------------------------------------------------------------
// wxGISMenu
//----------------------------------------------------------------------

class WXDLLIMPEXP_GIS_FRW wxGISMenu :
	public wxMenu,
	public wxGISCommandBar
{
public:
	wxGISMenu(const wxString& sName = NONAME, const wxString& sCaption = _("No Caption"), wxGISEnumCommandBars type = enumGISCBNone, const wxString& title = wxEmptyString, long style = 0);
	virtual ~wxGISMenu(void);
	virtual void AddCommand(ICommand* pCmd);
	virtual void RemoveCommand(size_t nIndex);
	virtual void MoveCommandLeft(size_t nIndex);
	virtual void MoveCommandRight(size_t nIndex);
	virtual void AddMenu(wxMenu* pMenu, wxString sName);
protected:
	typedef struct submenudata
	{
		wxMenuItem* pItem;
		IGISCommandBar* pBar;
	} SUBMENUDATA;
	std::vector<SUBMENUDATA> m_SubmenuArray;
};

//----------------------------------------------------------------------
// wxGISToolbar
//----------------------------------------------------------------------

class WXDLLIMPEXP_GIS_FRW wxGISToolBar :
	public wxAuiToolBar,
	public wxGISCommandBar
{
public:
	wxGISToolBar(wxWindow* parent, wxWindowID id = -1, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_TB_DEFAULT_STYLE, const wxString& sName = NONAME, const wxString& sCaption = _("No Caption"), wxGISEnumCommandBars type = enumGISCBNone );
	virtual ~wxGISToolBar(void);
	virtual void SetLeftDockable(bool bLDock);
	virtual void SetRightDockable(bool bRDock);
	virtual bool GetLeftDockable(void);
	virtual bool GetRightDockable(void);
	virtual void AddCommand(ICommand* pCmd);
	virtual void SetName(const wxString& sName);
	virtual wxString GetName(void);
	virtual void SetCaption(const wxString& sCaption);
	virtual wxString GetCaption(void);
	virtual void RemoveCommand(size_t nIndex);
	virtual void MoveCommandLeft(size_t nIndex);
	virtual void MoveCommandRight(size_t nIndex);
	virtual void Serialize(IFrameApplication* pApp, wxXmlNode* pNode, bool bStore = false);
	virtual void AddMenu(wxMenu* pMenu, wxString sName);
	virtual void Activate(IFrameApplication* pApp);
	virtual void Deactivate(void);
	virtual void UpdateControls(void);
protected: // handlers
    void OnMotion(wxMouseEvent& evt);
protected: 
	void ReAddCommand(ICommand* pCmd);
protected:
	IStatusBar* m_pStatusBar;
	bool m_bLDock;
	bool m_bRDock;

	std::map<size_t, IToolBarControl*> m_RemControlMap;

    DECLARE_EVENT_TABLE()
};