/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureDataset class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/datasource/quadtree.h"

//#include "wxgis/datasource/sysop.h"
//
//#include "wx/filename.h"
//#include "wx/file.h"

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

IMPLEMENT_CLASS(wxGISFeatureDataset, wxGISTable)

wxGISFeatureDataset::wxGISFeatureDataset(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISTable(sPath, nSubType, poLayer, poDS)
{
    m_nType = enumGISFeatureDataset;
    m_eGeomType = wkbUnknown;
    m_pQuadTree = NULL;

    if(m_bIsOpened)
        SetInternalValues();

    m_bIsGeometryLoaded = false;
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
	Close();
}

void wxGISFeatureDataset::Close(void)
{
    wxDELETE(m_pQuadTree);
    m_bIsGeometryLoaded = false;
	wxGISTable::Close();
}

wxGISDataset* wxGISFeatureDataset::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
			szPath.Printf("%s#%d", m_sPath.c_str(), nIndex);
			wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;
}

wxGISDataset* wxGISFeatureDataset::GetSubset(const wxString & sSubsetName)
{
    if(m_poDS)
    {
        CPLString szSubsetName(sSubsetName.mb_str(wxConvUTF8));
        OGRLayer* poLayer = m_poDS->GetLayerByName(szSubsetName);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
            szPath.Printf("%s#%s", m_sPath.c_str(), szSubsetName.c_str());
			wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;

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
    case enumVecDXF:
        szPath = (char*)CPLResetExtension(m_sPath, "dxf.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case emumVecPostGIS:
        return papszFileList;
    case enumVecKML:
    case enumVecKMZ:    
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
    szPath = (char*)CPLResetExtension(m_sPath, "sif");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

const wxGISSpatialReference wxGISFeatureDataset::GetSpatialReference(void)
{
	if(m_SpatialReference.IsOk() || !IsOpened())
		return m_SpatialReference;

	OGRSpatialReference* pSpaRef = m_poLayer->GetSpatialRef();
	if(!pSpaRef)
		return m_SpatialReference;
	m_SpatialReference = wxGISSpatialReference(pSpaRef);
	return m_SpatialReference;
}

bool wxGISFeatureDataset::Open(int iLayer, int bUpdate, bool bCache, ITrackCancel* const pTrackCancel)
{
	if(IsOpened())
    {
		return true;
    }

	if(!wxGISTable::Open(iLayer, bUpdate, bCache, pTrackCancel))
		return false;

    SetInternalValues();

    if(bCache)
        Cache(pTrackCancel);

	m_bIsOpened = true;

  //  //load all features
  //  LoadGeometry();
    //TODO: Increase extent while loading

	//if(bCache)
	//	LoadGeometry(pTrackCancel);

	return true;
}

void wxGISFeatureDataset::SetInternalValues()
{
    if(NULL == m_poLayer)
        return;

    OGRFeatureDefn *pDef = m_poLayer->GetLayerDefn();
    if(pDef)
        m_eGeomType = pDef->GetGeomType();

    //fill extent if fast
	bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent) != 0;
    if(bOLCFastGetExtent)
    {
        if(m_poLayer->GetExtent(&m_stExtent, FALSE) == OGRERR_NONE)
        {
            if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
            {
                m_stExtent.MaxX += 1;
                m_stExtent.MinX -= 1;
            }
            if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            {
                m_stExtent.MaxY += 1;
                m_stExtent.MinY -= 1;
            }
        }
    }

    wxGISTable::SetInternalValues();
}

void wxGISFeatureDataset::Cache(ITrackCancel* const pTrackCancel)
{
	wxGISTable::Cache(pTrackCancel);

    if(m_bIsGeometryLoaded)
        return;

    if(!m_pQuadTree)
    {
        m_pQuadTree = new wxGISQuadTree(this);
        if(m_pQuadTree->Load(pTrackCancel))
        {
            m_bIsGeometryLoaded = true;
        }
    }
//	LoadGeometry(pTrackCancel);
}

OGREnvelope wxGISFeatureDataset::GetEnvelope(void)
{
    if(m_stExtent.IsInit())
        return m_stExtent;
	//bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent);
 //   if(bOLCFastGetExtent)
    if(IsOpened())
    {
        if(m_poLayer->GetExtent(&m_stExtent) == OGRERR_NONE)
        {
            if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
            {
                m_stExtent.MaxX += 1;
                m_stExtent.MinX -= 1;
            }
            if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            {
                m_stExtent.MaxY += 1;
                m_stExtent.MinY -= 1;
            }
		}
    }
    return m_stExtent;
}
/*
void wxGISFeatureDataset::LoadGeometry(ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(m_bIsGeometryLoaded || !m_bIsOpened || !m_poLayer)
        return;

	IProgressor* pProgress(NULL);
	if(pTrackCancel)
	{
		pTrackCancel->Reset();
		pTrackCancel->PutMessage(wxString(_("PreLoad Geometry of ")) + m_sTableName, -1, enumGISMessageInfo);
		pProgress = pTrackCancel->GetProgressor();
		if(pProgress)
			pProgress->Show(true);
	}

	bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent) || m_stExtent.IsInit();
    if(bOLCFastGetExtent)
    {
        if(m_poLayer->GetExtent(&m_stExtent, true) == OGRERR_NONE)
        {
            if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
            {
                m_stExtent.MaxX += 1;
                m_stExtent.MinX -= 1;
            }
            if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            {
                m_stExtent.MaxY += 1;
                m_stExtent.MinY -= 1;
            }
			m_pQuadTree = boost::make_shared<wxGISQuadTree>(m_stExtent);
			if(pProgress)
				pProgress->SetRange(GetFeatureCount());
		}
    }
	else
	{
		if(pProgress)
			pProgress->SetRange(GetFeatureCount() * 2);
	}

	long nCounter(0);
	Reset();

	OGRFeatureSPtr poFeature;
	long nFID(0);
	while((poFeature = Next()) != NULL )
	{
		if(pProgress)
		{
			pProgress->SetValue(nCounter);
			nCounter++;
		}
        OGRGeometry* pGeom = poFeature->GetGeometryRef();
        if(poFeature && !m_bHasFID)
			poFeature->SetFID(++nFID);
        else
            nFID = poFeature->GetFID();

        //store features in array for speed
		if(!m_bIsDataLoaded)
			m_FeaturesMap[nFID] = poFeature;

        if(bOLCFastGetExtent)
		{
			if(pGeom && m_pQuadTree)
				m_pQuadTree->AddItem(pGeom, poFeature->GetFID());
		}
        else
        {
            if(pGeom)
            {
                OGREnvelope Env;
                pGeom->getEnvelope(&Env);
                m_stExtent.Merge(Env);
            }
        }
		if(pTrackCancel && !pTrackCancel->Continue())
		{
			if(pProgress)
				pProgress->Show(false);
			return;
		}
	}

	m_bIsDataLoaded = true;
	m_nFeatureCount = m_FeaturesMap.size();

    if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
    {
        m_stExtent.MaxX += 1;
        m_stExtent.MinX -= 1;
    }
    if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
    {
        m_stExtent.MaxY += 1;
        m_stExtent.MinY -= 1;
    }

    if(!bOLCFastGetExtent)
    {
        m_pQuadTree = boost::make_shared<wxGISQuadTree>(m_stExtent);
		Reset();
		while((poFeature = Next()) != NULL )
		{
			if(pProgress)
			{
				pProgress->SetValue(nCounter);
				nCounter++;
			}
			OGRGeometry* pGeom = poFeature->GetGeometryRef();
			if(pGeom && m_pQuadTree)
				m_pQuadTree->AddItem(pGeom, poFeature->GetFID());

			if(pTrackCancel && !pTrackCancel->Continue())
			{
				if(pProgress)
					pProgress->Show(false);
				return;
			}
		}
    }
	if(pProgress)
		pProgress->Show(false);

    m_bIsGeometryLoaded = true;
}
*/
//void wxGISFeatureDataset::UnloadGeometry(void)
//{
//    if(!m_bIsGeometryLoaded)
//        return;
//
//    m_bIsGeometryLoaded = false;
//}

OGRwkbGeometryType wxGISFeatureDataset::GetGeometryType(void) const
{
    return m_eGeomType;
}


wxFeatureCursor wxGISFeatureDataset::Search(const wxGISSpatialFilter &SpaFilter, bool bOnlyFirst)
{
    if(SpaFilter.GetGeometry().IsOk())
	{
        m_poLayer->SetSpatialFilter(SpaFilter.GetGeometry());
	}
	else
    {
        m_poLayer->SetSpatialFilter(NULL);
    }

    wxFeatureCursor oOutCursor = wxGISTable::Search(SpaFilter, bOnlyFirst);
    m_poLayer->SetSpatialFilter(NULL);
    return oOutCursor;
}

wxGISQuadTreeCursor wxGISFeatureDataset::SearchGeometry(const CPLRectObj* pAoi)
{
	if(m_pQuadTree)
		return m_pQuadTree->Search(pAoi);
	else
		return wxGISQuadTreeCursor();
}
/*
OGRErr wxGISFeatureDataset::SetFilter(wxGISQueryFilter* pQFilter)
{
    if(	!m_poLayer )
		return OGRERR_FAILURE;

	wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
	if(pSpaFil)
	{
		OGRGeometrySPtr pGeom = pSpaFil->GetGeometry();
        m_poLayer->SetSpatialFilter(pGeom.get());
	}
	else
        m_poLayer->SetSpatialFilter(NULL);

	if( wxGISTable::SetFilter(pQFilter) == OGRERR_NONE )
	{
		LoadGeometry();
		return OGRERR_NONE;
	}
    return OGRERR_FAILURE;
}

wxFeatureCursorSPtr wxGISFeatureDataset::Search(wxGISQueryFilter* pQFilter, bool bOnlyFirst)
{
	//select by attributes
	//filter by geometry
	//return fullfill wxFeatureCursorSPtr
	return wxGISTable::Search(pQFilter, bOnlyFirst);
}

//wxGISGeometrySet* wxGISFeatureDataset::GetGeometrySet(wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
//{
//    //spatial reference of pQFilter
//	wxGISGeometrySet* pGISGeometrySet = NULL;
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
//			    OGRGeometry** pGeometryArr = (OGRGeometry**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
//                pGISGeometrySet = new wxGISGeometrySet(false);
//			    for(int i = 0; i < count; ++i)
//			    {
//				    if(pTrackCancel && !pTrackCancel->Continue())
//					    break;
//				    pGISGeometrySet->AddGeometry(pGeometryArr[i], m_pGeometrySet->operator[](pGeometryArr[i]));
//			    }
//			    wxDELETEA( pGeometryArr );
//                pGISGeometrySet->Reference();
//                return pGISGeometrySet;
//            }
//		}
//	}
//	else
//	{
//        if(m_bIsGeometryLoaded)
//            return GetGeometries();
//        else
//        {
//            LoadGeometry();
//            return GetGeometrySet(pQFilter, pTrackCancel);
//        }
//	}
//	return pGISGeometrySet;
//}

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
//
//wxGISGeometrySet* wxGISFeatureDataset::GetGeometries(void)
//{
//    if(!m_bIsGeometryLoaded)
//        return NULL;
//    m_pGeometrySet->Reference();
//    return m_pGeometrySet;
//}
//
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
//
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
//
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
//			    for(int i = 0; i < count; ++i)
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

//TODO:
//1. Переписать SetFilter -> HRESULT Search(
//  IQueryFilter* filter,
//  VARIANT_BOOL Recycling,
//  IFeatureCursor** Cursor
//); Блокировка получения данных по инджексу или ОИД при выполнении операции
//2. На выходе Cursor -> массив OID и ссылка на table/featureclass
/*
3. Переписать Search для FeatureClass с использованием RTree и возможностью выбора произвольной геометрией
4. При загрузке в слой не создается копия RTree. Создается только при перепроецировании
5. В RTree содержиться геометрия и ОИД. RTree сохраняется в отдельном файле rtr/sig ...
6. При перепроецировании: а) перепроецируются все геомтерии с записью в новый массив; б) создается новое дерево RTree; в) многопоточно
7. При загрузке грузим RTree из файла. Создаем при первом Search если не загрузили при зауске? многопоточно.
8. При отрисовке: запрашивается по охвату (для окна или выделения) массив структуры геометрия* + ОИД. Если перепроецирование врублено - запрос идет у локальной копии массива
9. По ОИД можно получить быстро геометрию или атрибуты.
*/
