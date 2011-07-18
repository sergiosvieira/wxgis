/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wx/filename.h"

//#include "vrt/vrtwarpedoverview.h"
#include "gdal_rat.h"

wxGISRasterDataset::wxGISRasterDataset(CPLString sPath, wxGISEnumRasterDatasetType nType) : wxGISDataset(sPath), m_psExtent(NULL), m_bHasOverviews(false), m_bHasStats(false), m_poMainDataset(NULL), m_poDataset(NULL), m_nBandCount(0)
{
	m_bIsOpened = false;
    m_bIsReadOnly = true;
    m_nSubType = (int)nType;
	m_nType = enumGISRasterDataset;
}

wxGISRasterDataset::~wxGISRasterDataset(void)
{
	Close();
}

void wxGISRasterDataset::Close(void)
{
	if(m_bIsOpened)
    {
		m_pSpaRef.reset();
	    wxDELETE(m_psExtent);
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
    case enumRasterBmp:
	case enumRasterTiff:
	case enumRasterImg:
	case enumRasterJpeg:
	case enumRasterPng:
	case enumRasterGif:
    case enumRasterUnknown:
    default: 
        break;
    }
    szPath = m_sPath + CPLString("w");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
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
    szPath = (char*)CPLResetExtension(m_sPath, "ovr");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "rrd");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "rpb");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "rpc");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    CPLString szRpcName = CPLString(CPLGetBasename(m_sPath)) + CPLString("_rpc.txt");
    szPath = (char*)CPLFormFilename(CPLGetPath(m_sPath), (const char*)szRpcName.c_str(), NULL);
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

bool wxGISRasterDataset::Rename(wxString sNewName)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();

	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(ClearExt(sNewName));

	wxString sNewPath = PathName.GetFullPath();

	Close();
    CPLString szNewPath = sNewPath.mb_str(wxConvUTF8);

    if( !RenameFile(m_sPath, szNewPath) )
        return false;

    CPLString szOldPathAdd, szNewPathAdd;

    szOldPathAdd = m_sPath + CPLString("w");
    szNewPathAdd = szNewPath + CPLString("w");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = m_sPath + CPLString(".xml");
    szNewPathAdd = szNewPath + CPLString(".xml");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = m_sPath + CPLString(".aux");
    szNewPathAdd = szNewPath + CPLString(".aux");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = m_sPath + CPLString(".aux.xml");
    szNewPathAdd = szNewPath + CPLString(".aux.xml");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = m_sPath + CPLString(".ovr");
    szNewPathAdd = szNewPath + CPLString(".ovr");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = m_sPath + CPLString(".ovr.aux.xml");
    szNewPathAdd = szNewPath + CPLString(".ovr.aux.xml");
    RenameFile(szOldPathAdd, szNewPathAdd);

    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "xml");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "xml");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "lgo");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "lgo");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "aux");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "aux");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "ovr");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "ovr");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "rrd");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "rrd");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "rpb");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "rpb");
    RenameFile(szOldPathAdd, szNewPathAdd);
    szOldPathAdd = (char*)CPLResetExtension(m_sPath, "rpc");
    szNewPathAdd = (char*)CPLResetExtension(szNewPath, "rpc");
    RenameFile(szOldPathAdd, szNewPathAdd);

    CPLString szRpcName = CPLString(CPLGetBasename(m_sPath)) + CPLString("_rpc.txt");
    szOldPathAdd = (char*)CPLFormFilename(CPLGetPath(m_sPath), (const char*)szRpcName.c_str(), NULL);
    szRpcName = CPLString(CPLGetBasename(szNewPath)) + CPLString("_rpc.txt");
    szNewPathAdd = (char*)CPLFormFilename(CPLGetPath(szNewPath), (const char*)szRpcName.c_str(), NULL);    
    
    RenameFile(szOldPathAdd, szNewPathAdd);

    switch(m_nSubType)
    {
    case enumRasterBmp:
	case enumRasterTiff:
	case enumRasterImg:
	case enumRasterJpeg:
	case enumRasterPng:
	case enumRasterGif:
    case enumRasterUnknown:
    default: 
        break;
    }
	m_sPath = szNewPath;
	return true;
}

bool wxGISRasterDataset::Open(bool bReadOnly)
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);


    m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, bReadOnly == true ? GA_ReadOnly : GA_Update );//m_sPath.mb_str(wxConvUTF8)
    //bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]
	
	if( m_poDataset == NULL )
    {
        m_sPath.replace(m_sPath.begin(), m_sPath.end(), '\\', '/' );
        m_poDataset = (GDALDataset *) GDALOpenShared( m_sPath, bReadOnly == true ? GA_ReadOnly : GA_Update );
    }
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
		for(int i = 0; i < 4; ++i)
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
		m_pSpaRef = boost::make_shared<OGRSpatialReference>(m_poDataset->GetProjectionRef());
		//m_pSpaRef = OGRSpatialReferenceSPtr(new OGRSpatialReference(m_poDataset->GetProjectionRef()));
		return m_pSpaRef;
	}
	return OGRSpatialReferenceSPtr();
}

const OGREnvelope* wxGISRasterDataset::GetEnvelope(void)
{
	if(m_psExtent)
		return m_psExtent;
	return NULL;
}

wxString wxGISRasterDataset::GetName(void)
{
    wxString sOut;
    if(EQUALN(m_sPath, "/vsizip", 7))
        sOut = wxString(CPLGetFilename(m_sPath), wxCSConv(wxT("cp-866")));
    else
        sOut = wxString(CPLGetFilename(m_sPath), wxConvUTF8);
	return sOut;
}

bool wxGISRasterDataset::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)
        return false;

    CPLString szCopyFileName;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
        CPLString szNewDestFileName(CPLFormFilename(szDestPath, CPLGetFilename(papszFileList[i]), NULL));
        szNewDestFileName = CheckUniqPath(szNewDestFileName);
        szCopyFileName = szNewDestFileName;
        if(!CopyFile(szNewDestFileName, papszFileList[i], pTrackCancel))
            return false;
    }
    
    m_sPath = szCopyFileName;

	return true;
}

bool wxGISRasterDataset::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)
        return false;

    for(int i = 0; papszFileList[i] != NULL; ++i )
    {
        const char* szNewDestFileName = CPLFormFilename(szDestPath, CPLGetFilename(papszFileList[i]), NULL);
        if(!MoveFile(szNewDestFileName, papszFileList[i], pTrackCancel))
            return false;
    }

    m_sPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);

    return true;
}
