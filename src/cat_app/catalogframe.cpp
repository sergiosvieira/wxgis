/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main frame class.
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
#include "wxgis/cat_app/catalogframe.h"

//-----------------------------------------------------------
// wxGISCatalogFrame
//-----------------------------------------------------------


BEGIN_EVENT_TABLE(wxGISCatalogFrame, wxGxApplication)
END_EVENT_TABLE()


wxGISCatalogFrame::wxGISCatalogFrame(IGISConfig* pConfig, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGxApplication(pConfig, parent, id, title, pos, size, style)
{
	//set app main icon
	SetIcon(wxIcon(mainframe_xpm));
}

wxGISCatalogFrame::~wxGISCatalogFrame(void)
{
}

void wxGISCatalogFrame::OnAppAbout(void)
{
	wxMessageBox(wxString::Format(_("wxGIS [%s]\nVersion: %s\n(c) 2009 Dmitry Barishnikov (bishop)"), APP_NAME, APP_VER), _("About"), wxICON_INFORMATION | wxOK);
}

