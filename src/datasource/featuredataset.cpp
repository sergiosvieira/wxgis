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
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/sysop.h"

//#include "ogrsf_frmts/shape/ogrshape.h"

#include <wx/filename.h>
#include <wx/file.h>

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

wxGISFeatureDataset* CreateVectorLayer(wxString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *poFields, OGRSpatialReference *poSpatialRef, OGRwkbGeometryType eGType, char ** papszDataSourceOptions, char ** papszLayerOptions, wxMBConv* pPathEncoding)
{
    CPLErrorReset();
    poFields->Reference();
    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( wgWX2MB(sDriver) );
    if(poDriver == NULL)
    {
        wxLogError(_("The driver '%s' is not available"), sDriver.c_str());
        return NULL;
    }

    wxGISEnumVectorDatasetType nSubType = enumVecUnknown;
    if(sDriver == wxString(wxT("KML")))
        nSubType = enumVecKML;
    else if(sDriver == wxString(wxT("DXF")))
        nSubType = enumVecDXF;
    else if(sDriver == wxString(wxT("DXF")))
        nSubType = enumVecDXF;
    else if(sDriver == wxString(wxT("MapInfo File")) && sExt == wxString(wxT("tab")))
        nSubType = enumVecMapinfoTab;
    else if(sDriver == wxString(wxT("MapInfo File")) && sExt == wxString(wxT("mif")))
        nSubType = enumVecMapinfoMif;
    else if(sDriver == wxString(wxT("ESRI Shapefile")))
        nSubType = enumVecESRIShapefile;

    wxString sFullPath = sPath + wxFileName::GetPathSeparator() + sName + wxT(".") + sExt;
    OGRDataSource *poDS = poDriver->CreateDataSource( wgWX2MB(sFullPath), papszDataSourceOptions );
    if(poDS == NULL)
    {
        wxLogError(_("Creation of output file '%s' failed"), sName.c_str());
        return NULL;
    }

    sName.Replace(wxT("."), wxT("_"));
    sName.Replace(wxT(" "), wxT("_"));
    sName.Truncate(27);

	OGRLayer *poLayerDest = poDS->CreateLayer( sName.mb_str(wxConvUTF8)/*wgWX2MB(sName)*/, poSpatialRef, eGType, papszLayerOptions );
    if(poLayerDest == NULL)
    {
        wxLogError(_("Creation of output layer '%s' failed"), sName.c_str());
        return NULL;
    }

    if(nSubType != enumVecDXF)
    {
        for(size_t i = 0; i < poFields->GetFieldCount(); i++)
        {
	        if( poLayerDest->CreateField( poFields->GetFieldDefn(i) ) != OGRERR_NONE )
	        {
                wxLogError(_("Creation of output layer '%s' failed"), sName.c_str());
                return NULL;
            }
        }
    }
    poFields->Release();

    wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(poDS, poLayerDest, sFullPath, nSubType, pPathEncoding);
    return pDataSet;
}

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------
wxGISFeatureDataset::wxGISFeatureDataset(OGRDataSource *poDS, OGRLayer* poLayer, wxString sPath, wxGISEnumVectorDatasetType nType, wxMBConv* pPathEncoding) : wxGISDataset(sPath, pPathEncoding), m_poDS(NULL), m_bIsOpened(false), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1)
{
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

    if(m_nSubType == enumVecKML)
        m_Encoding = wxFONTENCODING_UTF8;
    else
        m_Encoding = wxFONTENCODING_DEFAULT;

    m_pGeometrySet = new wxGISGeometrySet(true);
    m_pGeometrySet->Reference();
}

