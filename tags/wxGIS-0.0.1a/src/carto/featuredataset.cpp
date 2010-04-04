/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  FeatureDataset class.
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
#include "wxgis/carto/featuredataset.h"
#include "ogrsf_frmts/shape/ogrshape.h"

#include <wx/filename.h>
#include <wx/file.h>

void GetGeometryBoundsFunc(const void* hFeature, CPLRectObj* pBounds)
{
	OGRGeometry* pGeometry = (OGRGeometry*)hFeature;
	if(!pGeometry)
		return;
	OGREnvelope Env;
	pGeometry->getEnvelope(&Env);
    if(Env.MinX == Env.MaxX) Env.MaxX += 0.0001;
    if(Env.MinY == Env.MaxY) Env.MaxY += 0.0001;

	pBounds->minx = Env.MinX;
	pBounds->maxx = Env.MaxX;
	pBounds->miny = Env.MinY;
	pBounds->maxy = Env.MaxY;
}

wxGISFeatureDataset::wxGISFeatureDataset(wxString sPath, wxMBConv* pPathEncoding, wxFontEncoding Encoding) : wxGISDataset(sPath, pPathEncoding), m_poDS(NULL), m_bIsOpened(false), m_psExtent(NULL), m_poLayer(NULL), m_Encoding(Encoding), m_bIsGeometryLoaded(false), m_pQuadTree(NULL)
{
    m_pGeometrySet = new wxGISGeometrySet(this, true);
    m_pGeometrySet->Reference();
    ////to protect from cicle of m_pGeometrySet this pointer and this class
    //Dereference();
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
    DeleteQuadTree();    

	wxDELETE(m_psExtent);
    wsDELETE(m_pGeometrySet);

	if( m_poDS )
		OGRDataSource::DestroyDataSource( m_poDS );
}

OGRLayer* wxGISFeatureDataset::GetLayerRef(int iLayer)
{
	if(m_bIsOpened)
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
    if(m_poDS)
    {
        DeleteQuadTree();
        m_pGeometrySet->Release();
        OGRDataSource::DestroyDataSource( m_poDS );
        m_poDS = NULL;
    }
	if( m_poDS == NULL )
       m_poDS = OGRSFDriverRegistrar::Open( (const char*) m_sPath.mb_str(*m_pPathEncoding), TRUE );
	if( m_poDS == NULL )
		return false;
    OGRErr err = m_poDS->DeleteLayer(iLayer);
    if(err == OGRERR_NONE)
        return true;
	return false;    
}

bool wxGISFeatureDataset::Open(int iLayer)
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);

    m_poDS = OGRSFDriverRegistrar::Open( (const char*) m_sPath.mb_str(*m_pPathEncoding)/*wgWX2MB(m_sPath.c_str())*/, FALSE );
	if( m_poDS == NULL )
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISFeatureDataset: Open failed! Path '%s'. OGR error: %s"), m_sPath.c_str(), wgMB2WX(err));
		wxLogError(sErr);
		return false;
	}


	m_poLayer = m_poDS->GetLayer(iLayer);
    //wxString sStatement(wxT("SELECT FID FROM gns-shp-utf-rs"));//  = wxString::Format
    //m_poLayer = m_poDS->ExecuteSQL(wgWX2MB(sStatement), NULL, NULL);
	if(m_poLayer)
	{
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
	//	bool bOLCFastSetNextByIndex= pOGRLayer->TestCapability(OLCFastSetNextByIndex);
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

	m_bIsOpened = true;
	return true;
}

OGRSpatialReference* wxGISFeatureDataset::GetSpatialReference(void)
{
	if(!m_bIsOpened)
		if(!Open(0))
			return NULL;
	if(	m_poLayer )
		return m_poLayer->GetSpatialRef();
	return NULL;
}

OGREnvelope* wxGISFeatureDataset::GetEnvelope(void)
{
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
                return m_psExtent;
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
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < GetSize());
	//bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);
 //   if(bOLCFastSetNextByIndex)
 //   {
    m_poLayer->SetNextByIndex(nIndex);
    return m_poLayer->GetNextFeature();
    //}
    //else
    //{
    //    std::map<long, OGRFeature*>::iterator it( m_FeaturesMap.begin() );
    //    std::advance( it, nIndex );
    //    return it->second->Clone();
    //}
}

OGRFeature* wxGISFeatureDataset::operator [](long nIndex) //const    same as GetAt
{
	return GetAt(nIndex);
}

