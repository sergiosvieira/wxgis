/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  FeatureDataset class.
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
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/sysop.h"

//#include "ogrsf_frmts/shape/ogrshape.h"

#include "wx/filename.h"
#include "wx/file.h"

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	OGRGeometry* pGeometry = (OGRGeometry*)hFeature;
    if(!pGeometry)
		return;
	OGREnvelope Env;
	pGeometry->getEnvelope(&Env);
    if(fabs(Env.MinX - Env.MaxX) < DELTA)
    {
        //wxLogDebug(wxT("%f %f"), Env.MinX, Env.MaxX);
        Env.MaxX += DELTA;
    }
    if(fabs(Env.MinY - Env.MaxY) < DELTA)
    {
        //wxLogDebug(wxT("%f %f"), Env.MinX, Env.MaxX);
        Env.MaxY += DELTA;
    }

	pBounds->minx = Env.MinX;
	pBounds->maxx = Env.MaxX;
	pBounds->miny = Env.MinY;
	pBounds->maxy = Env.MaxY;
}

wxGISFeatureDatasetSPtr CreateVectorLayer(CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *poFields, OGRSpatialReference *poSpatialRef, OGRwkbGeometryType eGType, char ** papszDataSourceOptions, char ** papszLayerOptions)
{
    CPLErrorReset();
    poFields->Reference();
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( wgWX2MB(sDriver) );
    if(poDriver == NULL)
    {
        wxString sErr = wxString::Format(_("The driver '%s' is not available! OGR error: "), sDriver.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        return wxGISFeatureDatasetSPtr();
    }

    wxGISEnumVectorDatasetType nSubType = enumVecUnknown;
    if(sDriver == wxString(wxT("LIBKML")) && sExt.CmpNoCase(wxT("kml")) == 0)
        nSubType = enumVecKML;
    else if(sDriver == wxString(wxT("LIBKML")) && sExt.CmpNoCase(wxT("kmz")) == 0)
        nSubType = enumVecKMZ;
    else if(sDriver == wxString(wxT("GML")) && sExt.CmpNoCase(wxT("gml")) == 0)
        nSubType = enumVecGML;
    else if(sDriver == wxString(wxT("DXF")))
        nSubType = enumVecDXF;
    else if(sDriver == wxString(wxT("MapInfo File")) && sExt == wxString(wxT("tab")))
        nSubType = enumVecMapinfoTab;
    else if(sDriver == wxString(wxT("MapInfo File")) && sExt == wxString(wxT("mif")))
        nSubType = enumVecMapinfoMif;
    else if(sDriver == wxString(wxT("ESRI Shapefile")))
        nSubType = enumVecESRIShapefile;

    CPLString sFullPath = CPLFormFilename(sPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    OGRDataSource *poDS = poDriver->CreateDataSource(sFullPath, papszDataSourceOptions );
    if(poDS == NULL)
    {
        wxString sErr = wxString::Format(_("Error create the output file '%s'! OGR error: "), sName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        return wxGISFeatureDatasetSPtr();
    }

    sName.Replace(wxT("."), wxT("_"));
    sName.Replace(wxT(" "), wxT("_"));
    sName.Replace(wxT("&"), wxT("_"));
    sName.Truncate(27);
    CPLString szName;
    if(nSubType == enumVecKMZ)
        szName = Transliterate(CPLString(sName.mb_str()).c_str());//CPLString(sName.mb_str(wxConvUTF8));//wxCSConv(wxT("cp-866"))));
    else
        szName = CPLString(sName.mb_str());

	OGRLayer *poLayerDest = poDS->CreateLayer(szName, poSpatialRef, eGType, papszLayerOptions );
    if(poLayerDest == NULL)
    {
        delete poSpatialRef;
        wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sName.c_str());
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        return wxGISFeatureDatasetSPtr();
    }

    if(nSubType != enumVecDXF)
    {
        for(size_t i = 0; i < poFields->GetFieldCount(); ++i)
        {
            OGRFieldDefn *pField = poFields->GetFieldDefn(i);
            if(nSubType == enumVecKML || nSubType == enumVecKMZ)
            {
                wxString sFieldName = wgMB2WX(pField->GetNameRef());
                pField->SetName(sFieldName.mb_str(wxConvUTF8));
            }

	        if( poLayerDest->CreateField( pField ) != OGRERR_NONE )
	        {
                wxString sErr = wxString::Format(_("Error create the output layer '%s'! OGR error: "), sName.c_str());
                CPLString sFullErr(sErr.mb_str());
                sFullErr += CPLGetLastErrorMsg();
                CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
                return wxGISFeatureDatasetSPtr();
            }
        }
    }
    poFields->Release();

    wxGISFeatureDatasetSPtr pDataSet = boost::make_shared<wxGISFeatureDataset>(poDS, poLayerDest, sFullPath, nSubType);
    return pDataSet;
}

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------
wxGISFeatureDataset::wxGISFeatureDataset(OGRDataSource *poDS, OGRLayer* poLayer, CPLString sPath, wxGISEnumVectorDatasetType nType) : wxGISDataset(sPath), m_poDS(NULL), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1), m_pGeometrySet(NULL)
{
	m_bIsOpened = false;
	m_poDS = poDS;
	m_poLayer = poLayer;
    if(m_poLayer == NULL)
        m_nType = enumGISContainer;
    else
    {
        m_nType = enumGISFeatureDataset;
        m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
    }
    m_nSubType = (int)nType;

	m_bIsOpened = true;

    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
        m_Encoding = wxFONTENCODING_UTF8;
    else
        m_Encoding = wxFONTENCODING_DEFAULT;
}

wxGISFeatureDataset::wxGISFeatureDataset(CPLString sPath, wxGISEnumVectorDatasetType nType) : wxGISDataset(sPath), m_poDS(NULL), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1), m_pGeometrySet(NULL)
{
	m_bIsOpened = false;

    m_nType = enumGISFeatureDataset;
    m_nSubType = (int)nType;
    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
        m_Encoding = wxFONTENCODING_UTF8;
    else
        m_Encoding = wxFONTENCODING_DEFAULT;
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
	Close();
}

