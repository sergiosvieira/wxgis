/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
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
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/datasource/rasterop.h"
#include "wx/filename.h"

//#include "vrt/vrtwarpedoverview.h"
#include "gdal_rat.h"

wxGISRasterDataset::wxGISRasterDataset(CPLString sPath, wxGISEnumRasterDatasetType nType) : wxGISDataset(sPath), m_bHasOverviews(false), m_bHasStats(false), m_poMainDataset(NULL), m_poDataset(NULL), m_nBandCount(0)
{
	m_bIsOpened = false;
    m_bIsReadOnly = true;
    m_nSubType = (int)nType;
	m_nType = enumGISRasterDataset;
	m_nDataType = GDT_Unknown;
    m_paNoData = NULL;
}

wxGISRasterDataset::~wxGISRasterDataset(void)
{
	Close();
    if(m_paNoData)
        delete [] m_paNoData;
}

void wxGISRasterDataset::Close(void)
{
	if(m_bIsOpened)
    {
		m_pSpaRef.reset();
		m_stExtent.MinX = m_stExtent.MaxX = m_stExtent.MinY = m_stExtent.MaxY = 0;
		m_nDataType = GDT_Unknown;
        m_bHasOverviews = false;
        m_bHasStats = false;
        m_nBandCount = 0;
        if(m_poDataset)
		{
            if(GDALDereferenceDataset(m_poDataset) < 1)
                GDALClose(m_poDataset);
			m_poDataset = NULL;
            m_poMainDataset = NULL;
		}
        if(m_poMainDataset)
		{
            //GDALDereferenceDataset(m_poMainDataset);
            if(GDALDereferenceDataset(m_poMainDataset) < 1)
                GDALClose(m_poMainDataset);
			m_poMainDataset = NULL;
		}
		m_bIsOpened = false;
        m_bIsReadOnly = true;
    }
}

bool wxGISRasterDataset::Delete(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();
    
    if(!DeleteFile(m_sPath))
        return false;
	
    char** papszFileList = GetFileList();
    if(papszFileList)
    {
        for(int i = 0; papszFileList[i] != NULL; ++i )
            DeleteFile(papszFileList[i]);
        CSLDestroy( papszFileList );
    }
	return true;
}

char **wxGISRasterDataset::GetFileList()
{
    char **papszFileList = NULL;
    CPLString szPath;
    //papszFileList = CSLAddString( papszFileList, osIMDFile );
    switch(m_nSubType)
    {
	case enumRasterSAGA:
		szPath = (char*)CPLResetExtension(m_sPath, "sgrd");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "mgrd");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		break;
    case enumRasterTiff:
    case enumRasterTil:
		szPath = (char*)CPLResetExtension(m_sPath, "imd");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "pvl");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "att");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
		szPath = (char*)CPLResetExtension(m_sPath, "eph");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
        //RPC specific
        szPath = (char*)CPLResetExtension(m_sPath, "rpb");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "rpc");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        {
        CPLString szRpcName = CPLString(CPLGetBasename(m_sPath)) + CPLString("_rpc.txt");
        szPath = (char*)CPLFormFilename(CPLGetPath(m_sPath), (const char*)szRpcName.c_str(), NULL);
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        }
        break;
	case enumRasterImg:
        szPath = (char*)CPLResetExtension(m_sPath, "ige");
		if(CPLCheckForFile((char*)szPath.c_str(), NULL))
			papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumRasterBmp:
	case enumRasterPng:
	case enumRasterGif:
    case enumRasterUnknown:
    default: 
        break;
    }
    //check for world file
	CPLString soWldFilePath = GetWorldFilePath(m_sPath);
	if(!soWldFilePath.empty())
		papszFileList = CSLAddString( papszFileList, soWldFilePath );
    szPath = m_sPath + CPLString(".xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".aux");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".aux.xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".ovr");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".ovr.aux.xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = m_sPath + CPLString(".rrd");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "lgo");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "aux");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "aux.xml");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "ovr");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "rrd");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

