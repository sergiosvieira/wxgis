/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMapView class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Bishop
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
#include "wxgis/cartoui/mapview.h"
#include "wxgis/datasource/featuredataset.h"
//#include "wxgis/datasource/vectorop.h"
#include "wxgis/core/format.h"
#include "wxgis/core/config.h"
#include "wxgis/core/app.h"
#include "wxgis/display/displayop.h"

//#include <wx/sysopt.h>
//#include <wx/thread.h>

#define TM_REFRESH 3700
#define TM_ZOOMING 250
#define TM_WHEELING 300
#define UNITS_IN_INCH 2.54
#define DEFAULT_FLASH_PERIOD 400

//#ifdef __WXGTK__
////#define THREAD_DRAW_BUG
//#endif

//-----------------------------------------------
// wxGISMapView
//-----------------------------------------------

IMPLEMENT_CLASS(wxGISMapView, wxWindow)

BEGIN_EVENT_TABLE(wxGISMapView, wxWindow)
    EVT_PAINT(wxGISMapView::OnPaint)
	EVT_ERASE_BACKGROUND(wxGISMapView::OnEraseBackground)
	EVT_SIZE(wxGISMapView::OnSize)
	EVT_MOUSEWHEEL(wxGISMapView::OnMouseWheel)
	EVT_TIMER( TIMER_ID, wxGISMapView::OnTimer )
	EVT_KEY_DOWN(wxGISMapView::OnKeyDown)
	EVT_MOUSE_CAPTURE_LOST(wxGISMapView::OnCaptureLost)
    EVT_MXMAP_DRAWING_START(wxGISMapView::OnMapDrawing)
    EVT_MXMAP_DRAWING_STOP(wxGISMapView::OnMapDrawing)
    //EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_ZOOMING, wxGISMapView::OnZooming)
END_EVENT_TABLE()


wxGISMapView::wxGISMapView(void) : wxGISExtentStack()
{
	m_pGISDisplay = NULL;
	m_pTrackCancel = NULL;
	m_pAni = NULL;
	m_nDrawingState = enumGISMapNone;
	m_nFactor = 0;
	m_dCurrentAngle = 0;
}

wxGISMapView::wxGISMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxGISExtentStack()
{
	m_pGISDisplay = NULL;
	m_pTrackCancel = NULL;
	m_pAni = NULL;
	m_nDrawingState = enumGISMapNone;
	m_nFactor = 0;
	m_dCurrentAngle = 0;
	//
    Create(parent, id, pos, size, style);
}

wxGISMapView::~wxGISMapView(void)
{
    wxDELETE(m_pGISDisplay);
}

bool wxGISMapView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxWindow::Create(parent, id, pos, size, style, name ))
		return false;
	m_pGISDisplay = new wxGISDisplay();

	UpdateFrameCenter();

    m_timer.SetOwner(this, TIMER_ID);

    //SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	return true;
}

void wxGISMapView::OnPaint(wxPaintEvent & event)
{
    //event.Skip();
    wxPaintDC paint_dc(this);
    wxRect rect = GetClientRect();

    if(rect.width == 0 || rect.height == 0)
    {
        return;
    }

	if(m_nDrawingState == enumGISMapZooming || m_nDrawingState == enumGISMapPanning || m_nDrawingState ==  enumGISMapRotating || m_nDrawingState ==  enumGISMapWheeling) //if operation - exit immediatly
	{
		return;
	}

	if(m_pGISDisplay)
	{
	    if(!GetThread() && m_pGISDisplay->IsDerty())//m_nDrawingState = enumGISMapDrawing
        {
            return;
        }
		else
		{
			//draw contents of m_pGISDisplay
            m_pGISDisplay->Output(&paint_dc, m_ClipGeometry);

		}
	}
}
/*
void wxGISMapView::OnStartDrawingThread( wxCommandEvent & event )
{
    wxCriticalSectionLocker locker(m_CritSect);
#ifdef __WXGTK__
    wxWakeUpIdle();
#endif
    if(m_pMapDrawingThread || !m_pGISDisplay->IsDerty())
        return;
//    CancelDrawThread();
//    #ifdef THREAD_DRAW_BUG
//    OnDraw(wxGISDPGeography);
//    wxPaintDC paint_dc(this);
//    m_pGISDisplay->Output(&paint_dc, m_ClipGeometry);
//    #else
    // 1. start drawing thread
    m_pMapDrawingThread = new wxMapDrawingThread(this);
    if(!CreateAndRunThread( m_pMapDrawingThread, wxT("wxMapDrawingThread"), wxT("Map Drawing Thread"), WXTHREAD_MAX_PRIORITY ) )
    {
        //if(m_pMapDrawingThread)
        //	m_pMapDrawingThread->Delete();
        return;
    }

    // start refresh timer
    //if(!m_timer.IsRunning())
    m_timer.Start(TM_REFRESH);
//    #endif //THREAD_DRAW_BUG
 }
 */
