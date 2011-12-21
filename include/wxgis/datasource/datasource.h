/******************************************************************************
 * Project:  wxGIS
 * Purpose:  datasource header.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/core/core.h"

#include <wx/datetime.h>

#include "ogrsf_frmts/ogrsf_frmts.h"
#include "gdal_priv.h"
#include "gdal.h"
#include "gdalwarper.h"
#include "gdal_alg_priv.h"
#include "cpl_quad_tree.h"
#include "cpl_string.h"

#define NOTNODATA -9999.0

/** \enum wxGISEnumDatasetType
    \brief The dataset types 
*/

enum wxGISEnumDatasetType
{
    enumGISAny = 0,
	enumGISFeatureDataset = 1,
	enumGISTableDataset = 2,
	enumGISRasterDataset = 3,
	enumGISContainer = 4
};

/** \enum wxGISEnumVectorDatasetType
    \brief The vector datasource types 
*/
enum wxGISEnumVectorDatasetType
{
	enumVecUnknown = 0,
	enumVecESRIShapefile,
	enumVecMapinfoTab,
    enumVecMapinfoMif,
    enumVecKML,
    enumVecKMZ,
    enumVecDXF,
	emumVecPostGIS,
	enumVecGML,
    emumVecMAX
};

/** \enum wxGISEnumRasterDatasetType
    \brief The raster datasource types 
*/
enum wxGISEnumRasterDatasetType
{
	enumRasterUnknown = 0,
	enumRasterBmp,
	enumRasterTiff,
	enumRasterTil,
	enumRasterImg,
	enumRasterJpeg,
	enumRasterPng,
    enumRasterGif,
    enumRasterSAGA,
    enumRasterMAX
};

/** \enum wxGISEnumTableDatasetType
    \brief The table datasource types 
*/
enum wxGISEnumTableDatasetType
{
	enumTableUnknown = 0,
    enumTableDBF,
	enumTablePostgres,
    enumTableQueryResult,
    enumTableMapinfoTab,
    enumTableMapinfoMif,
    enumTableCSV,
    emumTableMAX
};

/** \enum wxGISEnumPrjFileType
    \brief The projection file types 
*/
enum wxGISEnumPrjFileType
{
	enumESRIPrjFile = 1,
	enumSPRfile = 2
};

/** \enum wxGISEnumContainerType
    \brief The container types 
*/
enum wxGISEnumContainerType
{
    enumContUnknown = 0,
	enumContFolder,
	enumContGDBFolder,
    enumContDataset,
	enumContGDB,
	enumContRemoteConnection
};

/** \enum wxGISEnumWldExtType
    \brief The world file ext type 
*/

enum wxGISEnumWldExtType
{
	enumGISWldExt_FirstLastW = 1,
	enumGISWldExt_Wld = 2,
	enumGISWldExt_ExtPlusW = 3,
	enumGISWldExt_ExtPlusWX = 4
};


//GDAL SmartPointers
DEFINE_SHARED_PTR(OGRSpatialReference);
DEFINE_SHARED_PTR(OGRFeature);
DEFINE_SHARED_PTR(OGREnvelope);
DEFINE_SHARED_PTR(OGRGeometry);

static void OGRFeatureDeleter( OGRFeature* pFeature)
{
	OGRFeature::DestroyFeature(pFeature);
}

//typedef struct _Limits
//{
//    double minx, miny, maxx, maxy;
//}
//LIMITS, *LPLIMITS;

class wxGISDataset;
DEFINE_SHARED_PTR(wxGISDataset);

/** \class wxGISDataset datasource.h
    \brief The base class for datasets.
*/
class wxGISDataset
{
public:
	wxGISDataset(CPLString sPath)
    {
        m_sPath = sPath;
    };
	virtual ~wxGISDataset(void){ };
	virtual wxGISEnumDatasetType GetType(void){return m_nType;};
    virtual int GetSubType(void){return m_nSubType;};
    virtual void SetSubType(int nSubType){m_nSubType = nSubType;};
	virtual CPLString GetPath(void){return m_sPath;};
    virtual size_t GetSubsetsCount(void){return 0;};
    virtual wxGISDatasetSPtr GetSubset(size_t nIndex){return wxGISDatasetSPtr();};
    virtual wxString GetName(void){return wxEmptyString;};
	virtual void Close(void){};
	virtual const OGRSpatialReferenceSPtr GetSpatialReference(void){return OGRSpatialReferenceSPtr();};
	virtual bool IsOpened(void){return m_bIsOpened;};
	virtual bool IsReadOnly(void){return m_bIsReadOnly;};
	virtual bool IsCached(void) = 0;
	virtual void Cache(ITrackCancel* pTrackCancel = NULL) = 0;
protected:
	CPLString m_sPath;
    wxCriticalSection m_CritSect;
    int m_nSubType;
	wxGISEnumDatasetType m_nType;
	bool m_bIsOpened;
	bool m_bIsReadOnly;
};

