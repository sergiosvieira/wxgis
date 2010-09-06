/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  extended tooltip class.
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

#include "wxgis/framework/tooltip.h"
#include "wxgis/framework/application.h"

//#include "../../art/close_16.xpm"
//#include "../../art/close_bw_16.xpm"
#include "../../art/close_16a.xpm"

#define FRAMENAME wxT("baloontip")

BEGIN_EVENT_TABLE(wxGISBaloonTip, wxFrame)
    EVT_PAINT(wxGISBaloonTip::OnPaint)
    EVT_LEFT_DOWN(wxGISBaloonTip::OnClick)
    EVT_TIMER(TIMER_BALOON,wxGISBaloonTip::OnTimerTick)
    EVT_BUTTON(wxID_CLOSE, wxGISBaloonTip::OnClose)
END_EVENT_TABLE()
 
wxGISBaloonTip::wxGISBaloonTip(wxString sTitle, wxIcon Icon, wxString sMessage) : wxFrame(NULL, wxID_ANY, wxT("no title"), wxDefaultPosition, wxDefaultSize,wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_SHAPED | wxFRAME_NO_TASKBAR, FRAMENAME)
{
    //wxColour bgColour(255,255,231); // yellow BG
    wxColour bgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    SetBackgroundColour(bgColour);
    wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);

    //
 	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
    wxStaticBitmap* pStateBitmap = new wxStaticBitmap( this, wxID_ANY, Icon, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( pStateBitmap, 0, wxALL, 5 );
	
    wxStaticText * title = new wxStaticText(this, wxID_ANY, sTitle);
    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);

	//title->Wrap( -1 );
	fgSizer1->Add( title, 1, wxALL | wxEXPAND, 5 );

    title->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxGISBaloonTip::OnClick), NULL, this );
    title->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(wxGISBaloonTip::OnEscape), NULL, this );

    //wxStaticBitmap* pCloseBitmap = new wxStaticBitmap( this, wxID_ANY, wxIcon(close_16_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    wxBitmap NormalBitmap = wxBitmap(close_16a_xpm);
    wxImage bwImage = NormalBitmap.ConvertToImage();
    wxBitmap bwBitmap = bwImage.ConvertToGreyscale();
    wxBitmapButton* bpCloseButton = new wxBitmapButton( this, wxID_CLOSE, bwBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    bpCloseButton->SetBackgroundColour(bgColour);
    ////wxBitmapButton* bpCloseButton = new wxBitmapButton( this, wxID_CLOSE, wxBitmap(close_bw_16_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    bpCloseButton->SetBitmapDisabled(bwBitmap);
    bpCloseButton->SetBitmapLabel(bwBitmap);
    bpCloseButton->SetBitmapSelected(NormalBitmap);
 ////   //bpCloseButton->SetBitmapFocus(wxBitmap(close_16_xpm));
    bpCloseButton->SetBitmapHover(NormalBitmap);
    bpCloseButton->SetToolTip(_("Close"));
	fgSizer1->Add( bpCloseButton, 2, wxALL, 5 );
    //
    //wxStaticText * title = new wxStaticText(this, wxID_ANY, sTitle);
    //wxFont titleFont = this->GetFont();
    //titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    //title->SetFont(titleFont);
    //mainSizer->Add(title,0,wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5);
    //title->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxGISBaloonTip::OnClick), NULL, this );
    //title->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(wxGISBaloonTip::OnEscape), NULL, this );
    
    //mainSizer->Add(fgSizer1,1,wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
    mainSizer->Add(fgSizer1,0,wxEXPAND/* | wxBOTTOM */| wxLEFT | wxRIGHT, 5);
 
    wxStaticText * text = new wxStaticText(this, wxID_ANY, sMessage);
    mainSizer->Add(text,1,wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
    text->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(wxGISBaloonTip::OnClick), NULL, this );
    text->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(wxGISBaloonTip::OnEscape), NULL, this );

    mainSizer->AddStretchSpacer();
 
    SetSizer(mainSizer);
    mainSizer->SetSizeHints( this );
 
    this->timer = new wxTimer(this, TIMER_BALOON);
 
    // here, we try to align the frame to the right bottom corner
    wxWindow* pOtherTip = wxWindow::FindWindowByName(FRAMENAME);
    if(pOtherTip && pOtherTip != this)
    {
        wxSize size = GetSize();
        wxRect rect = pOtherTip->GetRect();
        int iX = 0, iY = 0;
        wxPoint PointTR = rect.GetTopRight();
        iX = PointTR.x - size.GetWidth();
        iY = PointTR.y - 3 - size.GetHeight();
        Move( iX, iY );
    }
    else
    {
        Center();
        int iX = 0, iY = 0;
        GetPosition( &iX, &iY );
        iX = (iX * 2) - 3;
        iY = (iY * 2) - 3;
        Move( iX, iY );
    }

    IApplication* pApp = ::GetApplication();
    pApp->RegisterChildWindow(this);
}
 
void wxGISBaloonTip::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
 
    int iWidth = 0, iHeight = 0;
    GetClientSize( &iWidth, &iHeight );

    //dc.SetBackground(*wxTRANSPARENT_BRUSH);
    //dc.SetBrush(*wxTRANSPARENT_BRUSH);
    //dc.Clear();
 
    wxPen pen(GetForegroundColour());
    dc.SetPen(pen);
 
    wxBrush brush(GetBackgroundColour());
    dc.SetBrush(brush);
 
//    dc.DrawRectangle(0,0,iWidth,iHeight);
    dc.DrawRoundedRectangle(0,0,iWidth,iHeight, 3);
}
 
/** closing frame at end of timeout */
void wxGISBaloonTip::OnTimerTick(wxTimerEvent & event)
{
    Close();
}
 
/** showing frame and running timer */
void wxGISBaloonTip::ShowBaloon(unsigned int iTimeout)
{
    Show(false);
    Show(true);
    this->timer->Start(iTimeout, wxTIMER_ONE_SHOT);
}

void wxGISBaloonTip::Close(void)
{
    IApplication* pApp = ::GetApplication();
    pApp->UnRegisterChildWindow(this);
    Destroy();
}
