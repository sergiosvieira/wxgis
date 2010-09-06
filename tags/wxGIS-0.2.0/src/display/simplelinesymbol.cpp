/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Simple Line Symbol class. For drawing lines
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
#include "wxgis/display/simplelinesymbol.h"

wxSimpleLineSymbol::wxSimpleLineSymbol(void)
{
	srand ( time(NULL) );
	int random_number1 = (rand() % 50); 
	int random_number2 = (rand() % 50); 
	int random_number3 = (rand() % 50); 
	m_Pen.SetColour(105 + random_number1, 105 + random_number2, 105 + random_number3);
	m_Pen.SetCap(wxCAP_ROUND);
	m_Pen.SetStyle(wxSOLID);
	m_Pen.SetWidth(1);

	m_Brush = wxNullBrush;
	m_Font = wxNullFont;//
}

wxSimpleLineSymbol::wxSimpleLineSymbol(wxPen Pen)
{
	m_Pen = Pen;
	m_Brush = wxNullBrush;
	m_Font = wxNullFont;//
}

wxSimpleLineSymbol::~wxSimpleLineSymbol(void)
{

}

void wxSimpleLineSymbol::Draw(OGRGeometry* pGeometry, IDisplay* pwxGISDisplay)
{
	OGRwkbGeometryType type = wkbFlatten(pGeometry->getGeometryType());
	switch(type)
	{
	case wkbLineString:
    case wkbLinearRing:
		pwxGISDisplay->SetPen(m_Pen);
		DrawLine((OGRLineString*)pGeometry, pwxGISDisplay); 
		break;	
	case wkbMultiLineString:
		pwxGISDisplay->SetPen(m_Pen);
		DrawPolyLine((OGRMultiLineString*)pGeometry, pwxGISDisplay); 
		break;
	case wkbPoint:
	case wkbMultiPoint:
	case wkbPolygon:
	case wkbMultiPolygon:
	case wkbGeometryCollection:
	case wkbUnknown:
	case wkbNone:
	default:
		break;
	}   
}

void wxSimpleLineSymbol::DrawLine(OGRLineString* pLine, IDisplay* pwxGISDisplay)
{
	IDisplayTransformation* pDisplayTransformation = pwxGISDisplay->GetDisplayTransformation();
    if(!pDisplayTransformation)
        return;

	int nPointCount = pLine->getNumPoints();
	if(nPointCount > 1)
	{
		OGRRawPoint* pOGRRawPoints = new OGRRawPoint[nPointCount];
		pLine->getPoints(pOGRRawPoints);
		wxPoint* pPoints = pDisplayTransformation->TransformCoordWorld2DC(pOGRRawPoints, nPointCount);
		wxDELETEA(pOGRRawPoints);
		pwxGISDisplay->DrawLines(nPointCount, pPoints);
		wxDELETEA(pPoints);
	}
}

void wxSimpleLineSymbol::DrawPolyLine(OGRMultiLineString* pLine, IDisplay* pwxGISDisplay)
{
	IDisplayTransformation* pDisplayTransformation = pwxGISDisplay->GetDisplayTransformation();
    if(!pDisplayTransformation)
        return;

    OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pLine;
	for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
	{
		OGRLineString* pLineString = (OGRLineString*)pOGRGeometryCollection->getGeometryRef(i);
		DrawLine(pLineString, pwxGISDisplay);
	}
}
