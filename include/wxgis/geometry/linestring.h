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
#pragma once

#include "wxgis/geometry/geometry.h"

class WXDLLIMPEXP_GIS_GEOM wxGISLineString : 
    public OGRLineString,
    public wxGISGeometry
{
public:
    wxGISLineString();
    wxGISLineString(OGRLineString* pLineString);
    wxGISLineString(wxGEOSGeometry* pGEOSGeom, OGRSpatialReference* poSRS, int nCoordDim);
    virtual ~wxGISLineString();
    virtual void empty();
    virtual wxGISLineString &operator=(const OGRLineString &oSource);
    virtual void FillGEOS(void);
    virtual OGREnvelope* GetEnvelope( void );
    virtual wxGISGeometry *Clone() const;
    virtual OGRErr Transform( OGRCoordinateTransformation *poCT );
    virtual void SetSpatialReference( OGRSpatialReference * poSR );
    virtual OGRSpatialReference *GetSpatialReference( void ) const;
    virtual void SetCoordinateDimension( int nCoordDim );
    virtual int GetCoordinateDimension( void ) const;
    virtual OGRGeometry* GetOGRGeom( void ){return this;};
protected:
    OGREnvelope * m_psEnvelope;
};
