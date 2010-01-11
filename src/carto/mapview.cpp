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

#define WAITTIME 650
#define wxUSE_GRAPHICS_CONTEXT 0

//-----------------------------------------------
//wxDrawingThread
//-----------------------------------------------

wxDrawingThread::wxDrawingThread(wxGISMapView* pView, std::vector<wxGISLayer*>& Layers) : wxThread(), m_pView(pView), m_Layers(Layers)
{
    m_pTrackCancel = m_pView->GetTrackCancel();
	m_pGISScreenDisplay = m_pView->GetCachedDisplay();

	if(m_pTrackCancel)
		m_pTrackCancel->Reset();
}

void *wxDrawingThread::Entry()
{
	wxClientDC CDC(m_pView);

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
				m_pGISScreenDisplay->SetCacheDerty(CacheIDCurrent, false);
			}

			if(m_pTrackCancel && !m_pTrackCancel->Continue())
				break;
			m_Layers[i]->Draw(wxGISDPGeography, m_pGISScreenDisplay, m_pTrackCancel);
		}
	}
	m_pGISScreenDisplay->SetDerty(false);
	m_pGISScreenDisplay->OnDraw(CDC);

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

BEGIN_EVENT_TABLE(wxGISMapView, wxScrolledWindow)
	EVT_ERASE_BACKGROUND(wxGISMapView::OnEraseBackground)
	EVT_SIZE(wxGISMapView::OnSize)
	EVT_MOUSEWHEEL(wxGISMapView::OnMouseWheel)
	EVT_KEY_DOWN(wxGISMapView::OnKeyDown)
	EVT_TIMER( TIMER_ID, wxGISMapView::OnTimer )
END_EVENT_TABLE()

wxGISMapView::wxGISMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxScrolledWindow(parent, id, pos, size, style | wxHSCROLL | wxVSCROLL )/*| wxSTATIC_BORDER | wxBORDER_NONEwxBORDER_SUNKEN*/, wxGISMap(), m_pTrackCancel(NULL), m_pThread(NULL), m_pAni(NULL), m_timer(this, TIMER_ID)
{
	//set map init envelope

#if wxUSE_GRAPHICS_CONTEXT	
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
	pDisplayTransformation->SetPPI(CDC.GetPPI());

	m_MouseState = enumGISMouseNone;
	m_MapToolState = enumGISMapNone;
}

wxGISMapView::~wxGISMapView(void)
{
	wxDELETE(pGISScreenDisplay);
	wxDELETE(m_pExtenStack);
	wxDELETE(m_pTrackCancel);
	if(m_pThread)
		m_pThread->Delete();
}

