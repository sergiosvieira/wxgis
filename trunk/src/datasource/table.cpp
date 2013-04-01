/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Table class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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

#include "wxgis/datasource/table.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/config.h"

//--------------------------------------------------------------------------------
// wxGISTable
//--------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTable, wxGISDataset)

wxGISTable::wxGISTable(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISDataset(sPath)
{
	m_poDS = poDS;
	m_poLayer = poLayer;
    m_nType = enumGISTableDataset;
    m_nSubType = nSubType;

    m_Encoding = wxLocale::GetSystemEncoding();

    poLayer == NULL ? m_bIsOpened = false : m_bIsOpened = true;

    m_bIsReadOnly = true;
    m_bIsCached = false;

	m_bHasFID = false;
    m_nCurrentFID = 0;
    m_nFeatureCount = wxNOT_FOUND;

    m_bOLCFastFeatureCount = false;
    m_bOLCStringsAsUTF8 = false;

    if(m_bIsOpened)
        SetInternalValues();

	//m_bIsDataLoaded = false;	
    //m_FeatureStringData.Alloc(10000);
}

wxGISTable::~wxGISTable(void)
{
	Close();
}

wxString wxGISTable::GetName(void) const
{
    if(!m_poLayer)
        return wxEmptyString;

    wxString sOut = GetConvName(m_poLayer->GetLayerDefn()->GetName(), false);
	if(sOut.IsEmpty())//CPLGetBasename
        sOut = GetConvName(CPLGetBasename(m_sPath), false);

    return sOut;
}

bool wxGISTable::Open(int iLayer, int bUpdate, bool bCache, ITrackCancel* const pTrackCancel)
{
	if(m_bIsOpened)
		return true;

	wxCriticalSectionLocker locker(m_CritSect);

	m_bIsReadOnly = bUpdate == FALSE;

    if(!m_poLayer)
	{
		if(m_nSubType == enumTableQueryResult)
			bUpdate = FALSE;

		m_poDS = OGRSFDriverRegistrar::Open( m_sPath, bUpdate );
		//bug in FindFileInZip() [gdal-root\port\cpl_vsil_gzip.cpp]
		if( m_poDS == NULL )
			m_poDS = OGRSFDriverRegistrar::Open( FixPathSeparator(m_sPath), bUpdate );

		if( m_poDS == NULL )
		{
			const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Table open failed! Path '%s'. OGR error: %s"), wxString(m_sPath, wxConvUTF8).c_str(), wxString(err, wxConvUTF8).c_str());
			wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
			return false;
		}

		int nLayerCount = m_poDS->GetLayerCount();
		if(nLayerCount == 1)
			m_poLayer = m_poDS->GetLayer(iLayer);
		else
			m_nType = enumGISContainer;
	}

    SetInternalValues();

	m_bIsOpened = true;

	//if(bCache)
	//	LoadFeatures(pTrackCancel);

	return true;
}

size_t wxGISTable::GetSubsetsCount(void) const
{
    if(m_poDS)
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDataset* wxGISTable::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
			szPath.Printf("%s#%d", m_sPath.c_str(), nIndex);
			wxGISTable* pDataSet = new wxGISTable(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;
}

wxGISDataset* wxGISTable::GetSubset(const wxString & sSubsetName)
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
			wxGISTable* pDataSet = new wxGISTable(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;

}

void wxGISTable::SetInternalValues(void)
{
    if(NULL == m_poLayer)
        return;
    //set string encoding
    //TODO: if cpg is not exist get encoding from header of dbf or etc. and write to cpg else reade from cpg
    m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8) != 0;
    m_bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount) != 0;

    if(m_bOLCStringsAsUTF8)
        m_Encoding = wxFONTENCODING_UTF8;
    else
    {
        if(m_nType == enumGISFeatureDataset)
        {
            switch(m_nSubType)
            {
            case enumVecKML:
            case enumVecKMZ:
            case enumVecGML:
                m_Encoding = wxFONTENCODING_UTF8;
                break;
            case enumVecESRIShapefile:
            case enumVecDXF:
                {
                    wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
                    if(FEnc != wxFONTENCODING_DEFAULT)
                        m_Encoding = FEnc;
                }
                break;
            default:
                m_Encoding = wxLocale::GetSystemEncoding();
                break;
            }
        }
        else if(m_nType == enumGISTableDataset)
        {
            wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
            if(FEnc != wxFONTENCODING_DEFAULT)
                m_Encoding = FEnc;
        }
    }

    m_bHasFID = !CPLString(m_poLayer->GetFIDColumn()).empty();
}

