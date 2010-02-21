/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISLineString header.
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

#include "wxgis/geometry/linestring.h"

wxGISLineString::wxGISLineString() : OGRLineString(), m_psEnvelope(NULL)
{
}

wxGISLineString::wxGISLineString(OGRLineString* pLineString) : OGRLineString(), m_psEnvelope(NULL)
{
    OGRRawPoint* pPoints = new OGRRawPoint[pLineString->getNumPoints()];
    double* pZPoints = NULL;
    if(nCoordDimension > 2)
        pZPoints = new double[pLineString->getNumPoints()];

    setPoints(pLineString->getNumPoints(), pPoints, pZPoints);

    SetCoordinateDimension(pLineString->getCoordinateDimension());
    SetSpatialReference(pLineString->getSpatialReference());
    FillGEOS();
}

wxGISLineString::wxGISLineString(wxGEOSGeometry* pGEOSGeom, OGRSpatialReference* poSRS, int nCoordDim) : OGRLineString(), m_psEnvelope(NULL)
{
    wxGEOSLineString* pGEOSLineString = dynamic_cast<wxGEOSLineString*>(pGEOSGeom);
    if(!pGEOSLineString)
        return;
    const wxGEOSCoordinateSequence* pCoordinateSequence = pGEOSLineString->getCoordinatesRO();
    for(size_t i = 0; i < pCoordinateSequence->size(); i++)
    {
         wxGEOSCoordinate Coord = pCoordinateSequence->getAt(i);
         addPoint(Coord.x, Coord.y, Coord.z);
    }
    
    m_pGeosGeom = pGEOSGeom;
    m_pGeosPrepGeom = wxGEOSPreparedGeometryFactory::prepare(m_pGeosGeom);
    SetCoordinateDimension(nCoordDim);
    SetSpatialReference(poSRS);
}

wxGISLineString::~wxGISLineString()
{
    wxDELETE(m_psEnvelope);
}

void wxGISLineString::empty()
{
    wxGISGeometry::empty();
    return OGRLineString::empty();
}

wxGISLineString &wxGISLineString::operator=(const OGRLineString &oSource)
{
    wxGISLineString::empty();

    OGRRawPoint* pPoints = new OGRRawPoint[oSource.getNumPoints()];
    double* pZPoints = NULL;
    if(nCoordDimension > 2)
        pZPoints = new double[oSource.getNumPoints()];

    setPoints(oSource.getNumPoints(), pPoints, pZPoints);

    SetCoordinateDimension(oSource.getCoordinateDimension());
    SetSpatialReference(oSource.getSpatialReference());
    FillGEOS();

    return *this;
}

void wxGISLineString::FillGEOS(void)
{
    wxGISGeometry::empty();
    m_pGeosGeom = (const wxGEOSGeometry*)exportToGEOS();
    m_pGeosPrepGeom = wxGEOSPreparedGeometryFactory::prepare(m_pGeosGeom);
    wxDELETE(m_psEnvelope);
}

OGREnvelope* wxGISLineString::GetEnvelope( void )
{
    if(NULL == m_psEnvelope)
    {
        m_psEnvelope = new OGREnvelope();
        OGRLineString::getEnvelope(m_psEnvelope);
    }
    return m_psEnvelope;
}

wxGISGeometry *wxGISLineString::Clone() const
{
    wxGISLineString  *poNewLineString = new wxGISLineString();
     
    poNewLineString->SetSpatialReference( GetSpatialReference() );
    poNewLineString->SetCoordinateDimension( GetCoordinateDimension() );

    OGRRawPoint* pPoints = new OGRRawPoint[getNumPoints()];
    double* pZPoints = NULL;
    if(nCoordDimension > 2)
        pZPoints = new double[getNumPoints()];

    poNewLineString->setPoints(getNumPoints(), pPoints, pZPoints);

    poNewLineString->FillGEOS();
    return poNewLineString;
}

OGRErr wxGISLineString::Transform( OGRCoordinateTransformation *poCT )
{
    return OGRLineString::transform( poCT );
}

void wxGISLineString::SetSpatialReference( OGRSpatialReference * poSR )
{
    OGRLineString::assignSpatialReference(poSR);
}

OGRSpatialReference *wxGISLineString::GetSpatialReference( void ) const
{
    return OGRLineString::getSpatialReference();
}

void wxGISLineString::SetCoordinateDimension( int nCoordDim )
{
    nCoordDimension = nCoordDim;
}

int wxGISLineString::GetCoordinateDimension( void ) const
{
    return nCoordDimension;
}
