/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main dataset functions.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/datasource/featuredataset.h"

bool WXDLLIMPEXP_GIS_GP CopyRows(wxGISFeatureDatasetSPtr pSrcDataSet, wxGISFeatureDatasetSPtr pDstDataSet, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL); 
bool WXDLLIMPEXP_GIS_GP ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_GP ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_GP Project(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRSpatialReference* pNewSpaRef, ITrackCancel* pTrackCancel);
OGRGeometry WXDLLIMPEXP_GIS_GP *Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv);
OGRGeometry WXDLLIMPEXP_GIS_GP *CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT);
