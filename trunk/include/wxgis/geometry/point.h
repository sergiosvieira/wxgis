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
#pragma once

#include "wxgis/geometry/geometry.h"

class WXDLLIMPEXP_GIS_GEOM wxGISPoint : 
    public OGRPoint,
    public wxGISGeometry
{
public:
    wxGISPoint();
    wxGISPoint( double x, double y );
    wxGISPoint( double x, double y, double z );
    wxGISPoint(OGRPoint* pPoint);
    virtual ~wxGISPoint();
    virtual void empty();
    virtual wxGISPoint &operator=(const OGRPoint &oSource);
    virtual void FillGEOS(void);
    virtual bool Intersects( wxGISGeometry* pGeom ) const;
};

