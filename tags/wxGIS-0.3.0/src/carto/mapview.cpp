/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISMapView class.
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
#include "wxgis/carto/mapview.h"
//#include "wxgis/display/screendisplay.h"
#include "wxgis/display/screendisplayplus.h"
#include "wx/sysopt.h"

#define WAITTIME 650
#define wxUSE_GRAPHICS_CONTEXT 0

//-----------------------------------------------
//wxDrawingThread
//-----------------------------------------------

wxDrawingThread::wxDrawingThread(wxGISMapView* pView, std::vector<wxGISLayer*>& Layers) : wxThread(wxTHREAD_DETACHED), m_pView(pView), m_Layers(Layers)
{
    m_pTrackCancel = m_pView->GetTrackCancel();
	m_pGISScreenDisplay = m_pView->GetCachedDisplay();

	if(m_pTrackCancel)
		m_pTrackCancel->Reset();
}

void *wxDrawingThread::Entry()
{
#ifdef __WXGTK__
    wxMutexGuiEnter();
#endif

	wxClientDC CDC(m_pView);
    m_pGISScreenDisplay->SetDC(&CDC);

	for(size_t i = 0; i < m_Layers.size(); i++)
	{
		if(m_pTrackCancel && !m_pTrackCancel->Continue())
			break;
		if(m_pGISScreenDisplay->IsCacheDerty(m_Layers[i]->GetCacheID()) && m_Layers[i]->GetVisible())
		{
			size_t CacheIDCurrent, CacheIDPrevious;
			if(i == 0)
				CacheIDPrevious = 0;
			else
				CacheIDPrevious = m_Layers[i - 1]->GetCacheID();

			CacheIDCurrent = m_Layers[i]->GetCacheID();

			if(CacheIDCurrent != CacheIDPrevious)
			{
				m_pGISScreenDisplay->MergeCaches(CacheIDPrevious, CacheIDCurrent);

                //???????????????
                //if(m_Layers.size() > i + 2 && m_Layers[i + 1]->GetCacheID() != CacheIDCurrent)
                //    m_pGISScreenDisplay->SetCacheDerty(CacheIDCurrent, false);
			}

			if(m_pTrackCancel && !m_pTrackCancel->Continue())
				break;
			m_Layers[i]->Draw(wxGISDPGeography, m_pGISScreenDisplay, m_pTrackCancel);
            if(m_Layers.size() - 1 == i || (m_Layers.size() - 1 > i && m_Layers[i + 1]->GetCacheID() != CacheIDCurrent))
                m_pGISScreenDisplay->SetCacheDerty(CacheIDCurrent, false);
		}
	}
	m_pGISScreenDisplay->SetDerty(false);
	//m_pGISScreenDisplay->OnDraw(CDC);

#ifdef __WXGTK__
    wxMutexGuiLeave();
#endif

	return NULL;
}
void wxDrawingThread::OnExit()
{
	m_pView->OnThreadExit();
}

//-----------------------------------------------
//ExtenStack
//-----------------------------------------------
ExtenStack::ExtenStack(wxGISMapView* pView)
{
	m_pView = pView;
	m_Pos = -1;
}

bool ExtenStack::CanRedo()
{
	if(m_EnvelopeArray.empty())
		return false;
	return m_Pos < m_EnvelopeArray.size() - 1;
}

bool ExtenStack::CanUndo()
{
	if(m_EnvelopeArray.empty())
		return false;
	return m_Pos > 0;
}

void ExtenStack::Do(OGREnvelope NewEnv)
{
	m_Pos++;
	if(m_Pos == m_EnvelopeArray.size())
		m_EnvelopeArray.push_back(NewEnv);
	else
	{
		m_EnvelopeArray[m_Pos] = NewEnv;
		m_EnvelopeArray.erase(m_EnvelopeArray.begin() + m_Pos + 1, m_EnvelopeArray.end());
	}

	SetExtent(NewEnv);
}

void ExtenStack::Redo()
{
	m_Pos++;
	if(m_Pos < m_EnvelopeArray.size())
	{
		OGREnvelope Env = m_EnvelopeArray[m_Pos];
		SetExtent(Env);
	}
}

void ExtenStack::Undo()
{
	m_Pos--;
	if(m_Pos > -1)
	{
		OGREnvelope Env = m_EnvelopeArray[m_Pos];
		SetExtent(Env);
	}
}

