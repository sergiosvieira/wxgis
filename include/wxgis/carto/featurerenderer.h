/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISSimpleRenderer class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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

#include "wxgis/carto/renderer.h"
#include "wxgis/datasource/featuredataset.h"

/** \class wxGISSimpleRenderer featurerenderer.h
    \brief The vector layer renderer
*/

class wxGISFeatureRenderer :
	public wxGISRenderer
{
    DECLARE_CLASS(wxGISFeatureRenderer)
public:
	wxGISFeatureRenderer(wxGISDataset* pwxGISDataset = NULL);
	virtual ~wxGISFeatureRenderer(void);
    virtual bool CanRender(wxGISDataset* pwxGISDataset) const;
    virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel = NULL);

	virtual void SetFillColor(const RGBA Color){m_stFillColour = Color;};
	virtual void SetLineColor(const RGBA Color){m_stLineColour = Color;};
	virtual void SetPointColor(const RGBA Color){m_stPointColour = Color;};
    virtual void SetLineWidth(double dWidth){m_dWidth = dWidth;};
    virtual void SetPointRadius(double dRadius){m_dRadius = dRadius;};
	virtual void Draw(const wxGISQuadTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = 0);
protected:
	wxGISFeatureDataset* m_pwxGISFeatureDataset;
	RGBA m_stFillColour, m_stLineColour, m_stPointColour;
    double m_dWidth;
    double m_dRadius;
};