bool wxGISRasterDataset::Rename(wxString sNewName)
{
	wxCriticalSectionLocker locker(m_CritSect);

    CPLString szDirPath = CPLGetPath(m_sPath);
    CPLString szName = CPLGetBasename(m_sPath);
	CPLString szNewName = ClearExt(sNewName).mb_str(wxConvUTF8);

    Close();

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)
        return false;

    char **papszNewFileList = NULL;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {		
        CPLString szNewPath(CPLFormFilename(szDirPath, szNewName, GetExtension(papszFileList[i], szName)));
        papszNewFileList = CSLAddString(papszNewFileList, szNewPath);
        if(!RenameFile(papszFileList[i], papszNewFileList[i]))
		{
            // Try to put the ones we moved back. 
            for( --i; i >= 0; i-- )
                RenameFile( papszNewFileList[i], papszFileList[i]);

 			CSLDestroy( papszFileList );
			CSLDestroy( papszNewFileList );
            return false;
		}
    }

    bool bRet = true;
    switch(m_nSubType)
    {
	case enumRasterSAGA://write some internal info to files (e.g. sgrd mgrd -> new file name
        {
            CPLString sMGRDFileName(CPLFormFilename(szDirPath, szNewName, "mgrd"));
            CPLXMLNode *psXMLRoot = CPLParseXMLFile(sMGRDFileName);
            if(psXMLRoot)
            {
                CPLXMLNode *pNode = psXMLRoot->psNext;//SAGA_METADATA
                bRet = CPLSetXMLValue(pNode, "SOURCE.FILE", CPLFormFilename(szDirPath, szNewName, "sgrd"));
                if(bRet)
                    bRet = CPLSerializeXMLTreeToFile(psXMLRoot, sMGRDFileName);
                CPLDestroyXMLNode( psXMLRoot );
            }
            CPLString sSGRDFileName(CPLFormFilename(szDirPath, szNewName, "sgrd"));
            char** papszNameValues = CSLLoad(sSGRDFileName);
            papszNameValues[0] = (char*)CPLSPrintf("NAME\t= %s", szNewName.c_str() );
            bRet = CSLSave(papszNameValues, sSGRDFileName);

        }
		break;
    default:
		break;
    };
    
	m_sPath = CPLString(CPLFormFilename(szDirPath, szNewName, CPLGetExtension(m_sPath)));
	CSLDestroy( papszFileList );
	CSLDestroy( papszNewFileList );
	return bRet;
}