void wxGISFeatureDataset::Close(void)
{
    DeleteQuadTree();

	wxDELETE(m_psExtent);
    wsDELETE(m_pGeometrySet);

    if(m_poDS && !m_FeaturesMap.empty())
    {
        for(m_IT = m_FeaturesMap.begin(); m_IT != m_FeaturesMap.end(); ++m_IT)
            OGRFeature::DestroyFeature(m_IT->second);
    }
    m_bIsGeometryLoaded = false;
	m_bIsOpened = false;
    if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
	m_poDS = NULL;
}

size_t wxGISFeatureDataset::GetSubsetsCount(void)
{
    if(!m_bIsOpened)
        if(!Open(0))
            return 0;
    if(m_poDS)
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDatasetSPtr wxGISFeatureDataset::GetSubset(size_t nIndex)
{
    if(!m_bIsOpened)
        if(!Open(0))
            return wxGISDatasetSPtr();
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
            wxGISFeatureDatasetSPtr pDataSet = boost::make_shared<wxGISFeatureDataset>(m_poDS, poLayer, "", (wxGISEnumVectorDatasetType)m_nSubType);
            pDataSet->SetEncoding(m_Encoding);
            return boost::static_pointer_cast<wxGISDataset>(pDataSet);
        }
    }
    return wxGISDatasetSPtr();
}

OGRLayer* wxGISFeatureDataset::GetLayerRef(int iLayer)
{
	if(m_bIsOpened && m_poLayer)
	{
		m_poLayer->ResetReading();
		return m_poLayer;
	}
	else
	{
		if(Open(iLayer))
			return GetLayerRef(iLayer);
		else
			return NULL;
	}
	return NULL;
}

bool wxGISFeatureDataset::Delete(int iLayer)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(m_poDS && m_poDS->Dereference() <= 0)
    {
        OGRDataSource::DestroyDataSource( m_poDS );
        m_poDS = NULL;
    }
    
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

char **wxGISFeatureDataset::GetFileList()
{
    char **papszFileList = NULL;
    CPLString szPath;
    //papszFileList = CSLAddString( papszFileList, osIMDFile );
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
        szPath = (char*)CPLResetExtension(m_sPath, "shx");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "dbf");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "prj");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "qix");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "sbn");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "sbx");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "shp.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecMapinfoTab:
        szPath = (char*)CPLResetExtension(m_sPath, "dat");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "id");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "ind");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "map");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "tab.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "tab.metadata.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecMapinfoMif:
        szPath = (char*)CPLResetExtension(m_sPath, "mid");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "mif.metadata.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "mif.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecGML:
        szPath = (char*)CPLResetExtension(m_sPath, "gfs");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecKML:
    case enumVecKMZ:
    case enumVecDXF:
    case enumVecUnknown:
    default: 
        break;
    }

    szPath = (char*)CPLResetExtension(m_sPath, "cpg");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "osf");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