void wxGISMapView::OnSize(wxSizeEvent & event)
{
    if(m_PrevSize == event.GetSize())
        return;

    //event.Skip(false);
    DestroyDrawThread();

    wxRect rc = GetClientRect();

    if(IsShownOnScreen())
    {
	    if(m_nDrawingState == enumGISMapZooming)
	    {
		    wxClientDC CDC(this);
		    if(m_pGISDisplay)
			    m_pGISDisplay->ZoomingDraw(GetClientRect(), &CDC);
	    }
	    else
	    {
		    //start zooming action
		    m_nDrawingState = enumGISMapZooming;
            if(m_pGISDisplay)
                m_pGISDisplay->SetDeviceFrame(rc);
			m_timer.Start(TM_ZOOMING);
        }
    }
    else
    {
        if(m_pGISDisplay)
            m_pGISDisplay->SetDeviceFrame(rc);
    }

	UpdateFrameCenter();
	//wxWakeUpIdle();
    m_PrevSize = event.GetSize();

}

//void wxGISMapView::OnZooming( wxCommandEvent & event )
//{
//    wxClientDC CDC(this);
//	if(m_pGISDisplay)
//		m_pGISDisplay->ZoomingDraw(GetClientRect(), &CDC);
//}

void wxGISMapView::OnEraseBackground(wxEraseEvent & event)
{
}

void wxGISMapView::OnTimer( wxTimerEvent& event )
{
    //event.Skip();
    //wxSafeYield(this, true);
#ifdef __WXGTK__
    wxWakeUpIdle();
#endif

    switch(m_nDrawingState)
    {
    case enumGISMapZooming:
    {
        //stop zooming action
		if(m_pGISDisplay && !wxGetMouseState().LeftIsDown()) //user release mouse button, so draw contents
		{
		    wxRect rc = GetClientRect();
        	m_pGISDisplay->SetDeviceFrame(rc);
			m_pGISDisplay->SetDerty(true);
			m_nDrawingState = enumGISMapDrawing;
            CreateAndRunDrawThread();
			//wxCommandEvent evt(wxEVT_COMMAND_STARTDRAWING);wxEVT_COMMAND_DATA_SENT
			//GetEventHandler()->ProcessEvent( evt );

//                if(!m_pMapDrawingThread)//m_nDrawingState = enumGISMapDrawing
//                {
//                    StartDraingThread();
//                }
            m_timer.Stop();	//Thaw();
		}
    }
    break;
    case enumGISMapWheeling:
		m_nDrawingState = enumGISMapWheelingStop;
		return;
    case enumGISMapWheelingStop:
		//stop wheeling action
		if(m_pGISDisplay)
		{
			double dZoom = 1;
			if(m_nFactor < 0)
				dZoom = fabs(1.0 / (m_nFactor - 1));
			else if(m_nFactor > 0)
				dZoom = 1 + m_nFactor;

			OGREnvelope Env = CreateEnvelopeFromZoomFactor(dZoom);
			if(Env.IsInit())//set new bounds
				Do(Env);
			m_nDrawingState = enumGISMapDrawing;
		}
		m_nFactor = 0;
		m_timer.Stop();
		return;
    break;
    case enumGISMapFlashing:
        m_nDrawingState = enumGISMapDrawing;
        m_pGISDisplay->SetDrawCache(m_pGISDisplay->GetLastCacheID(), true);
		m_timer.Stop();
    break;
    default:
    case enumGISMapNone:
		m_timer.Stop();
    break;
    };
	Refresh();
}

void wxGISMapView::OnKeyDown(wxKeyEvent & event)
{
	event.Skip();

	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		if(m_pTrackCancel)
			m_pTrackCancel->Cancel();
		break;
	default:
		break;
	}
}