bool wxGISRasterDataset::Open(bool bReadOnly)
{	
    if(bReadOnly != m_bIsReadOnly)
        Close();

	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);


    m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, bReadOnly == true ? GA_ReadOnly : GA_Update );
    //bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]
	
	if( m_poDataset == NULL )
    {
		for(size_t i = 0; i < m_sPath.size(); ++i)
			if(m_sPath[i] == '/')
				m_sPath[i] = '\\';
        m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, bReadOnly == true ? GA_ReadOnly : GA_Update );
    }
	if( m_poDataset == NULL )
    {
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISRasterDataset: Open failed! Path '%s'. GDAL error: %s"), m_sPath.c_str(), wxString(err, wxConvUTF8).c_str());
		wxLogError(sErr);

		return false;
    }

    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	CPLErr err = m_poDataset->GetGeoTransform(adfGeoTransform);
    bool bHasGeoTransform = false;
    if(err != CE_Fatal)
    {
        //look for metadata OV-3
        char** papszMetadata = m_poDataset->GetMetadata("IMD");
        //if(EQUAL(CSLFetchNameValue(papszMetadata, "sensorInfo.satelliteName"), "OV-3"))
        if(papszMetadata && EQUAL(CSLFetchNameValue(papszMetadata, "md_type"), "pvl") && m_poDataset->GetGCPCount() == 0)
        {
            OGRSpatialReference oOGRSpatialReference(SRS_WKT_WGS84);   
            double adfX[4], adfY[4], adfZ[4];
            adfX[0] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperLeftCorner.longitude", "0.0"));
            adfY[0] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperLeftCorner.latitude", "0.0"));
            adfZ[0] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperLeftCorner.height", "0.0"));
            adfX[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperRightCorner.longitude", "0.0"));
            adfY[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperRightCorner.latitude", "0.0"));
            adfZ[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.upperRightCorner.height", "0.0"));
            adfX[2] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerRightCorner.longitude", "0.0"));
            adfY[2] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerRightCorner.latitude", "0.0"));                    
            adfZ[2] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerRightCorner.height", "0.0"));
            adfX[3] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerLeftCorner.longitude", "0.0"));
            adfY[3] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerLeftCorner.latitude", "0.0"));   
            adfZ[3] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.geodeticCorners.lowerLeftCorner.height", "0.0"));

            //const char* pszSpacingUnits = CSLFetchNameValueDef(papszMetadata, "productInfo.spacingUnits", "meters");
            //// trim double quotes. 
            //if( pszSpacingUnits[0] == '"' )
            //    pszSpacingUnits++;
            //if( pszSpacingUnits[strlen(pszSpacingUnits)-1] == '"' )
            //    ((char *) pszSpacingUnits)[strlen(pszSpacingUnits)-1] = '\0';

            //if(EQUAL(pszSpacingUnits, "meters"))
            //{
            //    //if meters use spacing and transform upper left corner to WGS84/UTM
            //    adfGeoTransform[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.pixelSpacing", "0.0"));
            //    adfGeoTransform[5] = -CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.lineSpacing", "0.0"));
            //    //calc UTM Zone
                double dfXCenter = adfX[0] + (adfX[2] - adfX[0]) / 2.0;
                int nZoneNo = ceil( (180.0 + dfXCenter) / 6.0 );
                OGRSpatialReference oDstOGRSpatialReference(SRS_WKT_WGS84); 
                oDstOGRSpatialReference.SetUTM(nZoneNo, adfY[0] > 0);

                OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( &oOGRSpatialReference, &oDstOGRSpatialReference);
                if(poCT)
                {
                    int nResult = poCT->Transform(4, adfX, adfY, adfZ);
                    if(nResult)
                    {
                        if(bReadOnly)
                        {
                            GDALClose(m_poDataset);
                            m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, GA_Update );
                        }

            //            adfGeoTransform[0] = dfULX;
            //            adfGeoTransform[3] = dfULY;
            //            CPLErr eErr = m_poDataset->SetGeoTransform(adfGeoTransform);

                        GDAL_GCP *pasGCPList = (GDAL_GCP *) CPLCalloc(sizeof(GDAL_GCP), 4);                
                        GDALInitGCPs( 4, pasGCPList );
                        char *pszProjection = NULL;

                        if(m_poDataset)
                        {                            
                            if( oDstOGRSpatialReference.exportToWkt( &pszProjection ) == OGRERR_NONE )
                            {
                                m_poDataset->SetProjection(pszProjection);
                            }

                            m_nXSize = m_poDataset->GetRasterXSize();
                            m_nYSize = m_poDataset->GetRasterYSize();
                                            
                            pasGCPList[0].pszId = CPLStrdup( "1" );
                            pasGCPList[0].dfGCPX = adfX[0];
                            pasGCPList[0].dfGCPY = adfY[0];
                            pasGCPList[0].dfGCPZ = adfZ[0];
                            pasGCPList[0].dfGCPLine = 0.5;
                            pasGCPList[0].dfGCPPixel = 0.5;

                            pasGCPList[1].pszId = CPLStrdup( "2" );
                            pasGCPList[1].dfGCPX = adfX[1];
                            pasGCPList[1].dfGCPY = adfY[1];
                            pasGCPList[1].dfGCPZ = adfZ[1];
                            pasGCPList[1].dfGCPLine = 0.5;
                            pasGCPList[1].dfGCPPixel = m_nXSize - 0.5;

                            pasGCPList[2].pszId = CPLStrdup( "3" );
                            pasGCPList[2].dfGCPX = adfX[2];
                            pasGCPList[2].dfGCPY = adfY[2];
                            pasGCPList[2].dfGCPZ = adfZ[2];
                            pasGCPList[2].dfGCPLine = m_nYSize - 0.5;
                            pasGCPList[2].dfGCPPixel = m_nXSize - 0.5;

                            pasGCPList[3].pszId = CPLStrdup( "4" );
                            pasGCPList[3].dfGCPX = adfX[3];
                            pasGCPList[3].dfGCPY = adfY[3];
                            pasGCPList[3].dfGCPZ = adfZ[3];
                            pasGCPList[3].dfGCPLine = m_nYSize - 0.5;
                            pasGCPList[3].dfGCPPixel = 0.5;

                            CPLErr eErr = m_poDataset->SetGCPs(4, pasGCPList, pszProjection);
                            //nResult = GDALGCPsToGeoTransform(4, pasGCPList, adfGeoTransform, 1);
                            //if(nResult)
                            //    CPLErr eErr = m_poDataset->SetGeoTransform(adfGeoTransform);
                        }

                        if(bReadOnly)
                        {
                            GDALClose(m_poDataset);
                            m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, GA_ReadOnly );
                        }

                        GDALDeinitGCPs( 4, pasGCPList );

                        CPLFree( pasGCPList );
                        CPLFree( pszProjection );                    
                    }
                    OCTDestroyCoordinateTransformation(poCT);
                }
            //}
            //else
            //{
                //if(bReadOnly)
                //{
                //    GDALClose(m_poDataset);
                //    m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, GA_Update );
                //}                
            //    //if degrees use spacing and use WGS84
            //    adfGeoTransform[1] = CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.pixelSpacing", "0.0"));
            //    adfGeoTransform[5] = -CPLAtof(CSLFetchNameValueDef(papszMetadata, "productInfo.lineSpacing", "0.0"));
            //    adfGeoTransform[0] = dfULX;
            //    adfGeoTransform[3] = dfULY;
            //    CPLErr eErr = m_poDataset->SetGeoTransform(adfGeoTransform);
                //char *pszProjection = NULL;
                //if( oOGRSpatialReference.exportToWkt( &pszProjection ) == OGRERR_NONE )
                //{
                //    m_poDataset->SetProjection(pszProjection);                    
                //}
                
                //m_nXSize = m_poDataset->GetRasterXSize();
                //m_nYSize = m_poDataset->GetRasterYSize();

                //int nGCPCount(4);
                //GDAL_GCP *pasGCPList = (GDAL_GCP *) CPLCalloc(sizeof(GDAL_GCP),nGCPCount);                
                //GDALInitGCPs( nGCPCount, pasGCPList );
                //
                //pasGCPList[0].pszId = CPLStrdup( "1" );
                //pasGCPList[0].dfGCPX = dfULX;
                //pasGCPList[0].dfGCPY = dfULY;
                //pasGCPList[0].dfGCPZ = dfULZ;
                //pasGCPList[0].dfGCPLine = 0.5;
                //pasGCPList[0].dfGCPPixel = 0.5;

                //pasGCPList[1].pszId = CPLStrdup( "2" );
                //pasGCPList[1].dfGCPX = dfURX;
                //pasGCPList[1].dfGCPY = dfURY;
                //pasGCPList[1].dfGCPZ = dfURZ;
                //pasGCPList[1].dfGCPLine = 0.5;
                //pasGCPList[1].dfGCPPixel = m_nXSize - 0.5;

                //pasGCPList[2].pszId = CPLStrdup( "3" );
                //pasGCPList[2].dfGCPX = dfLRX;
                //pasGCPList[2].dfGCPY = dfLRY;
                //pasGCPList[2].dfGCPZ = dfLRZ;
                //pasGCPList[2].dfGCPLine = m_nYSize - 0.5;
                //pasGCPList[2].dfGCPPixel = m_nXSize - 0.5;

                //pasGCPList[3].pszId = CPLStrdup( "4" );
                //pasGCPList[3].dfGCPX = dfLLX;
                //pasGCPList[3].dfGCPY = dfLLY;
                //pasGCPList[3].dfGCPZ = dfLLZ;
                //pasGCPList[3].dfGCPLine = m_nYSize - 0.5;
                //pasGCPList[3].dfGCPPixel = 0.5;

                //CPLErr eErr = m_poDataset->SetGCPs(nGCPCount, pasGCPList, pszProjection);
                //GDALDeinitGCPs( nGCPCount, pasGCPList );
                //CPLFree( pasGCPList );
                //CPLFree( pszProjection );

                //if(bReadOnly)
                //{
                //    GDALClose(m_poDataset);
                //    m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, GA_ReadOnly );
                //} 
            //}
        }

        bHasGeoTransform = true;
        if ((adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 ) || m_poDataset->GetGCPCount() > 0)// adfGeoTransform[1] < 0.0 || adfGeoTransform[5] > 0.0 || 
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

		m_nDataType = pBand->GetRasterDataType(); //Assumed that raster have the same datatype in all other bands
    }

    if(m_nXSize < 2000 && m_nYSize < 2000)//for small rasters there is no need to create pyramids
        m_bHasOverviews = true;

    m_nBandCount = m_poDataset->GetRasterCount();

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

		m_stExtent.MaxX = -1000000000;
		m_stExtent.MaxY = -1000000000;
		m_stExtent.MinX = 1000000000;
		m_stExtent.MinY = 1000000000;
		for(int i = 0; i < 4; ++i)
		{
			double rX, rY;
			GDALApplyGeoTransform( adfGeoTransform, inX[i], inY[i], &rX, &rY );
			if(m_stExtent.MaxX < rX)
				m_stExtent.MaxX = rX;
			if(m_stExtent.MinX > rX)
				m_stExtent.MinX = rX;
			if(m_stExtent.MaxY < rY)
				m_stExtent.MaxY = rY;
			if(m_stExtent.MinY > rY)
				m_stExtent.MinY = rY;
		}
	}
	else
	{
		m_stExtent.MaxX = m_nXSize;
		m_stExtent.MaxY = m_nYSize;
		m_stExtent.MinX = 0;
		m_stExtent.MinY = 0;
	}

    //nodata check
    m_paNoData = new double[m_nBandCount];
    for(size_t nBand = 0; nBand < m_nBandCount; ++nBand)
    {
        GDALRasterBand* pBand;
        if(m_poMainDataset)
            pBand = m_poMainDataset->GetRasterBand(nBand + 1);
        else
            pBand = m_poDataset->GetRasterBand(nBand + 1);
        if(!pBand)
            continue;

        int         bGotNodata;
	    m_paNoData[nBand] = pBand->GetNoDataValue(&bGotNodata );
        if(bGotNodata)
            continue;
        else
            m_paNoData[nBand] = NOTNODATA;
        switch((wxGISEnumRasterDatasetType)m_nSubType)
        {
        case enumRasterGif:
            {
		        char** papszMetadata = pBand->GetMetadata();
                const char* pszBk = CSLFetchNameValue(papszMetadata, "GIF_BACKGROUND");
                if(pszBk)
                    m_paNoData[nBand] = atof(pszBk);
            }
            break;
        };
    }

	m_bIsOpened = true;
    m_bIsReadOnly = bReadOnly;
	return true;
}

