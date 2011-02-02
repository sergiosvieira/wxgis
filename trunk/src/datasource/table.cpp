/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
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

wxGISTable::wxGISTable(OGRLayer* poLayer, CPLString sPath, wxGISEnumTableDatasetType nType) : wxGISDataset(sPath), m_pDS(NULL)
{

	m_poLayer = poLayer;
        
    m_nType = enumGISTableDataset;
    if(m_poLayer)
    {
        m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
        m_bHasFID = CPLStrnlen(m_poLayer->GetFIDColumn(), 100) > 0;
        //if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
            m_sTableName = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        //else
        //{
        //    wxCSConv conv(m_Encoding);
        //    m_sTableName = conv.cMB2WX(m_poLayer->GetLayerDefn()->GetName());
        //    if(m_sTableName.IsEmpty())
        //        m_sTableName = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        //}
	    m_bIsOpened = true;
    }
	m_Encoding = wxFONTENCODING_DEFAULT;
    m_nSubType = (int)nType;
	m_bIsDataLoaded = false;
	m_FieldCount = -1;
	m_nSize = -1;
	m_FeatureStringData.Alloc(10000);
}

wxGISTable::~wxGISTable(void)
{
	Close();
}

void wxGISTable::Close(void)
{
	Unload();
	if(m_nSubType == enumTableQueryResult && m_pDS)
		m_pDS->ReleaseResultSet(m_poLayer);
	m_bHasFID = false;
 	m_FieldCount = -1;
	m_nSize = -1;
	m_poLayer = NULL;
}

size_t wxGISTable::GetSize(void)
{
	if(m_nSize != -1)
		return m_nSize;
    if(	m_poLayer )
    {
    	bool bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount);
        if(bOLCFastFeatureCount && m_nSubType != enumTableQueryResult)
		{
			m_nSize = m_poLayer->GetFeatureCount(true);
            return m_nSize;
		}
        //load all features

		PreLoad();
        if(m_bIsDataLoaded)
			m_nSize = m_FeaturesMap.size();
        else
            m_nSize = m_poLayer->GetFeatureCount(true);
		return m_nSize;
    }
    return 0;
}

OGRFeature* wxGISTable::Next(void)
{
    if(!m_poLayer)
        return NULL;

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
    return NULL;
}

void wxGISTable::Reset(void)
{
    if(!m_poLayer)
        return;

    wxCriticalSectionLocker locker(m_CritSect);
    bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(bOLCFastSetNextByIndex)
        m_poLayer->ResetReading();
    else
        m_IT = m_FeaturesMap.begin();
}

OGRFeatureDefn* wxGISTable::GetDefinition(void)
{
    if(	m_poLayer )
        return m_poLayer->GetLayerDefn();
    return NULL;
}

void wxGISTable::PreLoad(void)
{
    if(	m_poLayer )
    {
        m_poLayer->ResetReading();
        long counter = 1;

		OGRFeature *poFeature;
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
            if(!poFeature->GetDefnRef())
            {
                OGRFeature::DestroyFeature(poFeature);
                continue;
            }

            long nFID;
            if(!m_bHasFID)
			{
                nFID = counter;
				poFeature->SetFID(nFID);
			}
            else
                nFID = poFeature->GetFID();

            //store features in array for speed
            m_FeaturesMap[nFID] = poFeature;
            //store field as string in array
			counter++;
		}
		m_bIsDataLoaded = true;
    }
}

void wxGISTable::Unload(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(!m_bIsDataLoaded)
        return;
    if(!m_FeaturesMap.empty())
    {
        for(m_IT = m_FeaturesMap.begin(); m_IT != m_FeaturesMap.end(); ++m_IT)
            OGRFeature::DestroyFeature(m_IT->second);
        m_FeaturesMap.clear();
    }
    m_FeatureStringData.clear();
    m_bIsDataLoaded = false;
}

