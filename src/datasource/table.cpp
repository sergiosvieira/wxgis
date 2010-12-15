/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Table class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

wxGISTable::wxGISTable(OGRDataSource *poDS, OGRLayer* poLayer, wxString sPath, wxGISEnumTableDatasetType nType) : wxGISDataset(sPath)
{
	m_poDS = poDS;
	m_poLayer = poLayer;
    m_nType = enumGISTableDataset;
    m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
    m_nSubType = (int)nType;
	m_bIsOpened = true;
	m_bIsDataLoaded = false;
	m_FieldCount = -1;
	m_nSize = -1;
	m_sTableName = GetName();
	m_FeatureStringData.Alloc(10000);
	m_Encoding = wxFONTENCODING_DEFAULT;
}
wxGISTable::~wxGISTable(void)
{
	Close();
}

void wxGISTable::Close(void)
{
	Unload();
	m_bIsOpened = false;
	m_bHasFID = false;
 	m_FieldCount = -1;
	m_nSize = -1;
   if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
	m_poDS = NULL;
	m_poLayer = NULL;
}

wxString wxGISTable::GetName(void)
{
	if(!m_sTableName.IsEmpty())
		return m_sTableName;

	if(!m_bIsOpened)
		if(!Open())
            return wxEmptyString;
	if(	m_poLayer )
    {
        wxString sOut;
        if(m_bOLCStringsAsUTF8 || m_Encoding == wxFONTENCODING_DEFAULT)
            m_sTableName = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        else
        {
            wxCSConv conv(m_Encoding);
            m_sTableName = conv.cMB2WX(m_poLayer->GetLayerDefn()->GetName());
            if(m_sTableName.IsEmpty())
                m_sTableName = wgMB2WX(m_poLayer->GetLayerDefn()->GetName());
        }
        return m_sTableName;
    }
    return wxEmptyString;
}

bool wxGISTable::Open(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(m_bIsOpened)
		return true;

    m_poDS = OGRSFDriverRegistrar::Open( wgWX2MB(m_sPath), FALSE );
	if( m_poDS == NULL )
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISTable: Open failed! Path '%s'. OGR error: %s"), m_sPath.c_str(), wgMB2WX(err));
		wxLogError(sErr);
		return false;
	}
	m_poLayer = m_poDS->GetLayerByName(wgWX2MB(m_sTableName));
	if(m_poLayer)
	{
		m_bIsOpened = true;
		m_bHasFID = strlen(m_poLayer->GetFIDColumn()) > 0;
		return true;
	}
	return false;
}

size_t wxGISTable::GetSize(void)
{
	if(m_nSize != -1)
		return m_nSize;
    if(!m_bIsOpened)
        if(!Open())
            return NULL;
    if(	m_poLayer )
    {
    	bool bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount);
        if(bOLCFastFeatureCount)
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

void wxGISTable::Reset(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
    if(bOLCFastSetNextByIndex)
        m_poLayer->ResetReading();
    else
        m_IT = m_FeaturesMap.begin();
}

OGRFeatureDefn* wxGISTable::GetDefiniton(void)
{
    if(!m_bIsOpened)
        if(!Open())
            return NULL;
    if(	m_poLayer )
        return m_poLayer->GetLayerDefn();
    return NULL;
}

void wxGISTable::PreLoad(void)
{
    if(m_bIsDataLoaded)
        return;

    if(!m_bIsOpened)
        if(!Open())
            return;
    if(	m_poLayer && !m_poLayer->TestCapability(OLCFastSetNextByIndex))
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
    m_bIsDataLoaded = false;
}

OGRErr wxGISTable::CreateFeature(OGRFeature* poFeature)
{
    if(!m_bIsOpened)
        if(!Open())
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