const OGRSpatialReferenceSPtr wxGISRasterDataset::GetSpatialReference(void)
{
	if(!m_bIsOpened)
		if(!Open(true))
			return OGRSpatialReferenceSPtr();
	if(m_pSpaRef)
		return m_pSpaRef;
	if(	m_poDataset )
	{
        const char* pszProjection = m_poDataset->GetProjectionRef();
        if(CPLStrnlen(pszProjection, 10) > 0)
        {
            m_pSpaRef = boost::make_shared<OGRSpatialReference>(m_poDataset->GetProjectionRef());
		    return m_pSpaRef;
        }
	}
	return OGRSpatialReferenceSPtr();
}

OGREnvelope wxGISRasterDataset::GetEnvelope(void)
{
	return m_stExtent;
}

GDALDataType wxGISRasterDataset::GetDataType(void)
{
	return m_nDataType;
}

wxString wxGISRasterDataset::GetName(void)
{
    wxString sOut;
    if(EQUALN(m_sPath, "/vsizip", 7))
        sOut = wxString(CPLGetFilename(m_sPath), wxCSConv(wxT("cp-866")));//TODO: Get from config cp-866
    else
        sOut = wxString(CPLGetFilename(m_sPath), wxConvUTF8);
	return sOut;
}

bool wxGISRasterDataset::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    Close();

	wxCriticalSectionLocker locker(m_CritSect);
    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)
        return false;

    CPLString szCopyFileName;
	CPLString szFileName = CPLGetBasename(m_sPath);

    char** papszFileCopiedList = NULL;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
		CPLString szNewDestFileName = GetUniqPath(papszFileList[i], szDestPath, szFileName);
        papszFileCopiedList = CSLAddString(papszFileCopiedList, szNewDestFileName);
        szCopyFileName = szNewDestFileName;
        if(!CopyFile(szNewDestFileName, papszFileList[i], pTrackCancel))
		{
            // Try to put the ones we moved back. 
            for( --i; i >= 0; i-- )
                DeleteFile( papszFileCopiedList[i] );

			CSLDestroy( papszFileList );
			CSLDestroy( papszFileCopiedList );
            return false;
		}
    }
    
    bool bRet = true;
    switch(m_nSubType)
    {
	case enumRasterSAGA://write some internal info to files (e.g. sgrd mgrd -> new file name
        {
            CPLString sMGRDFileName(CPLFormFilename(szDestPath, CPLGetFilename(szCopyFileName), "mgrd"));
            CPLXMLNode *psXMLRoot = CPLParseXMLFile(sMGRDFileName);
            if(psXMLRoot)
            {
                CPLXMLNode *pNode = psXMLRoot->psNext;//SAGA_METADATA
                bRet = CPLSetXMLValue(pNode, "SOURCE.FILE", CPLFormFilename(szDestPath, CPLGetFilename(szCopyFileName), "sgrd"));
                if(bRet)
                    bRet = CPLSerializeXMLTreeToFile(psXMLRoot, sMGRDFileName);
                CPLDestroyXMLNode( psXMLRoot );
            }
        }
		break;
    default:
		break;
    };

    //m_sPath = szCopyFileName;

	CSLDestroy( papszFileList );
	CSLDestroy( papszFileCopiedList );
	return bRet;
}

