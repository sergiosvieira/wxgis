/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
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
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/sysop.h"
#include "wx/filename.h"

#include "vrtwarpedoverview.h"
#include "gdal_rat.h"

wxGISRasterDataset::wxGISRasterDataset(wxString sPath, wxGISEnumRasterDatasetType nType) : wxGISDataset(sPath), m_bIsOpened(false), m_pSpaRef(NULL), m_psExtent(NULL), m_bHasOverviews(false), m_poMainDataset(NULL), m_poDataset(NULL)
{
    m_nSubType = (int)nType;
}

wxGISRasterDataset::~wxGISRasterDataset(void)
{
	OSRDestroySpatialReference( m_pSpaRef );
	wxDELETE(m_psExtent);

	if(m_bIsOpened)
    {
        if(m_poMainDataset)
            GDALDereferenceDataset(m_poMainDataset);
            //if(GDALDereferenceDataset(m_poMainDataset) < 1)
            //    GDALClose(m_poMainDataset);
        if(m_poDataset)
            GDALClose(m_poDataset);
    }
}

bool wxGISRasterDataset::Delete(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    //GDALDriver* pDrv = NULL;
    //close everything
	if(m_bIsOpened)
    {
        if(m_poMainDataset)
        {
            //pDrv = m_poMainDataset->GetDriver();
            GDALDereferenceDataset(m_poMainDataset);
            m_poMainDataset = NULL;
        }
            //if(GDALDereferenceDataset(m_poMainDataset) < 1)
            //    GDALClose(m_poMainDataset);
        if(m_poDataset)
        {
            //if(!pDrv)
            //    pDrv = m_poDataset->GetDriver();
            GDALClose(m_poDataset);
            m_poDataset = NULL;
        }
    }
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
		wxString sErr = wxString::Format(_("wxGISRasterDataset: Open failed! Path '%s'. OGR error: %s"), m_sPath.c_str(), wgMB2WX(err));
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
    if(pBand && pBand->GetOverviewCount() > 0)
        m_bHasOverviews = true;

	//char** papszFileList = m_poDataset->GetFileList();
 //   if( CSLCount(papszFileList) == 0 )
 //   {
 //       wxLogDebug(wxT( "Files: none associated" ));
 //   }
 //   else
 //   {
 //       wxLogDebug(wxT("Files: %s"), wgMB2WX(papszFileList[0]) );
 //       for(int i = 1; papszFileList[i] != NULL; i++ )
	//	{
	//		wxString sFileName = wgMB2WX(papszFileList[i]);
	//		if(sFileName.Find(wxT(".rrd")) != wxNOT_FOUND || sFileName.Find(wxT(".ovr")) != wxNOT_FOUND)
	//			m_bHasOverviews = true;
	//		wxLogDebug( wxT("       %s"), sFileName.c_str() );
	//	}
 //   }
 //   CSLDestroy( papszFileList );

    if(m_nXSize < 2000 && m_nYSize < 2000)
        m_bHasOverviews = true;


	//GDALDriver* pDrv = m_poDataset->GetDriver();
	//const char* desc = pDrv->GetDescription();
	//wxLogDebug( wxT("Driver: %s/%s"), wgMB2WX(GDALGetDriverShortName( pDrv )), wgMB2WX(GDALGetDriverLongName( pDrv )) );

	//char** papszMetadata = m_poDataset->GetMetadata();
 //   if( CSLCount(papszMetadata) > 0 )
 //   {
 //       wxLogDebug( wxT( "Metadata:" ));
 //       for(int i = 0; papszMetadata[i] != NULL; i++ )
 //       {
 //           wxLogDebug( wxT( "  %s"), wgMB2WX(papszMetadata[i]) );
 //       }
 //   }

	///* -------------------------------------------------------------------- */
	///*      Report "IMAGE_STRUCTURE" metadata.                              */
	///* -------------------------------------------------------------------- */
 //   papszMetadata = m_poDataset->GetMetadata("IMAGE_STRUCTURE");
 //   if( CSLCount(papszMetadata) > 0 )
 //   {
 //       wxLogDebug( wxT( "Image Structure Metadata:" ));
 //       for(int i = 0; papszMetadata[i] != NULL; i++ )
 //       {
 //           wxLogDebug( wxT(  "  %s"), wgMB2WX(papszMetadata[i]) );
 //       }
 //   }

	///* -------------------------------------------------------------------- */
	///*      Report subdatasets.                                             */
	///* -------------------------------------------------------------------- */
 //   papszMetadata = m_poDataset->GetMetadata("SUBDATASETS");
 //   if( CSLCount(papszMetadata) > 0 )
 //   {
 //       wxLogDebug( wxT( "Subdatasets:" ));
 //       for(int i = 0; papszMetadata[i] != NULL; i++ )
 //       {
 //           wxLogDebug( wxT( "  %s"), wgMB2WX(papszMetadata[i]) );
 //       }
 //   }

	///* -------------------------------------------------------------------- */
	///*      Report geolocation.                                             */
	///* -------------------------------------------------------------------- */
 //   papszMetadata = m_poDataset->GetMetadata("GEOLOCATION");
 //   if( CSLCount(papszMetadata) > 0 )
 //   {
 //       wxLogDebug( wxT( "Geolocation:" ));
 //       for(int i = 0; papszMetadata[i] != NULL; i++ )
 //       {
 //           wxLogDebug( wxT( "  %s"), wgMB2WX(papszMetadata[i]) );
 //       }
 //   }

	///* -------------------------------------------------------------------- */
	///*      Report RPCs                                                     */
	///* -------------------------------------------------------------------- */
    //papszMetadata = m_poDataset->GetMetadata("RPC");
    //if( CSLCount(papszMetadata) > 0 )
    //{
    //    wxLogDebug( wxT( "RPC Metadata:" ));
    //    for(int i = 0; papszMetadata[i] != NULL; i++ )
    //    {
    //        wxLogDebug( wxT( "  %s"), wgMB2WX(papszMetadata[i]) );
    //    }
    //}

	//for(int nBand = 0; nBand < m_poDataset->GetRasterCount(); nBand++ )
 //   {
 //       double      dfMin, dfMax, adfCMinMax[2], dfNoData;
 //       int         bGotMin, bGotMax, bGotNodata, bSuccess;
 //       int         nBlockXSize, nBlockYSize, nMaskFlags;
 //       double      dfMean, dfStdDev;
 //       GDALColorTable*	hTable;
 //       CPLErr      eErr;

	//	GDALRasterBand* pBand = m_poDataset->GetRasterBand(nBand + 1);

 //       //if( bSample )
 //       //{
 //       //    float afSample[10000];
 //       //    int   nCount;

 //       //    nCount = GDALGetRandomRasterSample( hBand, 10000, afSample );
 //       //    printf( "Got %d samples.\n", nCount );
 //       //}
 //       
	//	pBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
	//	wxLogDebug( wxT( "Band %d Block=%dx%d Type=%s, ColorInterp=%s"), nBand + 1, nBlockXSize, nBlockYSize, wgMB2WX(GDALGetDataTypeName(pBand->GetRasterDataType())), wgMB2WX(GDALGetColorInterpretationName(pBand->GetColorInterpretation())));

	//	wxString sDescription = wgMB2WX(pBand->GetDescription());
 //       wxLogDebug( wxT( "  Description = %s"), sDescription.c_str());

	//	dfMin = pBand->GetMinimum(&bGotMin);
	//	dfMax = pBand->GetMaximum(&bGotMax);
 //       if( bGotMin || bGotMax )
 //       {
 //           if( bGotMin )
 //               wxLogDebug( wxT( "Min=%.3f "), dfMin );
 //           if( bGotMax )
 //               wxLogDebug( wxT( "Max=%.3f "), dfMax );
 //       
	//		pBand->ComputeRasterMinMax(FALSE, adfCMinMax );
 //           wxLogDebug( wxT("  Computed Min/Max=%.3f,%.3f"), adfCMinMax[0], adfCMinMax[1] );
 //       }

 //       eErr = pBand->GetStatistics(TRUE, TRUE, &dfMin, &dfMax, &dfMean, &dfStdDev );
 //       if( eErr == CE_None )
 //       {
 //           wxLogDebug( wxT("  Minimum=%.3f, Maximum=%.3f, Mean=%.3f, StdDev=%.3f"), dfMin, dfMax, dfMean, dfStdDev );
 //       }

 //       //if( bReportHistograms )
 //       //{
 //       //    int nBucketCount, *panHistogram = NULL;

 //       //    eErr = GDALGetDefaultHistogram( hBand, &dfMin, &dfMax, 
 //       //                                    &nBucketCount, &panHistogram, 
 //       //                                    TRUE, GDALTermProgress, NULL );
 //       //    if( eErr == CE_None )
 //       //    {
 //       //        int iBucket;

 //       //        printf( "  %d buckets from %g to %g:\n  ",
 //       //                nBucketCount, dfMin, dfMax );
 //       //        for( iBucket = 0; iBucket < nBucketCount; iBucket++ )
 //       //            printf( "%d ", panHistogram[iBucket] );
 //       //        printf( "\n" );
 //       //        CPLFree( panHistogram );
 //       //    }
 //       //}

 //       //wxLogDebug( wxT("  Checksum=%d"), GDALChecksumImage(pBand, 0, 0, nXSize, nYSize));

	//	dfNoData = pBand->GetNoDataValue(&bGotNodata );
 //       if( bGotNodata )
 //       {
 //           wxLogDebug( wxT("  NoData Value=%.18g"), dfNoData );
 //       }

	//	if( pBand->GetOverviewCount() > 0 )
 //       {
	//		wxString sOut(wxT("  Overviews: " ));
 //           for(int iOverview = 0; iOverview < pBand->GetOverviewCount(); iOverview++ )
 //           {
 //               const char *pszResampling = NULL;

 //               if( iOverview != 0 )
 //                   sOut += wxT( ", " );

	//			GDALRasterBand*	pOverview = pBand->GetOverview( iOverview );
	//			sOut += wxString::Format(wxT("%dx%d"), pOverview->GetXSize(), pOverview->GetYSize());

	//			pszResampling = pOverview->GetMetadataItem("RESAMPLING", "" );
 //               if( pszResampling != NULL && EQUALN(pszResampling, "AVERAGE_BIT2", 12) )
 //                   sOut += wxT( "*" );
 //           }
 //           wxLogDebug(sOut);

 //  //         sOut = wxT( "  Overviews checksum: " );
 //  //         for(int iOverview = 0; iOverview < pBand->GetOverviewCount(); iOverview++ )
	//		//{
 //  //             if( iOverview != 0 )
 //  //                 sOut += wxT( ", " );

	//		//	GDALRasterBand*	pOverview = pBand->GetOverview( iOverview );
	//		//	sOut += GDALChecksumImage(pOverview, 0, 0, pOverview->GetXSize(), pOverview->GetYSize());
 //  //         }
 //  //         wxLogDebug(sOut);
	//	}

	//	if( pBand->HasArbitraryOverviews() )
 //       {
 //          wxLogDebug( wxT("  Overviews: arbitrary" ));
 //       }
 //       
	//	nMaskFlags = pBand->GetMaskFlags();
 //       if( (nMaskFlags & (GMF_NODATA|GMF_ALL_VALID)) == 0 )
 //       {
	//		GDALRasterBand* pMaskBand = pBand->GetMaskBand() ;

 //           wxLogDebug( wxT("  Mask Flags: " ));
 //           if( nMaskFlags & GMF_PER_DATASET )
 //               wxLogDebug( wxT("PER_DATASET " ));
 //           if( nMaskFlags & GMF_ALPHA )
 //               wxLogDebug( wxT("ALPHA " ));
 //           if( nMaskFlags & GMF_NODATA )
 //               wxLogDebug( wxT("NODATA " ));
 //           if( nMaskFlags & GMF_ALL_VALID )
 //              wxLogDebug( wxT("ALL_VALID " ));    

	//		if( pMaskBand != NULL && pMaskBand->GetOverviewCount() > 0 )
 //           {
 //               int		iOverview;

 //               wxLogDebug( wxT("  Overviews of mask band: " ));
 //               for( int nOverview = 0; nOverview < pMaskBand->GetOverviewCount(); nOverview++ )
 //               {
 //                   GDALRasterBand*	pOverview;

 //                   if( nOverview != 0 )
 //                       wxLogDebug( wxT(", " ));

	//				pOverview = pMaskBand->GetOverview( nOverview );
 //                   wxLogDebug( wxT("%dx%d"), pOverview->GetXSize(), pOverview->GetYSize());
 //               }
 //           }
 //       }

	//	if( strlen(pBand->GetUnitType()) > 0 )
 //       {
	//		wxLogDebug( wxT("  Unit Type: %s"),wgMB2WX( pBand->GetUnitType()) );
 //       }

	//	char **papszCategories = pBand->GetCategoryNames();
 //       if( papszCategories != NULL )
 //       {            
 //           int i;
 //           wxLogDebug( wxT("  Categories:" ));
 //           for( i = 0; papszCategories[i] != NULL; i++ )
 //               wxLogDebug( wxT("    %3d: %s"), i, wgMB2WX(papszCategories[i]) );
 //       }

	//	if( pBand->GetScale( &bSuccess ) != 1.0 || pBand->GetOffset( &bSuccess ) != 0.0 )
 //           wxLogDebug( wxT("  Offset: %.15g,   Scale:%.15g"), pBand->GetOffset( &bSuccess ), pBand->GetScale( &bSuccess ) );

	//	papszMetadata = pBand->GetMetadata();
 //       if( CSLCount(papszMetadata) > 0 )
 //       {
 //           wxLogDebug( wxT("  Metadata:" ));
 //           for( int i = 0; papszMetadata[i] != NULL; i++ )
 //           {
 //               wxLogDebug( wxT("    %s"), wgMB2WX(papszMetadata[i]) );
 //           }
 //       }

 //       papszMetadata = pBand->GetMetadata( "IMAGE_STRUCTURE" );
 //       if( CSLCount(papszMetadata) > 0 )
 //       {
 //           wxLogDebug( wxT("  Image Structure Metadata:" ));
 //           for( int i = 0; papszMetadata[i] != NULL; i++ )
 //           {
 //               wxLogDebug( wxT("    %s"), wgMB2WX(papszMetadata[i]));
 //           }
 //       }

	//	if( pBand->GetColorInterpretation() == GCI_PaletteIndex && (hTable = pBand->GetColorTable()) != NULL )
 //       {
 //           int			i;

	//		wxLogDebug( wxT("  Color Table (%s with %d entries)"), wgMB2WX(GDALGetPaletteInterpretationName(hTable->GetPaletteInterpretation())), hTable->GetColorEntryCount() );

	//		for( i = 0; i < hTable->GetColorEntryCount(); i++ )
 //           {
 //               GDALColorEntry	sEntry;

	//			hTable->GetColorEntryAsRGB(i, &sEntry );
 //               wxLogDebug( wxT("  %3d: %d,%d,%d,%d"), i, sEntry.c1, sEntry.c2, sEntry.c3, sEntry.c4 );
 //           }
 //       }

	//	if( pBand->GetDefaultRAT() != NULL )
 //       {
	//		const GDALRasterAttributeTable* pRAT = (const GDALRasterAttributeTable*)pBand->GetDefaultRAT();
	//		GDALRasterAttributeTable* pRATn = (GDALRasterAttributeTable*)pRAT;
	//		pRATn->DumpReadable();
 //       }
	//}

    //CPLCleanupTLS();


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

//    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
//    {
//        printf( "Origin = (%.6f,%.6f)\n",
//                adfGeoTransform[0], adfGeoTransform[3] );
//
//        printf( "Pixel Size = (%.6f,%.6f)\n",
//                adfGeoTransform[1], adfGeoTransform[5] );
//    }

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

//
//poDataset = (GDALDataset *) GDALOpen( pszFilename, GA_ReadOnly );
//    if( poDataset == NULL )
//    {
//        ...;
//    }
//
//	double        adfGeoTransform[6];
//
//    printf( "Driver: %s/%s\n",
//            poDataset->GetDriver()->GetDescription(), 
//            poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
//
//    printf( "Size is %dx%dx%d\n", 
//            poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
//            poDataset->GetRasterCount() );
//
//    if( poDataset->GetProjectionRef()  != NULL )
//        printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );
//
//    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
//    {
//        printf( "Origin = (%.6f,%.6f)\n",
//                adfGeoTransform[0], adfGeoTransform[3] );
//
//        printf( "Pixel Size = (%.6f,%.6f)\n",
//                adfGeoTransform[1], adfGeoTransform[5] );
//    }
//
//
//
// GDALRasterBand  *poBand;
//        int             nBlockXSize, nBlockYSize;
//        int             bGotMin, bGotMax;
//        double          adfMinMax[2];
//        
//        poBand = poDataset->GetRasterBand( 1 );
//        poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
//        printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
//                nBlockXSize, nBlockYSize,
//                GDALGetDataTypeName(poBand->GetRasterDataType()),
//                GDALGetColorInterpretationName(
//                    poBand->GetColorInterpretation()) );
//
//        adfMinMax[0] = poBand->GetMinimum( &bGotMin );
//        adfMinMax[1] = poBand->GetMaximum( &bGotMax );
//        if( ! (bGotMin && bGotMax) )
//            GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
//
//        printf( "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
//        
//        if( poBand->GetOverviewCount() > 0 )
//            printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );
//
//        if( poBand->GetColorTable() != NULL )
//            printf( "Band has a color table with %d entries.\n", 
//                     poBand->GetColorTable()->GetColorEntryCount() );
//
//
//
//		 float *pafScanline;
//        int   nXSize = poBand->GetXSize();
//
//        pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);
//        poBand->RasterIO( GF_Read, 0, 0, nXSize, 1, 
//                          pafScanline, nXSize, 1, GDT_Float32, 
//                          0, 0 );