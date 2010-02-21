/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  geometry collection header.
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

#include "wxgis/geometry/geometrycollection.h"
#include "wxgis/geometry/point.h"
#include "wxgis/geometry/polygon.h"
#include "wxgis/geometry/linestring.h"
#include "wxgis/geometry/geometryfactory.h"

wxGISGeometryCollection::wxGISGeometryCollection() : OGRGeometryCollection(), m_psEnvelope(NULL)
{
}

wxGISGeometryCollection::wxGISGeometryCollection(OGRGeometryCollection* pGeometryCollection) : OGRGeometryCollection(), m_psEnvelope(NULL)
{
    for(size_t i = 0; i < pGeometryCollection->getNumGeometries(); i++)
        addGeometry(pGeometryCollection->getGeometryRef(i));
    nCoordDimension = pGeometryCollection->getCoordinateDimension();
    SetSpatialReference(pGeometryCollection->getSpatialReference());
    FillGEOS();
}

wxGISGeometryCollection::wxGISGeometryCollection(wxGEOSGeometry* pGEOSGeom, OGRSpatialReference* poSRS, int nCoordDim) : OGRGeometryCollection(), m_psEnvelope(NULL)
{
    wxGEOSGeometryCollection* pGEOSGeometryCollection = dynamic_cast<wxGEOSGeometryCollection*>(pGEOSGeom);
    if(!pGEOSGeometryCollection)
        return;
    for(size_t i = 0; i < pGEOSGeometryCollection->getNumGeometries(); i++)
    {
        const wxGEOSGeometry* pGEOSCollGeom = pGEOSGeometryCollection->getGeometryN(i);
        wxGISGeometry* pGISGeometry = wxGISGeometryFactory::CreateGeometry((wxGEOSGeometry*)pGEOSCollGeom, poSRS, nCoordDim);
        if(pGISGeometry)
            addGeometryDirectly(pGISGeometry->GetOGRGeom());
    }
    
    m_pGeosGeom = pGEOSGeom;
    m_pGeosPrepGeom = wxGEOSPreparedGeometryFactory::prepare(m_pGeosGeom);
    nCoordDimension = nCoordDim;
    SetSpatialReference(poSRS);
}

wxGISGeometryCollection::~wxGISGeometryCollection()
{
    wxDELETE(m_psEnvelope);
}

void wxGISGeometryCollection::empty()
{
    wxGISGeometry::empty();
    return OGRGeometryCollection::empty();
}

wxGISGeometryCollection &wxGISGeometryCollection::operator=(const OGRGeometryCollection &oSource)
{
    wxGISGeometryCollection::empty();
    for(size_t i = 0; i < oSource.getNumGeometries(); i++)
        OGRGeometryCollection::addGeometry(oSource.getGeometryRef(i));
    nCoordDimension = oSource.getCoordinateDimension();
    assignSpatialReference(oSource.getSpatialReference());
    FillGEOS();

    return *this;
}

void wxGISGeometryCollection::FillGEOS(void)
{
    wxGISGeometry::empty();
    m_pGeosGeom = (const wxGEOSGeometry*)exportToGEOS();
    m_pGeosPrepGeom = wxGEOSPreparedGeometryFactory::prepare(m_pGeosGeom);
    wxDELETE(m_psEnvelope);
}

OGREnvelope* wxGISGeometryCollection::GetEnvelope( void )
{
    if(NULL == m_psEnvelope)
    {
        m_psEnvelope = new OGREnvelope();
        OGRGeometryCollection::getEnvelope(m_psEnvelope);
    }
    return m_psEnvelope;
}

wxGISGeometry *wxGISGeometryCollection::Clone() const
{
    wxGISGeometryCollection  *poNewGeometryCollection = new wxGISGeometryCollection();
     
    poNewGeometryCollection->SetSpatialReference( GetSpatialReference() );
    poNewGeometryCollection->SetCoordinateDimension( GetCoordinateDimension() );

    for(size_t i = 0; i < getNumGeometries(); i++)
        poNewGeometryCollection->addGeometry(getGeometryRef(i));
    poNewGeometryCollection->FillGEOS();
    return poNewGeometryCollection;
}

OGRErr wxGISGeometryCollection::Transform( OGRCoordinateTransformation *poCT )
{
    return OGRGeometryCollection::transform( poCT );
}

void wxGISGeometryCollection::SetSpatialReference( OGRSpatialReference * poSR )
{
    OGRGeometryCollection::assignSpatialReference(poSR);
}

OGRSpatialReference *wxGISGeometryCollection::GetSpatialReference( void ) const
{
    return OGRGeometryCollection::getSpatialReference();
}

void wxGISGeometryCollection::SetCoordinateDimension( int nCoordDim )
{
    nCoordDimension = nCoordDim;
}

int wxGISGeometryCollection::GetCoordinateDimension( void ) const
{
    return nCoordDimension;
}
