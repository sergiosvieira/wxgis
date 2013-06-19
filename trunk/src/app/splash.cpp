/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Splash scree class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/app/splash.h"
#include "wxgis/core/config.h"

#include <wx/font.h>

#include "../../art/splash.xpm"

//----------------------------------------------------------------------------
// wxGISSplashScreen
//----------------------------------------------------------------------------

wxGISSplashScreen::wxGISSplashScreen(int milliseconds, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style) : wxSplashScreen(wxBitmap(splash_xpm), wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, milliseconds, parent, id, pos, size, style)
{
    IApplication* pApp = dynamic_cast<IApplication*>(parent);
    wxWindow *win = GetSplashWindow();

    wxWindowDC dc(win);
    int width, height;

	//wxString s(_("build by"));
	//wxFont font(18, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Sans");
 //   dc.SetFont(font);
 //   dc.SetBackgroundMode(wxTRANSPARENT); 
 //   dc.GetTextExtent(s, &width, &height);
 //   //110 x 200 = center

 //   dc.DrawText(s, wxPoint(200  - width / 2, 110 - height / 2));

	//wxFont font_t(10, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Sans");
 //   dc.SetFont(font_t);
	//wxString t(_("o p e n    g e o t e c n o l o g y"));
 //   dc.GetTextExtent(t, &width, &height);
 //   dc.DrawText(t, wxPoint(200  - width / 2, 170 - height / 2));

	wxFont font_n(18, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Sans");
    dc.SetFont(font_n);
    wxString sName = wxString::Format(wxT("%s"), pApp->GetUserAppNameShort().c_str());
    dc.GetTextExtent(sName, &width, &height);
    dc.DrawText(sName, wxPoint(200  - width / 2, 110 - height / 2));

	wxFont font_v(12, wxFONTFAMILY_DEFAULT , wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Sans");
    dc.SetFont(font_v);
    wxString v = wxString::Format(_T("Version: %s"), pApp->GetAppVersionString().c_str());
    dc.GetTextExtent(v, &width, &height);
    dc.DrawText(v, wxPoint(200  - width / 2, 150 - height / 2));
    wxString d = wxString::Format(_T("Build: %s"), wxString(__DATE__,wxConvLibc).c_str());
    dc.GetTextExtent(d, &width, &height);
    dc.DrawText(d, wxPoint(200  - width / 2, 170 - height / 2));    

    m_checkBoxShow = new wxCheckBox( win, ID_CHECK, _("Don't show this screen in future"), wxPoint(10, 195) );
    m_checkBoxShow->SetBackgroundColour(*wxWHITE);
//    wxStaticText *text = new wxStaticText( win, wxID_EXIT|wxTE_READONLY, s, wxPoint(200,100) );//wxPoint(200 - width, 100 - height)
//    text->SetFont(font);
//    text->SetBackgroundColour( wxBG_STYLE_TRANSPARENT );
 //   wxFont f(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Sans");
 //   wxFont titleFont = this->GetFont();
 //   titleFont.SetWeight(wxFONTWEIGHT_BOLD);
 //   m_title->SetFont(titleFont);
	//m_title->Wrap( -1 );
	//m_title->SetBackgroundColour( BackColor );

 //   m_title = new wxGenericStaticText( this, wxID_ANY, wxString::Format(_("\nwxGIS [%s] (x86)\nVersion: %s"), pApp->GetAppName(), pApp->GetAppVersionString()), wxDefaultPosition, wxDefaultSize, 0 );

 //  
 //   wxStaticText *text = new wxStaticText( win, wxID_EXIT, wxT("click somewhere\non this image"), wxPoint(m_isPda ? 0 : 13, m_isPda ? 0 : 11) );
 //   text->SetBackgroundColour(*wxWHITE);
 //   text->SetForegroundColour(*wxBLACK);
 //   wxFont font = text->GetFont();
 //   font.SetPointSize(2 * font.GetPointSize() / 3);
 //   text->SetFont(font);

 //   	m_pStaticText = new wxTextCtrl( this, ID_EDIT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_AUTO_URL, wxDefaultValidator, wxT("GISSimpleText") );


}

wxGISSplashScreen::~wxGISSplashScreen()
{
}

int wxGISSplashScreen::FilterEvent(wxEvent& event)
{
    if(event.GetId() == ID_CHECK)
    {
        wxGISAppConfig oConfig = GetConfig();
	    if(!oConfig.IsOk())
            return -1;
        wxCheckBoxState st = m_checkBoxShow->Get3StateValue();
        oConfig.Write(enumGISHKCU, wxT("wxGISCommon/splash/show"), st == wxCHK_CHECKED ? false : true);

        return -1;
    }
    return wxSplashScreen::FilterEvent(event);

    //const wxEventType t = event.GetEventType();
    //if ( t == wxEVT_KEY_DOWN || t == wxEVT_RIGHT_DOWN || t == wxEVT_MIDDLE_DOWN )
    //    Close(true);

    //return -1;
}