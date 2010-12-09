/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/sysop.h"
#include "wx/filename.h"

#include "vrt/vrtwarpedoverview.h"
#include "gdal_rat.h"

wxGISRasterDataset::wxGISRasterDataset(wxString sPath, wxGISEnumRasterDatasetType nType) : wxGISDataset(sPath), m_pSpaRef(NULL), m_psExtent(NULL), m_bHasOverviews(false), m_bHasStats(false), m_poMainDataset(NULL), m_poDataset(NULL), m_nBandCount(0)
{
	m_bIsOpened = false;
    m_nSubType = (int)nType;
	m_nType = enumGISRasterDataset;
}

wxGISRasterDataset::~wxGISRasterDataset(void)
{
	OSRDestroySpatialReference( m_pSpaRef );
	wxDELETE(m_psExtent);
	Close();
}

void wxGISRasterDataset::Close(void)
{
	if(m_bIsOpened)
    {
        if(m_poMainDataset)
		{
            GDALDereferenceDataset(m_poMainDataset);
			m_poMainDataset = NULL;
            //if(GDALDereferenceDataset(m_poMainDataset) < 1)
            //    GDALClose(m_poMainDataset);
		}
        if(m_poDataset)
		{
            GDALClose(m_poDataset);
			m_poDataset = NULL;
		}
		m_bIsOpened = false;
    }
}

bool wxGISRasterDataset::Delete(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    //GDALDriver* pDrv = NULL;
    //close everything

	Close();
	// else
   // {
   //    m_poDataset = (GDALDataset *) GDALOpen( (const char*) m_sPath.mb_str(*m_pPathEncoding)/*wgWX2MB(m_sPath.c_str())*/, GA_ReadOnly );
   //    if( m_poDataset == NULL )
   //        return false;
   //    pDrv = m_poDataset->GetDriver();
   //    GDALClose(m_poDataset);
   //}
   // if(pDrv)
   // {
   //     CPLErr eErr = pDrv->Delete((const char*) m_sPath.mb_str(*m_pPathEncoding)/*wgWX2MB(m_sPath.c_str())*/);
   //     if(eErr != CE_Fatal)
   //         return true;
   // }

    wxFileName FName( m_sPath );
    wxString sExt = FName.GetExt();
    sExt.Prepend(wxT("."));
    FName.ClearExt();
    wxString sPath = FName.GetFullPath();

    switch(m_nSubType)
    {
    case enumRasterBmp:
	case enumRasterTiff:
	case enumRasterImg:
	case enumRasterJpeg:
	case enumRasterPng:
        if(!DeleteFile(m_sPath))
            return false;
        DeleteFile(sPath + sExt  + wxT("w"));
        DeleteFile(sPath + sExt + wxT(".xml"));
        DeleteFile(sPath + wxT(".lgo"));
        DeleteFile(sPath + wxT(".aux"));
        DeleteFile(sPath + sExt + wxT(".aux"));
        DeleteFile(sPath + sExt  + wxT(".aux.xml"));
        DeleteFile(sPath + wxT(".ovr"));
        DeleteFile(sPath + sExt  + wxT(".ovr"));
        DeleteFile(sPath + sExt  + wxT(".ovr.aux.xml"));
        DeleteFile(sPath + wxT(".xml"));
        DeleteFile(sPath + wxT(".rrd"));
        DeleteFile(sPath + wxT(".rpb"));
        DeleteFile(sPath + wxT(".rpc"));
        DeleteFile(sPath + wxT("_rpc.txt"));
        return true;
    case enumRasterUnknown:
    default: return false;
    }
	return false;
}

