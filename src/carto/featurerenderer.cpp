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
#include "wxgis/carto/featurerenderer.h"
#include "wxgis/display/displayop.h"

//-----------------------------------------------------------------------------
// wxGISFeatureRenderer
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISFeatureRenderer, wxGISRenderer)

wxGISFeatureRenderer::wxGISFeatureRenderer(wxGISDataset* pwxGISDataset) : wxGISRenderer(pwxGISDataset)
{
	//m_pFillSymbol = new wxSimpleFillSymbol();
 //   m_pLineSymbol = new wxSimpleLineSymbol();
	//m_pMarkerSymbol = new wxSimpleMarkerSymbol();
	m_stLineColour.dRed = 0.0;
	m_stLineColour.dGreen = 0.0;
	m_stLineColour.dBlue = 0.0;
	m_stLineColour.dAlpha = 1.0;
	m_stPointColour = m_stLineColour;

    int random_number1 = (rand() % 50); 
	int random_number2 = (rand() % 50); 
	int random_number3 = (rand() % 50); 

	m_stFillColour.dRed = double(205 + random_number1) / 255;
	m_stFillColour.dGreen = double(205 + random_number2) / 255;
	m_stFillColour.dBlue = double(205 + random_number3) / 255;
	m_stFillColour.dAlpha = 1.0;

    m_dWidth = 0.5;
    m_dRadius = 1.5;

    m_pwxGISFeatureDataset = wxDynamicCast(pwxGISDataset, wxGISFeatureDataset);
}

wxGISFeatureRenderer::~wxGISFeatureRenderer(void)
{
}

bool wxGISFeatureRenderer::CanRender(wxGISDataset* pwxGISDataset) const
{
    wxCHECK_MSG(pwxGISDataset, false, wxT("Input dataset pointer is NULL"));
	return pwxGISDataset->GetType() == enumGISFeatureDataset ? true : false;
}

bool wxGISFeatureRenderer::Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(pDisplay && m_pwxGISFeatureDataset, false, wxT("Display or FeatureDataset pointer is NULL"));

	OGREnvelope stFeatureDatasetExtent = m_pwxGISFeatureDataset->GetEnvelope();
	OGREnvelope stDisplayExtentRotated = pDisplay->GetBounds(true);
	OGREnvelope stFeatureDatasetExtentRotated = stFeatureDatasetExtent;

	//rotate featureclass extent
	if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
	{
        wxRealPoint dfCenter = pDisplay->GetBoundsCenter();
		RotateEnvelope(stFeatureDatasetExtentRotated, pDisplay->GetRotate(), dfCenter.x, dfCenter.y);//dCenterX, dCenterY);
	}

	//if envelopes don't intersect exit
    if(!stDisplayExtentRotated.Intersects(stFeatureDatasetExtentRotated))
        return false;

	//get intersect envelope to fill vector data
	OGREnvelope stDrawBounds = stDisplayExtentRotated;
	stDrawBounds.Intersect(stFeatureDatasetExtentRotated);
	if(!stDrawBounds.IsInit())
		return false;

    bool bAllFeatures = stDrawBounds.Contains(stFeatureDatasetExtent);
	//if(!stFeatureDatasetExtent.Contains(stDrawBounds))
	//	stDrawBounds = stFeatureDatasetExtent;

    wxGISQuadTreeCursor QTCursor;
	if(bAllFeatures)
	{
		QTCursor = m_pwxGISFeatureDataset->SearchGeometry();
	}
	else
	{
		const CPLRectObj Rect = {stDrawBounds.MinX, stDrawBounds.MinY, stDrawBounds.MaxX, stDrawBounds.MaxY};
		QTCursor = m_pwxGISFeatureDataset->SearchGeometry(&Rect);
	}

    if(!QTCursor.IsOk())
        return false;

    Draw(QTCursor, DrawPhase, pDisplay, pTrackCancel);

    return true;
}

void wxGISFeatureRenderer::Draw(const wxGISQuadTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
	//set colors and etc.
	pDisplay->SetColor(enumGISDrawStyleFill, m_stFillColour);
	pDisplay->SetColor(enumGISDrawStyleOutline, m_stLineColour);
	pDisplay->SetColor(enumGISDrawStylePoint, m_stPointColour);

    for(size_t i = 0; i < Cursor.GetCount(); ++i)
    {
        wxGISQuadTreeItem* pItem = Cursor[i];
        if(!pItem)
            continue;

        switch(DrawPhase)
		{
		case wxGISDPGeography:
			{
				wxGISGeometry Geom = pItem->GetGeometry();
                if(!Geom.IsOk())
					break;
				OGRwkbGeometryType type = wkbFlatten(Geom.GetType());
				switch(type)
				{
				case wkbPoint:
				case wkbMultiPoint:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
					pDisplay->SetLineWidth(m_dWidth);
					pDisplay->SetPointRadius(m_dRadius);
					break;
				case wkbLineString:
				case wkbLinearRing:
				case wkbMultiLineString:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
					pDisplay->SetLineWidth(m_dWidth);
					break;
				case wkbMultiPolygon:
				case wkbPolygon:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
					pDisplay->SetLineWidth(m_dWidth);
					break;
				case wkbGeometryCollection:
					break;
				default:
				case wkbUnknown:
				case wkbNone:
					break;
				}

				pDisplay->DrawGeometry(Geom);
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