void wxGISMapView::SetTrackCancel(ITrackCancel* pTrackCancel)
{
	if(m_pTrackCancel)
		return;
	m_pTrackCancel = pTrackCancel;
	m_pAni = m_pTrackCancel->GetProgressor();
	if(m_pAni)
    {
		m_pAni->SetYield();
    }
	m_pTrackCancel->Reset();
}
//
//void wxGISMapView::CancelDrawThread(void)
//{
//	//wxCriticalSectionLocker locker(m_CritSect);
//	if(m_pTrackCancel)
//	{
//		m_pTrackCancel->Cancel();
//	}
//	//wxMilliSleep(150);
//	if(m_pMapDrawingThread)
//	{
//		if(m_pMapDrawingThread->IsRunning())
//			m_pMapDrawingThread->Delete();
//		//if(!m_pMapDrawingThread->IsDetached())
//		//	m_pMapDrawingThread->Wait();
//		m_pMapDrawingThread = NULL;
//	}
//}

void wxGISMapView::OnDraw(wxGISEnumDrawPhase nPhase)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_pTrackCancel)
		m_pTrackCancel->Reset();
	for(size_t i = 0; i < m_paLayers.size(); ++i)
	{
		if(m_pTrackCancel && !m_pTrackCancel->Continue())
			break;
		wxGISLayer* pLayer = m_paLayers[i];
   		if(!pLayer)
			continue; //not layer

		if(!pLayer->GetVisible())
			continue; //not visible
		if(m_pGISDisplay && m_pGISDisplay->IsCacheDerty(pLayer->GetCacheID()))
		{
			//erase bk if layer no 0
			if(i == 0)
				m_pGISDisplay->OnEraseBackground();

			if(m_pGISDisplay->GetDrawCache() != pLayer->GetCacheID())
				m_pGISDisplay->SetDrawCache(pLayer->GetCacheID());
			if(pLayer->Draw(nPhase, m_pGISDisplay, m_pTrackCancel))
            {
                //SetCacheDerty if next cache is not same
                //if(i < m_paLayers.size() - 1 && m_paLayers[i + 1]->GetCacheID() != pLayer->GetCacheID())
                    m_pGISDisplay->SetCacheDerty(pLayer->GetCacheID(), false);
                //else if(i == m_paLayers.size() - 1)
                //    m_pGISDisplay->SetCacheDerty(pLayer->GetCacheID(), false);
            }
		}
	}
}

void wxGISMapView::SetSpatialReference(const wxGISSpatialReference &SpatialReference)
{
	//TODO: show dialog reprojections for different ellipsoids
	wxGISExtentStack::SetSpatialReference(SpatialReference);
}

void wxGISMapView::OnCaptureLost(wxMouseCaptureLostEvent & event)
{
	event.Skip();
	if( HasCapture() )
		ReleaseMouse();
}

bool wxGISMapView::AddLayer(wxGISLayer* pLayer)
{
    wxCHECK_MSG(pLayer, false, wxT("The layer pointer is NULL"));
	if(m_pGISDisplay)
	{
		//Create cache if needed
		if(pLayer->IsCacheNeeded())
			pLayer->SetCacheID(m_pGISDisplay->AddCache());
		else
			pLayer->SetCacheID(m_pGISDisplay->GetLastCacheID());
	}

	return wxGISExtentStack::AddLayer(pLayer);
}

void wxGISMapView::Clear(void)
{
	//Clear caches
	m_pGISDisplay->Clear();
	wxGISExtentStack::Clear();
}

void wxGISMapView::OnMouseWheel(wxMouseEvent& event)
{
	//event.Skip(false);
	//CancelDrawThread();
    DestroyDrawThread();

	int nDirection = event.GetWheelRotation();
	int nDelta = event.GetWheelDelta();
	int nFactor = nDirection / nDelta;
	m_nDrawingState = enumGISMapWheeling;

	m_nFactor += ZOOM_FACTOR * (double)nFactor;


	if(m_pGISDisplay)
	{
		double dZoom = 1;
		if(m_nFactor < 0)
			dZoom = fabs(1.0 / (m_nFactor - 1));
		else if(m_nFactor > 0)
			dZoom = 1 + m_nFactor;
        wxClientDC CDC(this);
		m_pGISDisplay->WheelingDraw(dZoom, &CDC);

		//draw scale text
		OGREnvelope Env = CreateEnvelopeFromZoomFactor(dZoom);
		if(Env.IsInit())
		{
			double sScale = GetScaleRatio( Env, CDC );
			wxString sFormatScale = NumberScale(sScale);
			sFormatScale.Prepend(wxT("1 : "));
			DrawToolTip(CDC, sFormatScale);
		}
	}

	//if(!m_timer.IsRunning())
		m_timer.Start(TM_WHEELING);
}

