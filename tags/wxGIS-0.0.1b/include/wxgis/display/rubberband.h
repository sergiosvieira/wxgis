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

#pragma once

#include "wxgis/display/display.h"

class WXDLLIMPEXP_GIS_DSP wxGISRubberBand : 
	public wxEvtHandler
{
public:
	wxGISRubberBand(void);
	virtual ~wxGISRubberBand(void);
	virtual OGRGeometry* TrackNew(wxCoord x, wxCoord y, wxWindow *pWnd, ICachedDisplay* pCachedDisplay, ISymbol* pSymbol);
	//virtual bool TrackExisting(IScreenDisplay* ScreenDisplay, ISymbol* Symbol, OGRGeometry* pGeom) = 0;
	virtual void OnUnlock(void);
	//events
	virtual void OnKeyDown(wxKeyEvent & event);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseDown(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
	virtual void OnMouseDoubleClick(wxMouseEvent& event);
	virtual void OnLeave(wxMouseEvent& event);
	virtual void OnEnter(wxMouseEvent& event);
protected:
	OGRGeometry* m_pRetGeom;
	bool m_bLock;
	wxCoord m_StartX;
	wxCoord m_StartY;
	wxCoord m_StartXScr;
	wxCoord m_StartYScr;
	wxWindow* m_pWnd;
	ICachedDisplay* m_pCachedDisplay;
	ISymbol* m_pSymbol;

    wxRect m_PrevRect;

	DECLARE_EVENT_TABLE()
};

//----------------------------------------------------
// class wxGISRubberEnvelope
//----------------------------------------------------

class WXDLLIMPEXP_GIS_DSP wxGISRubberEnvelope :
	public wxGISRubberBand
{
public:
	wxGISRubberEnvelope(void);
	virtual ~wxGISRubberEnvelope(void);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
};