bool wxGISFeatureDataset::Rename(wxString sNewName)
{
	wxCriticalSectionLocker locker(m_CritSect);

	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(sNewName);

	wxString sNewPath = PathName.GetFullPath();

	Close();
    CPLString szNewPath = sNewPath.mb_str(wxConvUTF8);

    if( !RenameFile(m_sPath, szNewPath) )
        return false;
	m_sPath = szNewPath;
    RenameFile(CPLResetExtension(m_sPath, "cpg"), CPLResetExtension(szNewPath, "cpg"));
    RenameFile(CPLResetExtension(m_sPath, "osf"), CPLResetExtension(szNewPath, "osf"));

    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
        RenameFile(CPLResetExtension(m_sPath, "shx"), CPLResetExtension(szNewPath, "shx"));
        RenameFile(CPLResetExtension(m_sPath, "dbf"), CPLResetExtension(szNewPath, "dbf"));
        RenameFile(CPLResetExtension(m_sPath, "prj"), CPLResetExtension(szNewPath, "prj"));
        RenameFile(CPLResetExtension(m_sPath, "qix"), CPLResetExtension(szNewPath, "qix"));
        RenameFile(CPLResetExtension(m_sPath, "sbn"), CPLResetExtension(szNewPath, "sbn"));
        RenameFile(CPLResetExtension(m_sPath, "sbx"), CPLResetExtension(szNewPath, "sbx"));
        RenameFile(CPLResetExtension(m_sPath, "shp.xml"), CPLResetExtension(szNewPath, "shp.xml"));
        break;
    case enumVecMapinfoTab:
        RenameFile(CPLResetExtension(m_sPath, "dat"), CPLResetExtension(szNewPath, "dat"));
        RenameFile(CPLResetExtension(m_sPath, "id"), CPLResetExtension(szNewPath, "id"));
        RenameFile(CPLResetExtension(m_sPath, "ind"), CPLResetExtension(szNewPath, "ind"));
        RenameFile(CPLResetExtension(m_sPath, "map"), CPLResetExtension(szNewPath, "map"));
        RenameFile(CPLResetExtension(m_sPath, "tab.xml"), CPLResetExtension(szNewPath, "tab.xml"));
        RenameFile(CPLResetExtension(m_sPath, "tab.metadata.xml"), CPLResetExtension(szNewPath, "tab.metadata.xml"));
        break;
    case enumVecMapinfoMif:
        RenameFile(CPLResetExtension(m_sPath, "mid"), CPLResetExtension(szNewPath, "mid"));
        RenameFile(CPLResetExtension(m_sPath, "mif.metadata.xml"), CPLResetExtension(szNewPath, "mif.metadata.xml"));
        RenameFile(CPLResetExtension(m_sPath, "mif.xml"), CPLResetExtension(szNewPath, "mif.xml"));
        break;
    case enumVecGML:
        RenameFile(CPLResetExtension(m_sPath, "gfs"), CPLResetExtension(szNewPath, "gfs"));
    case enumVecKML:
    case enumVecKMZ:
    case enumVecDXF:
    case enumVecUnknown:
        break;
    default: 
        return false;
    }
	return true;
}

bool wxGISFeatureDataset::Open(int iLayer)
{
	if(m_bIsOpened)
		return true;

    m_pGeometrySet = new wxGISGeometrySet(true);
    m_pGeometrySet->Reference();

	wxCriticalSectionLocker locker(m_CritSect);

    m_poDS = OGRSFDriverRegistrar::Open( m_sPath, FALSE );
    //bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]
	if( m_poDS == NULL )
	{
		m_sPath.replace( m_sPath.begin(), m_sPath.end(), '\\', '/' );
        m_poDS = OGRSFDriverRegistrar::Open( m_sPath, FALSE );
    }

	if( m_poDS == NULL )
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISFeatureDataset: Open failed! Path '%s'. OGR error: %s"), m_sPath.c_str(), wgMB2WX(err));
		wxLogError(sErr);
		return false;
	}

    wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
    if(FEnc != wxFONTENCODING_DEFAULT)
        m_Encoding = FEnc;

    int nLayerCount = m_poDS->GetLayerCount();
    if(nLayerCount == 1)
    {
	    m_poLayer = m_poDS->GetLayer(iLayer);
        //wxString sStatement(wxT("SELECT FID FROM gns-shp-utf-rs"));//  = wxString::Format
        //m_poLayer = m_poDS->ExecuteSQL(wgWX2MB(sStatement), NULL, NULL);
	    if(m_poLayer)
	    {
            //OGREncoding nEnc = m_poLayer->GetEncoding();
            //const char *txxt = m_poLayer->GetLayerDefn()->GetName();
		    //bool bOLCFastGetExtent = pOGRLayer->TestCapability(OLCFastGetExtent);
      //      if(bOLCFastGetExtent)
      //      {
		    //    m_psExtent = new OGREnvelope();
		    //    if(m_poLayer->GetExtent(m_psExtent, true) != OGRERR_NONE)
		    //    {
		    //	    wxDELETE(m_psExtent);
		    //	    m_psExtent = NULL;
		    //    }
      //      }

		    //bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
		    //if(!bOLCFastSpatialFilter)
		    //{
		    //	if(m_psExtent)
		    //	{
		    //		OGREnvelope Env = *m_psExtent;
		    //		CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
		    //		m_pQuadTree = CPLQuadTreeCreate(&Rect, GetFeatureBoundsFunc);
		    //	}

		    //	//wxFileName FileName(m_sPath);
		    //	//wxString SQLStatement = wxString::Format(wxT("CREATE SPATIAL INDEX ON %s"), FileName.GetName().c_str());
		    //	//m_poDS->ExecuteSQL(wgWX2MB(SQLStatement), NULL, NULL);
		    //}
	    //	bool bOLCRandomRead = pOGRLayer->TestCapability(OLCRandomRead);
	    //	bool bOLCSequentialWrite = pOGRLayer->TestCapability(OLCSequentialWrite);
	    //	bool bOLCRandomWrite = pOGRLayer->TestCapability(OLCRandomWrite);
	    //	bool bOLCFastFeatureCount = pOGRLayer->TestCapability(OLCFastFeatureCount);
	    //	bool bOLCFastGetExtent = pOGRLayer->TestCapability(OLCFastGetExtent);
	    	//bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
      //      if(!bOLCFastSetNextByIndex)
      //          LoadGeometry();
	    //	bool bOLCCreateField = pOGRLayer->TestCapability(OLCCreateField);
	    //	bool bOLCDeleteFeature = pOGRLayer->TestCapability(OLCDeleteFeature);
	    //	bool bOLCTransactions = pOGRLayer->TestCapability(OLCTransactions);
	    //	wxString sFIDColName = wgMB2WX(pOGRLayer->GetFIDColumn());

		    m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);


		    //m_OGRFeatureArray.reserve(m_poLayer->GetFeatureCount(true));

		    //size_t count(0);
		    //OGRFeature *poFeature;
		    //while( (count < CACHE_SIZE) && ((poFeature = m_poLayer->GetNextFeature()) != NULL) )
		    //{
		    //	AddFeature(poFeature);
		    //	count++;
		    //}
	    }
    }
    else
        m_nType = enumGISContainer;
	m_bIsOpened = true;
	return true;
}

