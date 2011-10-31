/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Table class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

wxGISTable::wxGISTable(CPLString sPath, int nSubType, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISDataset(sPath)
{
	m_poDS = poDS;
	m_poLayer = poLayer;        
    m_nType = enumGISTableDataset;
    m_nSubType = nSubType;
	//
	m_Encoding = wxFONTENCODING_DEFAULT;
	m_bIsDataLoaded = false;
    m_bIsOpened = false;
	m_bHasFID = false;
	m_nFeatureCount = wxNOT_FOUND;

	//m_FeatureStringData.Alloc(10000);
}

wxGISTable::~wxGISTable(void)
{
	Close();
}

bool wxGISTable::Open(int iLayer, int bUpdate, bool bCache, ITrackCancel* pTrackCancel)
{
	if(m_bIsOpened)
		return true;

	m_bIsReadOnly = bUpdate == FALSE;

    if(!m_poLayer)
	{
		wxCriticalSectionLocker locker(m_CritSect);
		if(m_nSubType == enumTableQueryResult)
			bUpdate = FALSE;

		m_poDS = OGRSFDriverRegistrar::Open( m_sPath, bUpdate );
		//bug in FindFileInZip() [gdal-1.6.3\port\cpl_vsil_gzip.cpp]
		if( m_poDS == NULL )
		{
			m_sPath.replace( m_sPath.begin(), m_sPath.end(), '\\', '/' );
			m_poDS = OGRSFDriverRegistrar::Open( m_sPath, FALSE );
		}

		if( m_poDS == NULL )
		{
			const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("wxGISTable: Open failed! Path '%s'. OGR error: %s"), wxString(m_sPath, wxConvUTF8).c_str(), wxString(err, wxConvUTF8).c_str());
			wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
			return false;
		}

		wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
		if(FEnc != wxFONTENCODING_DEFAULT)
			m_Encoding = FEnc;

		int nLayerCount = m_poDS->GetLayerCount();
		if(nLayerCount == 1)
			m_poLayer = m_poDS->GetLayer(iLayer);
		else
			m_nType = enumGISContainer;
	}

    if(m_poLayer)
    {
        m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8) != 0;
        m_bHasFID = CPLStrnlen(m_poLayer->GetFIDColumn(), 100) > 0;
		wxString sOut;
		if(EQUALN(m_sPath, "/vsizip", 7))
			sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxCSConv(wxT("cp-866")));//TODO: Get from config cp-866
		else
			sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxConvUTF8);
		if(sOut.IsEmpty())
		{
			if(EQUALN(m_sPath, "/vsizip", 7))
				sOut = wxString(CPLGetBasename(m_sPath), wxCSConv(wxT("cp-866")));//TODO: Get from config cp-866
			else
				sOut = wxString(CPLGetBasename(m_sPath), wxConvUTF8);
		}
		m_sTableName = sOut;
	}

	m_bIsOpened = true;

	if(bCache)
		LoadFeatures(pTrackCancel);

	return true;
}

void wxGISTable::Cache(ITrackCancel* pTrackCancel)
{
	LoadFeatures(pTrackCancel);
}

wxString wxGISTable::GetName(void)
{
	if(!m_sTableName.IsEmpty())
		return m_sTableName;
	if( !m_poLayer )
		return wxEmptyString;

    wxString sOut;
    if(EQUALN(m_sPath, "/vsizip", 7))
        sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxCSConv(wxT("cp-866")));//TODO: Get from config cp-866
    else
        sOut = wxString(m_poLayer->GetLayerDefn()->GetName(), wxConvUTF8);
	if(sOut.IsEmpty())
	{
		if(EQUALN(m_sPath, "/vsizip", 7))
            sOut = wxString(CPLGetBasename(m_sPath), wxCSConv(wxT("cp-866")));//TODO: Get from config cp-866
        else
            sOut = wxString(CPLGetBasename(m_sPath), wxConvUTF8);
	}
	m_sTableName = sOut;
    return m_sTableName;
}

void wxGISTable::LoadFeatures(ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
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
	if(/*!m_bHasFID || */!bOLCFastFeatureCount || !bOLCRandomRead)
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
	wxCriticalSectionLocker locker(m_CritSect);
    //if(!m_FeaturesMap.empty())
    //{
    //    for(m_IT = m_FeaturesMap.begin(); m_IT != m_FeaturesMap.end(); ++m_IT)
    //        OGRFeature::DestroyFeature(m_IT->second);
        m_FeaturesMap.clear();
    //}
    m_bIsDataLoaded = false;
	m_nFeatureCount = wxNOT_FOUND;
}

