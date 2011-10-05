/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Filter classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/datasource/datasource.h"

/** \class wxGISQueryFilter filter.h
    \brief Attributes query filter.
*/
class WXDLLIMPEXP_GIS_DS wxGISQueryFilter
{
public:
    wxGISQueryFilter(void);
    wxGISQueryFilter(wxString sWhereClause);
	virtual ~wxGISQueryFilter(void);
    virtual void SetWhereClause(wxString sWhereClause);
    virtual wxString GetWhereClause(void);
protected:
    wxString m_sWhereClause;
};

/** \class wxGISQueryFilter filter.h
    \brief Spatial (by geometry) query filter.
*/
class WXDLLIMPEXP_GIS_DS wxGISSpatialFilter : 
	public wxGISQueryFilter
{
public:
	wxGISSpatialFilter(OGRGeometrySPtr pGeom, wxString sWhereClause = wxEmptyString);
	wxGISSpatialFilter(void);
	virtual ~wxGISSpatialFilter(void);
	virtual void SetEnvelope(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY);
	virtual void SetGeometry(OGRGeometrySPtr pGeom);
	virtual OGREnvelopeSPtr GetEnvelope(void);
	virtual OGRGeometrySPtr GetGeometry(void);
protected:
/** \fn OGRGeometrySPtr EnvelopeToGeometry(OGREnvelopeSPtr pEnv, OGSSpatialReferensSPtr pSpaRef)
 *  \brief Create OGRGeometry from OGREnvelope.
 *  \param pEnv Input envelope
 *  \param pSpaRef Spatial Refernce of output geometry
 *  \return Geometry
 */	
	OGRGeometrySPtr EnvelopeToGeometry(const OGREnvelope &Env, OGRSpatialReferenceSPtr pSpaRef = OGRSpatialReferenceSPtr());
protected:
	OGRGeometrySPtr m_pGeom;
};