void wxGISMapView::DrawToolTip(wxClientDC& dc, const wxString& sText)
{
	wxSize size = GetClientSize();
	int nWidth, nHeight;

	dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	dc.GetTextExtent(sText, &nWidth, &nHeight);

	wxBrush br(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	dc.SetBrush(br);
	wxColor TextColor(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));

	wxPen pen(TextColor);
	dc.SetPen(pen);
	int x1 = (size.x - nWidth) / 2, y1 = (size.y - nHeight) - 50;
	dc.DrawRectangle( x1 - 5, y1 - 2, nWidth + 10, nHeight + 4);

	dc.SetTextBackground(TextColor);
	dc.SetTextForeground(TextColor);

	dc.DrawText(sText, x1, y1);
}

double wxGISMapView::GetScaleRatio(OGREnvelope& Bounds, wxDC& dc)
{
	wxCHECK(m_pGISDisplay, 0);

	wxRect rc = m_pGISDisplay->GetDeviceFrame();
	wxSize ppi = dc.GetPPI();
	double screen_w = (double)rc.GetWidth() / ppi.GetWidth() * UNITS_IN_INCH;
	double screen_h = (double)rc.GetHeight() / ppi.GetHeight() * UNITS_IN_INCH;
	double w_w = fabs(Bounds.MaxX - Bounds.MinX);
	double w_h = fabs(Bounds.MaxY - Bounds.MinY);

	if(m_SpatialReference && m_SpatialReference->IsGeographic())
	{
		w_w = w_w * PIDEG * m_SpatialReference->GetSemiMajor();
		w_h = w_h * PIDEG * m_SpatialReference->GetSemiMinor();
	}

	double screen = std::min(screen_w, screen_h);
	double world = std::min(w_w, w_h);

	return (world * 100) / screen;
}

OGREnvelope wxGISMapView::CreateEnvelopeFromZoomFactor(double dZoom)
{
	OGREnvelope Env;
	wxCHECK(m_pGISDisplay, Env);
	//calc new virtual frame
	wxRect rc = m_pGISDisplay->GetDeviceFrame();
	double dUL_X, dUL_Y, dLR_X, dLR_Y;
	double dCenterX = double(rc.GetWidth()) / 2;
	double dCenterY = double(rc.GetHeight()) / 2;
	double dDCXDelta = dCenterX / dZoom;
	double dDCYDelta = dCenterY / dZoom;
	dUL_X = dCenterX - dDCXDelta;
	dUL_Y = dCenterY - dDCYDelta;
	dLR_X = dCenterX + dDCXDelta;
	dLR_Y = dCenterY + dDCYDelta;

	//get bounds for virtual frame
	wxRect new_rc(wxPoint(dUL_X, dUL_Y), wxPoint(dLR_X, dLR_Y));
	Env = m_pGISDisplay->TransformRect(new_rc);

	return Env;
}

void wxGISMapView::SetExtent(const OGREnvelope& Env)
{
	//CancelDrawThread();
    DestroyDrawThread();
	wxGISExtentStack::SetExtent(Env);

	if(m_pGISDisplay)
		m_pGISDisplay->SetBounds(Env);

	//wxGISMapView::Refresh();
//    if(!m_pMapDrawingThread && m_pGISDisplay->IsDerty())//m_nDrawingState = enumGISMapDrawing
//    {
//        StartDraingThread();
//    }
    //wxCommandEvent evt(wxEVT_COMMAND_STARTDRAWING);
    //GetEventHandler()->ProcessEvent( evt );
    CreateAndRunDrawThread();
}

void wxGISMapView::SetFullExtent(void)
{
	Do(GetFullExtent());
}

