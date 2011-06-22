/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Main frame class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/cat_app/gisaboutdlg.h"

#include "../../art/mainframe.xpm"
#include "../../art/mainframe_x.xpm"

#include <wx/datetime.h>

//-----------------------------------------------------------
// wxGISCatalogFrame
//-----------------------------------------------------------


BEGIN_EVENT_TABLE(wxGISCatalogFrame, wxGxApplication)
END_EVENT_TABLE()


wxGISCatalogFrame::wxGISCatalogFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGxApplication(parent, id, title, pos, size, style)
{
	//set app main icon
    wxDateTime now = wxDateTime::Now();
    if((now.GetMonth() == wxDateTime::Month::Dec && now.GetDay() > 15) || (now.GetMonth() == wxDateTime::Month::Jan && now.GetDay() < 15))
        SetIcon(wxIcon(mainframe_x_xpm));
    else
        SetIcon(wxIcon(mainframe_xpm));
}

wxGISCatalogFrame::~wxGISCatalogFrame(void)
{
}

void wxGISCatalogFrame::OnAppAbout(void)
{
    wxGISAboutDialog oDialog(this);
    oDialog.ShowModal();
}