void wxGISTable::Cache(ITrackCancel* pTrackCancel)
{
	//LoadFeatures(pTrackCancel);
}
/*
void wxGISTable::LoadFeatures(ITrackCancel* pTrackCancel)
{
	//wxCriticalSectionLocker locker(m_CritSect);
	if( !m_bIsOpened || !m_poLayer )
		return;

	IProgressor* pProgress(NULL);
	if(pTrackCancel)
	{
		pTrackCancel->Reset();
		pTrackCancel->PutMessage(wxString(_("PreLoad Features of ")) + m_sTableName, -1, enumGISMessageInfo);
		pProgress = pTrackCancel->GetProgressor();
		if(pProgress)
			pProgress->Show(true);
	}
	bool bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount) != 0;
	if(pProgress)
	{
		if(bOLCFastFeatureCount)
			pProgress->SetRange(m_poLayer->GetFeatureCount());
		else
			pProgress->Play();
	}

	if(m_nSubType == enumTableQueryResult)
		bOLCFastFeatureCount = false;
	bool bOLCRandomRead = m_poLayer->TestCapability(OLCRandomRead) != 0;

	//check FIDs and bOLCRandomRead
	if(!bOLCFastFeatureCount || !bOLCRandomRead)//!m_bHasFID || 
	{
        m_poLayer->ResetReading();
        long counter = 1;

		OGRFeature *poFeature;
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
			if(pProgress)
			{
				if(bOLCFastFeatureCount)
					pProgress->SetValue(counter);
				else
					pProgress->Play();
			}

            if(!poFeature->GetDefnRef())
            {
                OGRFeature::DestroyFeature(poFeature);
                continue;
            }

            long nFID;
            if(poFeature && !m_bHasFID)
			{
                nFID = counter;
				poFeature->SetFID(nFID);
			}
            else
                nFID = poFeature->GetFID();

            //store features in array for speed
			m_FeaturesMap[nFID] = OGRFeatureSPtr(poFeature, OGRFeatureDeleter);
			counter++;

			if(pTrackCancel && !pTrackCancel->Continue())
			{
				if(pProgress)
					pProgress->Show(false);
				return;
			}
		}
		m_bIsDataLoaded = true;
		//m_bHasFID = true;
		m_nFeatureCount = m_FeaturesMap.size();
    }

	if(pProgress)
	{
		pProgress->Stop();
		pProgress->Show(false);
	}
}

void wxGISTable::UnloadFeatures(void)
{
	//wxCriticalSectionLocker locker(m_CritSect);
    //if(!m_FeaturesMap.empty())
    //{
    //    for(m_IT = m_FeaturesMap.begin(); m_IT != m_FeaturesMap.end(); ++m_IT)
    //        OGRFeature::DestroyFeature(m_IT->second);
        m_FeaturesMap.clear();
    //}
    m_bIsDataLoaded = false;
	m_nFeatureCount = wxNOT_FOUND;
}
*/
void wxGISTable::Close(void)
{
    
    //TODO: Move to wxGISTableCached
	//UnloadFeatures();
    if(IsOpened())
    {
	    m_poLayer = NULL;
        if(m_poDS)
	    {
		    if(m_nSubType == enumTableQueryResult && m_poLayer)
			    m_poDS->ReleaseResultSet(m_poLayer);
		    if( m_poDS->Dereference() <= 0)
			    OGRDataSource::DestroyDataSource( m_poDS );
	        m_poDS = NULL;
	    }

	    m_Encoding = wxLocale::GetSystemEncoding();

	    m_bHasFID = false;
        m_nFeatureCount = wxNOT_FOUND;

        m_bOLCFastFeatureCount = false;
        m_bOLCStringsAsUTF8 = false;
    }
    wxGISDataset::Close();

    //Send event
    wxFeatureDSEvent event(wxDS_CLOSED);
    PostEvent(event);
}

