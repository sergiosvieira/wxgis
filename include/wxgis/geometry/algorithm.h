/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Algorithm header.
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

#include "wxgis/geometry/geometry.h"

class WXDLLIMPEXP_GIS_GEOM wxGISAlgorithm
{
public:
    wxGISAlgorithm(void);
    virtual ~wxGISAlgorithm(void);
    virtual OGRGeometry* FastLineIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2);
    virtual OGRGeometry* FastPolyIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2);
protected:
    virtual OGRRawPoint* wxGISAlgorithm::Crossing(OGRRawPoint p11, OGRRawPoint p12, OGRRawPoint p21, OGRRawPoint p22);
    virtual void wxGISAlgorithm::SetPointOnEnvelope(OGRRawPoint* a, OGRRawPoint* b, OGRRawPoint* c, OGREnvelope* r, int code);
};