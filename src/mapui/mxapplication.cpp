/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  wxMxApplication code.
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

#include "wxgis/mapui/mxapplication.h"

//-----------------------------------------------
// wxMxApplication
//-----------------------------------------------
IMPLEMENT_CLASS(wxMxApplication, wxGISApplication)

wxMxApplication::wxMxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplicationEx(parent, id, title, pos, size, style)
{
}

wxMxApplication::~wxMxApplication(void)
{
}

bool wxMxApplication::Create(IGISConfig* pConfig)
{
    wxGISApplicationEx::Create(pConfig);

	wxLogMessage(_("wxMxApplication: Creation complete"));

    return true;
}