void wxGISMapView::OnDraw(wxDC& dc)
{
	//if(m_MapToolState & enumGISMapPanning)
	//	return;

	//ITrackCancel* pTrackCancel(NULL);
	//if map has it's own cache check it and draw from cache if posible
	//else redraw diry caches and out to dc
	//wxClientDC CDC(this);

	if(m_pExtenStack->GetSize() == 0)
	{		
		IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
		m_pExtenStack->Do(pDisplayTransformation->GetBounds());
	}

	if(m_pTrackCancel && !m_pAni)
		m_pAni = static_cast<wxGISAnimation*>(m_pTrackCancel->GetProgressor());//static_cast<wxGISAnimation*>(m_pTrackCancel->GetProgressor());


	if(pGISScreenDisplay->IsDerty())
	{
		wxCriticalSectionLocker locker(m_CriticalSection);
		if(m_pAni)
		{
			m_pAni->Show(true);
			m_pAni->Play();
		}

		if(m_pThread)
			m_pThread->Delete();
			
		//start draw thread
		m_pThread = new wxDrawingThread(this, m_Layers);
		if ( !m_pThread || m_pThread->Create() != wxTHREAD_NO_ERROR )
		{
			wxLogError(wxString(_("wxGISMapView: Can't create wxDrawingThread!")));
			return;
		}
		if( !m_pThread || m_pThread->Run() != wxTHREAD_NO_ERROR )
		{
			wxLogError(wxString(_("wxGISMapView: Can't run wxDrawingThread!")));
			return;
		}

		wxScrolledWindow::SetFocus();

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
	Refresh(false);

	event.Skip();
}

void wxGISMapView::OnEraseBackground(wxEraseEvent & event)
{
}

void wxGISMapView::AddLayer(wxGISLayer* pLayer)
{
	wxGISMap::AddLayer(pLayer);
    //set spa ref for display transformation
	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	if(pDisplayTransformation->GetSpatialReference() == NULL)
	{
        pDisplayTransformation->SetSpatialReference(m_pSpatialReference);
	}
	
	OGREnvelope* pEnv = pLayer->GetEnvelope();
	if(pEnv == NULL)
		return;
     
    OGREnvelope Env;
    Env.MaxX = pEnv->MaxX;
    Env.MaxY = pEnv->MaxY;
    Env.MinX = pEnv->MinX;
    Env.MinY = pEnv->MinY;

	OGRSpatialReference* pSpaRef = pLayer->GetSpatialReference();
	if(pSpaRef && m_pSpatialReference)
	{
		if(!m_pSpatialReference->IsSame(pSpaRef))
		{
			OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pSpaRef, m_pSpatialReference );
			poCT->Transform(1, &Env.MaxX, &Env.MaxY);
			poCT->Transform(1, &Env.MinX, &Env.MinY);
            OCTDestroyCoordinateTransformation(poCT);
		}
	}

    //increase 10%
    double fDeltaX = (Env.MaxX - Env.MinX) / 20;
    double fDeltaY = (Env.MaxY - Env.MinY) / 20;
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
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pTrackCancel->Cancel();
		break;
	default:
		break;
	}
	event.Skip();
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
		m_pAni->Hide();
	}
}

void wxGISMapView::OnMouseWheel(wxMouseEvent& event)
{
	event.Skip();

	m_pTrackCancel->Cancel();
	if(m_pThread)
		m_pThread->Delete();

	wxClientDC CDC(this);

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
		double sc = pDisplayTransformation->GetScaleRatio();
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
		CDC.SetPen(*wxBLACK_PEN);
		int x1 = (size.x - width) / 2, y1 = (size.y - height) - 50/*/ 2*/;
		CDC.DrawRectangle( x1 - 5, y1 - 2, width + 10, height + 4);
		CDC.DrawText(format_s, x1, y1);

		//wxRect client_rc = GetScreenRect();
		//m_pTipWnd = new wxTipWindow(this, wxString::Format(_("1 : %.2f"), 1 / (m_virtualbounds.MaxX - m_virtualbounds.MinX)/*pDisplayTransformation->GetScaleRatio()*/ * 243.84), 200, &m_pTipWnd, &client_rc);
		//m_pTipWnd->Hide();		
		//wxPoint pt = client_rc.GetPosition();
		//pt.x += client_rc.width / 2 - m_pTipWnd->GetSize().GetWidth() * 1.5 ;
		//pt.y += client_rc.height / 2 - m_pTipWnd->GetSize().GetHeight() * 1.5;
		//m_pTipWnd->Position(pt,m_pTipWnd->GetSize());
		//m_pTipWnd->Show();
		//if(m_pTipWnd)
		//{
		//	wxDELETE(m_pTipWnd);
		//	m_pTipWnd = NULL;
		//}
		//, m_pTipWnd(NULL)

		m_timer.Start(WAITTIME);
	}
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
	Refresh(false);
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
	if(m_MapToolState & enumGISMapPanning)
	{
		m_MapToolState &= ~enumGISMapPanning;
		ReleaseMouse();
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

void wxGISMapView::SetSpatialReference(OGRSpatialReference* pSpatialReference, bool bShouldDeleteSP)
{
	if(pSpatialReference == NULL)
		return;

    wxGISMap::SetSpatialReference(pSpatialReference, bShouldDeleteSP);

	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
    pDisplayTransformation->SetBounds(GetFullExtent());
	pDisplayTransformation->SetSpatialReference(m_pSpatialReference);
	pGISScreenDisplay->SetDerty(true);
	Refresh(false);
}



