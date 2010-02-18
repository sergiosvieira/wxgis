/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPolygon header.
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

#include "wxgis/geometry/polygon.h"

wxGISPolygon::wxGISPolygon() : OGRPolygon(), m_psEnvelope(NULL)
{
}

wxGISPolygon::wxGISPolygon(OGRPolygon* pPolygon) : OGRPolygon(), m_psEnvelope(NULL)
{
    OGRPolygon::addRing(pPolygon->getExteriorRing());
    for(size_t i = 0; i < pPolygon->getNumInteriorRings(); i++)
        OGRPolygon::addRing(pPolygon->getInteriorRing(i));
    nCoordDimension = pPolygon->getCoordinateDimension();
    assignSpatialReference(pPolygon->getSpatialReference());
    FillGEOS();
}

wxGISPolygon::wxGISPolygon(Geometry* pGEOSGeom, OGRSpatialReference* poSRS, int nCoordDim)
{
    const LineString * 	getExteriorRing () const
    size_t 	getNumInteriorRing () const
    const LineString * 	getInteriorRingN (size_t n) const
    //Coordinate *pCoord = pGEOSGeom->getCoordinate();
    //if(pCoord)
    //{
    //    setX( pCoord->x );
    //    setY( pCoord->y );
    //    setZ( pCoord->z );
    //}
    m_pGeosGeom = pGEOSGeom;
    m_pGeosPrepGeom = PreparedGeometryFactory::prepare(m_pGeosGeom);
    nCoordDimension = nCoordDim;
    SetSpatialReference(poSRS);
}

wxGISPolygon::~wxGISPolygon()
{
    wxDELETE(m_psEnvelope);
}

void wxGISPolygon::empty()
{
    wxGISGeometry::empty();
    return OGRPolygon::empty();
}

wxGISPolygon &wxGISPolygon::operator=(const OGRPolygon &oSource)
{
    wxGISPolygon::empty();
    OGRPolygon::addRing((OGRLinearRing*)oSource.getExteriorRing());
    for(size_t i = 0; i < oSource.getNumInteriorRings(); i++)
        OGRPolygon::addRing((OGRLinearRing*)oSource.getInteriorRing(i));
    nCoordDimension = oSource.getCoordinateDimension();
    assignSpatialReference(oSource.getSpatialReference());
    FillGEOS();

    return *this;
}

void wxGISPolygon::FillGEOS(void)
{
    wxGISGeometry::empty();
    m_pGeosGeom = (const Geometry*)exportToGEOS();
    m_pGeosPrepGeom = PreparedGeometryFactory::prepare(m_pGeosGeom);
    GetEnvelope();
}

OGREnvelope* wxGISPolygon::GetEnvelope( void )
{
    if(NULL == m_psEnvelope)
    {
        m_psEnvelope = new OGREnvelope();
        OGRPolygon::getEnvelope(m_psEnvelope);
    }
    return m_psEnvelope;
}

wxGISGeometry *wxGISPolygon::Clone() const
{
    wxGISPolygon  *poNewPolygon = new wxGISPolygon();
     
    poNewPolygon->SetSpatialReference( GetSpatialReference() );
    poNewPolygon->SetCoordinateDimension( GetCoordinateDimension() );

    poNewPolygon->addRing((OGRLinearRing*)getExteriorRing());
    for(size_t i = 0; i < getNumInteriorRings(); i++)
        poNewPolygon->addRing((OGRLinearRing*)getInteriorRing(i));

    poNewPolygon->FillGEOS();
    return poNewPolygon;
}

OGRErr wxGISPolygon::Transform( OGRCoordinateTransformation *poCT )
{
    return OGRPolygon::transform( poCT );
}

void wxGISPolygon::SetSpatialReference( OGRSpatialReference * poSR )
{
    OGRPolygon::assignSpatialReference(poSR);
}

OGRSpatialReference *wxGISPolygon::GetSpatialReference( void ) const
{
    return OGRPolygon::getSpatialReference();
}

void wxGISPolygon::SetCoordinateDimension( int nCoordDim )
{
    nCoordDimension = nCoordDim;
}

int wxGISPolygon::GetCoordinateDimension( void ) const
{
    return nCoordDimension;
}