wxString wxGISFeatureDataset::GetName(void)
{
	if(!m_bIsOpened)
		if(!Open(0))
            return wxEmptyString;
    wxString sOut;
	if(	m_poLayer )
    {
        if(EQUALN(m_sPath, "/vsizip", 7))
            sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxCSConv(wxT("cp-866")));//wgMB2WX
        else
            sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxConvUTF8);//wgMB2WX
        //if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
        //    sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxConvUTF8);//wgMB2WX
        //else
        //{
        //    wxCSConv conv(m_Encoding);
        //    sOut = conv.cMB2WX(m_poLayer->GetLayerDefn()->GetName());
        //    if(sOut.IsEmpty())
        //        sOut = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        //}
    }
    else
    {
        if(EQUALN(m_sPath, "/vsizip", 7))
            sOut = wxString(CPLGetBasename(m_sPath), wxCSConv(wxT("cp-866")));
        else
            sOut = wxString(CPLGetBasename(m_sPath), wxConvUTF8);
       //if(m_Encoding == wxFONTENCODING_DEFAULT)
        //    sOut = wgMB2WX(CPLGetBasename(m_sPath));
        //else
        //{
        //    wxCSConv conv(m_Encoding);
        //    sOut = conv.cMB2WX(CPLGetBasename(m_sPath));
        //    if(sOut.IsEmpty())
        //        sOut = wgMB2WX(CPLGetBasename(m_sPath));
        //}
    }
    return sOut;
}

OGRSpatialReference* wxGISFeatureDataset::GetSpatialReference(void)
{
	if(!m_bIsOpened)
		if(!Open(0))
			return NULL;
	if(	m_poLayer )
    {
        OGRSpatialReference* pSpatialRef = m_poLayer->GetSpatialRef();
        if(pSpatialRef)
            return pSpatialRef;
    }
	return NULL;
}

OGRDataSource* wxGISFeatureDataset::GetDataSource(void)
{
    if(m_poDS)
        return m_poDS;
    if(!m_bIsOpened)
        if(!Open(0))
            return NULL;
    return m_poDS;
}

OGREnvelope* wxGISFeatureDataset::GetEnvelope(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(m_psExtent)
        return m_psExtent;
    if(!m_bIsOpened)
        if(!Open(0))
            return NULL;
    if(	m_poLayer )
    {
        bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent);
        if(bOLCFastGetExtent)
        {
            m_psExtent = new OGREnvelope();
            if(m_poLayer->GetExtent(m_psExtent, true) == OGRERR_NONE)
            {
                if(fabs(m_psExtent->MinX - m_psExtent->MaxX) < DELTA)
                {
                    m_psExtent->MaxX += 1;
                    m_psExtent->MinX -= 1;
                }
                if(fabs(m_psExtent->MinY - m_psExtent->MaxY) < DELTA)
                {
                    m_psExtent->MaxY += 1;
                    m_psExtent->MinY -= 1;
                }
                CreateQuadTree(m_psExtent);
                return m_psExtent;
            }
        }
        //load all features
        LoadGeometry();
        if(m_bIsGeometryLoaded)
            return m_psExtent;
        else
        {
            if(m_poLayer->GetExtent(m_psExtent, true) == OGRERR_NONE)
            {
                if(fabs(m_psExtent->MinX - m_psExtent->MaxX) < DELTA)
                {
                    m_psExtent->MaxX += 1;
                    m_psExtent->MinX -= 1;
                }
                if(fabs(m_psExtent->MinY - m_psExtent->MaxY) < DELTA)
                {
                    m_psExtent->MaxY += 1;
                    m_psExtent->MinY -= 1;
                }

                return m_psExtent;
            }
            return NULL;
        }
    }
    return NULL;
}