OGRErr wxGISTable::CreateFeature(OGRFeature* poFeature)
{
    if(!m_poLayer)
        return OGRERR_FAILURE;
    wxCriticalSectionLocker locker(m_CritSect);
    OGRErr eErr = m_poLayer->CreateFeature(poFeature);
    if(eErr == OGRERR_NONE)
    {
		m_nSize++;

        bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);

		long nFID;
        if(m_bHasFID)
            nFID = poFeature->GetFID();
        else
		{
            nFID = m_poLayer->GetFeatureCount(false);
			poFeature->SetFID(nFID);
		}

        if(!bOLCFastSetNextByIndex)
			m_FeaturesMap[nFID] = poFeature;
		else
            OGRFeature::DestroyFeature(poFeature);
    }
    return eErr;
}

OGRFeature* wxGISTable::GetAt(long nIndex)
{
	wxASSERT(nIndex >= 0);
	wxASSERT(nIndex < GetSize());
	wxASSERT(m_poLayer != NULL);

	bool bOLCFastSetNextByIndex = m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(bOLCFastSetNextByIndex)
    {
        m_poLayer->SetNextByIndex(nIndex);
        OGRFeature* pFeature = m_poLayer->GetNextFeature();
        return pFeature;
    }
	else
	{
		if(m_FeaturesMap.empty())
			PreLoad();

		wxCriticalSectionLocker locker(m_CritSect);
        if(m_FeaturesMap.size() - 1 < nIndex)
        {
            m_poLayer->SetNextByIndex(nIndex);
            OGRFeature* pFeature = m_poLayer->GetNextFeature();
            if(!pFeature)
                return NULL;

			long nFID;
			if(m_bHasFID)
				nFID = pFeature->GetFID();
			else
			{
				nFID = m_poLayer->GetFeatureCount(false);
				pFeature->SetFID(nFID);
			}

            m_FeaturesMap[nFID] = pFeature;
            return pFeature->Clone();
        }
        std::map<long, OGRFeature*>::iterator it = m_FeaturesMap.begin();
        std::advance( it, nIndex );
        if(it == m_FeaturesMap.end())
			return NULL;
        return it->second->Clone();
    }
    return NULL;
}

OGRFeature* wxGISTable::operator [](long nIndex)
{
	return GetAt(nIndex);
}

wxString wxGISTable::GetAsString(OGRFeature* pFeature, int nField, wxFontEncoding Encoding)
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
                sOut = wxT("NULL");
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
                sOut = wxT("NULL");
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
                sOut = wxT("NULL");
		}
        break;
	case OFTReal:
		sOut = wxString::Format(wxT("%.12f"), pFeature->GetFieldAsDouble(nField));
        if(sOut == wxEmptyString)
            sOut = wxT("NULL");
        break;
	case OFTRealList:
		{
			int nCount(0);
			const double* pDblLst = pFeature->GetFieldAsDoubleList(nField, &nCount);
			for(size_t i = 0; i < nCount; i++)
			{
				sOut += wxString::Format(wxT("%.12f|"), pDblLst[i]);
			}
		}
		break;
	case OFTIntegerList:
		{
			int nCount(0);
			const int* pIntLst = pFeature->GetFieldAsIntegerList(nField, &nCount);
			for(size_t i = 0; i < nCount; i++)
			{
				sOut += wxString::Format(wxT("%.d|"), pIntLst[i]);
			}
		}
		break;
	case OFTStringList:
		{
			char** papszLinkList = pFeature->GetFieldAsStringList(nField);
			for(int i = 0; papszLinkList[i] != NULL; i++ )
			{
				sOut += wgMB2WX(papszLinkList[i]);
				sOut += wxString(wxT("|"));
			}
		}
		break;
	default:
        if(Encoding == wxFONTENCODING_DEFAULT)
        {
            sOut = wgMB2WX(pFeature->GetFieldAsString(nField));
            if(sOut == wxEmptyString)
                sOut = wxT(" ");
        }
        else
        {
            wxCSConv conv(Encoding);
            sOut = conv.cMB2WX(pFeature->GetFieldAsString(nField));
            if(sOut.IsEmpty())
                sOut = wgMB2WX(pFeature->GetFieldAsString(nField));
            if(sOut == wxEmptyString)
                sOut = wxT(" ");
        }
	}
	return sOut;
}

