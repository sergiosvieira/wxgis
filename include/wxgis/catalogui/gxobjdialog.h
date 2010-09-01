/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog class.
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
#include <wx/combo.h>
#include <wx/treectrl.h>
#include <wx/combobox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "wxgis/core/config.h"
//#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalogui/gxtreeview.h"

#include "wx/aui/aui.h"

#define OBJDLG_NAME wxT("wxGISObjDialog")

//////////////////////////////////////////////////////////////////////////////
// wxGxToolBarArt
//////////////////////////////////////////////////////////////////////////////

class wxGxToolBarArt : public wxAuiDefaultToolBarArt
{
public:
	virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
	{
		dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE )));
		dc.Clear();
	}
};

//////////////////////////////////////////////////////////////////////////////
// wxTreeViewComboPopup
//////////////////////////////////////////////////////////////////////////////

class wxTreeViewComboPopup : public wxGxTreeViewBase,
                             public wxComboPopup
{
    DECLARE_DYNAMIC_CLASS(wxTreeViewComboPopup)
public:

    // Initialize member variables
    virtual void Init();
    virtual void OnPopup();
    virtual void OnDismiss();
    // Create popup control
    virtual bool Create(wxWindow* parent);
    // Return pointer to the created control
    virtual wxWindow *GetControl() { return this; }
    // Translate string into a list selection
    virtual void SetStringValue(const wxString& s);
    // Get list selection as a string
    virtual wxString GetStringValue() const;
    //
    // Do mouse hot-tracking (which is typical in list popups)
    void OnMouseMove(wxMouseEvent& event);
     // On mouse left up, set the value and close the popup
    void OnMouseClick(wxMouseEvent& event);
    void OnDblClick(wxTreeEvent& event);
    virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight);
//IGxSelectionEvents
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator);
//wxGxTreeViewBase
    virtual void AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent);
    //
    virtual IGxSelection* GetSelectedObjects(void)
    {
        m_pSelection->SetInitiator(TREECTRLID);
        return m_pSelection;
    }
protected:
    wxTreeItemId m_PrewItemId;
    bool m_bClicked;
private:
    DECLARE_EVENT_TABLE()
};

//////////////////////////////////////////////////////////////////////////////
// wxGxDialogContentView
//////////////////////////////////////////////////////////////////////////////

class wxGxDialogContentView : public wxGxContentView
{
public:
	wxGxDialogContentView(wxWindow* parent, wxWindowID id = LISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING);
	virtual ~wxGxDialogContentView();

//IGxView
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
// events
    virtual void OnActivated(wxListEvent& event);
//IGxCatalogEvents
	virtual void OnObjectAdded(IGxObject* object);
	virtual void OnObjectDeleted(IGxObject* object);
//
	virtual void SetFilters(LPOBJECTFILTERS pFiltersArray){m_pFiltersArray = pFiltersArray;};
	virtual void SetCurrentFilter(size_t nFilterIndex);
//wxGxContentView
	virtual void AddObject(IGxObject* pObject);
    //
    virtual IGxSelection* GetSelectedObjects(void)
    {
        m_pSelection->SetInitiator(NOTFIRESELID/*LISTCTRLID*/);
        return m_pSelection;
    }
    virtual void SetExternalCatalog(IGxCatalog* pCatalog = NULL){m_pExternalCatalog = pCatalog;};
protected:
  	IGxCatalog* m_pExternalCatalog;
	IConnectionPointContainer* m_pConnectionPointSelection;
	long m_ConnectionPointSelectionCookie;
	LPOBJECTFILTERS m_pFiltersArray;
	size_t m_nFilterIndex;
};

