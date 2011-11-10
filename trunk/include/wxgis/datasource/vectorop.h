/******************************************************************************
 * Project:  wxGIS
 * Purpose:  vector operations.
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

void WXDLLIMPEXP_GIS_DS IncreaseEnvelope(OGREnvelope &Env, double dSize);
void WXDLLIMPEXP_GIS_DS SetEnvelopeRatio(OGREnvelope &Env, double dRatio);
void WXDLLIMPEXP_GIS_DS ClipGeometryByEnvelope(OGRRawPoint* pOGRRawPoints, int *pnPointCount, const OGREnvelope &Env, bool shapeOpen);
void WXDLLIMPEXP_GIS_DS MoveEnvelope(OGREnvelope &MoveEnv, const OGREnvelope &Env);
/** \fn OGRGeometrySPtr EnvelopeToGeometry(OGREnvelopeSPtr pEnv, OGSSpatialReferensSPtr pSpaRef)
 *  \brief Create OGRGeometry from OGREnvelope.
 *  \param pEnv Input envelope
 *  \param pSpaRef Spatial Refernce of output geometry
 *  \return Geometry
 */
OGRGeometrySPtr WXDLLIMPEXP_GIS_DS EnvelopeToGeometry(const OGREnvelope &Env, OGRSpatialReferenceSPtr pSpaRef = OGRSpatialReferenceSPtr());
