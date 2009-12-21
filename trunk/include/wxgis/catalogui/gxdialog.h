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
// wxGxDialog
//////////////////////////////////////////////////////////////////////////////
class wxGxDialog : public wxDialog 
{
private:

protected:
	wxBoxSizer* bMainSizer;
	wxBoxSizer* bHeaderSizer;
	wxStaticText* m_staticText1;
	wxComboBox* m_treecombo;
	wxToolBar* m_toolBar;
	wxListCtrl* m_listCtrl;
	wxFlexGridSizer* fgCeilSizer;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_NameTextCtrl;
	wxButton* m_OkButton;
	wxStaticText* m_staticText6;
	wxComboBox* m_comboBox2;
	wxButton* m_CancelButton;

public:
	wxGxDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,338 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	~wxGxDialog();	
};