//void wxGISFeatureDataset::SetSpatialFilter(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY)
//{
//	if(!m_bIsOpened)
//		if(!Open(0))
//			return;
//	if(	m_poLayer )
//	{
		//bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
//		m_poLayer->SetSpatialFilterRect(dfMinX, dfMinY, dfMaxX, dfMaxY);
//	}
//}

OGRFeature* wxGISFeatureDataset::GetAt(long nIndex) //const    0 based
{
	wxCriticalSectionLocker locker(m_CritSect);
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < GetSize());
	bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(!bOLCFastSetNextByIndex && m_FeaturesMap.empty())
        LoadGeometry();
    if(bOLCFastSetNextByIndex)
    {
        m_poLayer->SetNextByIndex(nIndex);
        OGRFeature* pFeature = m_poLayer->GetNextFeature();
        //store in m_FeaturesMap
        return pFeature;
    }
    else
    {
        wxCriticalSectionLocker locker(m_CritSect);
        if(m_FeaturesMap.size() - 1 < nIndex)
        {
            m_poLayer->SetNextByIndex(nIndex);
            OGRFeature* pFeature = m_poLayer->GetNextFeature();
            if(!pFeature)
                return NULL;
            m_FeaturesMap[pFeature->GetFID()] = pFeature;
            return pFeature->Clone();
        }
        std::map<long, OGRFeature*>::iterator it = m_FeaturesMap.begin();
        std::advance( it, nIndex );
        if(it == m_FeaturesMap.end())
        {
            //m_poLayer->SetNextByIndex(nIndex);
            //OGRFeature* pFeature = m_poLayer->GetNextFeature();
            //if(!pFeature)
                return NULL;
            //m_FeaturesMap[pFeature->GetFID()] = pFeature;
            //return pFeature->Clone();
        }
        return it->second->Clone();
    }
    return NULL;
}

OGRFeature* wxGISFeatureDataset::operator [](long nIndex) //const    same as GetAt
{
	return GetAt(nIndex);
}

wxString wxGISFeatureDataset::GetAsString(long row, int col)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(GetSize() <= row)
		return wxEmptyString;
	else
	{
        //wxCriticalSectionLocker locker(m_CritSect);
        if(m_FieldCount == -1)
        {
            OGRFeatureDefn *pFDef = m_poLayer->GetLayerDefn();
            m_FieldCount = pFDef->GetFieldCount();
        }

        long pos = row * m_FieldCount + col;
        if(m_FeatureStringData.GetCount() > pos)
            if(m_FeatureStringData[pos] != wxEmptyString)
                return m_FeatureStringData[pos];

		OGRFeature* pFeature = GetAt(row);
        if(!pFeature)
    		return wxEmptyString;
		OGRFieldDefn* pDef = pFeature->GetFieldDefnRef(col);
        wxString sOut(wxT(" "));
		switch(pDef->GetType())
		{
		case OFTDate:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				sOut = dt.Format(_("%d-%m-%Y"));//wxString::Format(_("%.2u-%.2u-%.4u"), day, mon, year );
                if(sOut == wxEmptyString)
                    sOut = wxT("NULL");
			}
		case OFTTime:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				sOut = dt.Format(_("%H:%M:%S"));//wxString::Format(_("%.2u:%.2u:%.2u"), hour, min, sec);
                if(sOut == wxEmptyString)
                    sOut = wxT("NULL");
			}
		case OFTDateTime:
			{
				int year, mon, day, hour, min, sec, flag;
				BOOL bRes = pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
                if(bRes)
                {
    				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
	    			sOut = dt.Format(_("%d-%m-%Y %H:%M:%S"));//wxString::Format(_("%.2u-%.2u-%.4u %.2u:%.2u:%.2u"), day, mon, year, hour, min, sec);
                }
                if(sOut == wxEmptyString)
                    sOut = wxT("NULL");
			}
		case OFTReal:
			sOut = wxString::Format(_("%.6f"), pFeature->GetFieldAsDouble(col));
            if(sOut == wxEmptyString)
                sOut = wxT("NULL");
		default:
            if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
            {
                sOut = wgMB2WX(pFeature->GetFieldAsString(col));
                if(sOut == wxEmptyString)
                    sOut = wxT(" ");
            }
            else
            {
                wxCSConv conv(m_Encoding);
                sOut = conv.cMB2WX(pFeature->GetFieldAsString(col));
                if(sOut.IsEmpty())
                    sOut = wgMB2WX(pFeature->GetFieldAsString(col));
                if(sOut == wxEmptyString)
                    sOut = wxT(" ");
            }
		}
        OGRFeature::DestroyFeature(pFeature);

        if(pos < 1000000)
        {
            if(m_FeatureStringData.GetCount() <= pos)
            {
                m_FeatureStringData.Add(wxEmptyString, pos - m_FeatureStringData.GetCount());
                m_FeatureStringData.Add(sOut);
            }
            else
                m_FeatureStringData[pos] = sOut;
        }
		return sOut;//wgMB2WX(GetAt(row)->GetFieldAsString(col));
	}
}

