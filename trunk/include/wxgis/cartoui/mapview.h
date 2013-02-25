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
#pragma once

#include "wxgis/cartoui/cartoui.h"
#include "wxgis/display/gisdisplay.h"
#include "wxgis/carto/map.h"
#include "wxgis/cartoui/mxeventui.h"

class WXDLLIMPEXP_GIS_CTU wxGISMapView :
	public wxWindow,
	public wxGISExtentStack,
	public wxGISConnectionPointContainer,
    public wxThreadHelper
{
    DECLARE_CLASS(wxGISMapView)
	enum
    {
        TIMER_ID = 1015
    };
public:
	friend class wxMapDrawingThread;
public:
    wxGISMapView(void);
	wxGISMapView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE);//wxSTATIC_BORDER|
	virtual ~wxGISMapView(void);
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE, const wxString& name = wxT("GISMapView"));//wxSTATIC_BORDER|
	virtual void SetTrackCancel(ITrackCancel* pTrackCancel);
	virtual wxGISDisplay* GetDisplay(void){return m_pGISDisplay;};
	//wxGISExtentStack
	virtual bool AddLayer(wxGISLayer* pLayer);
	virtual void Clear(void);
	virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
	virtual void SetExtent(const OGREnvelope& Env);
	virtual void SetFullExtent(void);
	virtual OGREnvelope GetFullExtent(void);
	//
	virtual double GetScaleRatio(OGREnvelope& Bounds, wxDC& dc);
	virtual void PanStart(wxPoint MouseLocation);
	virtual void PanMoveTo(wxPoint MouseLocation);
	virtual void PanStop(wxPoint MouseLocation);
	virtual void RotateStart(wxPoint MouseLocation);
	virtual void RotateBy(wxPoint MouseLocation);
	virtual void RotateStop(wxPoint MouseLocation);
	virtual void SetRotate(double dAngleRad);
	virtual double GetCurrentRotate(void);
    //virtual void FlashGeometry(const GeometryArray& Geoms);
protected:
	//events
	virtual void OnPaint(wxPaintEvent & event);
	virtual void OnEraseBackground(wxEraseEvent & event);
	virtual void OnSize(wxSizeEvent & event);
    virtual void OnTimer( wxTimerEvent & event);
	virtual void OnKeyDown(wxKeyEvent & event);
    virtual void OnCaptureLost(wxMouseCaptureLostEvent & event);
	virtual void OnMouseWheel(wxMouseEvent& event);
	//
	//virtual void OnDrawThreadStart(void);
	//virtual void OnDrawThreadStop(void);
	//virtual void CancelDrawThread(void);
	virtual void OnDraw(wxGISEnumDrawPhase nPhase);
    virtual void OnMapDrawing(wxMxMapViewEvent& event);
	//misc
	virtual void DrawToolTip(wxClientDC& dc, const wxString& sText);
	virtual OGREnvelope CreateEnvelopeFromZoomFactor(double dZoom);
	virtual void UpdateFrameCenter(void);
	//virtual void FillClipGeometry(wxRect rect, wxCoord x, wxCoord y);
protected:
    void Refresh(void);
    //void OnStartDrawingThread( wxCommandEvent & event );
    //void OnZooming( wxCommandEvent & event );
protected:
    virtual wxThread::ExitCode Entry();
    bool CreateAndRunDrawThread(void);
    void DestroyDrawThread(void);
protected:
	wxGISDisplay *m_pGISDisplay;
	wxTimer m_timer;
	//wxMapDrawingThread *m_pMapDrawingThread;
	ITrackCancel *m_pTrackCancel;
	IProgressor *m_pAni;
	WXDWORD m_nDrawingState;
	double m_nFactor;
	wxPoint m_StartMouseLocation;
	wxPoint m_FrameCenter;
	double m_dOriginAngle;
	double m_dCurrentAngle;

	wxCriticalSection m_CritSect;

	wxGISPointsArray m_ClipGeometry;
    wxSize m_PrevSize;

	DECLARE_EVENT_TABLE()
};