size_t wxGISTable::GetFeatureCount(bool bForce, ITrackCancel* const pTrackCancel)
{
	if(m_nFeatureCount != wxNOT_FOUND)
		return m_nFeatureCount;
    if(	m_poLayer )
    {
        if(bForce)
            m_nFeatureCount = m_poLayer->GetFeatureCount(1);
        else if(m_bOLCFastFeatureCount)
            m_nFeatureCount = m_poLayer->GetFeatureCount(0);//TODO: Check
        else 
        {

        //TODO: Move to wxGISTableCached
		//LoadFeatures(pTrackCancel);
  //      if(!m_bIsDataLoaded)
  //          m_nFeatureCount = m_poLayer->GetFeatureCount(true);
        }

		m_nFeatureCount = m_poLayer->GetFeatureCount(0);

		return m_nFeatureCount;
    }
    return 0;
}

bool wxGISTable::CanDeleteFeature(void)
{
    if(!IsOpened())
        return false;
	return m_nSubType != enumTableQueryResult && m_poLayer && m_poLayer->TestCapability(OLCDeleteFeature);
}

OGRErr wxGISTable::DeleteFeature(long nFID)
{
	if(!CanDeleteFeature())
		return OGRERR_UNSUPPORTED_OPERATION;

	OGRErr eErr = m_poLayer->DeleteFeature(nFID);
    if(eErr != OGRERR_NONE)
        return eErr;

	m_nFeatureCount--;

    //TODO: move to wxGISTableCached
	//if(m_FeaturesMap[nFID] != NULL)
	//	m_FeaturesMap[nFID].reset();

    return eErr;
}

OGRErr wxGISTable::StoreFeature(wxGISFeature &Feature)
{
    if(!m_poLayer)
        return OGRERR_FAILURE;

    wxCriticalSectionLocker locker(m_CritSect);
    OGRErr eErr = m_poLayer->CreateFeature(Feature);
    if(eErr == OGRERR_NONE)
    {
		m_nFeatureCount++;

        if(!m_bHasFID)
            Feature.SetFID(m_nFeatureCount - 1);

        //bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);

		//long nFID;
  //      if(m_bHasFID)
  //          nFID = poFeature->GetFID();
  //      else
		//{
  //          nFID = m_nFeatureCount;
		//	poFeature->SetFID(nFID);
		//}

		//m_FeaturesMap[nFID] = poFeature;
    }
    return eErr;
}

wxGISFeature wxGISTable::CreateFeature(void)
{
    if(!m_poLayer)
        return wxGISFeature();
	OGRFeature* poFeature = OGRFeature::CreateFeature( m_poLayer->GetLayerDefn() );
	//if(poFeature && !m_bHasFID)
	//	poFeature->SetFID(GetFeatureCount());	
    wxGISFeature Feature( poFeature, m_Encoding );
	return Feature;
}

OGRErr wxGISTable::SetFeature(const wxGISFeature &Feature)
{
    if(!m_poLayer || m_nSubType == enumTableQueryResult || !Feature.IsOk())
		return OGRERR_FAILURE;

	OGRErr eErr = m_poLayer->SetFeature(Feature);
    if(eErr != OGRERR_NONE)
        return eErr;

    //TODO: move to wxGISTableCached
    /*
    if(m_bIsDataLoaded)
	{
		//std::map<long, OGRFeatureSPtr>::iterator pos = std::find(m_FeaturesMap.begin(), m_FeaturesMap.end(), poFeature);
		//if(pos != m_FeaturesMap.end())
		//	pos->second = OGRFeatureSPtr();

        for(std::map<long, OGRFeatureSPtr>::iterator IT = m_FeaturesMap.begin(); IT != m_FeaturesMap.end(); ++IT)
		{
			if(IT->second == poFeature)
				IT->second = OGRFeatureSPtr();
		}

		m_FeaturesMap[poFeature->GetFID()] = poFeature;
	}*/
    return eErr;
}

wxGISFeature wxGISTable::GetFeatureByID(long nFID)
{
	//if(m_FeaturesMap[nFID])
	//	return m_FeaturesMap[nFID];
	//else 
    if(m_poLayer)
	{
		OGRFeature* pFeature = m_poLayer->GetFeature(nFID);
		if(pFeature)
		{
			//m_FeaturesMap[nFID] = OGRFeatureSPtr(pFeature, OGRFeatureDeleter);
			//return m_FeaturesMap[nFID];
            return wxGISFeature(pFeature, m_Encoding);
		}
	}
	return wxGISFeature();
}