wxGISGeometrySet* wxGISFeatureDataset::GetGeometries(void)
{
    if(!m_bIsGeometryLoaded)
        LoadGeometry();
    m_pGeometrySet->Reference();
    return m_pGeometrySet;
}

wxGISGeometrySet* wxGISFeatureDataset::GetGeometrySet(wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    //spatial reference of pQFilter
	wxGISGeometrySet* pGISGeometrySet = NULL;
	if(pQFilter)
	{
		wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
		if(pSpaFil)
		{
			OGREnvelope Env = pSpaFil->GetEnvelope();
            if(m_pQuadTree)
            {
			    int count(0);
			    CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
			    OGRGeometry** pGeometryArr = (OGRGeometry**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
                pGISGeometrySet = new wxGISGeometrySet(false);
			    for(int i = 0; i < count; ++i)
			    {
				    if(pTrackCancel && !pTrackCancel->Continue())
					    break;
				    pGISGeometrySet->AddGeometry(pGeometryArr[i], m_pGeometrySet->operator[](pGeometryArr[i]));
			    }
			    wxDELETEA( pGeometryArr );
                pGISGeometrySet->Reference();
                return pGISGeometrySet;
            }
		}
	}
	else
	{
        if(m_bIsGeometryLoaded)
            return GetGeometries();
        else
        {
            LoadGeometry();
            return GetGeometrySet(pQFilter, pTrackCancel);
        }
	}
	return pGISGeometrySet;
}

//wxGISFeatureSet* wxGISFeatureDataset::GetFeatureSet(IQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
//{
//    //spatial reference of pQFilter
//	wxGISFeatureSet* pGISFeatureSet = NULL;
//	if(pQFilter)
//	{
//		wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
//		if(pSpaFil)
//		{
//			OGREnvelope Env = pSpaFil->GetEnvelope();
//            if(m_pQuadTree)
//            {
//			    int count(0);
//			    CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
//			    OGRFeature** pFeatureArr = (OGRFeature**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
//                pGISFeatureSet = new wxGISFeatureSet(m_FeaturesMap.size());
//			    for(int i = 0; i < count; i++)
//			    {
//				    if(pTrackCancel && !pTrackCancel->Continue())
//					    break;
//				    pGISFeatureSet->AddFeature(pFeatureArr[i]);
//			    }
//			    wxDELETEA( pFeatureArr );
//                return pGISFeatureSet;
//            }
//            else
//            {
//                if(!m_bIsOpened)
//                    if(!Open(0))
//                        return NULL;
//                if(m_poLayer)
//                {
// 		            bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
//                    if(bOLCFastSpatialFilter)
//                    {
//                        m_poLayer->SetSpatialFilterRect(Env.MinX, Env.MinY, Env.MaxX, Env.MaxY);
//                        pGISFeatureSet = new wxGISFeatureSet(m_poLayer->GetFeatureCount(true));
//
// 		                OGRFeature *poFeature;
//		                while((poFeature = m_poLayer->GetNextFeature()) != NULL )
//                        {
//                            pGISFeatureSet->AddFeature(poFeature);
//                            if(pTrackCancel && !pTrackCancel->Continue())
//                                break;
//                        }
//                        return pGISFeatureSet;
//                    }
//                    else
//                    {
//                        LoadFeatures();
//                        if(m_bIsFeaturesLoaded)
//                            return GetFeatureSet(pQFilter, pTrackCancel);
//                        else
//                        {
//                            pGISFeatureSet = new wxGISFeatureSet(m_poLayer->GetFeatureCount(true));
//
// 		                    OGRFeature *poFeature;
//		                    while((poFeature = m_poLayer->GetNextFeature()) != NULL )
//                            {
//                                pGISFeatureSet->AddFeature(poFeature);
//                                if(pTrackCancel && !pTrackCancel->Continue())
//                                    break;
//                            }
//                            return pGISFeatureSet;
//                        }
//                    }
//                }
//            }
//		}
//	}
//	else
//	{
//        if(m_FeaturesMap.empty())
//        {
//            LoadFeatures();
//            if(m_bIsFeaturesLoaded)
//                return GetFeatureSet(pQFilter, pTrackCancel);
//            else
//            {
//                pGISFeatureSet = new wxGISFeatureSet(m_poLayer->GetFeatureCount(true));
//
//                OGRFeature *poFeature;
//                while((poFeature = m_poLayer->GetNextFeature()) != NULL )
//                {
//                    pGISFeatureSet->AddFeature(poFeature);
//                    if(pTrackCancel && !pTrackCancel->Continue())
//                        break;
//                }
//                return pGISFeatureSet;
//            }
//        }
//        else
//        {
//            pGISFeatureSet = new wxGISFeatureSet(m_FeaturesMap.size());
//            for(Iterator IT = m_FeaturesMap.begin(); IT != m_FeaturesMap.end(); ++IT)
//		    {
//			    pGISFeatureSet->AddFeature(IT->second);
//			    if(pTrackCancel && !pTrackCancel->Continue())
//				    break;
//		    }
//	        return pGISFeatureSet;
//        }
//	}
//	return pGISFeatureSet;
//}

void wxGISFeatureDataset::CreateQuadTree(OGREnvelope* pEnv)
{
    DeleteQuadTree();
    CPLRectObj Rect = {pEnv->MinX, pEnv->MinY, pEnv->MaxX, pEnv->MaxY};
    m_pQuadTree = CPLQuadTreeCreate(&Rect, GetGeometryBoundsFunc);
}

void wxGISFeatureDataset::DeleteQuadTree(void)
{
	if(m_pQuadTree)
    {
		CPLQuadTreeDestroy(m_pQuadTree);
        m_pQuadTree = NULL;
    }
}

size_t wxGISFeatureDataset::GetSize(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(m_bIsGeometryLoaded && m_pGeometrySet)
        return m_pGeometrySet->GetSize();
    if(!m_bIsOpened)
        if(!Open(0))
            return NULL;
    if(	m_poLayer )
    {
    	bool bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount);
        if(bOLCFastFeatureCount)
            return m_poLayer->GetFeatureCount(true);
        //load all features
        LoadGeometry();
        if(m_bIsGeometryLoaded && m_pGeometrySet)
            return m_pGeometrySet->GetSize();
        else
            return m_poLayer->GetFeatureCount(true);
    }
    return 0;
}

