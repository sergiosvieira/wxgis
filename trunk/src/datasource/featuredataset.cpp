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

#include "wx/filename.h"
#include "wx/file.h"

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	OGRGeometry* pGeometry = (OGRGeometry*)hFeature;
    if(!pGeometry)
		return;
	OGREnvelope Env;
	pGeometry->getEnvelope(&Env);
    if(IsDoubleEquil(Env.MinX, Env.MaxX))
    {
        //wxLogDebug(wxT("%f %f"), Env.MinX, Env.MaxX);
        Env.MaxX += DELTA;
    }
    if(IsDoubleEquil(Env.MinY, Env.MaxY))
    {
        //wxLogDebug(wxT("%f %f"), Env.MinX, Env.MaxX);
        Env.MaxY += DELTA;
    }

	pBounds->minx = Env.MinX;
	pBounds->maxx = Env.MaxX;
	pBounds->miny = Env.MinY;
	pBounds->maxy = Env.MaxY;
}

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------
wxGISFeatureDataset::wxGISFeatureDataset(CPLString sPath, int nSubType, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISTable(sPath, nSubType, poLayer, poDS)
{
    m_nType = enumGISFeatureDataset;
    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
        m_Encoding = wxFONTENCODING_UTF8;
    else
        m_Encoding = wxFONTENCODING_DEFAULT;
	m_pQuadTree = NULL;
	m_pGeometrySet = NULL;
	m_bIsGeometryLoaded = false;
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
	Close();
}

void wxGISFeatureDataset::Close(void)
{
    DeleteQuadTree();

    wsDELETE(m_pGeometrySet);

    m_bIsGeometryLoaded = false;

	wxGISTable::Close();
}

size_t wxGISFeatureDataset::GetSubsetsCount(void)
{
    if(m_poDS)
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDatasetSPtr wxGISFeatureDataset::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
			szPath.Printf("%s#%d", m_sPath.c_str(), nIndex);
			wxGISFeatureDatasetSPtr pDataSet = boost::make_shared<wxGISFeatureDataset>(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return boost::static_pointer_cast<wxGISDataset>(pDataSet);
        }
    }
    return wxGISDatasetSPtr();
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

bool wxGISFeatureDataset::Open(int iLayer, int bUpdate, ITrackCancel* pTrackCancel)
{
	if(m_bIsOpened)
		return true;

	if(!wxGISTable::Open(iLayer, bUpdate, pTrackCancel))
		return false;

	LoadGeometry();

	//m_bIsOpened = true;
	return true;
}

const OGRSpatialReferenceSPtr wxGISFeatureDataset::GetSpatialReference(void)
{
	if(m_pSpatialReference)
		return m_pSpatialReference;
	if(	!m_poLayer )
		return OGRSpatialReferenceSPtr();

	m_pSpatialReference = OGRSpatialReferenceSPtr(m_poLayer->GetSpatialRef()->Clone());
	return m_pSpatialReference;
}

const OGREnvelopeSPtr wxGISFeatureDataset::GetEnvelope(void)
{
    if(m_psExtent)
        return m_psExtent;
    if(!m_poLayer )
		return OGREnvelopeSPtr();

	bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent);
    if(bOLCFastGetExtent)
    {
		OGREnvelope* pEnv = new OGREnvelope();
        if(m_poLayer->GetExtent(pEnv, true) == OGRERR_NONE)
        {
            if(IsDoubleEquil(pEnv->MinX, pEnv->MaxX))
            {
                pEnv->MaxX += 1;
                pEnv->MinX -= 1;
            }
            if(IsDoubleEquil(pEnv->MinY, pEnv->MaxY))
            {
                pEnv->MaxY += 1;
                pEnv->MinY -= 1;
            }
			m_psExtent = OGREnvelopeSPtr(pEnv);
            CreateQuadTree(m_psExtent);
            return m_psExtent;
        }
    }
  //  //load all features
  //  LoadGeometry();
  //  if(m_bIsGeometryLoaded)
  //      return m_psExtent;
  //  else
  //  {
		//OGREnvelope* pEnv = new OGREnvelope();
  //      if(m_poLayer->GetExtent(&pEnv, true) == OGRERR_NONE)
  //      {
  //          if(IsDoubleEquil(pEnv->MinX, pEnv->MaxX))
  //          {
  //              pEnv->MaxX += 1;
  //              pEnv->MinX -= 1;
  //          }
  //          if(IsDoubleEquil(pEnv->MinY, pEnv->MaxY))
  //          {
  //              pEnv->MaxY += 1;
  //              pEnv->MinY -= 1;
  //          }
		//	m_psExtent = OGREnvelopeSPtr(pEnv);
  //          return m_psExtent;
  //      }
  //      return OGREnvelopeSPtr();
  //  }
    return OGREnvelopeSPtr();
}