//////////////////////////////////////////////////////////////////////////////
// wxGxObjectDialog
//////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLU wxGxObjectDialog :
    public wxDialog,
    public IGxApplication,
    public IApplication
{
private:

protected:
	wxBoxSizer* bMainSizer;
	wxBoxSizer* bHeaderSizer;
	wxStaticText* m_staticText1;
	wxStaticText* m_staticText2;
	wxComboCtrl* m_TreeCombo;
	//wxToolBar* m_toolBar;
	wxAuiToolBar* m_toolBar;
	//wxListCtrl* m_listCtrl;
	wxFlexGridSizer* fgCeilSizer;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_NameTextCtrl;
	wxButton* m_OkButton;
	wxStaticText* m_staticText6;
	wxComboBox* m_WildcardCombo;
	wxButton* m_CancelButton;

public:
	wxGxObjectDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,338 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	~wxGxObjectDialog();
//IGxApplication
    virtual IGxCatalog* GetCatalog(void){return static_cast<IGxCatalog*>(m_pCatalog);};
//IApplication
	virtual ICommand* GetCommand(long CmdID);
	virtual ICommand* GetCommand(wxString sCmdName, unsigned char nCmdSubType);
    virtual wxString GetAppName(void){return wxString(OBJDLG_NAME);};
    virtual IStatusBar* GetStatusBar(void){return NULL;};
    virtual IGISConfig* GetConfig(void){return m_pConfig;};
    virtual void OnAppAbout(void){};
    virtual IGISCommandBar* GetCommandBar(wxString sName){return NULL;};
    virtual void RemoveCommandBar(IGISCommandBar* pBar){};
    virtual bool AddCommandBar(IGISCommandBar* pBar){return false;};
    virtual void ShowStatusBar(bool bShow){};
    virtual bool IsStatusBarShown(void){return false;};
    virtual void ShowToolBarMenu(void){};
	virtual WINDOWARRAY* GetChildWindows(void);
    virtual void RegisterChildWindow(wxWindow* pWnd);
    virtual void UnRegisterChildWindow(wxWindow* pWnd);
    virtual void Customize(void){};
    virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true){};
    virtual bool IsApplicationWindowShown(wxWindow* pWnd){return true;};
	virtual void OnMouseDown(wxMouseEvent& event){};
	virtual void OnMouseUp(wxMouseEvent& event){};
	virtual void OnMouseDoubleClick(wxMouseEvent& event){};
	virtual void OnMouseMove(wxMouseEvent& event){};
    virtual bool Create(IGISConfig* pConfig){return true;};
//wxGxObjectDialog
	virtual void SetButtonCaption(wxString sOkBtLabel);
	virtual void SetStartingLocation(wxString sStartPath);
	virtual void SetName(wxString sName);
	virtual void SetAllowMultiSelect(bool bAllowMultiSelect);
	virtual void SetOverwritePrompt(bool bOverwritePrompt);
	virtual void SetAllFilters(bool bAllFilters);
	virtual int ShowModalOpen();
	virtual int ShowModalSave();
	virtual void AddFilter(IGxObjectFilter* pFilter, bool bDefault = false);
    virtual void SetOwnsFilter(bool bOwnFilter){m_bOwnFilter = bOwnFilter;};
	virtual void RemoveAllFilters(void);
    virtual GxObjectArray* GetSelectedObjects(void){return &m_ObjectArray;}
    virtual wxString GetName(void);
    virtual wxString GetNameWithExt(void);
    virtual wxString GetFullPath(void);
    virtual wxString GetPath(void);
    virtual IGxObject* GetLocation(void);
    virtual IGxObjectFilter* GetCurrentFilter(void);
    virtual size_t GetCurrentFilterId(void);
protected:
// events
    virtual void OnCommand(wxCommandEvent& event);
	virtual void OnCommandUI(wxUpdateUIEvent& event);
	virtual void OnDropDownCommand(wxCommandEvent& event);
	virtual void OnToolDropDown(wxAuiToolBarEvent& event);
	virtual void OnItemSelected(wxListEvent& event);
	virtual void OnFilterSelect(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void Command(ICommand* pCmd);
//wxGxObjectDialog
	virtual void OnInit();
    virtual void SerializeFramePos(bool bSave);
    virtual bool DoSaveObject(wxGISEnumSaveObjectResults Result);
public:
    virtual void SetExternalCatalog(IGxCatalog* pCatalog = NULL){m_pExternalCatalog = pCatalog;};
protected:
 	COMMANDARRAY m_CommandArray;
  	wxGxCatalog* m_pCatalog;
  	IGxCatalog* m_pExternalCatalog;
    wxGISAppConfig* m_pConfig;
    wxGxDialogContentView* m_pwxGxContentView;
    wxTreeViewComboPopup* m_PopupCtrl;
    IDropDownCommand* m_pDropDownCommand;
	wxString m_sOkBtLabel;
	wxString m_sStartPath;
	wxString m_sName;
	bool m_bAllowMultiSelect, m_bOverwritePrompt, m_bAllFilters;
    bool m_bIsSaveDlg;
	OBJECTFILTERS m_FilterArray;
	size_t m_nDefaultFilter;
    GxObjectArray m_ObjectArray;
    int m_nRetCode;
   	WINDOWARRAY m_WindowArray;
    bool m_bOwnFilter;

    DECLARE_EVENT_TABLE()
};

