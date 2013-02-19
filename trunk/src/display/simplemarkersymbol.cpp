/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Simple Marker Symbol class. For drawing points
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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
#include "wxgis/display/simplemarkersymbol.h"

/*
wxSimpleMarkerSymbol::wxSimpleMarkerSymbol(void)
{
	m_Brush.SetColour(168, 0, 132);
	m_Brush.SetStyle(wxSOLID);

	m_Pen.SetColour(128, 128, 128);
	m_Pen.SetCap(wxCAP_ROUND);
	m_Pen.SetStyle(wxSOLID);
	m_Pen.SetWidth(1);

	m_Size = 4;
}

wxSimpleMarkerSymbol::~wxSimpleMarkerSymbol(void)
{
}

void wxSimpleMarkerSymbol::Draw(OGRGeometry* pGeometry, IDisplay* pwxGISDisplay)
{
	IDisplayTransformation* pDisplayTransformation = pwxGISDisplay->GetDisplayTransformation();
    if(!pDisplayTransformation)
        return;
	OGRwkbGeometryType type = wkbFlatten(pGeometry->getGeometryType());
	switch(type)
	{
	case wkbPoint:
		{
			OGRPoint *pPoint = (OGRPoint*)pGeometry;
			OGRRawPoint Point;
			Point.x = pPoint->getX();
			Point.y = pPoint->getY();
			wxPoint* pPoints = pDisplayTransformation->TransformCoordWorld2DC(&Point, 1);
			pwxGISDisplay->SetBrush(m_Brush);
			pwxGISDisplay->SetPen(m_Pen);
			pwxGISDisplay->DrawCircle(pPoints[0].x, pPoints[0].y, m_Size);
			delete[](pPoints);
		}
		break;
	case wkbPolygon:
	case wkbMultiPolygon:
	case wkbLineString:
	case wkbLinearRing:
	case wkbMultiPoint:
	case wkbMultiLineString:
	case wkbGeometryCollection:
		{
			OGREnvelope sEnvelope;
			pGeometry->getEnvelope(&sEnvelope);
			//double EnvWidth = sEnvelope.MaxX - sEnvelope.MinX;
			//double EnvHeight = sEnvelope.MaxY - sEnvelope.MinY;
			OGRRawPoint Point;
			Point.x = sEnvelope.MinX;
			Point.y = sEnvelope.MinY;
			wxPoint* pPoints = pDisplayTransformation->TransformCoordWorld2DC(&Point, 1);
			pwxGISDisplay->SetBrush(m_Brush);
			pwxGISDisplay->SetPen(m_Pen);
			pwxGISDisplay->DrawPoint(pPoints[0].x, pPoints[0].y);
			delete[](pPoints);
		}
		break;
	case wkbUnknown:
	case wkbNone:
	default:
		break;
	}   
}
*/
