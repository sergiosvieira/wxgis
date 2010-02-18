/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPoint header.
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

#include "wxgis/geometry/point.h"

wxGISPoint::wxGISPoint() : OGRPoint(), wxGISGeometry(), m_psEnvelope(NULL)
{
}
    
wxGISPoint::wxGISPoint( double x, double y ) : OGRPoint(x, y), wxGISGeometry(), m_psEnvelope(NULL)
{
    FillGEOS();
}

wxGISPoint::wxGISPoint( double x, double y, double z ) : OGRPoint(x, y, z), wxGISGeometry(), m_psEnvelope(NULL)
{
    FillGEOS();
}

wxGISPoint::wxGISPoint(OGRPoint* pPoint) : OGRPoint(pPoint->getX(), pPoint->getY(), pPoint->getZ()), wxGISGeometry(), m_psEnvelope(NULL)
{
    SetCoordinateDimension(pPoint->getCoordinateDimension());
    SetSpatialReference(pPoint->getSpatialReference());
    FillGEOS();
}

wxGISPoint::wxGISPoint(Geometry* pGEOSGeom, OGRSpatialReference* poSRS, int nCoordDim)
{
    const Coordinate *pCoord = pGEOSGeom->getCoordinate();
    if(pCoord)
    {
        setX( pCoord->x );
        setY( pCoord->y );
        setZ( pCoord->z );
    }
    m_pGeosGeom = pGEOSGeom;
    m_pGeosPrepGeom = PreparedGeometryFactory::prepare(m_pGeosGeom);
    SetCoordinateDimension(nCoordDim);
    SetSpatialReference(poSRS);
}

wxGISPoint::~wxGISPoint()
{
    wxDELETE(m_psEnvelope);
}

void wxGISPoint::empty()
{
   wxGISGeometry::empty();
   return OGRPoint::empty();
}

wxGISPoint &wxGISPoint::operator=(const OGRPoint &oSource)
{
    wxGISPoint::empty();
    setX( oSource.getX() );
    setY( oSource.getY() );
    setZ( oSource.getZ() );
    SetCoordinateDimension( oSource.getCoordinateDimension() );
    SetSpatialReference(oSource.getSpatialReference());
    FillGEOS();

    return *this;
}

void wxGISPoint::FillGEOS(void)
{
    wxDELETE(m_psEnvelope);
    wxGISGeometry::empty();
    m_pGeosGeom = (const Geometry*)exportToGEOS();
    m_pGeosPrepGeom = PreparedGeometryFactory::prepare(m_pGeosGeom);
}

wxGISGeometry *wxGISPoint::Clone() const
{
    wxGISPoint *poNewPoint = new wxGISPoint( getX(), getY(), getZ() );

    poNewPoint->SetSpatialReference( GetSpatialReference() );
    poNewPoint->SetCoordinateDimension( GetCoordinateDimension() );
    poNewPoint->FillGEOS();

    return poNewPoint;
}

OGRErr wxGISPoint::Transform( OGRCoordinateTransformation *poCT )
{
    return OGRPoint::transform( poCT );
}

OGREnvelope* wxGISPoint::GetEnvelope( void )
{
    if(NULL == m_psEnvelope)
    {
        m_psEnvelope = new OGREnvelope();
        OGRPoint::getEnvelope(m_psEnvelope);
    }
    return m_psEnvelope;
}

void wxGISPoint::SetSpatialReference( OGRSpatialReference * poSR )
{
    OGRPoint::assignSpatialReference(poSR);
}

OGRSpatialReference *wxGISPoint::GetSpatialReference( void ) const
{
    return OGRPoint::getSpatialReference();
}

void wxGISPoint::SetCoordinateDimension( int nCoordDim )
{
    nCoordDimension = nCoordDim;
}

int wxGISPoint::GetCoordinateDimension( void ) const
{
    return nCoordDimension;
}