bool wxGISRasterDataset::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    Close();

	wxCriticalSectionLocker locker(m_CritSect);
    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)
        return false;

	CPLString szFileName = CPLGetBasename(m_sPath);

    char** papszMovedFileList = NULL;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
		CPLString szNewDestFileName = GetUniqPath(papszFileList[i], szDestPath, szFileName);
        papszMovedFileList = CSLAddString(papszMovedFileList, szNewDestFileName);
        if(!MoveFile(szNewDestFileName, papszFileList[i], pTrackCancel))
		{
            // Try to put the ones we moved back. 
            pTrackCancel->Reset();
            for( --i; i >= 0; i-- )
                MoveFile( papszFileList[i], papszMovedFileList[i], pTrackCancel);

			CSLDestroy( papszFileList );
			CSLDestroy( papszMovedFileList );
            return false;
		}
    }

    bool bRet = true;
    switch(m_nSubType)
    {
	case enumRasterSAGA://write some internal info to files (e.g. sgrd mgrd -> new file name
        {
            CPLString sMGRDFileName(CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), "mgrd"));
            CPLXMLNode *psXMLRoot = CPLParseXMLFile(sMGRDFileName);
            if(psXMLRoot)
            {
                CPLXMLNode *pNode = psXMLRoot->psNext;//SAGA_METADATA
                bRet = CPLSetXMLValue(pNode, "SOURCE.FILE", CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), "sgrd"));
                if(bRet)
                    bRet = CPLSerializeXMLTreeToFile(psXMLRoot, sMGRDFileName);
                CPLDestroyXMLNode( psXMLRoot );
            }
        }
		break;
    default:
		break;
    };

    m_sPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);

	CSLDestroy( papszFileList );
	CSLDestroy( papszMovedFileList );
    return bRet;
}