wxGISFeature wxGISTable::GetFeature(long nIndex)
{
    if(!m_poLayer)
        return wxGISFeature();
    if(m_poLayer->SetNextByIndex(nIndex) != OGRERR_NONE)
        return wxGISFeature();
    return wxGISFeature(m_poLayer->GetNextFeature(), m_Encoding);
}

void wxGISTable::Reset(void)
{
    if(!m_poLayer)
        return;
    m_nCurrentFID = 0;
    m_poLayer->ResetReading();
}

wxGISFeature wxGISTable::Next(void)
{
    if(!m_poLayer)
        return wxGISFeature();
    OGRFeature* pFeature = m_poLayer->GetNextFeature();
    if(!HasFID() && pFeature)
        pFeature->SetFID(m_nCurrentFID++);
    return wxGISFeature(pFeature, m_Encoding);
}

wxFeatureCursor wxGISTable::Search(const wxGISQueryFilter &QFilter, bool bOnlyFirst)
{
    wxFeatureCursor oOutCursor;
    if(	!m_poLayer )
		return oOutCursor;

	OGRErr eErr;
    if(QFilter.GetWhereClause().IsEmpty())
	{
        m_poLayer->ResetReading();
		//create and fill cursor
		OGRFeature* pFeature = NULL;
        while((pFeature = m_poLayer->GetNextFeature()) != NULL)
		{
			oOutCursor.Add(wxGISFeature(pFeature, m_Encoding));
			if(bOnlyFirst)
				break;
		}

		oOutCursor.Reset();
		return oOutCursor;
	}
    else
	{
		wxString sFilter = QFilter.GetWhereClause();
		//if(!m_sCurrentFilter.IsEmpty() && !sFilter.IsEmpty())
		//{
		//	//combain two clauses
		//	sFilter.Prepend(wxT("(") + m_sCurrentFilter + wxT(") AND ("));
		//	sFilter.Append(wxT(")"));
		//}
        eErr = m_poLayer->SetAttributeFilter(sFilter.mb_str());
		if(eErr != OGRERR_NONE)
			return oOutCursor;
		//create and fill cursor
		m_poLayer->ResetReading();
		OGRFeature* poFeature = NULL;
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
			oOutCursor.Add(wxGISFeature(poFeature, m_Encoding));
			if(bOnlyFirst)
				break;
		}

		oOutCursor.Reset();

        //set full selection back
        m_poLayer->SetAttributeFilter(NULL);

		return oOutCursor;
	}

    return oOutCursor;
}

OGRFeatureDefn* const wxGISTable::GetDefinition(void)
{
    if(	m_poLayer )
        return m_poLayer->GetLayerDefn();
    return NULL;
}

