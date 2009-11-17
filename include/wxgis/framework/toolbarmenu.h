/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  toolbar check menu class.
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
#include "wxgis/framework/framework.h"
#include "wxgis/framework/application.h"

#define TOOLBARMENUNAME _("Application.ToolbarsMenu")

class wxGISToolBarMenu :
	public wxGISMenu,
	public ICommand
{
public:
	wxGISToolBarMenu(const wxString& sName = TOOLBARMENUNAME, const wxString& sCaption = _("Toolbars"), wxGISEnumCommandBars type = enumGISCBContextmenu, const wxString& title = _(""), long style = 0);
	~wxGISToolBarMenu(void);
	//wxGISMenu
	virtual void AddCommand(ICommand* pCmd){};
	virtual void RemoveCommand(size_t nIndex){};
	virtual void MoveCommandLeft(size_t nIndex){};
	virtual void MoveCommandRight(size_t nIndex){};
	//ICommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(IApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
	virtual void Update(void);
	//events
	void OnCommand(wxCommandEvent& event);
protected:
	wxGISApplication* m_pApp;
	std::vector<wxMenuItem*> m_delitems;

	DECLARE_EVENT_TABLE()
};

