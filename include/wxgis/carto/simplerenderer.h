/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISSimpleRenderer class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/carto/carto.h"
#include "wxgis/datasource/featuredataset.h"

//#include <cstdlib>

/** \class wxGISSimpleRenderer simplerenderer.h
    \brief The vector layer renderer
*/
class wxGISSimpleRenderer :
	public IFeatureRenderer
{
public:
	wxGISSimpleRenderer(void);
	virtual ~wxGISSimpleRenderer(void);
	virtual void SetFillColor(const RGBA Color){m_stFillColour = Color;};
	virtual void SetLineColor(const RGBA Color){m_stLineColour = Color;};
	virtual void SetPointColor(const RGBA Color){m_stPointColour = Color;};
    virtual void SetLineWidth(double dWidth){m_dWidth = dWidth;};
    virtual void SetPointRadius(double dRadius){m_dRadius = dRadius;};
//IFeatureRenderer
	virtual bool CanRender(wxGISDatasetSPtr pDataset);
	virtual void Draw(wxGISQuadTreeCursorSPtr pCursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = 0);
protected:
	wxGISDatasetSPtr m_pDataset;
	RGBA m_stFillColour, m_stLineColour, m_stPointColour;
    double m_dWidth;
    double m_dRadius;
};

