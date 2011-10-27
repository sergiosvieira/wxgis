/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnectionUI class.
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

#include "wxgis/catalogui/gxremoteconnui.h"

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(CPLString Path, wxString Name, wxIcon LargeIconConn, wxIcon SmallIconConn, wxIcon LargeIconDisconn, wxIcon SmallIconDisconn) : wxGxRemoteConnection(Path, Name)
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;
}

wxGxRemoteConnectionUI::~wxGxRemoteConnectionUI(void)
{
}

wxIcon wxGxRemoteConnectionUI::GetLargeImage(void)
{
	//TODO: get connection state
    return m_oLargeIconDisconn;
}

wxIcon wxGxRemoteConnectionUI::GetSmallImage(void)
{
	//TODO: get connection state
    return m_oSmallIconDisconn;
}

void wxGxRemoteConnectionUI::EditProperties(wxWindow *parent)
{
    //wxPropertySheetDialog PropertySheetDialog;
    //if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
    //    return;
    //PropertySheetDialog.SetIcon(properties_xpm);
    //PropertySheetDialog.CreateButtons(wxOK);
    //wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    //wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(GetSpatialReference(), pParentWnd);
    //PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());

    //PropertySheetDialog.LayoutDialog();
    ////center?
    //PropertySheetDialog.SetSize(480,640);
    //PropertySheetDialog.Center();

    //PropertySheetDialog.ShowModal();
}

bool wxGxRemoteConnectionUI::Invoke(wxWindow* pParentWnd)
{
    EditProperties(pParentWnd);
    return true;
}