/*
OGRFeatureSPtr wxGISTable::Next(void)
{
    if(!m_poLayer)
        return OGRFeatureSPtr();

 	//bool bOLCRandomRead = m_poLayer->TestCapability(OLCRandomRead);

	wxCriticalSectionLocker locker(m_CritSect);
    if(m_bIsDataLoaded)
    {
        if(m_IT == m_FeaturesMap.end())
            return OGRFeatureSPtr();
        OGRFeatureSPtr pFeature = m_IT->second;
        m_IT++;
        return pFeature;
    }
    else
	{
		OGRFeatureSPtr pFeature( m_poLayer->GetNextFeature(), OGRFeatureDeleter );
		//if(pFeature && !m_bHasFID)
		//	pFeature->SetFID(++m_nFeatureCount);
        return pFeature;
	}

    return OGRFeatureSPtr();
}

void wxGISTable::Reset(void)
{
    if(!m_poLayer)
        return;

    //bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);
	m_nFIDCounter = 0;

    wxCriticalSectionLocker locker(m_CritSect);
    if(m_bIsDataLoaded)
        m_IT = m_FeaturesMap.begin();
    else
        m_poLayer->ResetReading();
}


OGRFeatureSPtr wxGISTable::GetAt(size_t nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < GetFeatureCount());

	if(!m_poLayer)
		return OGRFeatureSPtr();

	//bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(m_bIsDataLoaded)
	{
		wxCriticalSectionLocker locker(m_CritSect);
        std::map<long, OGRFeatureSPtr>::iterator it = m_FeaturesMap.begin();
        std::advance( it, nIndex );
        if(it == m_FeaturesMap.end())
			return OGRFeatureSPtr();
        return it->second;
    }
	else
    {
        m_poLayer->SetNextByIndex(nIndex);
		OGRFeatureSPtr pFeature( m_poLayer->GetNextFeature(), OGRFeatureDeleter );
		if(pFeature && !m_bHasFID)
			pFeature->SetFID(++m_nFeatureCount);
        return pFeature;
    }
    return OGRFeatureSPtr();
}

OGRFeatureSPtr wxGISTable::operator [](size_t nIndex)
{
	return GetAt(nIndex);
}

OGRErr wxGISTable::SetFilter(wxGISQueryFilter* pQFilter)
{
    if(	!m_poLayer )
		return OGRERR_FAILURE;

	OGRErr eErr;
	wxString sNewFilter;
    if(pQFilter)
	{
        eErr = m_poLayer->SetAttributeFilter(pQFilter->GetWhereClause().mb_str());
		sNewFilter = pQFilter->GetWhereClause();
	}
    else
        eErr = m_poLayer->SetAttributeFilter(NULL);

    if(eErr != OGRERR_NONE)
        return eErr;

	m_sCurrentFilter = sNewFilter;
    UnloadFeatures();
	LoadFeatures(NULL);
	Reset();
    return eErr;
}
*/
OGRErr wxGISTable::SetIgnoredFields(const wxArrayString &saIgnoredFields)
{
    if(	m_poLayer )
    {
        bool bOLCIgnoreFields = m_poLayer->TestCapability(OLCIgnoreFields) != 0;
        if(!bOLCIgnoreFields)
            return OGRERR_UNSUPPORTED_OPERATION;

        char **papszIgnoredFields = NULL;
        for(size_t i = 0; i < saIgnoredFields.GetCount(); ++i)
            papszIgnoredFields = CSLAddString( papszIgnoredFields, saIgnoredFields[i].mb_str(wxConvUTF8) );

        OGRErr eErr = m_poLayer->SetIgnoredFields((const char**)papszIgnoredFields);
        CSLDestroy( papszIgnoredFields );

        return eErr;
    }
    return OGRERR_FAILURE;
}