wxString wxGISFeatureDataset::GetAsString(long row, int col)
{
	if(GetSize() <= row) 
		return wxString(); 
	else 
	{
		OGRFeature* pFeature = GetAt(row);
		OGRFieldDefn* pDef = pFeature->GetFieldDefnRef(col);
        wxString sOut;
		switch(pDef->GetType())
		{
		case OFTDate:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				sOut = dt.Format(_("%d-%m-%Y"));//wxString::Format(_("%.2u-%.2u-%.4u"), day, mon, year );
			}
		case OFTTime:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				sOut = dt.Format(_("%H:%M:%S"));//wxString::Format(_("%.2u:%.2u:%.2u"), hour, min, sec);
			}
		case OFTDateTime:
			{
				int year, mon, day, hour, min, sec, flag;
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				sOut = dt.Format(_("%d-%m-%Y %H:%M:%S"));//wxString::Format(_("%.2u-%.2u-%.4u %.2u:%.2u:%.2u"), day, mon, year, hour, min, sec);
			}
		case OFTReal:				
			sOut = wxString::Format(_("%.6f"), pFeature->GetFieldAsDouble(col));
		default:
            if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
                sOut = wgMB2WX(pFeature->GetFieldAsString(col));
            else            
            {                
                wxCSConv conv(m_Encoding);
                sOut = conv.cMB2WX(pFeature->GetFieldAsString(col));
                if(sOut.IsEmpty())
                    sOut = wgMB2WX(pFeature->GetFieldAsString(col));
            }
		}
        OGRFeature::DestroyFeature(pFeature);
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

wxGISGeometrySet* wxGISFeatureDataset::GetGeometrySet(IQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
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
                pGISGeometrySet = new wxGISGeometrySet(this, false);
			    for(int i = 0; i < count; i++)
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
    if(m_bIsGeometryLoaded)
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
        if(m_bIsGeometryLoaded)
            return m_pGeometrySet->GetSize();
        else
            return m_poLayer->GetFeatureCount(true);
    }
    return 0;    
}

void wxGISFeatureDataset::LoadGeometry(void)
{
    if(m_bIsGeometryLoaded)
        return;
    
    if(!m_bIsOpened)
        if(!Open(0))
            return;
    if(	m_poLayer )
    {
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
                CreateQuadTree(m_psExtent);
            }
        }

		OGRFeature *poFeature;
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
            long nFID;
            if(!bHasFID)
                nFID = counter;
            else
                nFID = poFeature->GetFID();
            OGRGeometry* pGeom = poFeature->StealGeometry();// GetGeometryRef()->clone();
            m_pGeometrySet->AddGeometry(pGeom, nFID);
            if(bOLCFastGetExtent)
                CPLQuadTreeInsert(m_pQuadTree, (void*)pGeom);
            else
            {
                OGREnvelope Env;
                pGeom->getEnvelope(&Env);
                m_psExtent->Merge(Env);
            }
			counter++;
            OGRFeature::DestroyFeature(poFeature);
		} 

        if(!bOLCFastGetExtent)
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
    if(!m_bIsGeometryLoaded)
        return;
    DeleteQuadTree();
    m_pGeometrySet->Clear();
    m_bIsGeometryLoaded = false;
}

OGRFeature* wxGISFeatureDataset::Next(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    //if(m_FeaturesMap.empty())
        return m_poLayer->GetNextFeature();
    //else
    //{
    //    if(m_IT == m_FeaturesMap.end())
    //        return NULL;
    //    return m_IT->second;
    //    m_IT++;
    //}
}

void wxGISFeatureDataset::Reset(void)
{
    m_poLayer->ResetReading();
    m_pGeometrySet->Reset();
}

////---------------------------------------
//// wxGISShapeFeatureDataset
////---------------------------------------
//
//wxGISShapeFeatureDataset::wxGISShapeFeatureDataset(wxString sPath, wxMBConv* pPathEncoding, wxFontEncoding Encoding) : wxGISFeatureDataset(sPath, pPathEncoding, Encoding)
//{
//}
//
//wxGISShapeFeatureDataset::~wxGISShapeFeatureDataset(void)
//{
//}
//
//bool wxGISShapeFeatureDataset::Open(int iLayer)
//{
//    bool bRes = wxGISFeatureDataset::Open(iLayer);
//    if(!bRes)
//        return bRes;
//    OGRShapeLayer* pShapeLayer = dynamic_cast<OGRShapeLayer*>(m_poLayer);
//    if(!pShapeLayer)
//        return false;
//    m_bHasSpatialIndex = pShapeLayer->CheckForQIX();
//    return true;
//}
//
//OGRErr wxGISShapeFeatureDataset::CreateSpatialIndex(void)
//{
//    OGRShapeLayer* pShapeLayer = dynamic_cast<OGRShapeLayer*>(m_poLayer);
//    if(!pShapeLayer)
//        return CE_Failure;
//    return pShapeLayer->CreateSpatialIndex(0);
//}