void wxGISFeatureDataset::LoadGeometry(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(m_bIsGeometryLoaded)
        return;

    if(!m_bIsOpened)
        if(!Open(0))
            return;
    if(	m_poLayer )
    {
        m_poLayer->ResetReading();
        wxString sFIDColName = wgMB2WX(m_poLayer->GetFIDColumn());
        bool bHasFID = true;
        if(sFIDColName.IsEmpty())
            bHasFID = false;
        long counter = 1;

        bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent);
        if(!m_psExtent)
        {
            m_psExtent = new OGREnvelope();
            if( bOLCFastGetExtent )
            {
                m_poLayer->GetExtent(m_psExtent, true);
                if(fabs(m_psExtent->MinX - m_psExtent->MaxX) < DELTA)
                {
                    m_psExtent->MaxX += 1;
                    m_psExtent->MinX -= 1;
                }
                if(fabs(m_psExtent->MinY - m_psExtent->MaxY) < DELTA)
                {
                    m_psExtent->MaxY += 1;
                    m_psExtent->MinY -= 1;
                }

                CreateQuadTree(m_psExtent);
            }
        }

		OGRFeature *poFeature;
        bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
            if(!poFeature->GetDefnRef())
            {
                OGRFeature::DestroyFeature(poFeature);
                continue;
            }
            long nFID;
            if(!bHasFID)
                nFID = counter;
            else
                nFID = poFeature->GetFID();

            OGRGeometry* pGeom(NULL);
            //store features in array for speed
            if(bOLCFastSetNextByIndex)
                pGeom = poFeature->StealGeometry();
            else
            {
                m_FeaturesMap[nFID] = poFeature;
                OGRGeometry* poFeatureGeom = poFeature->GetGeometryRef();
                if(poFeatureGeom)
                    pGeom = poFeatureGeom->clone();
            }

            if(pGeom && m_pGeometrySet)
                m_pGeometrySet->AddGeometry(pGeom, nFID);

            if(bOLCFastGetExtent)
                CPLQuadTreeInsert(m_pQuadTree, (void*)pGeom);
            else
            {
                if(pGeom)
                {
                    OGREnvelope Env;
                    pGeom->getEnvelope(&Env);
                    m_psExtent->Merge(Env);
                }
            }
            //store field as string in array
			counter++;
            if(bOLCFastSetNextByIndex)
                OGRFeature::DestroyFeature(poFeature);
		}

        if(fabs(m_psExtent->MinX - m_psExtent->MaxX) < DELTA)
        {
            m_psExtent->MaxX += 1;
            m_psExtent->MinX -= 1;
        }
        if(fabs(m_psExtent->MinY - m_psExtent->MaxY) < DELTA)
        {
            m_psExtent->MaxY += 1;
            m_psExtent->MinY -= 1;
        }


        if(!bOLCFastGetExtent && m_pGeometrySet)
        {
            CreateQuadTree(m_psExtent);
            m_pGeometrySet->Reset();
            OGRGeometry* pGeom;
            while((pGeom = m_pGeometrySet->Next()) != NULL)
                CPLQuadTreeInsert(m_pQuadTree, pGeom);
        }
    }
    m_bIsGeometryLoaded = true;
}