void wxGISMapView::PanStart(wxPoint MouseLocation)
{
	//CancelDrawThread();
    DestroyDrawThread();
	m_StartMouseLocation = MouseLocation;
    m_nDrawingState = enumGISMapPanning;
	CaptureMouse();
}

void wxGISMapView::PanMoveTo(wxPoint MouseLocation)
{
	if(m_nDrawingState == enumGISMapPanning)
	{
		wxCoord x =  m_StartMouseLocation.x - MouseLocation.x;
		wxCoord y =  m_StartMouseLocation.y - MouseLocation.y;
		wxClientDC CDC(this);
		if(m_pGISDisplay)
			m_pGISDisplay->PanningDraw(x, y, &CDC);
	}
}

void wxGISMapView::PanStop(wxPoint MouseLocation)
{
    ReleaseMouse();

	wxCoord x =  m_StartMouseLocation.x - MouseLocation.x;
	wxCoord y =  m_StartMouseLocation.y - MouseLocation.y;

	wxRect rect = m_pGISDisplay->GetDeviceFrame();//GetClientRect();
	if(x == 0 && y == 0)
	{
		x = m_StartMouseLocation.x - (rect.GetLeft() + rect.GetWidth() / 2);
		y = m_StartMouseLocation.y - (rect.GetTop() + rect.GetHeight() / 2);
	}
	rect.Offset(x, y);

	if(m_pGISDisplay)
	{
		OGREnvelope Env = m_pGISDisplay->TransformRect(rect);

		//set clip geom
		//FillClipGeometry(GetClientRect(), x, y);

		if(Env.IsInit())//set new bounds
			Do(Env);
		m_nDrawingState = enumGISMapDrawing;
	}
}

//void wxGISMapView::FillClipGeometry(wxRect rect, wxCoord x, wxCoord y)
//{
//	m_ClipGeometry.Clear();
//
//	//if(x < 0 && y > 0)
//	//{//3
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetTop() - y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft() + x, rect.GetTop() - y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft() + x, rect.GetBottom()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetBottom()));
//	//}
//	//else if(x < 0 && y < 0)
//	//{//4
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft() + x, rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft() + x, rect.GetBottom() - y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight() + x, rect.GetBottom() - y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetBottom()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetBottom()));
//	//}
//	//else if(x > 0 && y > 0)
//	//{//1
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetBottom()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight() + x, rect.GetBottom()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight() + x, rect.GetTop() - y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetTop() - y));
//	//}
//	//else if(x > 0 && y < 0)
//	//{//2
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetBottom() + y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight() + x, rect.GetBottom() + y));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight() + x, rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetTop()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetRight(), rect.GetBottom()));
//	//	m_ClipGeometry.Add(wxRealPoint(	rect.GetLeft(), rect.GetBottom()));
//	//}
//}

void wxGISMapView::RotateStart(wxPoint MouseLocation)
{
	m_StartMouseLocation = MouseLocation;
	//compute angle
	double dX = m_FrameCenter.x - MouseLocation.x;
	double dY = m_FrameCenter.y - MouseLocation.y;
	m_dOriginAngle = atan2(dY, dX);

    m_nDrawingState = enumGISMapRotating;
	CaptureMouse();
}

void wxGISMapView::RotateBy(wxPoint MouseLocation)
{
	if(m_nDrawingState == enumGISMapRotating)
	{
		if(m_pGISDisplay)
		{
			//compute angle
			double dX = m_FrameCenter.x - MouseLocation.x;
			double dY = m_FrameCenter.y - MouseLocation.y;
			double dAngle = atan2(dY, dX);
			dAngle -= m_dOriginAngle;
			if(dAngle < 0)
				dAngle += 2 * M_PI;

			wxClientDC CDC(this);
			m_pGISDisplay->RotatingDraw(dAngle, &CDC);

			m_dCurrentAngle = DOUBLEPI - m_pGISDisplay->GetRotate() - dAngle;
			if(m_dCurrentAngle >= DOUBLEPI)
				m_dCurrentAngle -= DOUBLEPI;
			if(m_dCurrentAngle < 0)
				m_dCurrentAngle += DOUBLEPI;
			DrawToolTip(CDC, wxString::Format(_("%.4f degree"), m_dCurrentAngle * DEGPI));
		}
	}
}