void ExtenStack::SetExtent(OGREnvelope Env)
{
	m_pView->GetTrackCancel()->Cancel();
	if(m_pView->m_pThread)
		m_pView->m_pThread->Delete();
	IDisplayTransformation* pDisplayTransformation = m_pView->pGISScreenDisplay->GetDisplayTransformation();
    if(!pDisplayTransformation)
        return;
	pDisplayTransformation->SetBounds(Env);
	m_pView->pGISScreenDisplay->SetDerty(true);

	m_pView->Refresh(false);
}

void ExtenStack::Reset()
{
	m_EnvelopeArray.clear();
	m_Pos = -1;
}

size_t ExtenStack::GetSize()
{
	return m_EnvelopeArray.size();
}


//-----------------------------------------------
// wxGISMapView
//-----------------------------------------------

IMPLEMENT_CLASS(wxGISMapView, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxGISMapView, wxScrolledWindow)
	EVT_ERASE_BACKGROUND(wxGISMapView::OnEraseBackground)
	EVT_SIZE(wxGISMapView::OnSize)
	EVT_MOUSEWHEEL(wxGISMapView::OnMouseWheel)
	EVT_KEY_DOWN(wxGISMapView::OnKeyDown)
	EVT_TIMER( TIMER_ID, wxGISMapView::OnTimer )
	EVT_MOUSE_CAPTURE_LOST(wxGISMapView::OnCaptureLost)
END_EVENT_TABLE()

wxGISMapView::wxGISMapView(void) : wxGISMap(), m_pTrackCancel(NULL), m_pThread(NULL), m_pAni(NULL), m_timer(this, TIMER_ID)
{
}

wxGISMapView::wxGISMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxGISMap(), m_pTrackCancel(NULL), m_pThread(NULL), m_pAni(NULL), m_timer(this, TIMER_ID)
{
    Create(parent, id, pos, size, style);
}

wxGISMapView::~wxGISMapView(void)
{
	wxDELETE(pGISScreenDisplay);
	wxDELETE(m_pExtenStack);
	wxDELETE(m_pTrackCancel);
	if(m_pThread)
		m_pThread->Delete();
}

bool wxGISMapView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	//set map init envelope
#ifdef __WXGTK__
//    wxSystemOptions::SetOption(wxT("gtk.window.force-background-colour"), 1);
	SetBackgroundColour(wxColour(240, 255, 255));
	SetForegroundColour(wxColour(240, 255, 255));
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
//	SetThemeEnabled(false);
#endif

#if wxUSE_GRAPHICS_CONTEXT	& __WXMSW__
	pGISScreenDisplay = new wxGISScreenDisplayPlus();
#else
    pGISScreenDisplay = new wxGISScreenDisplay();
#endif
	m_pExtenStack = new ExtenStack(this);

	m_pTrackCancel = new ITrackCancel();
	m_pTrackCancel->Reset();

	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	pDisplayTransformation->SetDeviceFrame(GetClientRect());

	wxClientDC CDC(this);
	//pGISScreenDisplay->SetDC(&CDC);
	pDisplayTransformation->SetPPI(CDC.GetPPI());

	m_MouseState = enumGISMouseNone;
	m_MapToolState = enumGISMapNone;

    return wxScrolledWindow::Create(parent, id, pos, size, style | wxHSCROLL | wxVSCROLL );
}

