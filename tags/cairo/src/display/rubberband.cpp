/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRubberBand class.
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

#include "wxgis/display/rubberband.h"
#include "wx/evtloop.h"

BEGIN_EVENT_TABLE(wxGISRubberBand, wxEvtHandler)
	EVT_KEY_DOWN(wxGISRubberBand::OnKeyDown)
	EVT_LEFT_DOWN(wxGISRubberBand::OnMouseDown)
	EVT_MIDDLE_DOWN(wxGISRubberBand::OnMouseDown)
	EVT_RIGHT_DOWN(wxGISRubberBand::OnMouseDown)
	EVT_LEFT_UP(wxGISRubberBand::OnMouseUp)
	EVT_MIDDLE_UP(wxGISRubberBand::OnMouseUp)
	EVT_RIGHT_UP(wxGISRubberBand::OnMouseUp)
	EVT_LEFT_DCLICK(wxGISRubberBand::OnMouseDoubleClick)
	EVT_MIDDLE_DCLICK(wxGISRubberBand::OnMouseDoubleClick)
	EVT_RIGHT_DCLICK(wxGISRubberBand::OnMouseDoubleClick)
	EVT_MOTION(wxGISRubberBand::OnMouseMove)
	EVT_LEAVE_WINDOW(wxGISRubberBand::OnLeave)
	EVT_ENTER_WINDOW(wxGISRubberBand::OnEnter)
	EVT_MOUSE_CAPTURE_LOST(wxGISRubberBand::OnCaptureLost)
END_EVENT_TABLE()

wxGISRubberBand::wxGISRubberBand(wxPen &oPen, wxWindow *pWnd, wxGISDisplayEx *pDisp) :  m_bLock(true)
{
	m_pWnd = pWnd;
	m_pDisp = pDisp;
	m_oPen = oPen;
}

wxGISRubberBand::~wxGISRubberBand(void)
{
}

OGREnvelope wxGISRubberBand::TrackNew(wxCoord x, wxCoord y)
{
	m_StartX = x;
	m_StartY = y;

	wxRect ScrRect = m_pWnd->GetScreenRect();
	wxRect Rect = m_pWnd->GetRect();
	m_StartXScr = ScrRect.GetLeft() + m_StartX - Rect.GetLeft();
	m_StartYScr = ScrRect.GetTop() + m_StartY - Rect.GetTop();

	m_pWnd->CaptureMouse();
	m_pWnd->PushEventHandler(this);

	wxEventLoopBase* const loop = wxEventLoop::GetActive();
	while(loop->IsRunning())
	{
		loop->Dispatch();
		if(!m_bLock) break;
	}
	m_pWnd->PopEventHandler();
	m_pWnd->ReleaseMouse();
	return m_RetEnv;
}

void wxGISRubberBand::OnUnlock(void)
{
	m_bLock = false;
}

void wxGISRubberBand::OnKeyDown(wxKeyEvent & event)
{
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pWnd->Refresh(false);
		OnUnlock();
		break;
	default:
		break;
	}
	//event.Skip();
}

void wxGISRubberBand::OnMouseMove(wxMouseEvent& event)
{
}

void wxGISRubberBand::OnMouseDown(wxMouseEvent& event)
{
}

void wxGISRubberBand::OnMouseUp(wxMouseEvent& event)
{
}

void wxGISRubberBand::OnMouseDoubleClick(wxMouseEvent& event)
{
}

void wxGISRubberBand::OnLeave(wxMouseEvent& event)
{
}

void wxGISRubberBand::OnEnter(wxMouseEvent& event)
{
}

void wxGISRubberBand::OnCaptureLost(wxMouseCaptureLostEvent & event)
{
	event.Skip();
	if( m_pWnd->HasCapture() )
		m_pWnd->ReleaseMouse();
}
//----------------------------------------------------
// class wxGISRubberEnvelope
//----------------------------------------------------

wxGISRubberEnvelope::wxGISRubberEnvelope(wxPen &oPen, wxWindow *pWnd, wxGISDisplayEx *pDisp) : wxGISRubberBand(oPen, pWnd, pDisp)
{
}

wxGISRubberEnvelope::~wxGISRubberEnvelope()
{
}

void wxGISRubberEnvelope::OnMouseMove(wxMouseEvent& event)
{
	int EvX = event.GetX(), EvY = event.GetY();
	int width, height, X, Y;
	width = abs(EvX - m_StartX);
	height = abs(EvY - m_StartY);
	X = std::min(m_StartX, EvX);
	Y = std::min(m_StartY, EvY);    

	wxClientDC CDC(m_pWnd);
    if(!m_PrevRect.IsEmpty())
    {
        m_PrevRect.Inflate(2,2);
        //m_pCachedDisplay->AddInvalidRect(m_PrevRect);
		//m_pWnd->RefreshRect(m_PrevRect.Inflate(2,2), false);
		double dX1 = double(m_PrevRect.GetLeft());
		double dY1 = double(m_PrevRect.GetTop());
		double dX2 = double(m_PrevRect.GetRight());
		double dY2 = double(m_PrevRect.GetBottom());
		m_pDisp->Output(&CDC, &dX1, &dY1, &dX2, &dY2);
    }
	else
		m_pDisp->Output(&CDC);

	CDC.SetPen(m_oPen);
	CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
	CDC.SetLogicalFunction(wxOR_REVERSE);
	CDC.DrawRectangle(X, Y, width, height);
    m_PrevRect = wxRect(X, Y, width, height);
}

void wxGISRubberEnvelope::OnMouseUp(wxMouseEvent& event)
{
	double dX1 = std::min(m_StartX, event.GetX());
	double dY1 = std::max(m_StartY, event.GetY());
	double dX2 = std::max(m_StartX, event.GetX());
	double dY2 = std::min(m_StartY, event.GetY());
	wxRect rc(wxPoint(dX1, dY1), wxPoint(dX2, dY2));
	m_RetEnv = m_pDisp->TransformRect(rc);
	OnUnlock();
    m_PrevRect.width = -1;
    m_PrevRect.height = -1;
}