void wxGISMapView::RotateStop(wxPoint MouseLocation)
{
    ReleaseMouse();
	//compute angle
	double dX = m_FrameCenter.x - MouseLocation.x;
	double dY = m_FrameCenter.y - MouseLocation.y;
	double dAngle = atan2(dY, dX);
	dAngle -= m_dOriginAngle;
	double dPrevRotate = m_pGISDisplay->GetRotate();
	double dfRotate = dPrevRotate + dAngle;

	//m_dCurrentAngle = DOUBLEPI - m_pGISDisplay->GetRotate() - dAngle;
	//if(m_dCurrentAngle > DOUBLEPI)
	//	m_dCurrentAngle -= DOUBLEPI;
	//if(m_dCurrentAngle < 0)
	//	m_dCurrentAngle += DOUBLEPI;

	if(dfRotate >= DOUBLEPI)
		dfRotate -= DOUBLEPI;
	if(dfRotate < 0)
		dfRotate += DOUBLEPI;
	m_nDrawingState = enumGISMapDrawing;

	SetRotate(dfRotate);//(dPrevRotate + dAngle);
}

void wxGISMapView::SetRotate(double dAngleRad)
{
	if(m_pGISDisplay)
		m_pGISDisplay->SetRotate(dAngleRad);
	m_dCurrentAngle = DOUBLEPI - dAngleRad;
	if(m_dCurrentAngle >= DOUBLEPI)
		m_dCurrentAngle -= DOUBLEPI;
	if(m_dCurrentAngle < 0)
		m_dCurrentAngle += DOUBLEPI;

	wxMxMapViewEvent event(wxMXMAP_ROTATED, m_dCurrentAngle);
	PostEvent(event);

	//wxGISMapView::Refresh();
//    if(!m_pMapDrawingThread && m_pGISDisplay->IsDerty())//m_nDrawingState = enumGISMapDrawing
//    {
//        StartDraingThread();
//    }
    ////wxCommandEvent evt(wxEVT_COMMAND_STARTDRAWING);
    ////GetEventHandler()->ProcessEvent( evt );
    CreateAndRunDrawThread();
}

double wxGISMapView::GetCurrentRotate(void)
{
	return m_dCurrentAngle;
}

void wxGISMapView::UpdateFrameCenter(void)
{
	wxRect rc = GetClientRect();
	m_FrameCenter.x = rc.x + rc.width / 2;
	m_FrameCenter.y = rc.y + rc.height / 2;
}


OGREnvelope wxGISMapView::GetFullExtent(void)
{
	OGREnvelope OutputEnv;
	if(!IsDoubleEquil(m_dCurrentAngle, 0.0))
	{
		OutputEnv = m_FullExtent;
		double dCenterX = m_FullExtent.MinX + (m_FullExtent.MaxX - m_FullExtent.MinX) / 2;
		double dCenterY = m_FullExtent.MinY + (m_FullExtent.MaxY - m_FullExtent.MinY) / 2;
		RotateEnvelope(OutputEnv, m_dCurrentAngle, dCenterX, dCenterY);
		IncreaseEnvelope(OutputEnv, 0.1);
	}
	else
		OutputEnv = wxGISMap::GetFullExtent();
	return OutputEnv;
}
/*
void wxGISMapView::FlashGeometry(const GeometryArray& Geoms)
{
	CancelDrawThread();

    //draw geometries
    m_pGISDisplay->SetDrawCache(m_pGISDisplay->GetFlashCacheID());
    //set colors and etc.
    wxGISAppConfig oConfig = GetConfig();
    //create vector flash renderer
    RGBA stFillColour, stLineColour;
	stLineColour.dRed = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_line_red")), 0)) / 255;
	stLineColour.dGreen = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_line_green")), 210)) / 255;
	stLineColour.dBlue = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_line_blue")), 255)) / 255;
	stLineColour.dAlpha = 1.0;

    stFillColour.dRed = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_fill_red")), 0)) / 255;
	stFillColour.dGreen = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_fill_green")), 255)) / 255;
	stFillColour.dBlue = double(oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_fill_blue")), 255)) / 255;
	stFillColour.dAlpha = 1.0;

    for(size_t i = 0; i < Geoms.GetCount(); ++i)
    {
		OGRGeometry* pGeom = Geoms[i];
		OGRwkbGeometryType type = wkbFlatten(pGeom->getGeometryType());
		switch(type)
		{
		case wkbPoint:
		case wkbMultiPoint:
	        m_pGISDisplay->SetPointColor(stLineColour);
	        m_pGISDisplay->SetLineColor(stLineColour);
	        m_pGISDisplay->SetFillColor(stFillColour);
			m_pGISDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
			m_pGISDisplay->SetLineWidth(1.0);
			m_pGISDisplay->SetPointRadius(2.0);
			break;
		case wkbLineString:
		case wkbLinearRing:
		case wkbMultiLineString:
	        m_pGISDisplay->SetPointColor(stFillColour);
	        m_pGISDisplay->SetLineColor(stFillColour);
	        m_pGISDisplay->SetFillColor(stFillColour);
			m_pGISDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
			m_pGISDisplay->SetLineWidth(2.0);
			break;
		case wkbMultiPolygon:
		case wkbPolygon:
	        m_pGISDisplay->SetLineColor(stLineColour);
	        m_pGISDisplay->SetFillColor(stFillColour);
			m_pGISDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
			m_pGISDisplay->SetLineWidth(1.0);
			break;
		case wkbGeometryCollection:
			break;
		default:
		case wkbUnknown:
		case wkbNone:
			break;
		}

		m_pGISDisplay->DrawGeometry(pGeom);
	}

	wxGISMapView::Refresh();
    //clean flash layer
    int nMilliSec = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/map/flash_time")), DEFAULT_FLASH_PERIOD);
    m_nDrawingState = enumGISMapFlashing;
    m_timer.Start(nMilliSec);
}
*/
void wxGISMapView::Refresh(void)
{
//    wxPostEvent(GetEventHandler(), wxPaintEvent(GetId()));
    wxWindow::Refresh(false);
}

