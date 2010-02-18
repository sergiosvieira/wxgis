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
using geos::geom::Coordinate;
using geos::geom::GeometryTypeId;

//using geos::geom::LineString;
//using geos::geom::Polygon;
//using geos::geom::CoordinateSequence;

class WXDLLIMPEXP_GIS_GEOM wxGISGeometry
{
public:
    wxGISGeometry(void);
    virtual ~wxGISGeometry(void);
    virtual const Geometry* GetGEOSGeom(void);

    virtual bool Intersects( wxGISGeometry* pGeom ) const;
    virtual bool Within( wxGISGeometry* pGeom ) const;
    virtual bool Touches( wxGISGeometry* pGeom ) const;
    virtual bool Disjoint( wxGISGeometry* pGeom ) const;
    virtual bool Crosses( wxGISGeometry* pGeom ) const;
    virtual bool Contains( wxGISGeometry* pGeom ) const;
    virtual bool Overlaps( wxGISGeometry* pGeom ) const;

    virtual void empty(void);

    virtual wxGISGeometry *Intersection( wxGISGeometry *pwxGISGeometry) const;

    virtual void FillGEOS(void) = 0;
    virtual wxGISGeometry *Clone() const = 0;
    virtual OGRErr Transform( OGRCoordinateTransformation *poCT ) = 0;
    virtual OGREnvelope* GetEnvelope( void ) = 0;
    virtual void SetSpatialReference( OGRSpatialReference * poSR ) = 0;
    virtual OGRSpatialReference *GetSpatialReference( void ) const = 0;
    virtual void SetCoordinateDimension( int nCoordDim ) = 0;
    virtual int GetCoordinateDimension( void ) const = 0;
    virtual OGRGeometry* GetOGRGeom( void ) const = 0;

protected:
    const PreparedGeometry* m_pGeosPrepGeom;
    const Geometry* m_pGeosGeom;
};

