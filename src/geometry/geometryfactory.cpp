/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISGeometryFactory header.
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

#include "wxgis/geometry/geometryfactory.h"
#include "wxgis/geometry/point.h"
#include "wxgis/geometry/polygon.h"
#include "wxgis/geometry/linestring.h"
#include "wxgis/geometry/geometrycollection.h"

wxGISGeometryFactory::wxGISGeometryFactory(void)
{
}

wxGISGeometryFactory::~wxGISGeometryFactory(void)
{
}

wxGISGeometry *wxGISGeometryFactory::CreateGeometry( wxGEOSGeometry* pGEOSGeom, OGRSpatialReference* poSRS, int nCoordDimension )
{
    wxGEOSGeometryTypeId nType = pGEOSGeom->getGeometryTypeId();
    switch(nType)
    {
    case wxGEOSGeometryTypeId::GEOS_POINT:
        return new wxGISPoint(pGEOSGeom, poSRS, nCoordDimension);
    case wxGEOSGeometryTypeId::GEOS_LINESTRING:
        return new wxGISLineString(pGEOSGeom, poSRS, nCoordDimension);
	case wxGEOSGeometryTypeId::GEOS_LINEARRING:
        wxDELETE(pGEOSGeom);
        break;
	case wxGEOSGeometryTypeId::GEOS_POLYGON:
        return new wxGISPolygon(pGEOSGeom, poSRS, nCoordDimension);
	case wxGEOSGeometryTypeId::GEOS_MULTIPOINT:
        return new wxGISGeometryCollection(pGEOSGeom, poSRS, nCoordDimension);
	case wxGEOSGeometryTypeId::GEOS_MULTILINESTRING:
        return new wxGISGeometryCollection(pGEOSGeom, poSRS, nCoordDimension);
	case wxGEOSGeometryTypeId::GEOS_MULTIPOLYGON:
        return new wxGISGeometryCollection(pGEOSGeom, poSRS, nCoordDimension);
	case wxGEOSGeometryTypeId::GEOS_GEOMETRYCOLLECTION:
        return new wxGISGeometryCollection(pGEOSGeom, poSRS, nCoordDimension);
    default:
        wxDELETE(pGEOSGeom);
        return NULL;
    };
    return NULL;
}

