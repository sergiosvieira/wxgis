/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContainerDialog class.
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

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
//#include <wx/combo.h>
#include <wx/treectrl.h>
//#include <wx/combobox.h>
//#include <wx/bitmap.h>
//#include <wx/image.h>
//#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
//#include <wx/listctrl.h>
//#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "wxgis/core/config.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalogui/gxtreeview.h"

#include "wx/aui/aui.h"

#define CONTDLG_NAME wxT("wxGISContDialog") 

//////////////////////////////////////////////////////////////////////////////
// wxTreeContainerView
//////////////////////////////////////////////////////////////////////////////

class wxTreeContainerView : public wxGxTreeViewBase
{
    DECLARE_DYNAMIC_CLASS(wxTreeContainerView)
public:
    wxTreeContainerView(void);
    wxTreeContainerView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE/* | wxTR_HIDE_ROOT*/);
    virtual ~wxTreeContainerView(void);
//wxGxTreeViewBase
    virtual void AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent, bool sort = true);
//wxTreeContainerView
	virtual void AddShowFilter(IGxObjectFilter* pFilter);
	virtual void RemoveAllShowFilters(void);
    virtual bool CanChooseObject( IGxObject* pObject );
//events
    virtual void OnSelChanged(wxTreeEvent& event);
protected:
	OBJECTFILTERS m_ShowFilterArray;

    DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////////
//// wxGxContainerDialog
////////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLU wxGxContainerDialog : 
    public wxDialog,
    public IGxApplication,
    public IApplication
{
enum
{
    ID_CREATE = wxID_HIGHEST + 40
};
public:
	wxGxContainerDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,338 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	~wxGxContainerDialog();	

//IGxApplication
    virtual IGxCatalog* GetCatalog(void){return static_cast<IGxCatalog*>(m_pCatalog);};
//IApplication
    virtual ICommand* GetCommand(long CmdID){return NULL;};
	virtual ICommand* GetCommand(wxString sCmdName, unsigned char nCmdSubType){return NULL;};
    virtual wxString GetAppName(void){return wxString(CONTDLG_NAME);};
    virtual IStatusBar* GetStatusBar(void){return NULL;};
    virtual IGISConfig* GetConfig(void){return m_pConfig;};
    virtual void OnAppAbout(void){};
    virtual IGISCommandBar* GetCommandBar(wxString sName){return NULL;};
    virtual void RemoveCommandBar(IGISCommandBar* pBar){};
    virtual bool AddCommandBar(IGISCommandBar* pBar){return false;};
    virtual void ShowStatusBar(bool bShow){};
    virtual bool IsStatusBarShown(void){return false;};
    virtual void ShowToolBarMenu(void){};
	virtual WINDOWARRAY* GetChildWindows(void){return NULL;};
    virtual void RegisterChildWindow(wxWindow* pWnd){};
    virtual void Customize(void){};
    virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true){};
	virtual void OnMouseDown(wxMouseEvent& event){};
	virtual void OnMouseUp(wxMouseEvent& event){};
	virtual void OnMouseDoubleClick(wxMouseEvent& event){};
	virtual void OnMouseMove(wxMouseEvent& event){};
    virtual bool Create(IGISConfig* pConfig){return true;};
//wxDialog
    int ShowModal(void);
//wxGxContainerDialog
	virtual void SetButtonCaption(wxString sOkBtLabel);
	virtual void SetStartingLocation(wxString sStartPath);
	virtual void SetDescriptionText(wxString sText);
    virtual void ShowCreateButton(bool bShow = false);
    virtual void ShowExportFormats(bool bShow = false);
	virtual void SetAllFilters(bool bAllFilters);
	virtual void AddFilter(IGxObjectFilter* pFilter, bool bDefault = false);
	virtual void RemoveAllFilters(void);
	virtual void AddShowFilter(IGxObjectFilter* pFilter);
	virtual void RemoveAllShowFilters(void);
    virtual GxObjectArray* GetSelectedObjects(void){return &m_ObjectArray;}
//	virtual void SetName(wxString sName);
//	virtual void SetAllowMultiSelect(bool bAllowMultiSelect);
//	virtual void SetOverwritePrompt(bool bOverwritePrompt);
//	virtual int ShowModalOpen();
//	virtual int ShowModalSave();
//    virtual wxString GetName(void);
//    virtual wxString GetFullPath(void);
    virtual wxString GetPath(void);
    virtual IGxObject* GetLocation(void);
    virtual IGxObjectFilter* GetCurrentFilter(void);
protected:
// events
	virtual void OnFilerSelect(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnCreate(wxCommandEvent& event);
    virtual void OnCreateUI(wxUpdateUIEvent& event);
//    virtual void OnCommand(wxCommandEvent& event);
//	virtual void OnCommandUI(wxUpdateUIEvent& event);
//	virtual void OnDropDownCommand(wxCommandEvent& event);
//	virtual void OnToolDropDown(wxAuiToolBarEvent& event);
	//virtual void OnItemSelected(wxListEvent& event);
//    virtual void OnCommand(ICommand* pCmd);
//wxGxContainerDialog
	virtual void OnInit();
    virtual void SerializeFramePos(bool bSave);
//    virtual bool DoSaveObject(wxGISEnumSaveObjectResults Result);
protected:
  	wxGxCatalog* m_pCatalog;
    wxGISAppConfig* m_pConfig;
    wxTreeContainerView* m_pTree;
	wxString m_sOkBtLabel;
	wxString m_sStartPath;
    bool m_bShowCreateButton, m_bAllFilters, m_bShowExportFormats;
	OBJECTFILTERS m_FilterArray;
	size_t m_nDefaultFilter;
    GxObjectArray m_ObjectArray;
    int m_nRetCode;
    ICommand* m_pCreateCmd;
// 	COMMANDARRAY m_CommandArray;
//    wxGxDialogContentView* m_pwxGxContentView;
//    wxTreeViewComboPopup* m_PopupCtrl;
//    IDropDownCommand* m_pDropDownCommand;
//	wxString m_sName;
//	bool m_bAllowMultiSelect, m_bOverwritePrompt, m_bAllFilters;
//    bool m_bIsSaveDlg;
//   	WINDOWARRAY m_WindowArray;

protected:
	wxBoxSizer* bMainSizer;
	wxStaticText* m_staticDescriptionText;
	wxStaticText* m_staticWildText;
	wxBoxSizer* bFooterSizer;
	wxComboBox* m_WildcardCombo;
	wxButton* m_CancelButton;
	wxButton* m_CreateButton;
//	wxStaticText* m_staticText2;
//	wxComboCtrl* m_TreeCombo;
//	//wxToolBar* m_toolBar;
//	wxAuiToolBar* m_toolBar;
//	//wxListCtrl* m_listCtrl;
//	wxFlexGridSizer* fgCeilSizer;
//	wxStaticText* m_staticText4;
//	wxTextCtrl* m_NameTextCtrl;
	wxButton* m_OkButton;
//	wxStaticText* m_staticText6;
//
    DECLARE_EVENT_TABLE()
};