void wxGISMapView::OnDraw(wxDC& dc)
{
#ifdef __WXGTK__
    if(m_MapToolState & enumGISMapPanning)
    {
        PanMoveTo(ScreenToClient(wxGetMousePosition()));
        return;
    }
#endif

	//ITrackCancel* pTrackCancel(NULL);
	//if map has it's own cache check it and draw from cache if posible
	//else redraw diry caches and out to dc
	//wxClientDC CDC(this);

	if(m_pExtenStack->GetSize() == 0)
	{
		IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
        if(!pDisplayTransformation)
		    m_pExtenStack->Do(pDisplayTransformation->GetBounds());
	}

	if(pGISScreenDisplay->IsDerty())
	{
	    dc.SetBackgroundMode(wxTRANSPARENT);
	    wxCriticalSectionLocker locker(m_CriticalSection);

        if(m_pTrackCancel)
            m_pTrackCancel->Cancel();

		if(m_pThread)
			m_pThread->Delete();

        if(m_pTrackCancel && !m_pAni)
		    m_pAni = m_pTrackCancel->GetProgressor();
		if(m_pAni)
		{
			m_pAni->Show(true);
			m_pAni->Play();
		}

		//start draw thread
		m_pThread = new wxDrawingThread(this, m_Layers);
		if(!CreateAndRunThread(m_pThread, wxT("wxGISMapView"), wxT("wxDrawingThread")))
			return;

		//wxScrolledWindow::SetFocus();

		//m_pTrackCancel->Reset();

		//for(size_t i = 0; i < m_Layers.size(); i++)
		//{
		//	if((m_pTrackCancel && !m_pTrackCancel->Continue()) || wxGetKeyState(WXK_ESCAPE))
		//		return;
		//	if(pGISScreenDisplay->IsCacheDerty(m_Layers[i]->GetCacheID()) && m_Layers[i]->GetVisible())
		//	{
		//		size_t CacheIDCurrent, CacheIDPrevious;
		//		if(i == 0)
		//			CacheIDPrevious = 0;
		//		else
		//			CacheIDPrevious = m_Layers[i - 1]->GetCacheID();

		//		CacheIDCurrent = m_Layers[i]->GetCacheID();

		//		if(CacheIDCurrent != CacheIDPrevious)
		//		{
		//			pGISScreenDisplay->MergeCaches(CacheIDPrevious, CacheIDCurrent);
		//			pGISScreenDisplay->SetCacheDerty(CacheIDCurrent, false);
		//		}

		//		m_Layers[i]->Draw(wxGISDPGeography, pGISScreenDisplay, m_pTrackCancel);
		//	}
		//}
		//pGISScreenDisplay->SetDerty(false);
		//pGISScreenDisplay->OnDraw(CDC/*dc*/);

		return;
	}

	//wxMouseState state = wxGetMouseState();
	//if(state.LeftDown() && (m_MouseState & enumGISMouseLeftDown))
	//{
	//	wxRect rc = GetClientRect();
	//	pGISScreenDisplay->OnStretchDraw(dc, rc.width, rc.height);
	//	//get moment button release
	//	wxMilliSleep(50);
	//	Refresh(false);
	//}
	////else if(m_MouseState & enumGISMouseWheel)
	////{
	////}
	//else
	//{
	//	if(m_MouseState & enumGISMouseLeftDown)
	//	{
	//		wxSizeEvent event(wxRect(0,0,0,0));
	//		OnSize(event);
	//		return;
	//	}
	//if(!state.LeftDown())

	if(m_MouseState != enumGISMouseNone)
		return;
	if(m_MapToolState != enumGISMapNone)
		return;

	pGISScreenDisplay->OnDraw(dc);
	//}
}

void wxGISMapView::OnSize(wxSizeEvent & event)
{
    event.Skip(true);

	m_pTrackCancel->Cancel();
	if(m_pThread)
		m_pThread->Delete();

	wxClientDC CDC(this);
	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	pDisplayTransformation->SetPPI(CDC.GetPPI());

	wxMouseState state = wxGetMouseState();
	if(state.LeftDown())
	{
		wxRect rc = GetClientRect();
		pGISScreenDisplay->OnStretchDraw(CDC, rc.width, rc.height);
		m_MouseState |= enumGISMouseLeftDown;
		m_timer.Start(300);
	}
	else
	{
		m_MouseState &= ~enumGISMouseLeftDown;//enumGISMouseNone;
		//set map init envelope
		pDisplayTransformation->SetDeviceFrame(GetClientRect());
		pGISScreenDisplay->SetDerty(true);
	}
#if __WXMSW__
	Refresh(false);
#else
	Update();
#endif
}

void wxGISMapView::OnEraseBackground(wxEraseEvent & event)
{
}