bool wxGISRasterDataset::GetPixelData(void *data, int nXOff, int nYOff, int nXSize, int nYSize, int nBufXSize, int nBufYSize, GDALDataType eDT, int nBandCount, int *panBandList)
{
    CPLErrorReset();

    CPLErr err = m_poDataset->AdviseRead(nXOff, nYOff, nXSize, nYSize, nBufXSize, nBufYSize, eDT, nBandCount, panBandList, NULL);
    if(err != CE_None)
    {
        const char* pszerr = CPLGetLastErrorMsg();
		wxLogError(_("AdviseRead failed! GDAL error: %s"), wxString(pszerr, *wxConvCurrent).c_str());
        return false;
    }

    int nPixelSpace(0);
    int nLineSpace(0);
	int nBandSpace(0);
	if(nBandCount > 1)
	{
		int nDataSize = GDALGetDataTypeSize(eDT) / 8;
		nPixelSpace = nDataSize * nBandCount;
		nLineSpace = nBufXSize * nPixelSpace;
		nBandSpace = nDataSize;
	}
    err = m_poDataset->RasterIO(GF_Read, nXOff, nYOff, nXSize, nYSize, data, nBufXSize, nBufYSize, eDT, nBandCount, panBandList, nPixelSpace, nLineSpace, nBandSpace);
    if(err != CE_None)
    {
        const char* pszerr = CPLGetLastErrorMsg();
        wxLogError(_("RasterIO failed! GDAL error: %s"), wxString(pszerr, *wxConvCurrent).c_str());
        return false;
    }
	return true;
}

bool wxGISRasterDataset::WriteWorldFile(wxGISEnumWldExtType eType)
{
    CPLString sExt = CPLGetExtension(m_sPath);
    CPLString sNewExt;

	switch(eType)
	{
	case enumGISWldExt_FirstLastW:
		sNewExt += sExt[0];
		sNewExt += sExt[sExt.size() - 1];
		sNewExt += 'w';
		break;
	case enumGISWldExt_ExtPlusWX:
		sNewExt += sExt[0];
		sNewExt += sExt[sExt.size() - 1];
		sNewExt += 'w';
		sNewExt += 'x';
		break;
	case enumGISWldExt_Wld:
		sNewExt.append("wld");
		break;
	case enumGISWldExt_ExtPlusW:
		sNewExt = sExt + CPLString("w");
		break;
	};
    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	if(m_poDataset->GetGeoTransform(adfGeoTransform) != CE_None)
		return false;
	return GDALWriteWorldFile(m_sPath, sNewExt, adfGeoTransform);
}