wxGISGeometrySet* wxGISFeatureDataset::GetGeometries(void)
{
    if(!m_bIsGeometryLoaded)
        return NULL;
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

void wxGISFeatureDataset::CreateQuadTree(const OGREnvelopeSPtr pEnv)
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

void wxGISFeatureDataset::LoadGeometry(void)
{
	return;
	wxCriticalSectionLocker locker(m_CritSect);
    if(m_bIsGeometryLoaded || !m_bIsOpened || !m_poLayer)
        return;


	bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent);
    if(bOLCFastGetExtent)
    {
		OGREnvelope* pEnv = new OGREnvelope();
        if(m_poLayer->GetExtent(pEnv, true) == OGRERR_NONE)
        {
            if(IsDoubleEquil(pEnv->MinX, pEnv->MaxX))
            {
                pEnv->MaxX += 1;
                pEnv->MinX -= 1;
            }
            if(IsDoubleEquil(pEnv->MinY, pEnv->MaxY))
            {
                pEnv->MaxY += 1;
                pEnv->MinY -= 1;
            }
			m_psExtent = OGREnvelopeSPtr(pEnv);
            CreateQuadTree(m_psExtent);
        }
    }
	else
		m_psExtent = OGREnvelopeSPtr(new OGREnvelope());

	Reset();

	m_pGeometrySet = new wxGISGeometrySet(true);
    m_pGeometrySet->Reference();


	OGRFeatureSPtr poFeature;
	while((poFeature = Next()) != NULL )
	{
        OGRGeometry* pGeom(NULL);
        //store features in array for speed
        pGeom = poFeature->StealGeometry();
        if(pGeom && m_pGeometrySet)
            m_pGeometrySet->AddGeometry(pGeom, poFeature->GetFID());

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
	}

    if(IsDoubleEquil(m_psExtent->MinX, m_psExtent->MaxX))
    {
        m_psExtent->MaxX += 1;
        m_psExtent->MinX -= 1;
    }
    if(IsDoubleEquil(m_psExtent->MinY, m_psExtent->MaxY))
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
    m_bIsGeometryLoaded = true;
}

void wxGISFeatureDataset::UnloadGeometry(void)
{
    if(!m_bIsGeometryLoaded)
        return;
	wxCriticalSectionLocker locker(m_CritSect);
    DeleteQuadTree();
    if(m_pGeometrySet)
        m_pGeometrySet->Clear();

    m_bIsGeometryLoaded = false;
}

const OGRwkbGeometryType wxGISFeatureDataset::GetGeometryType(void)
{
    OGRFeatureDefn *pDef = GetDefinition();
    if(pDef)
        return pDef->GetGeomType();
    return wkbUnknown;
}

OGRErr wxGISFeatureDataset::SetFilter(wxGISQueryFilter* pQFilter)
{
	if(wxGISTable::SetFilter(pQFilter) == OGRERR_NONE)
		//unload/load geoms
		return OGRERR_NONE;
    //wxCriticalSectionLocker locker(m_CritSect);
    //if(!m_bIsOpened)
    //    if(!Open(0))
    //        return OGRERR_FAILURE;
    //if(	m_poLayer )
    //{
    //    OGRErr eErr = m_poLayer->SetAttributeFilter(wgWX2MB(pQFilter->GetWhereClause()));
    //    if(eErr != OGRERR_NONE)
    //        return eErr;

    //    wxDELETE(m_psExtent);
    //    UnloadGeometry();

    //    m_bIsGeometryLoaded = false;
    //    return eErr;
    //}
    return OGRERR_FAILURE;
}