void wxGISMapView::AddLayer(wxGISLayer* pLayer)
{
    if(!pLayer)
        return;
	wxGISMap::AddLayer(pLayer);
    //set spatial reference for display transformation
	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
    if(m_pSpatialReference)
    {
	    if(pDisplayTransformation->GetSpatialReference() == NULL)
            pDisplayTransformation->SetSpatialReference(m_pSpatialReference);
        pLayer->SetSpatialReference(m_pSpatialReference);
    }

	const OGREnvelope* pEnv = pLayer->GetEnvelope();
	if(pEnv == NULL)
		return;
    OGREnvelope Env = *pEnv;

    //increase 10%
    double fDeltaX = (pEnv->MaxX - pEnv->MinX) / 20;
    double fDeltaY = (pEnv->MaxY - pEnv->MinY) / 20;
    double fDelta = std::max(fDeltaX, fDeltaY);
    Env.MaxX += fDelta;
    Env.MinX -= fDelta;
    Env.MaxY += fDelta;
    Env.MinY -= fDelta;

	if(!pDisplayTransformation->IsBoundsSet())
    {
		pDisplayTransformation->SetBounds(Env);
    }
	else
	{
		OGREnvelope Bounds = pDisplayTransformation->GetBounds();
		Bounds.Merge(Env);
		pDisplayTransformation->SetBounds(Bounds);
	}

	//caching
	if(pLayer->GetCached())
	{
		pLayer->SetCacheID(pGISScreenDisplay->AddCache());
	}
	else
	{
		if(m_Layers.size() > 0 && m_Layers[m_Layers.size() - 1]->GetCached())
			pLayer->SetCacheID(pGISScreenDisplay->AddCache());
		else
			pLayer->SetCacheID(pGISScreenDisplay->GetLastCacheID());
	}
}

void wxGISMapView::ClearLayers(void)
{
	m_pTrackCancel->Cancel();
	if(m_pThread)
		m_pThread->Delete();

	//delete all cashes
	pGISScreenDisplay->ClearCaches();
	//reset spatial reference
	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	pDisplayTransformation->Reset();
	//reset views stack
	m_pExtenStack->Reset();

	wxGISMap::ClearLayers();
}

void wxGISMapView::OnKeyDown(wxKeyEvent & event)
{
	event.Skip();
    if(event.GetKeyCode() == WXK_SHIFT || event.GetKeyCode() == WXK_ALT || event.GetKeyCode() == WXK_CONTROL)
        return;
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pTrackCancel->Cancel();
		break;
	default:
		break;
	}
}

void wxGISMapView::SetTrackCancel(ITrackCancel* pTrackCancel)
{
	if(pTrackCancel == NULL)
		return;

	if(m_pTrackCancel)
	{
		m_pTrackCancel->Cancel();
		//wait while the thread is exited;
		wxSleep(1);
		delete m_pTrackCancel;
	}
	m_pTrackCancel = pTrackCancel;
}

void wxGISMapView::OnThreadExit(void)
{
	m_pThread = NULL;
	if(m_pAni)
	{
		m_pAni->Stop();
		m_pAni->Show(false);
	}
	Refresh(false);
}