void wxGISTable::Close(void)
{
	UnloadFeatures();

	m_bHasFID = false;
	m_poLayer = NULL;
    if(m_poDS)
	{
		if(m_nSubType == enumTableQueryResult)
			m_poDS->ReleaseResultSet(m_poLayer);
		if( m_poDS->Dereference() <= 0)
			OGRDataSource::DestroyDataSource( m_poDS );
	}
	m_poDS = NULL;

	m_Encoding = wxFONTENCODING_DEFAULT;
    m_bIsOpened = false;
	m_bHasFID = false;
}

size_t wxGISTable::GetFeatureCount(ITrackCancel* pTrackCancel)
{
	if(m_nFeatureCount != wxNOT_FOUND)
		return m_nFeatureCount;
    if(	m_poLayer )
    {
		LoadFeatures(pTrackCancel);
        if(!m_bIsDataLoaded)
            m_nFeatureCount = m_poLayer->GetFeatureCount(true);
		if(m_nFeatureCount == wxNOT_FOUND)
			m_nFeatureCount = 0;

		return m_nFeatureCount;
    }
    return 0;
}

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

OGRFeatureDefn* const wxGISTable::GetDefinition(void)
{
    if(	m_poLayer )
        return m_poLayer->GetLayerDefn();
    return NULL;
}

OGRErr wxGISTable::StoreFeature(OGRFeatureSPtr poFeature)
{
    if(!m_poLayer)
        return OGRERR_FAILURE;

    wxCriticalSectionLocker locker(m_CritSect);
    OGRErr eErr = m_poLayer->CreateFeature(poFeature.get());
    if(eErr == OGRERR_NONE)
    {
		m_nFeatureCount++;

        //bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);

		long nFID;
        if(m_bHasFID)
            nFID = poFeature->GetFID();
        else
		{
            nFID = m_nFeatureCount;
			poFeature->SetFID(nFID);
		}

		m_FeaturesMap[nFID] = poFeature;
    }
    return eErr;
}

OGRFeatureSPtr wxGISTable::CreateFeature(void)
{
    if(!m_poLayer)
        return OGRFeatureSPtr();
	OGRFeature* poFeature = OGRFeature::CreateFeature( m_poLayer->GetLayerDefn() );
	OGRFeatureSPtr pFeature( poFeature, OGRFeatureDeleter );
	if(pFeature && !m_bHasFID)
		pFeature->SetFID(GetFeatureCount());
	return pFeature;
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
        if(Encoding == wxFONTENCODING_DEFAULT)
        {
            sOut = wxString(pFeature->GetFieldAsString(nField), wxConvLocal);
            //if(sOut == wxEmptyString)
            //    sOut = wxT(" ");
        }
        else
        {
            sOut = wxString(pFeature->GetFieldAsString(nField), wxCSConv(Encoding));
            if(sOut.IsEmpty())
                sOut = wxString(pFeature->GetFieldAsString(nField), wxConvLocal);
            //if(sOut == wxEmptyString)
            //    sOut = wxT(" ");
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

wxFeatureCursorSPtr wxGISTable::Search(wxGISQueryFilter* pQFilter, bool bOnlyFirst)
{
    if(	!m_poLayer )
		return wxFeatureCursorSPtr();

	OGRErr eErr;
    if(pQFilter)
	{
		wxString sFilter = pQFilter->GetWhereClause();
		if(!m_sCurrentFilter.IsEmpty() && !sFilter.IsEmpty())
		{
			//combain two clauses
			sFilter.Prepend(wxT("(") + m_sCurrentFilter + wxT(") AND ("));
			sFilter.Append(wxT(")"));
		}
        eErr = m_poLayer->SetAttributeFilter(sFilter.mb_str());
		if(eErr != OGRERR_NONE)
			return wxFeatureCursorSPtr();
		//create and fill cursor
		m_poLayer->ResetReading();
		OGRFeature* poFeature = NULL;
		wxFeatureCursorSPtr pCursor = boost::make_shared<wxFeatureCursor>();
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
			pCursor->Add(OGRFeatureSPtr(poFeature, OGRFeatureDeleter));
			if(bOnlyFirst)
				break;
		}
		pCursor->Reset();
		return pCursor;
	}
    else
	{
        eErr = m_poLayer->SetAttributeFilter(NULL);
		if(eErr != OGRERR_NONE)
			return wxFeatureCursorSPtr();
		Reset();
		//create and fill cursor
		OGRFeatureSPtr pFeature;
		wxFeatureCursorSPtr pCursor = boost::make_shared<wxFeatureCursor>();
		while((pFeature = Next()) != NULL)
		{
			pCursor->Add(pFeature);
			if(bOnlyFirst)
				break;
		}
		pCursor->Reset();
		return pCursor;
	}
	return wxFeatureCursorSPtr();
}

bool wxGISTable::CanDeleteFeature(void)
{
	return m_poLayer && m_nSubType != enumTableQueryResult && m_poLayer->TestCapability(OLCDeleteFeature);
}

OGRErr wxGISTable::DeleteFeature(long nFID)
{
	if(!CanDeleteFeature())
		return OGRERR_UNSUPPORTED_OPERATION;

	OGRErr eErr = m_poLayer->DeleteFeature(nFID);
    if(eErr != OGRERR_NONE)
        return eErr;

	m_nFeatureCount--;
	if(m_FeaturesMap[nFID] != NULL)
		m_FeaturesMap[nFID].reset();
    return eErr;
}

OGRErr wxGISTable::SetFeature(OGRFeatureSPtr poFeature)
{
	if(!m_poLayer || m_nSubType == enumTableQueryResult || !poFeature)
		return OGRERR_FAILURE;

	OGRErr eErr = m_poLayer->SetFeature(poFeature.get());
    if(eErr != OGRERR_NONE)
        return eErr;
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
	}
    return eErr;
}

