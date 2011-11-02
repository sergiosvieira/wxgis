/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Filter classes.
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
#include "wxgis/datasource/filter.h"
#include "wxgis/datasource/vectorop.h"

//----------------------------------------------------------------------------
// wxGISQueryFilter
//----------------------------------------------------------------------------

wxGISQueryFilter::wxGISQueryFilter(void)
{
}
    
wxGISQueryFilter::wxGISQueryFilter(wxString sWhereClause)
{
	m_sWhereClause = sWhereClause;
}

wxGISQueryFilter::~wxGISQueryFilter(void)
{
}

void wxGISQueryFilter::SetWhereClause(wxString sWhereClause)
{
	m_sWhereClause = sWhereClause;
}
    
wxString wxGISQueryFilter::GetWhereClause(void)
{
	return m_sWhereClause;
}

//----------------------------------------------------------------------------
// wxGISSpatialFilter
//----------------------------------------------------------------------------

wxGISSpatialFilter::wxGISSpatialFilter(OGRGeometrySPtr pGeom, wxString sWhereClause) : wxGISQueryFilter(sWhereClause)
{
	m_pGeom = pGeom;
}
	
wxGISSpatialFilter::wxGISSpatialFilter() : wxGISQueryFilter()
{
}

wxGISSpatialFilter::~wxGISSpatialFilter()
{
}

void wxGISSpatialFilter::SetEnvelope(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY)
{
	OGREnvelope Envelope;
	Envelope.MinX = dfMinX;
	Envelope.MaxX = dfMaxX;
	Envelope.MinY = dfMinY;
	Envelope.MaxY = dfMaxY;
	m_pGeom = EnvelopeToGeometry(Envelope);
}

void wxGISSpatialFilter::SetGeometry(OGRGeometrySPtr pGeom)
{
	m_pGeom = pGeom;
}
	
OGREnvelopeSPtr wxGISSpatialFilter::GetEnvelope(void)
{
	if(m_pGeom)
	{
		OGREnvelopeSPtr pEnvelope = boost::make_shared<OGREnvelope>();
		m_pGeom->getEnvelope(pEnvelope.get());
		return pEnvelope;
	}
	else
		return OGREnvelopeSPtr();
}
	
OGRGeometrySPtr wxGISSpatialFilter::GetGeometry(void)
{
	return m_pGeom;
}
