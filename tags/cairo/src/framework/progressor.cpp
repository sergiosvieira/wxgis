/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressor class. Progress of some process
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/framework/progressor.h"

//BEGIN_EVENT_TABLE(wxGISProgressor, wxControl)
//	EVT_SIZE(wxGISProgressor::OnSize)
//	EVT_PAINT(wxGISProgressor::OnPaint)
//	EVT_ERASE_BACKGROUND(wxGISProgressor::OnEraseBackground)
//    EVT_TIMER( PTIMER_ID, wxGISProgressor::OnTimer )
//END_EVENT_TABLE()
//
//wxGISProgressor::wxGISProgressor(wxWindow * parent, wxWindowID id, int range, const wxPoint & pos, const wxSize & size, long style, const wxString name) : wxControl(parent, id, pos, size, style, wxDefaultValidator, name), m_nValue(0), m_bPulse(false), m_timer(this, PTIMER_ID), m_bYield(true)
//{
//    if(range <= 0)
//        m_nRange = 100;
//    else
//        m_nRange = range;
//}
//
//wxGISProgressor::~wxGISProgressor()
//{
//}
//
//void wxGISProgressor::OnSize(wxSizeEvent & event)
//{
//    Refresh();
//}
//
//void wxGISProgressor::OnPaint(wxPaintEvent & event)
//{
//	wxPaintDC dc(this);
//    wxSize size = wxControl::GetClientSize();
//
//	wxBrush br(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
//    if(m_bPulse)
//    {
//        if(m_nValue >= m_nRange + 22)
//            m_nValue = 0;
//	    dc.SetBrush(br);
//	    dc.SetPen(*wxTRANSPARENT_PEN);
////	    dc.SetPen(wxNullPen);
//
//        int pos = size.x * m_nValue / m_nRange;
//        int psize = 8/*m_nRange / 12*/;
//
//	    dc.DrawRectangle( pos, 1, psize, size.y - 2);
//        pos -= psize + 2;
//	    dc.DrawRectangle( pos, 1, psize, size.y - 2);
//        pos -= psize + 2;
//	    dc.DrawRectangle( pos, 1, psize, size.y - 2);
//        m_nValue++;
//    }
//    else
//    {
//	    int width, height;
//        wxString format_s = wxString::Format(wxT("%d%%"), 100 * m_nValue / m_nRange);
//        wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
//        Font.SetWeight(wxFONTWEIGHT_BOLD);
//        //dc.SetBackgroundMode(wxTRANSPARENT);
//        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));//*wxBLACK);
//        dc.SetTextBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));//*wxWHITE);
//
//	    dc.SetFont(Font);
//	    dc.GetTextExtent(format_s, &width, &height);
//        int x1 = (size.x - width) / 2, y1 = (size.y - height) / 2;
//        dc.DrawText(format_s, x1, y1);
//
//        dc.SetLogicalFunction(wxOR_REVERSE);
//
//	    dc.SetPen(*wxTRANSPARENT_PEN/*wxNullPen*/);
//	    dc.SetBrush(br);
//	    dc.DrawRectangle( 0, 0, size.x * m_nValue / m_nRange, size.y);
//    }
//}
//
//void wxGISProgressor::OnEraseBackground(wxEraseEvent & event)
//{
//    ClearBackground();//wxControl::OnEraseBackground(event);
//}
//
//void wxGISProgressor::SetRange(int range)
//{
//    if(range <= 0)
//        m_nRange = 100;
//    else
//        m_nRange = range;
//    m_nValue = 0;
//}
//
//int wxGISProgressor::GetRange()
//{
//    return m_nRange;
//}
//
//void wxGISProgressor::SetValue(int value)
//{
//    Stop();
//    if(m_nValue == value)
//        return;
//    m_nValue = value;
//    if(m_nValue > m_nRange)
//        m_nRange = m_nValue;
//
//    //static wxWindow* pWnd(NULL);
//    //if(!pWnd)
//    //    pWnd = GetParent()->GetParent();
//    //if(pWnd)
//    if(m_bYield)
//        ::wxSafeYield(NULL, true);//(pWnd, true);
//
//
//    Refresh();
//}
//
//void wxGISProgressor::SetYield(bool bYield)
//{
//    m_bYield = bYield;
//}
//
//
//int wxGISProgressor::GetValue()
//{
//    return m_nValue;
//}
//
//void wxGISProgressor::Play()
//{
//    m_bPulse = true;
//    m_timer.Start(50);
//}
//
//void wxGISProgressor::Stop()
//{
//    m_bPulse = false;
//    m_timer.Stop();
//}
//
//void wxGISProgressor::OnTimer( wxTimerEvent& event )
//{
//	Refresh();
//}
//
//bool wxGISProgressor::Show(bool bShow)
//{
//    return wxControl::Show(bShow);
//}

//NEW --
wxGISProgressor::wxGISProgressor(wxWindow * parent, wxWindowID id, int range, const wxPoint & pos, const wxSize & size, long style, const wxString name) : wxGauge(parent, id, range, pos, size, style, wxDefaultValidator, name), m_nValue(0), m_bYield(false)
{
}

wxGISProgressor::~wxGISProgressor()
{
}

bool wxGISProgressor::Show(bool bShow)
{
	return wxGauge::Show(bShow);
}

void wxGISProgressor::SetRange(int range)
{
	wxGauge::SetRange(range);
}

int wxGISProgressor::GetRange()
{
	return wxGauge::GetRange();
}

void wxGISProgressor::SetValue(int value)
{
	m_nValue = value;
	wxGauge::SetValue(value);
    if(m_bYield)
        ::wxSafeYield(NULL, true);
}

int wxGISProgressor::GetValue()
{
	return wxGauge::GetValue();
}

void wxGISProgressor::Play(void)
{
	wxGauge::Pulse();
    if(m_bYield)
        ::wxSafeYield(NULL, true);
}

void wxGISProgressor::Stop(void)
{
	wxGauge::SetValue(m_nValue);
    if(m_bYield)
        ::wxSafeYield(NULL, true);
}

void wxGISProgressor::SetYield(bool bYield)
{
	m_bYield = bYield;
}

