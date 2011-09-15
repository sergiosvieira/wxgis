/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISSimpleRenderer class.
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
#include "wxgis/carto/simplerenderer.h"

//#include "wxgis/display/simplefillsymbol.h"
//#include "wxgis/display/simplelinesymbol.h"
//#include "wxgis/display/simplemarkersymbol.h"

wxGISSimpleRenderer::wxGISSimpleRenderer(void)
{
	//m_pFillSymbol = new wxSimpleFillSymbol();
 //   m_pLineSymbol = new wxSimpleLineSymbol();
	//m_pMarkerSymbol = new wxSimpleMarkerSymbol();
	m_stLineColour.dRed = 0.0;
	m_stLineColour.dGreen = 0.0;
	m_stLineColour.dBlue = 0.0;
	m_stLineColour.dAlpha = 1.0;
	m_stPointColour = m_stLineColour;
	//m_stFillColour = m_stLineColour;
	int random_number1 = (rand() % 50); 
	int random_number2 = (rand() % 50); 
	int random_number3 = (rand() % 50); 
	m_stFillColour.dRed = double(205 + random_number1) / 255;
	m_stFillColour.dGreen = double(205 + random_number2) / 255;
	m_stFillColour.dBlue = double(205 + random_number3) / 255;
	//m_stFillColour.dRed = 0.0;
	//m_stFillColour.dGreen = 0.0;
	//m_stFillColour.dBlue = 0.7;
	m_stFillColour.dAlpha = 1.0;
}

wxGISSimpleRenderer::~wxGISSimpleRenderer(void)
{
	//wxDELETE(m_pFillSymbol);
	//wxDELETE(m_pLineSymbol);
	//wxDELETE(m_pMarkerSymbol);
}

bool wxGISSimpleRenderer::CanRender(wxGISDatasetSPtr pDataset)
{
	bool bRet = pDataset->GetType() == enumGISFeatureDataset ? true : false;
	if(bRet)
		m_pDataset = pDataset;
	return bRet;
}

void wxGISSimpleRenderer::Draw(wxGISQuadTreeCursorSPtr pCursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	if(NULL == pCursor)
		return;

	//set colors and etc.
	pDisplay->SetFillColor(m_stFillColour);
	pDisplay->SetLineColor(m_stLineColour);
	pDisplay->SetPointColor(m_stPointColour);
	//pDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);

	pCursor->Reset();
	wxGISQuadTreeItem* pItem;
    while((pItem = pCursor->Next()) != NULL)	
    {
		switch(DrawPhase)
		{
		case wxGISDPGeography:
			{
				OGRGeometry* pGeom = pItem->GetGeometry();
				if(!pGeom)
					break;
				OGRwkbGeometryType type = wkbFlatten(pGeom->getGeometryType());
				switch(type)
				{
				case wkbPoint:
				case wkbMultiPoint:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
					pDisplay->SetLineWidth(0.5);
					pDisplay->SetPointRadius(1.5);
					break;
				case wkbLineString:
				case wkbLinearRing:
				case wkbMultiLineString:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
					pDisplay->SetLineWidth(0.5);
					break;
				case wkbMultiPolygon:
				case wkbPolygon:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
					pDisplay->SetLineWidth(0.5);
					break;
				case wkbGeometryCollection:
					break;
				default:
				case wkbUnknown:
				case wkbNone:
					break;
				}

				pDisplay->DrawGeometry(pGeom);
			}
			break;
		case wxGISDPAnnotation:
			break;
		case wxGISDPSelection:
			break;
		default:
			break;
		}

		if(pTrackCancel && !pTrackCancel->Continue())
			break;
	}
}