/*
wxString wxGISTable::GetAsString(OGRFeatureSPtr pFeature, int nField, wxFontEncoding Encoding)
{
    if(!pFeature)
		return wxEmptyString;

	OGRFieldDefn* pDef = pFeature->GetFieldDefnRef(nField);
    wxString sOut;
	switch(pDef->GetType())
	{
	case OFTDate:
		{
			int year, mon, day, hour, min, sec, flag;
			if(pFeature->GetFieldAsDateTime(nField, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
            {
			    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
                if(dt.IsValid())
				    sOut = dt.Format(_("%d-%m-%Y"));//wxString::Format(_("%.2u-%.2u-%.4u"), day, mon, year );
            }
            if(sOut == wxEmptyString)
                sOut = wxT("<NULL>");
		}
        break;
	case OFTTime:
		{
			int year, mon, day, hour, min, sec, flag;
			if(pFeature->GetFieldAsDateTime(nField, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
            {
			    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
                if(dt.IsValid())
				    sOut = dt.Format(_("%H:%M:%S"));//wxString::Format(_("%.2u:%.2u:%.2u"), hour, min, sec);
            }
            if(sOut == wxEmptyString)
                sOut = wxT("<NULL>");
		}
        break;
	case OFTDateTime:
		{
			int year, mon, day, hour, min, sec, flag;
			if(pFeature->GetFieldAsDateTime(nField, &year, &mon, &day, &hour, &min, &sec, &flag) == TRUE)
            {
			    wxDateTime dt(day, wxDateTime::Month(mon - 1), year, hour, min, sec);
                if(dt.IsValid())
                    sOut = dt.Format(_("%d-%m-%Y %H:%M:%S"));//wxString::Format(_("%.2u-%.2u-%.4u %.2u:%.2u:%.2u"), day, mon, year, hour, min, sec);
            }
            if(sOut == wxEmptyString)
                sOut = wxT("<NULL>");
		}
        break;
	case OFTReal:
		sOut = wxString::Format(wxT("%.12f"), pFeature->GetFieldAsDouble(nField));
        if(sOut == wxEmptyString)
            sOut = wxT("<NULL>");
        break;
	case OFTRealList:
		{
			int nCount(0);
			const double* pDblLst = pFeature->GetFieldAsDoubleList(nField, &nCount);
			for(size_t i = 0; i < nCount; ++i)
			{
				sOut += wxString::Format(wxT("%.12f|"), pDblLst[i]);
			}
		}
		break;
	case OFTIntegerList:
		{
			int nCount(0);
			const int* pIntLst = pFeature->GetFieldAsIntegerList(nField, &nCount);
			for(size_t i = 0; i < nCount; ++i)
			{
				sOut += wxString::Format(wxT("%.d|"), pIntLst[i]);
			}
		}
		break;
	case OFTStringList:
		{
			char** papszLinkList = pFeature->GetFieldAsStringList(nField);
			for(int i = 0; papszLinkList[i] != NULL; ++i )
			{
				sOut += wxString(papszLinkList[i], wxConvLocal);
				sOut += wxString(wxT("|"));
			}
		}
		break;
	default:
        {
            const char* pszStringData = pFeature->GetFieldAsString(nField);
            if(CPLStrnlen(pszStringData, 10) > 0)
            {
                if(Encoding <= wxFONTENCODING_DEFAULT)
                {
                    sOut = wxString(pFeature->GetFieldAsString(nField), wxConvLocal);
                    //if(sOut == wxEmptyString)
                    //    sOut = wxT(" ");
                }
                else if(Encoding == wxFONTENCODING_UTF8)
                    sOut = wxString(pFeature->GetFieldAsString(nField), wxConvUTF8);
                else
                {
                    sOut = wxString(pFeature->GetFieldAsString(nField), wxCSConv(Encoding));
                    if(sOut.IsEmpty())
                        sOut = wxString(pFeature->GetFieldAsString(nField), wxConvLocal);
                    //if(sOut == wxEmptyString)
                    //    sOut = wxT(" ");
                }
            }
        }
	}
	return sOut;
}

wxString wxGISTable::GetAsString(size_t row, int col)
{
	wxASSERT(GetFeatureCount() >= row);
	if(!m_poLayer)
		return wxEmptyString;

    //OGRFeatureDefn *pFDef = m_poLayer->GetLayerDefn();
    //int nFieldCount = pFDef->GetFieldCount();

    //long pos = row * nFieldCount + col;
    //if(m_FeatureStringData.GetCount() > pos)
    //    if(m_FeatureStringData[pos] != wxEmptyString)
    //        return m_FeatureStringData[pos];

	wxString sOut = GetAsString(GetAt(row), col, m_Encoding);
	//if(sOut.IsEmpty())
	//	return wxEmptyString;
 //   if(pos < MAXSTRINGSTORE)
 //   {
 //       if(m_FeatureStringData.GetCount() <= pos)
 //       {
 //           m_FeatureStringData.Add(wxEmptyString, pos - m_FeatureStringData.GetCount());
 //           m_FeatureStringData.Add(sOut);
 //       }
 //       else
 //           m_FeatureStringData[pos] = sOut;
 //   }
	return sOut;
}
*/

char **wxGISTable::GetFileList()
{
    char **papszFileList = NULL;
    CPLString szPath;
    //papszFileList = CSLAddString( papszFileList, osIMDFile );
    switch(m_nSubType)
    {
    case enumTableMapinfoTab:
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
	case enumTableMapinfoMif:
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
	case enumTablePostgres:
	case enumTableQueryResult:
	case enumTableDBF:
    default:
        break;
    }

    szPath = (char*)CPLResetExtension(m_sPath, "cpg");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "prj");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

wxFontEncoding wxGISTable::GetEncoding(void) const
{
     return m_Encoding;
}

void wxGISTable::SetEncoding(const wxFontEncoding &oEncoding)
{
    m_Encoding = oEncoding;
}