//wxGISFeatureDataset::wxGISFeatureDataset(OGRDataSource *poDS, OGRLayer* poLayer, CPLString sPath, wxGISEnumVectorDatasetType nType) : wxGISDataset(sPath), m_poDS(NULL), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1), m_pGeometrySet(NULL)
//{
//	m_bIsOpened = false;
//	m_poDS = poDS;
//	m_poLayer = poLayer;
//    if(m_poLayer == NULL)
//        m_nType = enumGISContainer;
//    else
//    {
//        m_nType = enumGISFeatureDataset;
//        m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
//    }
//    m_nSubType = (int)nType;
//
//	m_bIsOpened = true;
//
//    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
//        m_Encoding = wxFONTENCODING_UTF8;
//    else
//        m_Encoding = wxFONTENCODING_DEFAULT;
//}
//
//wxGISFeatureDataset::wxGISFeatureDataset(CPLString sPath, wxGISEnumVectorDatasetType nType) : wxGISDataset(sPath), m_poDS(NULL), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1), m_pGeometrySet(NULL)
//{
//	m_bIsOpened = false;
//
//    m_nType = enumGISFeatureDataset;
//    m_nSubType = (int)nType;
//    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
//        m_Encoding = wxFONTENCODING_UTF8;
//    else
//        m_Encoding = wxFONTENCODING_DEFAULT;
//}

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

OGRDataSource* wxGISFeatureDataset::GetDataSource(void)
{
    if(m_poDS)
        return m_poDS;
    return NULL;
}

//OGRErr wxGISFeatureDataset::CreateFeature(OGRFeature* poFeature)
//{
//    wxCriticalSectionLocker locker(m_CritSect);
//
////open if closed
// //   if(m_poDS)
// //   {
// //       DeleteQuadTree();
// //       wsDELETE(m_pGeometrySet);
// //       OGRDataSource::DestroyDataSource( m_poDS );
// //       m_poDS = NULL;
// //   }
//	//if( m_poDS == NULL )
// //      m_poDS = OGRSFDriverRegistrar::Open( (const char*) m_sPath.mb_str(*m_pPathEncoding), TRUE );
//	//if( m_poDS == NULL )
//	//	return false;
//
//
//    OGRErr eErr = m_poLayer->CreateFeature(poFeature);
//    if(eErr == OGRERR_NONE)
//    {
//        bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
//        wxString sFIDColName = wgMB2WX(m_poLayer->GetFIDColumn());
//        bool bHasFID = true;
//        if(sFIDColName.IsEmpty())
//            bHasFID = false;
//
//        long nFID;
//        if(!bHasFID)
//            nFID = m_poLayer->GetFeatureCount(false);
//        else
//            nFID = poFeature->GetFID();
//
//        OGRGeometry* pGeom(NULL);
//        //store features in array for speed
//        if(bOLCFastSetNextByIndex)
//            pGeom = poFeature->StealGeometry();
//        else
//        {
//            m_FeaturesMap[nFID] = poFeature;
//            if(poFeature->GetGeometryRef())
//                pGeom = poFeature->GetGeometryRef()->clone();
//        }
//
//        if(m_pGeometrySet)
//            m_pGeometrySet->AddGeometry(pGeom, nFID);
//  //      if(bOLCFastGetExtent)
//  //          CPLQuadTreeInsert(m_pQuadTree, (void*)pGeom);
//  //      else
//  //      {
//  //          OGREnvelope Env;
//  //          pGeom->getEnvelope(&Env);
//  //          m_psExtent->Merge(Env);
//  //      }
//		//counter++;
//        if(bOLCFastSetNextByIndex)
//            OGRFeature::DestroyFeature(poFeature);
//    }
//    return eErr;
//}

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
