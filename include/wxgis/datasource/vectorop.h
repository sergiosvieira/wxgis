/******************************************************************************
 * Project:  wxGIS
 * Purpose:  vector operations.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Bishop
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

#include "wxgis/datasource/gdalinh.h"
#include "wxgis/datasource/quadtree.h"

#include "wx/event.h"

class WXDLLIMPEXP_GIS_DS wxFeatureDSEvent;

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_FEATURES_ADDED, wxFeatureDSEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_GIS_DS, wxDS_CLOSED, wxFeatureDSEvent);

/** \class wxFeatureDSEvent vectorop.h
    \brief The FeatureDS class event.
*/
class WXDLLIMPEXP_GIS_DS wxFeatureDSEvent : public wxEvent
{
public:
    wxFeatureDSEvent(wxEventType eventType = wxDS_FEATURES_ADDED, const wxGISQuadTreeCursor &Cursor = wxNullQuadTreeCursor) : wxEvent(0, eventType)
	{
        m_Cursor = Cursor;
	}
	wxFeatureDSEvent(const wxFeatureDSEvent& event) : wxEvent(event)
	{
        m_Cursor = event.m_Cursor;
	}

	void SetCursor(const wxGISQuadTreeCursor &Cursor){m_Cursor = Cursor;};
	wxGISQuadTreeCursor GetCursor(void){return m_Cursor;};

    virtual wxEvent *Clone() const { return new wxFeatureDSEvent(*this); }

protected:
    wxGISQuadTreeCursor m_Cursor;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxFeatureDSEvent)
};

typedef void (wxEvtHandler::*wxFeatureDSEventFunction)(wxFeatureDSEvent&);

#define wxFeatureDSEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxFeatureDSEventFunction, func)

#define EVT_DS_FEATURES_ADDED(func)  wx__DECLARE_EVT0(wxDS_FEATURES_ADDED, wxFeatureDSEventHandler(func))
#define EVT_DS_CLOSED(func)  wx__DECLARE_EVT0(wxDS_CLOSED, wxFeatureDSEventHandler(func))


void WXDLLIMPEXP_GIS_DS IncreaseEnvelope(OGREnvelope &Env, double dSize);
void WXDLLIMPEXP_GIS_DS SetEnvelopeRatio(OGREnvelope &Env, double dRatio);
void WXDLLIMPEXP_GIS_DS MoveEnvelope(OGREnvelope &MoveEnv, const OGREnvelope &Env);

/** \fn wxGISGeometry EnvelopeToGeometry(const OGREnvelope &Env, const wxGISSpatialReference &SpaRef)
 *  \brief Create wxGISGeometry from OGREnvelope.
 *  \param Env Input envelope
 *  \param SpaRef Spatial Refernce of output geometry
 *  \return Geometry
 */
wxGISGeometry WXDLLIMPEXP_GIS_DS EnvelopeToGeometry(const OGREnvelope &Env, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