void wxGISMapView::OnMouseWheel(wxMouseEvent& event)
{
	//event.Skip(false);

	m_pTrackCancel->Cancel();
	if(m_pThread)
		m_pThread->Delete();

	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	if(pDisplayTransformation)
	{
		if(!(m_MouseState & enumGISMouseWheel))
		{
			m_virtualrc = GetClientRect();
			m_virtualbounds = pDisplayTransformation->GetBounds();

			m_MouseState |= enumGISMouseWheel;
		}
		int direction = event.GetWheelRotation();
		int delta = event.GetWheelDelta();
		int factor = direction / delta;

		double world_zoom = ZOOM_FACTOR * (double)factor;

        double sc = pDisplayTransformation->GetScaleRatio();
        if(sc <= 1 && world_zoom > 0)
			return;

		//calc zoom dc
		int dx(0), dy(0);
		if(factor < 0)
		{
			dx = ((double)m_virtualrc.width * (world_zoom / (world_zoom - 1))) / -2;
			dy = ((double)m_virtualrc.height * (world_zoom / (world_zoom - 1))) / -2;
		}
		else
		{
			//dx = (double)m_virtualrc.width * (world_zoom) / (-2 * (1 + world_zoom));
			//dy = (double)m_virtualrc.height * (world_zoom) / (-2 * (1 + world_zoom));
			dx = ((double)m_virtualrc.width * (world_zoom)) / 2;
			dy = ((double)m_virtualrc.height * (world_zoom)) / 2;
		}

		m_virtualrc.Inflate(dx, dy);
		if(m_virtualrc.width <= 10 || m_virtualrc.height <= 10)
			return;

		m_virtualrc.CenterIn(GetClientRect());
		wxCoord x = m_virtualrc.x;
		wxCoord y = m_virtualrc.y;

		//calc zoom world
		double dwx(0), dwy(0);

		if(factor < 0)
		{
			dwx = (m_virtualbounds.MaxX - m_virtualbounds.MinX) * (world_zoom) / 2;
			dwy = (m_virtualbounds.MaxY - m_virtualbounds.MinY) * (world_zoom) / 2;
		}
		else
		{
			dwx = (m_virtualbounds.MaxX - m_virtualbounds.MinX) * (world_zoom) / (2 * (1 + world_zoom));
			dwy = (m_virtualbounds.MaxY - m_virtualbounds.MinY) * (world_zoom) / (2 * (1 + world_zoom));
		}

		m_virtualbounds.MaxX -= dwx;
		m_virtualbounds.MinX += dwx;
		m_virtualbounds.MaxY -= dwy;
		m_virtualbounds.MinY += dwy;


		wxClientDC CDC(this);

		if(!m_virtualrc.Contains(GetClientRect()))//(factor < 0)
			pGISScreenDisplay->OnStretchDraw(CDC, m_virtualrc.width, m_virtualrc.height, x, y, true, enumGISQualityBicubic/*enumGISQualityBilinear*/);
		else
		{
			wxRect client_rc = GetClientRect();
			wxRect rc = client_rc;
			rc.width = rc.width * rc.width / m_virtualrc.width;
			rc.height = rc.height * rc.height / m_virtualrc.height;
			rc.x = client_rc.x + (client_rc.width - rc.width) / 2;
			rc.y = client_rc.y + (client_rc.height - rc.height) / 2;
			pGISScreenDisplay->OnStretchDraw2(CDC, rc, false, enumGISQualityHalfBilinear);////enumGISQualityHalfQuadBilinear//enumGISQualityNearest//enumGISQualityFourQuadBilinear
		}

		pDisplayTransformation->SetBounds(m_virtualbounds);
		//draw scale text
		sc = pDisplayTransformation->GetScaleRatio();
		wxString format_s = NumberScale(sc);
		format_s.Prepend(wxT("1 : "));
		//wxString s = wxString::Format(_("1 : %.2f"), sc);
		//1 / (m_virtualbounds.MaxX - m_virtualbounds.MinX)/*pDisplayTransformation->GetScaleRatio()*/ * 243.84);
		wxSize size = GetClientSize();
		int width, height;

		CDC.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
		CDC.GetTextExtent(format_s, &width, &height);

		wxBrush br(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
		CDC.SetBrush(br);
		wxPen pen(*wxBLACK_PEN);
		CDC.SetPen(pen);
		int x1 = (size.x - width) / 2, y1 = (size.y - height) - 50/*/ 2*/;
		CDC.DrawRectangle( x1 - 5, y1 - 2, width + 10, height + 4);

		wxColor TextColor(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
		CDC.SetTextBackground(TextColor);
		CDC.SetTextForeground(TextColor);

		CDC.DrawText(format_s, x1, y1);

		m_timer.Start(WAITTIME);

	}
}

void wxGISMapView::OnCaptureLost(wxMouseCaptureLostEvent & event)
{
    ReleaseMouse();
}

void wxGISMapView::OnTimer( wxTimerEvent& event )
{
	wxMouseState state = wxGetMouseState();
	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();

	if(!state.LeftDown() && (m_MouseState & enumGISMouseLeftDown))
	{
		m_MouseState &= ~enumGISMouseLeftDown;
		//set map init envelope
		pDisplayTransformation->SetDeviceFrame(GetClientRect());
	}
	else if(m_MouseState & enumGISMouseWheel)
	{
		m_MouseState &= ~enumGISMouseWheel;
		//set new map envelope
		//OGREnvelope virtualbounds;// = pDisplayTransformation->GetBounds();
		//wxRect rc = GetClientRect();
		//double zoomfactor = MIN((double)m_virtualrc.width / rc.width, (double)m_virtualrc.height / rc.height);
		//if(m_zoomfactor < 0)
		//	m_zoomfactor = 2 / fabs(m_zoomfactor);
		//double delta_x = (virtualbounds.MaxX - virtualbounds.MinX) /*/ 2*/ * m_zoomfactor;
		//double delta_y = (virtualbounds.MaxY - virtualbounds.MinY) /*/ 2*/ * m_zoomfactor;
		//virtualbounds.MinX += delta_x;
		//virtualbounds.MaxX -= delta_x;
		//virtualbounds.MinY += delta_y;
		//virtualbounds.MaxY -= delta_y;

		//m_virtualrc2.CenterIn(GetClientRect());
		//wxPoint points[2];
		//points[0].x = m_virtualrc2.GetLeft();
		//points[0].y = m_virtualrc2.GetTop();
		//points[1].x = m_virtualrc2.GetRight();
		//points[1].y = m_virtualrc2.GetBottom();
		//OGRRawPoint* pPoints = pDisplayTransformation->TransformCoordDC2World(points, 2);
		//virtualbounds.MinX = pPoints[0].x;
		//virtualbounds.MinY = pPoints[1].y;
		//virtualbounds.MaxX = pPoints[1].x;
		//virtualbounds.MaxY = pPoints[0].y;
		//delete [] pPoints;
//		pDisplayTransformation->SetBounds(m_virtualbounds);

		m_timer.Stop();
		m_pExtenStack->Do(m_virtualbounds);
		return;
	}
	else
		return;

	pGISScreenDisplay->SetDerty(true);
	m_timer.Stop();

#if __WXMSW__
	Refresh(false);
#else
	Update();
#endif
}

void wxGISMapView::SetFullExtent(void)
{
	SetExtent(GetFullExtent());
}

void wxGISMapView::SetExtent(OGREnvelope Env)
{
	m_pExtenStack->Do(Env);
}

void wxGISMapView::PanStart(wxPoint MouseLocation)
{
	if(m_timer.IsRunning())
	{
		wxTimerEvent ev;
		OnTimer( ev );
#ifdef WAITTIME
		wxMilliSleep(WAITTIME);
#endif
		m_pTrackCancel->Cancel();
		if(m_pThread)
			m_pThread->Delete();
	}
	m_StartMouseLocation = MouseLocation;
	m_MapToolState |= enumGISMapPanning;
	CaptureMouse();//events ???
}

void wxGISMapView::PanMoveTo(wxPoint MouseLocation)
{

	if(m_MapToolState & enumGISMapPanning)
	{
		wxCoord x =  m_StartMouseLocation.x - MouseLocation.x;
		wxCoord y =  m_StartMouseLocation.y - MouseLocation.y;
		wxClientDC CDC(this);
		pGISScreenDisplay->OnPanDraw(CDC, x, y);
	}
}

void wxGISMapView::PanStop(wxPoint MouseLocation)
{
    ReleaseMouse();
	if(m_MapToolState & enumGISMapPanning)
	{
		m_MapToolState &= ~enumGISMapPanning;
		wxCoord x =  m_StartMouseLocation.x - MouseLocation.x;
		wxCoord y =  m_StartMouseLocation.y - MouseLocation.y;
		//calc new Envelope
		IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
		wxRect rect = pDisplayTransformation->GetDeviceFrame();
		rect.Offset(x, y);
		//rect.SetX(rect.GetX() - x);
		//rect.SetY(rect.GetY() - y);

		if(m_pThread)
			m_pThread->Wait();

		wxClientDC CDC(this);
        pGISScreenDisplay->OnPanStop(CDC);

		OGREnvelope Env = pDisplayTransformation->TransformRect(rect);
		m_pExtenStack->Do(Env);
        //m_pExtenStack->SetExtent(Env);
//		pDisplayTransformation->SetBounds(Env);
//		pGISScreenDisplay->SetDerty(true);
//		Refresh(false);
    }
}

void wxGISMapView::SetSpatialReference(OGRSpatialReference* pSpatialReference)
{
	if(NULL == pSpatialReference)
		return;

    wxGISMap::SetSpatialReference(pSpatialReference);

	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	pDisplayTransformation->SetSpatialReference(pSpatialReference);
    pDisplayTransformation->SetBounds(GetFullExtent());
	pGISScreenDisplay->SetDerty(true);

	Refresh(false);
}




