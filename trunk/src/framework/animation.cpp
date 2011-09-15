/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISAnimation class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009 Bishop
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
#include "wxgis/framework/animation.h"

BEGIN_EVENT_TABLE(wxGISAnimation, wxControl)
	EVT_PAINT(wxGISAnimation::OnPaint)
	EVT_ERASE_BACKGROUND(wxGISAnimation::OnEraseBackground)
	EVT_TIMER( ATIMER_ID, wxGISAnimation::OnTimer )
	//EVT_SIZE(wxGISAnimation::OnSize)
END_EVENT_TABLE()

wxGISAnimation::wxGISAnimation(wxWindow * parent, wxWindowID id, const wxBitmap & bitmap, const int bitmap_size, const wxPoint & pos, const wxSize & size, long style, const wxString name) : wxControl(parent, id, pos, size, style, wxDefaultValidator, name), m_timer(this, ATIMER_ID), m_bYield(true)
{
	m_nImgPos = 0;
	m_ImageList.Create(bitmap_size, bitmap_size);
	m_ImageList.Add(bitmap);

	//Play();

	//	m_BackgroundBitmap = bitmap;
	//	m_Timer = new wxTimer(this, ID_PAINTING_TEST_CTRL_TIMER);
	//	m_Timer->Start(500);
	//Create(parent, id, bitmap, bitmap_size, pos, size, style, name);
}

wxGISAnimation::~wxGISAnimation()
{
	//if(m_Timer)
	//{
	//	wxDELETE(m_Timer);
	//}
}

void wxGISAnimation::OnPaint(wxPaintEvent & event)
{
	wxPaintDC dc(this);
	int x(0), y(0);
	m_ImageList.GetSize(0, x, y);
	wxRect rect;
	wxRect rc = GetClientRect();
	x = (rc.width - x) / 2;
	y = (rc.height - y) / 2;
	m_ImageList.Draw(m_nImgPos, dc, x, y, wxIMAGELIST_DRAW_TRANSPARENT /*wxIMAGELIST_DRAW_NORMAL*/, true);
	m_nImgPos++;
	if(m_ImageList.GetImageCount() <= m_nImgPos)
		m_nImgPos = 0;
}

//void wxPaintingTestCtrl::DoDraw(wxDC & dc)
//{
//	int x, y, w, h, i;
//	double angle;
//	GetClientSize(&w, &h);
//	int cx(w/2), cy(h/2), radius(min(cx,cy)-10);
//	if(m_BackgroundBitmap.Ok())
//	{
//		for(y = 0; y < h; y += m_BackgroundBitmap.GetHeight())
//		{
//			for(x = 0; x < w; x += m_BackgroundBitmap.GetWidth())
//			{
//				dc.DrawBitmap(m_BackgroundBitmap, x, y, true);
//			}
//		}
//	}
//	else
//	{
//		dc.SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
//		dc.Clear();
//	}
//	if(m_CenterBitmap.Ok())
//	{
//		radius = (m_CenterBitmap.GetWidth()/2) * 5.0 / 6.0;
//		dc.DrawBitmap(m_CenterBitmap, cx-m_CenterBitmap.GetWidth()/2,
//			cy-m_CenterBitmap.GetHeight()/2, true);
//	}
//	wxPen linePen(*wxBLACK, 2, wxSOLID);
//	dc.SetPen(linePen);
//	for(i = 0; i < 12; ++i)
//	{
//		angle = 2.0*3.1415926/12.0*(double)i;
//		dc.DrawLine(cx+radius*cos(angle), cy+radius*sin(angle),
//			cx+(radius/5.0*4.0)*cos(angle), cy+(radius/5.0*4.0)*sin(angle));
//	}
//	wxDateTime now = wxDateTime::Now();
//	linePen.SetColour(*wxRED);
//	linePen.SetWidth(3);
//	dc.SetPen(linePen);
//	angle = 2.0*3.1415926/12.0*(double)now.GetHour()-3.1415926/2;
//	dc.DrawLine(cx, cy, cx+radius*cos(angle), cy+radius*sin(angle));
//	linePen.SetWidth(2);
//	dc.SetPen(linePen);
//	angle = 2.0*3.1415926/60.0*(double)now.GetMinute()-3.1415926/2;
//	dc.DrawLine(cx, cy, cx+radius*cos(angle), cy+radius*sin(angle));
//	linePen.SetColour(wxColour(0,0,127));
//	linePen.SetWidth(1);
//	dc.SetPen(linePen);
//	angle = 2.0*3.1415926/60.0*(double)now.GetSecond()-3.1415926/2;
//	dc.DrawLine(cx, cy, cx+radius*cos(angle), cy+radius*sin(angle));
//}
//
//void wxPaintingTestCtrl::OnSize(wxSizeEvent & event)
//{
//	m_DoubleBufferDC.SelectObject(wxNullBitmap);
//	m_DoubleBuffer = wxBitmap(event.GetSize().GetWidth(), event.GetSize().GetHeight());
//	m_DoubleBufferDC.SelectObject(m_DoubleBuffer);
//	DoDraw(m_DoubleBufferDC);
//	Refresh();
//}

void wxGISAnimation::OnEraseBackground(wxEraseEvent & event)
{
}

//void wxPaintingTestCtrl::OnRefreshTimer(wxTimerEvent & event)
//{
//	DoDraw(m_DoubleBufferDC);
//	Refresh();
//}

void wxGISAnimation::OnTimer( wxTimerEvent& event )
{
	Refresh();
    if(m_bYield)
		::wxSafeYield(NULL, true);
}

void wxGISAnimation::SetYield(bool bYield)
{
    m_bYield = bYield;
}