wxGISFeatureDataset::wxGISFeatureDataset(wxString sPath, wxGISEnumVectorDatasetType nType, wxMBConv* pPathEncoding) : wxGISDataset(sPath, pPathEncoding), m_poDS(NULL), m_bIsOpened(false), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1)
{
    m_pGeometrySet = new wxGISGeometrySet(true);
    m_pGeometrySet->Reference();
    ////to protect from cicle of m_pGeometrySet this pointer and this class
    //Dereference();

    m_nType = enumGISFeatureDataset;
    m_nSubType = (int)nType;
    if(m_nSubType == enumVecKML)
        m_Encoding = wxFONTENCODING_UTF8;
    else
        m_Encoding = wxFONTENCODING_DEFAULT;
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
    DeleteQuadTree();    

	wxDELETE(m_psExtent);
    wsDELETE(m_pGeometrySet);

    if(!m_FeaturesMap.empty())
    {
        for(m_IT = m_FeaturesMap.begin(); m_IT != m_FeaturesMap.end(); ++m_IT)
            OGRFeature::DestroyFeature(m_IT->second);
    }
    m_bIsGeometryLoaded = false;

	//if( m_poDS )
	//	OGRDataSource::DestroyDataSource( m_poDS );
    if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
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

wxGISDataset* wxGISFeatureDataset::GetSubset(size_t nIndex)
{
    if(!m_bIsOpened)
        if(!Open(0))
            return NULL;
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
            wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(m_poDS, poLayer, wxEmptyString, (wxGISEnumVectorDatasetType)m_nSubType, m_pPathEncoding);
            pDataSet->SetEncoding(m_Encoding);
            pDataSet->Reference();
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;
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
    if(m_poDS && m_poDS->Dereference() <= 0)
    {
        OGRDataSource::DestroyDataSource( m_poDS );
        m_poDS = NULL;
    }    
	//if( m_poDS == NULL )
 //      m_poDS = OGRSFDriverRegistrar::Open( (const char*) m_sPath.mb_str(*m_pPathEncoding), TRUE );
	//if( m_poDS == NULL )
 //   {
 //       //try to delete on file basis
 //       int ret = VSIUnlink((const char*) m_sPath.mb_str(*m_pPathEncoding));
 //       if(ret == 0)
	//	    return true;
 //       return false;
 //   }
 //   OGRErr err = m_poDS->DeleteLayer(iLayer);
 //   if(err == OGRERR_NONE)
 //       return true;
 //   //try to kill file
 //   if(m_poDS && m_poDS->Dereference() <= 0)
 //   {
 //       OGRDataSource::DestroyDataSource( m_poDS );
 //       m_poDS = NULL;
 //   }
	//if( m_poDS == NULL )
 //   {
 //       //try to delete on file basis
 //       int ret = VSIUnlink((const char*) m_sPath.mb_str(*m_pPathEncoding));
 //       if(ret == 0)
	//	    return true;
 //       return false;
 //   }

    wxFileName FName( m_sPath );
    wxString sExt = FName.GetExt();
    sExt.Prepend(wxT("."));
    FName.ClearExt();
    wxString sPath = FName.GetFullPath();
    switch(m_nSubType)
    {
    case enumVecESRIShapefile: 
        if(!DeleteFile(m_sPath, m_pPathEncoding))
            return false;
        DeleteFile(sPath + wxT(".shx"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".dbf"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".prj"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".qix"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".sbn"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".sbx"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".cpg"), m_pPathEncoding);
        DeleteFile(sPath + sExt + wxT(".xml"), m_pPathEncoding);
        return true;
    case enumVecMapinfoTab:
        if(!DeleteFile(m_sPath, m_pPathEncoding))
            return false;
        DeleteFile(sPath + wxT(".dat"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".id"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".ind"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".map"), m_pPathEncoding);
        DeleteFile(sPath + wxT(".cpg"), m_pPathEncoding);
        DeleteFile(sPath + sExt + wxT(".metadata.xml"), m_pPathEncoding);
        DeleteFile(sPath + sExt + wxT(".xml"), m_pPathEncoding);
        return true;
    case enumVecMapinfoMif: 
        if(!DeleteFile(m_sPath, m_pPathEncoding))
            return false;
        DeleteFile(sPath + wxT(".mid"), m_pPathEncoding);
        DeleteFile(sPath + sExt + wxT(".metadata.xml"), m_pPathEncoding);
        DeleteFile(sPath + sExt + wxT(".xml"), m_pPathEncoding);
        return true;
    case enumVecKML:
        if(enumGISContainer)
        {
            if(!DeleteFile(m_sPath, m_pPathEncoding))
                return false;
        }
        else
            return false;
        return true;
    case enumVecDXF:
        if(!DeleteFile(m_sPath, m_pPathEncoding))
            return false;
        return true;
    case enumVecUnknown: 
    default: return false;
    }
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

    wxFileName FName( m_sPath );
    FName.ClearExt();
    wxString sPath = FName.GetFullPath();

    wxFontEncoding FEnc = GetEncodingFromCpg(sPath, m_pPathEncoding);
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
	if(	m_poLayer )
    {
        //wxCSConv conv(m_Encoding);
        //const char* psName = m_poLayer->GetLayerDefn()->GetName();
        //return conv.cMB2WX(psName);

        wxString sOut;
        if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
            sOut = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        else            
        {                
            wxCSConv conv(m_Encoding);
            sOut = conv.cMB2WX(m_poLayer->GetLayerDefn()->GetName());
            if(sOut.IsEmpty())
                sOut = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        }
        return sOut;
    }
    return wxEmptyString;
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
	if(GetSize() <= row) 
		return wxEmptyString; 
	else 
	{
        wxCriticalSectionLocker locker(m_CritSect);
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
				pFeature->GetFieldAsDateTime(col, &year, &mon, &day, &hour, &min, &sec, &flag);
				wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
				sOut = dt.Format(_("%d-%m-%Y %H:%M:%S"));//wxString::Format(_("%.2u-%.2u-%.4u %.2u:%.2u:%.2u"), day, mon, year, hour, min, sec);
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

            OGRGeometry* pGeom;
            //store features in array for speed
            if(bOLCFastSetNextByIndex)
                pGeom = poFeature->StealGeometry();
            else
            {
                m_FeaturesMap[nFID] = poFeature;
                pGeom = poFeature->GetGeometryRef()->clone();
            }

            m_pGeometrySet->AddGeometry(pGeom, nFID);
            if(bOLCFastGetExtent)
                CPLQuadTreeInsert(m_pQuadTree, (void*)pGeom);
            else
            {
                OGREnvelope Env;
                pGeom->getEnvelope(&Env);
                m_psExtent->Merge(Env);
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
    if(m_FeaturesMap.empty() || m_FeaturesMap.size() < GetSize())
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
        m_IT = m_FeaturesMap.begin();
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

        OGRGeometry* pGeom;
        //store features in array for speed
        if(bOLCFastSetNextByIndex)
            pGeom = poFeature->StealGeometry();
        else
        {
            m_FeaturesMap[nFID] = poFeature;
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
