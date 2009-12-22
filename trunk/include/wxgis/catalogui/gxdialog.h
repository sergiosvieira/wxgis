/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDialog class.
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

//////////////////////////////////////////////////////////////////////////////
// wxTreeViewComboPopup
//////////////////////////////////////////////////////////////////////////////

class wxTreeViewComboPopup : public wxTreeCtrl,
                             public wxComboPopup
{
public:

    // Initialize member variables
    virtual void Init()
    {
        m_value = -1;
    }

    // Create popup control
    virtual bool Create(wxWindow* parent)
    {
        return wxTreeCtrl::Create(parent,1,wxPoint(0,0),wxDefaultSize, wxBORDER_SIMPLE | wxTR_NO_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE );
    }

    // Return pointer to the created control
    virtual wxWindow *GetControl() { return this; }

    // Translate string into a list selection
    virtual void SetStringValue(const wxString& s)
    {
        //int n = wxListView::FindItem(-1,s);
        //if ( n >= 0 && n < wxListView::GetItemCount() )
        //    wxListView::Select(n);
    }

    // Get list selection as a string
    virtual wxString GetStringValue() const
    {
        //if ( m_value >= 0 )
        //    return wxListView::GetItemText(m_value);
        return wxEmptyString;
    }

    // Do mouse hot-tracking (which is typical in list popups)
    void OnMouseMove(wxMouseEvent& event)
    {
        // TODO: Move selection to cursor
    }

    // On mouse left up, set the value and close the popup
    void OnMouseClick(wxMouseEvent& WXUNUSED(event))
    {
        //m_value = wxListView::GetFirstSelected();

        // TODO: Send event as well

        Dismiss();
    }

protected:

    int             m_value; // current item index

private:
    DECLARE_EVENT_TABLE()
};


//////////////////////////////////////////////////////////////////////////////
// wxGxDialog
//////////////////////////////////////////////////////////////////////////////
class wxGxDialog : public wxDialog 
{
private:

protected:
	wxBoxSizer* bMainSizer;
	wxBoxSizer* bHeaderSizer;
	wxStaticText* m_staticText1;
	wxComboCtrl* m_TreeCombo;
	wxToolBar* m_toolBar;
	wxListCtrl* m_listCtrl;
	wxFlexGridSizer* fgCeilSizer;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_NameTextCtrl;
	wxButton* m_OkButton;
	wxStaticText* m_staticText6;
	wxComboBox* m_WildcardCombo;
	wxButton* m_CancelButton;

public:
	wxGxDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,338 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	~wxGxDialog();	
};
//wxFileDialog(wxWindow* parent, const wxString& message = "Choose a file", const wxString& defaultDir = "", const wxString& defaultFile = "", const wxString& wildcard = "*.*", long style = wxFD_DEFAULT_STYLE, const wxPoint& pos = wxDefaultPosition, const wxSize& sz = wxDefaultSize, const wxString& name = "filedlg")