OGRErr wxGISTable::SetIgnoredFields(wxArrayString &saIgnoredFields)
{
    if(	m_poLayer )
    {
        bool bOLCIgnoreFields = m_poLayer->TestCapability(OLCIgnoreFields) != 0;
        if(!bOLCIgnoreFields)
            return OGRERR_UNSUPPORTED_OPERATION;

        char **papszIgnoredFields = NULL;
        for(size_t i = 0; i < saIgnoredFields.GetCount(); ++i)
            papszIgnoredFields = CSLAddString( papszIgnoredFields, saIgnoredFields[i].mb_str(wxConvLocal) );

        OGRErr eErr = m_poLayer->SetIgnoredFields((const char**)papszIgnoredFields);
        CSLDestroy( papszIgnoredFields );

        if(eErr != OGRERR_NONE)
            return eErr;
        	
        UnloadFeatures();
		LoadFeatures(NULL);
        return eErr;
    }
    return OGRERR_FAILURE;
}

OGRFeatureSPtr wxGISTable::GetFeature(long nFID)
{
	if(m_FeaturesMap[nFID])
		return m_FeaturesMap[nFID];
	else if(m_poLayer)
	{
		OGRFeature* pFeature = m_poLayer->GetFeature(nFID);
		if(pFeature)
		{
			m_FeaturesMap[nFID] = OGRFeatureSPtr(pFeature, OGRFeatureDeleter);
			return m_FeaturesMap[nFID];
		}
	}
	return OGRFeatureSPtr();
}

bool wxGISTable::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();

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
    
    m_sPath = szCopyFileName;

	CSLDestroy( papszFileList );
	CSLDestroy( papszFileCopiedList );
	return true;
}

bool wxGISTable::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();

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

    m_sPath = CPLFormFilename(szDestPath, CPLGetFilename(m_sPath), NULL);

	CSLDestroy( papszFileList );
	CSLDestroy( papszMovedFileList );
    return true;
}

bool wxGISTable::Delete(int iLayer, ITrackCancel* pTrackCancel)
{
	wxCriticalSectionLocker locker(m_CritSect);
    Close();
    
    if(!DeleteFile(m_sPath))
        return false;
	
    char** papszFileList = GetFileList();
    if(papszFileList)
    {
		IProgressor* pProgressor(NULL);
		if(pTrackCancel)
		{
			pProgressor = pTrackCancel->GetProgressor();
			if(pProgressor)
				pProgressor->SetRange(CSLCount(papszFileList));
		}
        for(int i = 0; papszFileList[i] != NULL; ++i )
		{
			DeleteFile(papszFileList[i]);
			if(pProgressor)
				pProgressor->SetValue(i);
		}
        CSLDestroy( papszFileList );
    }
	return true;
}

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

    return papszFileList;
}

bool wxGISTable::Rename(wxString sNewName)
{
	wxCriticalSectionLocker locker(m_CritSect);

    CPLString szDirPath = CPLGetPath(m_sPath);
    CPLString szName = CPLGetBasename(m_sPath);
	CPLString szNewName = ClearExt(sNewName).mb_str(wxConvUTF8);

    char** papszFileList = GetFileList();
    papszFileList = CSLAddString( papszFileList, m_sPath );
    if(!papszFileList)
        return false;

    char **papszNewFileList = NULL;

	Close();

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
    
	m_sPath = CPLString(CPLFormFilename(szDirPath, szNewName, CPLGetExtension(m_sPath)));

	CSLDestroy( papszFileList );
	CSLDestroy( papszNewFileList );
	return true;
}

OGRLayer* const wxGISTable::GetLayerRef(int iLayer)
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