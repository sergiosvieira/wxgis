/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  wxMxMapView class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/mapui/mxmapview.h"

BEGIN_EVENT_TABLE(wxMxMapView, wxGISMapView)
	EVT_LEFT_DOWN(wxMxMapView::OnMouseDown)
	EVT_MIDDLE_DOWN(wxMxMapView::OnMouseDown)
	EVT_RIGHT_DOWN(wxMxMapView::OnMouseDown)
	EVT_LEFT_UP(wxMxMapView::OnMouseUp)
	EVT_MIDDLE_UP(wxMxMapView::OnMouseUp)
	EVT_RIGHT_UP(wxMxMapView::OnMouseUp)
	EVT_LEFT_DCLICK(wxMxMapView::OnMouseDoubleClick)
	EVT_MIDDLE_DCLICK(wxMxMapView::OnMouseDoubleClick)
	EVT_RIGHT_DCLICK(wxMxMapView::OnMouseDoubleClick)
	EVT_MOTION(wxMxMapView::OnMouseMove)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxMxMapView, wxGISMapView)

wxMxMapView::wxMxMapView(void)
{
	m_pTrackCancel = NULL;
}

wxMxMapView::wxMxMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISMapView(parent, id, pos, size), m_pStatusBar(NULL)
{
	m_pTrackCancel = NULL;
}

wxMxMapView::~wxMxMapView(void)
{
}
    
bool wxMxMapView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_pStatusBar = NULL;
    return wxGISMapView::Create(parent, id, pos, size, style, name);
}

bool wxMxMapView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	//Serialize(m_pXmlConf, false);

    m_pApp = application;
    if(!m_pApp)
        return false;
	m_pStatusBar = m_pApp->GetStatusBar();

	m_pTrackCancel = new ITrackCancel();
	if(m_pStatusBar)
		m_pTrackCancel->SetProgressor(m_pStatusBar->GetAnimation());
	SetTrackCancel(m_pTrackCancel);

	return true;
}

void wxMxMapView::Deactivate(void)
{
	//Serialize(m_pXmlConf, true);
	//wxGxView::Deactivate();
	wxDELETE(m_pTrackCancel);
}

void wxMxMapView::OnMouseMove(wxMouseEvent& event)
{
	if(m_pGISDisplay)
	{
		double dX(event.m_x), dY(event.m_y);
		m_pGISDisplay->DC2World(&dX, &dY);
        int nPanePos = m_pStatusBar->GetPanePos(enumGISStatusPosition);
		m_pStatusBar->SetMessage(wxString::Format(_("X: %.4f  Y: %.4f"), dX, dY), nPanePos);
	}

	if(m_pApp)
		m_pApp->OnMouseMove(event);
	event.Skip();
}

void wxMxMapView::OnMouseDown(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDown(event);
	event.Skip();
}

void wxMxMapView::OnMouseUp(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseUp(event);
	event.Skip();
}

void wxMxMapView::OnMouseDoubleClick(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDoubleClick(event);
	event.Skip();
}

