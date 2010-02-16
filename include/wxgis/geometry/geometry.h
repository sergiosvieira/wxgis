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

#pragma once

#include "wxgis/base.h"
#include "ogr_geometry.h"
#include "geos_c.h"
#include "geos/geom/prep/PreparedGeometry.h"
#include "geos/geom/prep/PreparedGeometryFactory.h"
#include "geos/geom/Geometry.h" 
#include "geos/geom/GeometryFactory.h" 

#include <geos/export.h>

using geos::geom::Geometry;
using geos::geom::GeometryFactory;
using geos::geom::prep::PreparedGeometry;
using geos::geom::prep::PreparedGeometryFactory;
//using geos::geom::LineString;
//using geos::geom::Polygon;
//using geos::geom::CoordinateSequence;

class wxGISGeometry
{
public:
    wxGISGeometry(void) : m_pGeosPrepGeom(NULL), m_pGeosGeom(NULL){};
    virtual ~wxGISGeometry(void)
    {
        empty();
    }
    virtual const Geometry* GetGEOSGeom(void)
    {
        return m_pGeosGeom;
        //if(m_pGeosPrepGeom)
        //{
        //    const Geometry &wxGEOSGeom = m_pGeosPrepGeom->getGeometry();
        //    return &wxGEOSGeom;
        //}
        //else
        //    return NULL;
    }
    virtual bool Intersects( wxGISGeometry* pGeom ) const
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
    virtual bool Within( wxGISGeometry* pGeom ) const
    {
        if(!m_pGeosPrepGeom)
            return true;
        if(!pGeom)
            return true;
        const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
        if(!pGEOSGeom)
            return true;
        return m_pGeosPrepGeom->within(pGEOSGeom);
    }
    virtual bool Touches( wxGISGeometry* pGeom ) const
    {
        if(!m_pGeosPrepGeom)
            return true;
        if(!pGeom)
            return true;
        const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
        if(!pGEOSGeom)
            return true;
        return m_pGeosPrepGeom->touches(pGEOSGeom);
    }
    virtual bool Disjoint( wxGISGeometry* pGeom ) const
    {
        if(!m_pGeosPrepGeom)
            return true;
        if(!pGeom)
            return true;
        const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
        if(!pGEOSGeom)
            return true;
        return m_pGeosPrepGeom->disjoint(pGEOSGeom);
    }
    virtual bool Crosses( wxGISGeometry* pGeom ) const
    {
        if(!m_pGeosPrepGeom)
            return true;
        if(!pGeom)
            return true;
        const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
        if(!pGEOSGeom)
            return true;
        return m_pGeosPrepGeom->crosses(pGEOSGeom);
    }
    virtual bool Contains( wxGISGeometry* pGeom ) const
    {
        if(!m_pGeosPrepGeom)
            return true;
        if(!pGeom)
            return true;
        const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
        if(!pGEOSGeom)
            return true;
        return m_pGeosPrepGeom->contains(pGEOSGeom);
    }
    virtual bool Overlaps( wxGISGeometry* pGeom ) const
    {
        if(!m_pGeosPrepGeom)
            return true;
        if(!pGeom)
            return true;
        const Geometry* pGEOSGeom = pGeom->GetGEOSGeom();
        if(!pGEOSGeom)
            return true;
        return m_pGeosPrepGeom->overlaps(pGEOSGeom);
    }
    virtual void FillGEOS(void) = 0;
    virtual void empty(void)
    {
        if(m_pGeosGeom)
            GEOSGeom_destroy((GEOSGeometry*)m_pGeosGeom);
        if(m_pGeosPrepGeom)
            PreparedGeometryFactory::destroy(m_pGeosPrepGeom);
    }
protected:
    const PreparedGeometry* m_pGeosPrepGeom;
    const Geometry* m_pGeosGeom;
};

