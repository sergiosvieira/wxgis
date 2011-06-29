/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMap class.
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
#include "wxgis/carto/map.h"

//////////////////////////////////////////////////////////////////////////////
// wxGISMap
//////////////////////////////////////////////////////////////////////////////

wxGISMap::wxGISMap(void)
{
	m_sMapName = wxString(_("new map"));
}

wxGISMap::~wxGISMap(void)
{
	Clear();
}

bool wxGISMap::AddLayer(wxGISLayerSPtr pLayer)
{
	if(!pLayer)
		return false;

	if(m_pSpatialReference == NULL)
    {
        OGRSpatialReferenceSPtr pSpaRef = pLayer->GetSpatialReference();
        if(pSpaRef)
            m_pSpatialReference = pSpaRef;
		if(!m_pSpatialReference)
		{
			OGREnvelope Env = pLayer->GetEnvelope();
			if(Env.IsInit())
			{
				if(Env.MaxX <= ENVMAX_X && Env.MaxY <= ENVMAX_Y && Env.MinX >= ENVMIN_X && Env.MinY >= ENVMIN_Y)
				{
					m_pSpatialReference = boost::make_shared<OGRSpatialReference>();
					m_pSpatialReference->importFromEPSG(4326);//SetWellKnownGeogCS("WGS84");
				}
			}
		}
    }
	else
	{
		OGRSpatialReferenceSPtr pInputSpatialReference = pLayer->GetSpatialReference();
		if(!m_pSpatialReference->IsSame(pInputSpatialReference.get()))
			pLayer->SetSpatialReference(m_pSpatialReference);
	}
	//recalc full  envelope
	OGREnvelope Env = pLayer->GetEnvelope();
	if(!Env.IsInit())
		return false;
    m_FullExtent.Merge(Env);
	m_paLayers.push_back(pLayer);
	return true;
}

void wxGISMap::Clear(void)
{
	m_paLayers.clear();
    m_pSpatialReference.reset();
	m_FullExtent.MaxX = ENVMAX_X;
	m_FullExtent.MinX = ENVMIN_X;
	m_FullExtent.MaxY = ENVMAX_Y;
	m_FullExtent.MinY = ENVMIN_Y;
}

OGREnvelope wxGISMap::GetFullExtent(void)
{
	OGREnvelope OutputEnv = m_FullExtent;
    //increase 10%
	IncreaseEnvelope(&OutputEnv, 0.1);

    //double fDeltaX = (m_FullExtent.MaxX - m_FullExtent.MinX) / 20;
    //double fDeltaY = (m_FullExtent.MaxY - m_FullExtent.MinY) / 20;
    //double fDelta = std::max(fDeltaX, fDeltaY);
    //OutputEnv.MaxX = m_FullExtent.MaxX + fDelta;
    //OutputEnv.MinX = m_FullExtent.MinX - fDelta;
    //OutputEnv.MaxY = m_FullExtent.MaxY + fDelta;
    //OutputEnv.MinY = m_FullExtent.MinY - fDelta;
	return OutputEnv;
}

void wxGISMap::SetSpatialReference(OGRSpatialReferenceSPtr pSpatialReference)
{
	if(NULL == pSpatialReference)
		return;
	if(m_pSpatialReference && m_pSpatialReference->IsSame(pSpatialReference.get()))
		return;
	for(size_t i = 0; i < m_paLayers.size(); i++)
		m_paLayers[i]->SetSpatialReference(pSpatialReference);
	m_pSpatialReference = pSpatialReference;
}

OGRSpatialReferenceSPtr wxGISMap::GetSpatialReference(void)
{
	return m_pSpatialReference;
}


//The AddLayer method adds a layer to the Map. Use the LayerCount property to get the total number of layers in the map.
//AddLayer automatically attempts to set the Map's SpatialReference property if a coordinate system has not yet been defined for the map.  
//When the SpatialReference property is set, the Map's MapUnits and DistanceUnits properties are additionally set.  
//AddLayer also sets the spatial reference of the layer (ILayer::SpatialReference ). 
//If no layers have a spatial reference, AddLayer checks the extent of the first layer (ILayer::AreaOfInterest) and if it has coordinates 
//that look like geographic coordinates (XMin >= -180 and XMax <= 180 and YMin >= -90 and YMax <= 90), ArcMap assumes the data is in decimal 
//degrees and sets the MapUnits to esriDecimalDegrees and DistanceUnits to esriMiles. 
//If no spatial reference is found and the coordinates do not look like geographic coordinates, ArcMap sets no spatial reference and sets the 
//MapUnits to esriMeters and the DistanceUnits to esriMeters. 
//The full extent is recalculated each time a layer added.

//////////////////////////////////////////////////////////////////////////////
// wxGISExtentStack
//////////////////////////////////////////////////////////////////////////////

wxGISExtentStack::wxGISExtentStack() : wxGISMap()
{
	m_nPos = wxNOT_FOUND;
}

wxGISExtentStack::~wxGISExtentStack()
{
}

bool wxGISExtentStack::CanRedo()
{
	if(m_staEnvelope.empty())
		return false;
	return m_nPos < m_staEnvelope.size() - 1;
}

bool wxGISExtentStack::CanUndo()
{
	if(m_staEnvelope.empty())
		return false;
	return m_nPos > 0;
}

void wxGISExtentStack::Do(OGREnvelope &NewEnv)
{
	m_nPos++;
	if(m_nPos == m_staEnvelope.size())
		m_staEnvelope.push_back(NewEnv);
	else
	{
		m_staEnvelope[m_nPos] = NewEnv;
		m_staEnvelope.erase(m_staEnvelope.begin() + m_nPos + 1, m_staEnvelope.end());
	}
	SetExtent(NewEnv);
}

void wxGISExtentStack::Redo()
{
	m_nPos++;
	if(m_nPos < m_staEnvelope.size())
	{
		OGREnvelope Env = m_staEnvelope[m_nPos];
		SetExtent(Env);
	}
}

void wxGISExtentStack::Undo()
{
	m_nPos--;
	if(m_nPos > -1)
	{
		OGREnvelope Env = m_staEnvelope[m_nPos];
		SetExtent(Env);
	}
}

void wxGISExtentStack::SetExtent(OGREnvelope &Env)
{
	m_CurrentExtent = Env;
}

void wxGISExtentStack::Clear()
{
	wxGISMap::Clear();
	m_staEnvelope.clear();
	m_nPos = wxNOT_FOUND;
	m_CurrentExtent = m_FullExtent;
}

size_t wxGISExtentStack::GetSize()
{
	return m_staEnvelope.size();
}

OGREnvelope wxGISExtentStack::GetCurrentExtent(void)
{
	return m_CurrentExtent;
}