bool wxGISRasterDataset::Rename(wxString sNewName)
{
	wxCriticalSectionLocker locker(m_CritSect);
	Close();
	sNewName = ClearExt(sNewName);
    wxFileName FName( m_sPath );
    wxString sExt = FName.GetExt();
    sExt.Prepend(wxT("."));
    FName.ClearExt();
    wxString sOldPath = FName.GetFullPath();
	FName.SetName(sNewName);
    wxString sNewPath = FName.GetFullPath();

    switch(m_nSubType)
    {
    case enumRasterBmp:
	case enumRasterTiff:
	case enumRasterImg:
	case enumRasterJpeg:
	case enumRasterPng:
        if(!RenameFile(sOldPath + sExt, sNewPath + sExt))
            return false;
        RenameFile(sOldPath + sExt + wxT("w"), sNewPath + sExt + wxT("w"));
        RenameFile(sOldPath + sExt + wxT(".xml"), sNewPath + sExt + wxT(".xml"));
        RenameFile(sOldPath + wxT(".lgo"), sNewPath + wxT(".lgo"));
        RenameFile(sOldPath + wxT(".aux"), sNewPath + wxT(".aux"));
        RenameFile(sOldPath + sExt + wxT(".aux"), sNewPath + sExt + wxT(".aux"));
        RenameFile(sOldPath + sExt + wxT(".aux.xml"), sNewPath + sExt + wxT(".aux.xml"));
        RenameFile(sOldPath + wxT(".ovr"), sNewPath + wxT(".ovr"));
        RenameFile(sOldPath + sExt + wxT(".ovr"), sNewPath + sExt + wxT(".ovr"));
        RenameFile(sOldPath + sExt + wxT(".ovr.aux.xml"), sNewPath + sExt + wxT(".ovr.aux.xml"));
        RenameFile(sOldPath + wxT(".xml"), sNewPath + wxT(".xml"));
        RenameFile(sOldPath + wxT(".rrd"), sNewPath + wxT(".rrd"));
        RenameFile(sOldPath + wxT(".rpb"), sNewPath + wxT(".rpb"));
        RenameFile(sOldPath + wxT(".rpc"), sNewPath + wxT(".rpc"));
        RenameFile(sOldPath + wxT("_rpc.txt"), sNewPath + wxT("_rpc.txt"));
        m_sPath = sNewPath + sExt;
        return true;
    case enumRasterUnknown:
    default: return false;
    }
	return false;
}