void wxGISMapView::OnMapDrawing(wxMxMapViewEvent& event)
{
    wxEventType eType = event.GetEventType();
    if(eType == wxMXMAP_DRAWING_START)
    {
        m_nDrawingState = enumGISMapDrawing;

        wxCHECK_RET(m_pAni, wxT("Animation progress is not initiated"));
        m_pAni->ShowProgress(true);
        m_pAni->Play();
    }
    else if(eType == wxMXMAP_DRAWING_STOP)
    {
        if(!m_pGISDisplay->IsDerty() && m_nDrawingState == enumGISMapDrawing)
            m_nDrawingState = enumGISMapNone;

        if( m_nDrawingState == enumGISMapNone )
        {
            m_ClipGeometry.Clear();
        }

	    m_nDrawingState = enumGISMapNone;
	    Refresh();

        wxCHECK_RET(m_pAni, wxT("Animation progress is not initiated"));
        m_pAni->Stop();
        m_pAni->ShowProgress(false);
    }

	PostEvent(event);
}

wxThread::ExitCode wxGISMapView::Entry()
{
    wxEvtHandler* pEHandle = this->GetEventHandler();//dynamic_cast<wxEvtHandler*>(GetApplication());wxApp::GetInstance();//
    //GetEventHandler()->QueueEvent(new wxThreadEvent(wxEVT_MAP_DRAWING_START));
    wxQueueEvent(pEHandle, new wxMxMapViewEvent(wxMXMAP_DRAWING_START));
	//draw geo data
	OnDraw(wxGISDPGeography);
	//draw selection
	//draw anno
    wxQueueEvent(pEHandle, new wxMxMapViewEvent(wxMXMAP_DRAWING_STOP));
	return (wxThread::ExitCode)0;     // success
}

bool wxGISMapView::CreateAndRunDrawThread(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
#ifdef __WXGTK__
    wxWakeUpIdle();
#endif
    if((GetThread() && GetThread()->IsRunning())|| !m_pGISDisplay->IsDerty())
        return true;

    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the draw thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the draw thread!"));
        return false;
    }

    m_timer.Start(TM_REFRESH);//refresh time for map during drawing

    return true;
}

void wxGISMapView::DestroyDrawThread(void)
{
    wxCriticalSectionLocker lock(m_CritSect);

	if(m_pTrackCancel)
	{
		m_pTrackCancel->Cancel();
	}

    if (GetThread() && GetThread()->IsRunning())
        GetThread()->Wait();
}