wxString wxGISTable::GetAsString(long row, int col)
{
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
		wxString sOut = GetAsString(pFeature, col, m_Encoding);
        OGRFeature::DestroyFeature(pFeature);
		if(sOut.IsEmpty())
			return wxEmptyString;
        if(pos < MAXSTRINGSTORE)
        {
            if(m_FeatureStringData.GetCount() <= pos)
            {
                m_FeatureStringData.Add(wxEmptyString, pos - m_FeatureStringData.GetCount());
                m_FeatureStringData.Add(sOut);
            }
            else
                m_FeatureStringData[pos] = sOut;
        }
		return sOut;
	}
	return wxEmptyString;
}

OGRErr wxGISTable::SetFilter(wxGISQueryFilter* pQFilter)
{
    if(	m_poLayer )
    {
        OGRErr eErr;
        if(pQFilter)
            eErr = m_poLayer->SetAttributeFilter(wgWX2MB(pQFilter->GetWhereClause()));
        else
            eErr = m_poLayer->SetAttributeFilter(NULL);
        if(eErr != OGRERR_NONE)
            return eErr;

        m_nSize = -1;
        Unload();
        return eErr;
    }
    return OGRERR_FAILURE;
}

OGRErr wxGISTable::DeleteFeature(long nFID)
{
    if(	m_poLayer && m_nSubType != enumTableQueryResult)
    {
        OGRErr eErr = m_poLayer->DeleteFeature(nFID);
        if(eErr != OGRERR_NONE)
            return eErr;

        //m_nSize = -1;
        //Unload();
		//May be erase m_FeaturesMap[nFID]
		m_nSize--;
		if(!m_FeaturesMap.empty())
		{
			OGRFeature::DestroyFeature(m_FeaturesMap[nFID]);
			m_FeaturesMap[nFID] = NULL;
		}
        return eErr;
    }
    return OGRERR_FAILURE;
}

OGRErr wxGISTable::SetFeature(OGRFeature *poFeature)
{
    if(	m_poLayer && m_nSubType != enumTableQueryResult && poFeature)
    {
        OGRErr eErr = m_poLayer->SetFeature(poFeature);
        if(eErr != OGRERR_NONE)
            return eErr;
		if(!m_FeaturesMap.empty())
		{
			//std::map<long, OGRFeature*>::iterator pos = std::find(m_FeaturesMap.begin(), m_FeaturesMap.end(), poFeature);
			//if(pos != m_FeaturesMap.end())
			//{
			//	OGRFeature::DestroyFeature(pos->second);
			//	pos->second = NULL;
			//}


	        for(std::map<long, OGRFeature*>::iterator IT = m_FeaturesMap.begin(); IT != m_FeaturesMap.end(); ++IT)
			{
				if(IT->second == poFeature)
				{
					OGRFeature::DestroyFeature(IT->second);
					IT->second = NULL;
				}
			}

			m_FeaturesMap[poFeature->GetFID()] = poFeature;
		}
        return eErr;
    }
    return OGRERR_FAILURE;
}

OGRErr wxGISTable::SetIgnoredFields(wxArrayString &saIgnoredFields)
{
    if(	m_poLayer )
    {
        bool bOLCIgnoreFields = m_poLayer->TestCapability(OLCIgnoreFields);
        if(!bOLCIgnoreFields)
            return OGRERR_UNSUPPORTED_OPERATION;

        char **papszIgnoredFields = NULL;
        for(size_t i = 0; i < saIgnoredFields.GetCount(); i++)
            papszIgnoredFields = CSLAddString( papszIgnoredFields, wgWX2MB(saIgnoredFields[i]) );

        OGRErr eErr = m_poLayer->SetIgnoredFields((const char**)papszIgnoredFields);
        CSLDestroy( papszIgnoredFields );

        if(eErr != OGRERR_NONE)
            return eErr;
        	
        m_FieldCount = -1;
        Unload();
        return eErr;
    }
    return OGRERR_FAILURE;
}

OGRFeature* wxGISTable::GetFeature(long nFID)
{
	if(m_FeaturesMap[nFID])
		return m_FeaturesMap[nFID];
	else if(m_poLayer)
		return m_poLayer->GetFeature(nFID);
	else
		return NULL;
}
