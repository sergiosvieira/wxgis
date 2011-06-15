/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMapView class.
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
#pragma once

#include "wxgis/cartoui/cartoui.h"
#include "wxgis/carto/map.h"
#include "wxgis/framework/animation.h"

#include <wx/scrolwin.h>
#include <wx/tipwin.h>

///** \enum wxGISEnumMouseState
//    \brief A mouse state.
//*/
//enum wxGISEnumMouseState
//{
//	enumGISMouseNone = 0x0000, 
//	enumGISMouseLeftDown = 0x0001,
//	enumGISMouseRightDown = 0x0002,
//	enumGISMouseWheel = 0x0004
//};

class wxGISMapView;

//wxDrawingThread
class wxDrawingThread : public wxThread
{
public:
	//wxDrawingThread(IwxGISCachedDisplay* pGISScreenDisplay, wxWindow* pWindow, std::vector<IwxGISLayer*>& Layers, ITrackCancel* pTrackCancel);
	wxDrawingThread(wxGISMapView* pView, std::vector<wxGISLayer*>& Layers);
    virtual void *Entry();
    virtual void OnExit();
private:
    ITrackCancel* m_pTrackCancel;
	ICachedDisplay* m_pGISScreenDisplay;
	std::vector<wxGISLayer*>& m_Layers;
	wxGISMapView* m_pView;
};

class WXDLLIMPEXP_GIS_CTU wxGISMapView;

//ExtenStack
class WXDLLIMPEXP_GIS_CTU ExtenStack
{
public:
	ExtenStack(wxGISMapView* pView);
	virtual bool CanRedo();
	virtual bool CanUndo();
	virtual void Do(OGREnvelope NewEnv);
	virtual void Redo();
	virtual void Undo();
	virtual void Reset();
	virtual size_t GetSize();
protected:
	virtual void SetExtent(OGREnvelope Env);
protected:
	wxGISMapView* m_pView;
	std::vector<OGREnvelope> m_EnvelopeArray;
	int m_Pos;
};

//wxGISMapView
class WXDLLIMPEXP_GIS_CTU wxGISMapView :
	public wxScrolledWindow,
	public wxGISMap
{
    DECLARE_CLASS(wxGISMapView)
    enum
    {
        TIMER_ID = 1013
    };
public:
	friend class ExtenStack;
public:
    wxGISMapView(void);
	wxGISMapView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSTATIC_BORDER|wxTAB_TRAVERSAL);
	virtual ~wxGISMapView(void);
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSTATIC_BORDER|wxTAB_TRAVERSAL, const wxString& name = wxT("GISMapView"));
	virtual void OnDraw(wxDC& dc);
	virtual void PanStart(wxPoint MouseLocation);
	virtual void PanMoveTo(wxPoint MouseLocation);
	virtual void PanStop(wxPoint MouseLocation);
	//wxGISMap
	virtual void AddLayer(wxGISLayer* pLayer);
	virtual void ClearLayers(void);
	virtual void SetSpatialReference(OGRSpatialReference* pSpatialReference);
	//wxGISMapView
	void OnThreadExit(void);
	virtual ICachedDisplay* GetCachedDisplay(void){return pGISScreenDisplay;};
	virtual ITrackCancel* GetTrackCancel(void){return m_pTrackCancel;};
	virtual void SetTrackCancel(ITrackCancel* pTrackCancel);
	virtual void SetFullExtent(void);
	virtual void SetExtent(OGREnvelope Env);
	virtual ExtenStack* GetExtenStack(void){return m_pExtenStack;};
protected:
	//events
	virtual void OnEraseBackground(wxEraseEvent & event);
	virtual void OnSize(wxSizeEvent & event);
	virtual void OnKeyDown(wxKeyEvent & event);
	virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnTimer( wxTimerEvent & event);
    virtual void OnCaptureLost(wxMouseCaptureLostEvent & event);
protected:
	ICachedDisplay* pGISScreenDisplay;
	WXDWORD m_MouseState;
	WXDWORD m_MapToolState;
	ITrackCancel* m_pTrackCancel;
	wxDrawingThread* m_pThread;
	IProgressor* m_pAni;
	wxCriticalSection m_CriticalSection;
	wxTimer m_timer;
	wxRect m_virtualrc;
	OGREnvelope m_virtualbounds;
	wxPoint m_StartMouseLocation;
	bool m_bZoomIn;
	ExtenStack* m_pExtenStack;

	DECLARE_EVENT_TABLE()
};

//Here's the basic idea behind display caching. The main application window is controlled by a view (IActiveView).
//There are currently two view coclasses implemented: Map (data view) and PageLayout (layout view).
//ScreenDisplay makes it possible for clients to create any number of caches (a cache is just a device dependent bitmap).
//When a cache is created, the client gets a cacheID. The id is used to specify the active cache (last argument of StartDrawing, i.e.,
//where output is directed), invalidate the cache, or draw the cache to a destination HDC. In addition to dynamic caches, the
//ScreenDisplay also provides a recording cache that accumulates all drawing that happens on the display.
//Clients manage recording using the StartRecording and FinishRecording methods.

//In the simplest case, Map creates one cache for all the layers, another cache if there are annotation or graphics, and a third cache if there's a feature selection. It also records all output. (In addition to these caches, it's also possible for individual layers to request a private cache by returning true for their Cached property. If a layer requests a cache, the Map creates a separate cache for the layer and groups the layers above and below it into different caches.) The IActiveView::PartialRefresh method uses it's knowledge of the cache layout to invalidate as little as possible so that we can draw as much as possible from cache. Given these caches, all of the following scenarios are possible:
//
//Use the recording cache to redraw when the application is moved or exposed or when drawing editing rubberbanding. This is very efficient since only one BitBlt is needed.
//Select a new set of features, invalidate only the selection cache. Features draw from cache. Graphics and annotation draw from cache. Only the feature selection draws from scratch.
//Move a graphic element or annotation over some features. Only invalidate the annotation cache. Features draw from cache. Feature selection draws from cache. Only the annotation draws from scratch.
//Create a new kind of layer called a tracking layer. Always return true for it's cached property. To show vehicle movement when new GPS signals are received, move the markers in the layer and only invalidate the tracking layer. All other layers draw from cache. Only the vehicle layer draws from scratch. This makes it possible to animate a layer.
//Create a basemap by moving several layers into a group layer and setting the group layer's Cached property to true. Now you can edit and interact with layers that draw on top of the basemap without having to redraw the basemap from scratch.
//The concept of a display filter allows raster operations to be performed on any kind of layer including feature layers that use custom symbols. It will be possible to create a slider dialog that attaches to a layer. It sets the layer's Cached property to true and uses a transparency display filter to interactively control the layer's transparency using the slider. Other display filters can be created to implement clipping, contrast, brightness, etc.