bool wxGISRasterDataset::Open(void)
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);


    m_poDataset = (GDALDataset *) GDALOpenShared( wgWX2MB(m_sPath), GA_ReadOnly );
    //bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]
	if( m_poDataset == NULL )
    {
        m_sPath.Replace(wxT("\\"), wxT("/"));
        m_poDataset = (GDALDataset *) GDALOpenShared( wgWX2MB(m_sPath), GA_ReadOnly );
    }

    //m_poDataset = (GDALDataset *) GDALOpen( wgWX2MB(m_sPath), GA_ReadOnly );
	if( m_poDataset == NULL )
    {
        //if ( CPLGetLastErrorNo() != CPLE_OpenFailed )
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISRasterDataset: Open failed! Path '%s'. GDAL error: %s"), m_sPath.c_str(), wgMB2WX(err));
		wxLogError(sErr);

		return false;
    }

    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	CPLErr err = m_poDataset->GetGeoTransform(adfGeoTransform);
    bool bHasGeoTransform = false;
    if(err != CE_Fatal)
    {
        bHasGeoTransform = true;
        if (( adfGeoTransform[1] < 0.0 || adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 || adfGeoTransform[5] > 0.0 ) || m_poDataset->GetGCPCount() > 0)
        {
            bHasGeoTransform = false;
            m_poMainDataset = m_poDataset;
            m_poDataset = (GDALDataset *) GDALAutoCreateWarpedVRT( m_poMainDataset, NULL, NULL, GRA_NearestNeighbour, 0.3, NULL );
            m_poMainDataset->Dereference();
            if(m_poDataset == NULL)
            {
                m_poDataset = m_poMainDataset;
                m_poMainDataset = NULL;
            }
          //  else
          //  {
          //      //create pyramids
          //      //int anOverviewList[9] = { 2, 4, 8, 16, 32, 64, 96, 128, 256 };
          //      //CPLErr err = m_poDataset->BuildOverviews( "NEAREST", 9, anOverviewList, 0, NULL, GDALDummyProgress, NULL );
		        ////"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"

          //      //get first overview
          //      //GDALDataset* pDS = m_poMainDataset->GetRasterBand(1)->GetOverview(4)->GetDataset();
          //      //VRTWarpedOverviewDataset* pOVDS = new  VRTWarpedOverviewDataset(pDS, m_poMainDataset);
          //      ////m_poDataset = pDS;
          //      //m_poDataset = (GDALDataset *) GDALAutoCreateWarpedVRT( pOVDS, NULL, NULL, GRA_NearestNeighbour, 0.5, NULL );
          //  }
        }
    }

	m_nXSize = m_poDataset->GetRasterXSize();
	m_nYSize = m_poDataset->GetRasterYSize();

    GDALRasterBand* pBand(NULL);
    if(m_poMainDataset)
        pBand = m_poMainDataset->GetRasterBand(1);
    else if(m_poDataset)
        pBand = m_poDataset->GetRasterBand(1);
    if(!pBand)
    {
		wxLogError(_("wxGISRasterDataset: Open failed! Path '%s'. Raster has no bands"), m_sPath.c_str());
		return false;
    }
    if(pBand)
    {
        if(pBand->GetOverviewCount() > 0)
            m_bHasOverviews = true;

        double dfMin, dfMax, dfMean, dfStdDev;
        CPLErr eErr = pBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev ); 
        if( eErr == CE_None )
            m_bHasStats =  true;
    }

    if(m_nXSize < 2000 && m_nYSize < 2000)
        m_bHasOverviews = true;

    m_nBandCount = m_poDataset->GetRasterCount();

	m_psExtent = new OGREnvelope();

    if(!bHasGeoTransform)
    {
        CPLErr err = m_poDataset->GetGeoTransform(adfGeoTransform);
        if(err != CE_Fatal)
            bHasGeoTransform = true;
    }

	if(bHasGeoTransform)
	{
		double inX[4];
		double inY[4];

		inX[0] = 0;
		inY[0] = 0;
		inX[1] = m_nXSize;
		inY[1] = 0;
		inX[2] = m_nXSize;
		inY[2] = m_nYSize;
		inX[3] = 0;
		inY[3] = m_nYSize;

		m_psExtent->MaxX = -1000000000;
		m_psExtent->MaxY = -1000000000;
		m_psExtent->MinX = 1000000000;
		m_psExtent->MinY = 1000000000;
		for(int i = 0; i < 4; i++)
		{
			double rX, rY;
			GDALApplyGeoTransform( adfGeoTransform, inX[i], inY[i], &rX, &rY );
			if(m_psExtent->MaxX < rX)
				m_psExtent->MaxX = rX;
			if(m_psExtent->MinX > rX)
				m_psExtent->MinX = rX;
			if(m_psExtent->MaxY < rY)
				m_psExtent->MaxY = rY;
			if(m_psExtent->MinY > rY)
				m_psExtent->MinY = rY;
		}
	}
	else
	{
		m_psExtent->MaxX = m_nXSize;
		m_psExtent->MaxY = m_nYSize;
		m_psExtent->MinX = 0;
		m_psExtent->MinY = 0;
	}

	m_bIsOpened = true;
	return true;
}

OGRSpatialReference* wxGISRasterDataset::GetSpatialReference(void)
{
	if(!m_bIsOpened)
		if(!Open())
			return NULL;
	if(m_pSpaRef)
		return m_pSpaRef;
	if(	m_poDataset )
	{
		m_pSpaRef = new OGRSpatialReference(m_poDataset->GetProjectionRef());
		return m_pSpaRef;
	}
	return NULL;
}

const OGREnvelope* wxGISRasterDataset::GetEnvelope(void)
{
	if(m_psExtent)
		return m_psExtent;
	return NULL;
}

wxString wxGISRasterDataset::GetName(void)
{
    wxFileName FName(m_sPath);
    return FName.GetFullName();
}

