/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISApplication class. Base application functionality (commands, menues, etc.)
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
#include "wxgis/framework/statusbar.h"
#include "wxgis/framework/accelerator.h"
#include "wxgis/framework/menubar.h"
#include "wxgis/framework/commandbar.h"


class WXDLLIMPEXP_GIS_FRW wxGISApplication : 
	public wxFrame,
	public IApplication
{
public:
	enum
    {
		ID_PLUGINCMD = wxID_HIGHEST + 1
    };	

	//constructor
	wxGISApplication(IGISConfig* pConfig, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER );//| wxWS_EX_VALIDATE_RECURSIVELY
	//destructor
	virtual ~wxGISApplication(void);
	//events
    void OnEraseBackground(wxEraseEvent& event);
    void OnSize(wxSizeEvent& event);
	void OnCommand(wxCommandEvent& event);
	void OnCommandUI(wxUpdateUIEvent& event);
	void OnRightDown(wxMouseEvent& event);
//
	virtual wxStatusBar* OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name);
	virtual void LoadCommands(wxXmlNode* pRootNode);
	virtual void LoadMenues(wxXmlNode* pRootNode);
	virtual void LoadToolbars(wxXmlNode* pRootNode);
	virtual COMMANDBARARRAY* GetCommandBars(void);
	virtual COMMANDARRAY* GetCommands(void);
	virtual wxGISMenuBar* GetMenuBar(void);
	virtual wxGISAcceleratorTable* GetGISAcceleratorTable(void);
	virtual void SerializeFramePos(bool bSave = false);
	virtual void SerializeCommandBars(bool bSave = false);
	virtual void OnCommand(ICommand* pCmd);
	virtual void OnMouseDown(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
	virtual void OnMouseDoubleClick(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
//IApplication
	virtual ICommand* GetCommand(long CmdID);
	virtual ICommand* GetCommand(wxString sCmdName, unsigned char nCmdSubType);
	virtual IStatusBar* GetStatusBar(void);
	virtual IGISConfig* GetConfig(void);
	virtual IGISCommandBar* GetCommandBar(wxString sName);
	virtual void RemoveCommandBar(IGISCommandBar* pBar);
	virtual bool AddCommandBar(IGISCommandBar* pBar);
	virtual void ShowStatusBar(bool bShow);
	virtual bool IsStatusBarShown(void);
	virtual void ShowToolBarMenu(void);

protected:
	unsigned int m_nCmdCounter;
	IGISConfig* m_pConfig;
	COMMANDARRAY m_CommandArray;
	COMMANDBARARRAY m_CommandBarArray;
	wxGISAcceleratorTable* m_pGISAcceleratorTable;
	wxGISMenuBar* m_pMenuBar;
	ITool* m_CurrentTool;

    DECLARE_EVENT_TABLE()
};
