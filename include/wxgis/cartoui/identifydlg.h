/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISIdentifyDlg class - dialog/dock window with the results of identify.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#pragma once

#include "wxgis/cartoui/cartoui.h"

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/panel.h>

/** \class wxGISIdentifyDlg identifydlg.h
 *  \brief The wxGISIdentifyDlg class is dialog/dock window with the results of identify.
 */
class wxGISIdentifyDlg : public wxPanel 
{
protected:
	enum
	{
		ID_WXGISIDENTIFYDLG = 1000,
	};		
	
	public:
	wxGISIdentifyDlg( wxWindow* parent, wxWindowID id = ID_WXGISIDENTIFYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 343,576 ), long style = wxTAB_TRAVERSAL );
	virtual ~wxGISIdentifyDlg();
		
	void SplitterOnIdle( wxIdleEvent& )
	{
		m_splitter->SetSashPosition( 0 );
		m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
	}	
protected:
	wxBoxSizer* m_bMainSizer;
	wxFlexGridSizer* m_fgTopSizer;
	wxStaticText* m_staticText1;
	wxChoice* m_LayerChoice;
	wxBitmapButton* m_bpSplitButton;
	wxSplitterWindow* m_splitter;
};