void wxGISFeatureDataset::UnloadGeometry(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(!m_bIsGeometryLoaded)
        return;
    DeleteQuadTree();
    m_pGeometrySet->Clear();

    if(!m_FeaturesMap.empty())
    {
        for(m_IT = m_FeaturesMap.begin(); m_IT != m_FeaturesMap.end(); ++m_IT)
            OGRFeature::DestroyFeature(m_IT->second);
        m_FeaturesMap.clear();
    }
    m_bIsGeometryLoaded = false;
}

OGRFeature* wxGISFeatureDataset::Next(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(bOLCFastSetNextByIndex)
        return m_poLayer->GetNextFeature();
    else
    {
        if(m_IT == m_FeaturesMap.end())
            return NULL;
        OGRFeature* pFeature = m_IT->second->Clone();
        m_IT++;
        return pFeature;
    }
}

void wxGISFeatureDataset::Reset(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(bOLCFastSetNextByIndex)
        m_poLayer->ResetReading();
    else
    {
        if(m_FeaturesMap.empty())
            LoadGeometry();
        m_IT = m_FeaturesMap.begin();
    }
    if(m_pGeometrySet)
        m_pGeometrySet->Reset();
}

OGRErr wxGISFeatureDataset::CreateFeature(OGRFeature* poFeature)
{
    wxCriticalSectionLocker locker(m_CritSect);

//open if closed
 //   if(m_poDS)
 //   {
 //       DeleteQuadTree();
 //       wsDELETE(m_pGeometrySet);
 //       OGRDataSource::DestroyDataSource( m_poDS );
 //       m_poDS = NULL;
 //   }
	//if( m_poDS == NULL )
 //      m_poDS = OGRSFDriverRegistrar::Open( (const char*) m_sPath.mb_str(*m_pPathEncoding), TRUE );
	//if( m_poDS == NULL )
	//	return false;


    OGRErr eErr = m_poLayer->CreateFeature(poFeature);
    if(eErr == OGRERR_NONE)
    {
        bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
        wxString sFIDColName = wgMB2WX(m_poLayer->GetFIDColumn());
        bool bHasFID = true;
        if(sFIDColName.IsEmpty())
            bHasFID = false;

        long nFID;
        if(!bHasFID)
            nFID = m_poLayer->GetFeatureCount(false);
        else
            nFID = poFeature->GetFID();

        OGRGeometry* pGeom(NULL);
        //store features in array for speed
        if(bOLCFastSetNextByIndex)
            pGeom = poFeature->StealGeometry();
        else
        {
            m_FeaturesMap[nFID] = poFeature;
            if(poFeature->GetGeometryRef())
                pGeom = poFeature->GetGeometryRef()->clone();
        }

        m_pGeometrySet->AddGeometry(pGeom, nFID);
  //      if(bOLCFastGetExtent)
  //          CPLQuadTreeInsert(m_pQuadTree, (void*)pGeom);
  //      else
  //      {
  //          OGREnvelope Env;
  //          pGeom->getEnvelope(&Env);
  //          m_psExtent->Merge(Env);
  //      }
		//counter++;
        if(bOLCFastSetNextByIndex)
            OGRFeature::DestroyFeature(poFeature);
    }
    return eErr;
}

OGRwkbGeometryType wxGISFeatureDataset::GetGeometryType(void)
{
    OGRFeatureDefn *pDef = GetDefiniton();
    if(pDef)
        return pDef->GetGeomType();
    return wkbUnknown;
}

OGRFeatureDefn* wxGISFeatureDataset::GetDefiniton(void)
{
    if(!m_bIsOpened)
        if(!Open(0))
            return NULL;
    if(	m_poLayer )
        return m_poLayer->GetLayerDefn();
    return NULL;
}

OGRErr wxGISFeatureDataset::SetFilter(wxGISQueryFilter* pQFilter)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if(!m_bIsOpened)
        if(!Open(0))
            return OGRERR_FAILURE;
    if(	m_poLayer )
    {
        OGRErr eErr = m_poLayer->SetAttributeFilter(wgWX2MB(pQFilter->GetWhereClause()));
        if(eErr != OGRERR_NONE)
            return eErr;

        wxDELETE(m_psExtent);
        UnloadGeometry();

        m_bIsGeometryLoaded = false;
        return eErr;
    }
    return OGRERR_FAILURE;
}

bool wxGISFeatureDataset::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
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

bool wxGISFeatureDataset::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
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
