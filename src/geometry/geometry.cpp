/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  geometry header.
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

#include "wxgis/geometry/geometry.h"
#include "wxgis/geometry/geometryfactory.h"

wxGISGeometry::wxGISGeometry(void) : m_pGeosPrepGeom(NULL), m_pGeosGeom(NULL)
{

}

wxGISGeometry::~wxGISGeometry(void)
{
    empty();
}

const wxGEOSGeometry* wxGISGeometry::GetGEOSGeom(void)
{
    return m_pGeosGeom;
}

bool wxGISGeometry::Intersects( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->intersects(pGEOSGeom);
}

bool wxGISGeometry::Within( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->within(pGEOSGeom);
}

bool wxGISGeometry::Touches( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->touches(pGEOSGeom);
}

bool wxGISGeometry::Disjoint( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->disjoint(pGEOSGeom);
}

bool wxGISGeometry::Crosses( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->crosses(pGEOSGeom);
}

bool wxGISGeometry::Contains( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->contains(pGEOSGeom);
}

bool wxGISGeometry::Overlaps( wxGISGeometry* pGeom ) const
{
    if(!m_pGeosPrepGeom)
        return true;
    if(!pGeom)
        return true;
    const wxGEOSGeometry* pGEOSGeom = pGeom->GetGEOSGeom();
    if(!pGEOSGeom)
        return true;
    return m_pGeosPrepGeom->overlaps(pGEOSGeom);
}

void wxGISGeometry::empty(void)
{
    wxDELETE(m_pGeosPrepGeom);
    wxDELETE(m_pGeosGeom);
    //if(m_pGeosGeom)
    //{
    //    GEOSGeom_destroy((GEOSGeometry*)m_pGeosGeom);
    //    m_pGeosGeom = NULL;
    //}
    //if(m_pGeosPrepGeom)
    //{
    //    PreparedGeometryFactory::destroy(m_pGeosPrepGeom);
    //    m_pGeosPrepGeom = NULL;
    //}
}

wxGISGeometry *wxGISGeometry::Intersection( wxGISGeometry *pwxGISGeometry) const
{
    const wxGEOSGeometry* pOtherGeosGeom = pwxGISGeometry->GetGEOSGeom();

    if( m_pGeosGeom != NULL && pOtherGeosGeom != NULL )
    {
        wxGEOSGeometry* pResultGeom =  m_pGeosGeom->intersection(pOtherGeosGeom);
        if( pResultGeom != NULL )
            return wxGISGeometryFactory::CreateGeometry(pResultGeom, GetSpatialReference(), GetCoordinateDimension() );
    }
    return NULL;
}
