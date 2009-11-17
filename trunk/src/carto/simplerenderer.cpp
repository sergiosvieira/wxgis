/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSimpleRenderer class.
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
#include "wxgis/carto/simplerenderer.h"
#include "wxgis/display/simplefillsymbol.h"
#include "wxgis/display/simplelinesymbol.h"
#include "wxgis/display/simplemarkersymbol.h"

#define MINPOLYAREA 0.25
#define MINPOLYDRAWAREA 2.2

wxGISSimpleRenderer::wxGISSimpleRenderer(void)
{
	m_pFillSymbol = new wxSimpleFillSymbol();
    m_pLineSymbol = new wxSimpleLineSymbol();
	m_pMarkerSymbol = new wxSimpleMarkerSymbol();
}

wxGISSimpleRenderer::~wxGISSimpleRenderer(void)
{
	wxDELETE(m_pFillSymbol);
	wxDELETE(m_pLineSymbol);
	wxDELETE(m_pMarkerSymbol);
}

bool wxGISSimpleRenderer::CanRender(wxGISDataset* pDataset)
{
	return pDataset->GetType() == enumGISFeatureDataset ? true : false;
}

void wxGISSimpleRenderer::Draw(wxGISFeatureSet* pSet, wxGISEnumDrawPhase DrawPhase, IDisplay* pDisplay, ITrackCancel* pTrackCancel)
{
	if(pSet == NULL || pSet->IsEmpty())
		return;
	IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
	OGRSpatialReference* pDisplaySpatialReference = pDisplayTransformation->GetSpatialReference();
	OGRSpatialReference* pSetSpatialReference = pSet->GetAt(0)->GetGeometryRef()->getSpatialReference();
	bool IsSpaRefSame(true);
	if(pDisplaySpatialReference && pDisplaySpatialReference)
		IsSpaRefSame = pDisplaySpatialReference->IsSame(pSetSpatialReference);

	//if( (m_pDestroyThread->Pause() != wxTHREAD_NO_ERROR) )
	//{
	//	wxLogError(wxString(_("wxGISSimpleRenderer: Can't Pause wxDestroyFeaturesThread!")));
	//	return;
	//}

    //pLayer->ResetReading();
	//OGRFeature *poFeature;
 //   while( (poFeature = pLayer->GetNextFeature()) != NULL )
	for(size_t i = 0; i < pSet->GetSize(); i++)
    {
		OGRGeometry *poGeometry = pSet->GetAt(i)->GetGeometryRef();
        if( poGeometry != NULL)
		{
			//proj goes here
			OGRErr err = OGRERR_NONE;
			if(!IsSpaRefSame)
				err = poGeometry->transformTo(pDisplaySpatialReference);
			if(err == OGRERR_NONE)
			{
				switch(DrawPhase)
				{
				case wxGISDPGeography:
					DrawGeometry(poGeometry, pDisplay);
					break;
				case wxGISDPAnnotation:
					break;
				case wxGISDPSelection:
					break;
				default:
					break;
				}
			}
		}

		if(pTrackCancel && !pTrackCancel->Continue()) //|| wxGetKeyState(WXK_ESCAPE)
			break;
	}
}

void wxGISSimpleRenderer::DrawGeometry(OGRGeometry *poGeometry, IDisplay* pDisplay)
{
	IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
	OGRwkbGeometryType type = wkbFlatten(poGeometry->getGeometryType());
	switch(type)
	{
	case wkbPoint:
		m_pMarkerSymbol->Draw(poGeometry, pDisplay);
		break;
	case wkbLineString:
	case wkbLinearRing:
		{
			//check if line is too small
			double m_World2DC = pDisplayTransformation->GetRatio();
			OGREnvelope sEnvelope;
			poGeometry->getEnvelope(&sEnvelope);
			double EnvWidth = sEnvelope.MaxX - sEnvelope.MinX;
			double EnvHeight = sEnvelope.MaxY - sEnvelope.MinY;
			if(	m_World2DC * EnvWidth <= MINPOLYDRAWAREA && m_World2DC * EnvHeight <= MINPOLYDRAWAREA )
			{
				if(	m_World2DC * EnvWidth >= MINPOLYAREA && m_World2DC * EnvHeight >= MINPOLYAREA )
					m_pMarkerSymbol->Draw(poGeometry, pDisplay);
				return;
			}
			m_pLineSymbol->Draw(poGeometry, pDisplay);
		}
		break;
	case wkbPolygon:
		{
			//check if poly is too small
			double m_World2DC = pDisplayTransformation->GetRatio();
			OGREnvelope sEnvelope;
			poGeometry->getEnvelope(&sEnvelope);
			double EnvWidth = sEnvelope.MaxX - sEnvelope.MinX;
			double EnvHeight = sEnvelope.MaxY - sEnvelope.MinY;
			if(	m_World2DC * EnvWidth <= MINPOLYDRAWAREA && m_World2DC * EnvHeight <= MINPOLYDRAWAREA )
			{
				if(	m_World2DC * EnvWidth >= MINPOLYAREA && m_World2DC * EnvHeight >= MINPOLYAREA )
					m_pMarkerSymbol->Draw(poGeometry, pDisplay);
				return;
			}
			m_pFillSymbol->Draw(poGeometry, pDisplay);
		}
		break;
	case wkbMultiPolygon:
#ifndef RENDERPOLYPOLY
		//1. type drawing
		//break;
#else
		{
			//check if poly is too small
			double m_World2DC = pDisplayTransformation->GetRatio();
			OGREnvelope sEnvelope;
			poGeometry->getEnvelope(&sEnvelope);
			double EnvWidth = sEnvelope.MaxX - sEnvelope.MinX;
			double EnvHeight = sEnvelope.MaxY - sEnvelope.MinY;
			if(	m_World2DC * EnvWidth <= MINPOLYAREA && m_World2DC * EnvHeight <= MINPOLYAREA )
			{
				m_pMarkerSymbol->Draw(poGeometry, pDisplay);
				return;
			}
		}
		m_pFillSymbol->Draw(poGeometry, pDisplay);
		break;
#endif
	case wkbMultiPoint:
		//break;
	case wkbMultiLineString:
		//break;
	case wkbGeometryCollection:
		{
		OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)poGeometry;
		for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
			DrawGeometry(pOGRGeometryCollection->getGeometryRef(i), pDisplay);
		}
		break;
	default:
	case wkbUnknown:
	case wkbNone:
		break;
	}   
}

