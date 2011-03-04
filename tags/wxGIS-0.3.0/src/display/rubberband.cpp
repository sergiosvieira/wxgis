/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRubberBand class.
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
END_EVENT_TABLE()

wxGISRubberBand::wxGISRubberBand(void) :  m_pRetGeom(NULL), m_bLock(true)
{
}

wxGISRubberBand::~wxGISRubberBand(void)
{
}

OGRGeometry* wxGISRubberBand::TrackNew(wxCoord x, wxCoord y, wxWindow *pWnd, ICachedDisplay* pCachedDisplay, ISymbol* pSymbol)
{
	m_StartX = x;
	m_StartY = y;
	m_pWnd = pWnd;
	m_pCachedDisplay = pCachedDisplay;
	m_pSymbol = pSymbol;

	wxRect ScrRect = m_pWnd->GetScreenRect();
	wxRect Rect = m_pWnd->GetRect();
	m_StartXScr = ScrRect.GetLeft() + m_StartX - Rect.GetLeft();
	m_StartYScr = ScrRect.GetTop() + m_StartY - Rect.GetTop();

	pWnd->CaptureMouse();
	pWnd->PushEventHandler(this);

	wxEventLoopBase* const loop = wxEventLoop::GetActive();
	while(loop->IsRunning())
	{
		loop->Dispatch();
		if(!m_bLock) break;
	}
	pWnd->PopEventHandler();
	pWnd->ReleaseMouse();
	return m_pRetGeom;
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
		{
			wxClientDC CDC(m_pWnd);
			m_pCachedDisplay->OnDraw(CDC);
		}
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

//----------------------------------------------------
// class wxGISRubberEnvelope
//----------------------------------------------------

wxGISRubberEnvelope::wxGISRubberEnvelope() : wxGISRubberBand()
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
	X = MIN(m_StartX, EvX);
	Y = MIN(m_StartY, EvY);    

    if(!m_PrevRect.IsEmpty())
    {
        m_PrevRect.Inflate(2,2);
        m_pCachedDisplay->AddInvalidRect(m_PrevRect);
    }
	wxClientDC CDC(m_pWnd);
    m_pCachedDisplay->OnDraw(CDC);


//#if wxUSE_GRAPHICS_CONTEXT	
//	wxGCDC GDC(CDC);
//	GDC.SetPen(m_pSymbol->GetPen());
//	GDC.SetBrush(m_pSymbol->GetBrush());
//	GDC.DrawRectangle(X, Y, width, height);
//#else
	CDC.SetPen(m_pSymbol->GetPen());
	CDC.SetBrush(wxBrush(m_pSymbol->GetBrush().GetColour(), wxTRANSPARENT));
	CDC.SetLogicalFunction(wxOR_REVERSE);
	CDC.DrawRectangle(X, Y, width, height);
    m_PrevRect = wxRect(X, Y, width, height);
//#endif
}

void wxGISRubberEnvelope::OnMouseUp(wxMouseEvent& event)
{
	IDisplayTransformation* pDT = m_pCachedDisplay->GetDisplayTransformation();
	wxPoint Points[2];
	Points[0] = wxPoint(m_StartX, m_StartY);
	Points[1] = wxPoint(event.GetX(), event.GetY());
	OGRRawPoint* pOGRPoints = pDT->TransformCoordDC2World(Points, 2);
	OGRLineString* pLine = new OGRLineString();
	pLine->setPoints(2, pOGRPoints);
	m_pRetGeom = static_cast<OGRGeometry*>(pLine);
	delete [] pOGRPoints;
	OnUnlock();
    m_PrevRect.width = -1;
    m_PrevRect.height = -1;
}
