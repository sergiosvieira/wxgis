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

wxGISPoint::wxGISPoint() : OGRPoint(), wxGISGeometry()
{
}
    
wxGISPoint::wxGISPoint( double x, double y ) : OGRPoint(x, y), wxGISGeometry()
{
    FillGEOS();
}

wxGISPoint::wxGISPoint( double x, double y, double z ) : OGRPoint(x, y, z), wxGISGeometry()
{
    FillGEOS();
}

wxGISPoint::wxGISPoint(OGRPoint* pPoint) : OGRPoint(pPoint->getX(), pPoint->getY(), pPoint->getZ()), wxGISGeometry()
{
    assignSpatialReference(pPoint->getSpatialReference());
    nCoordDimension = pPoint->getCoordinateDimension();
    FillGEOS();
}

wxGISPoint::~wxGISPoint()
{
}

void wxGISPoint::empty()
{
    return OGRPoint::empty();
}

wxGISPoint &wxGISPoint::operator=(const OGRPoint &oSource)
{
    setX( oSource.getX() );
    setY( oSource.getY() );
    setZ( oSource.getZ() );
    nCoordDimension = oSource.getCoordinateDimension();
    assignSpatialReference(oSource.getSpatialReference());
    FillGEOS();

    return *this;
}

void wxGISPoint::FillGEOS(void)
{
    PreparedGeometryFactory::destroy(m_pGeosPrepGeom);
    m_pGeosPrepGeom = PreparedGeometryFactory::prepare((const Geometry*)exportToGEOS());
}

bool wxGISPoint::Intersects( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->intersects(pGEOSGeom);
}