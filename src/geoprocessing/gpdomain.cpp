/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing tool parameters domains.
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

#include "wxgis/geoprocessing/gpdomain.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPValueDomain
///////////////////////////////////////////////////////////////////////////////

wxGISGPValueDomain::wxGISGPValueDomain(void) 
{
	m_bAltered = false;
}

wxGISGPValueDomain::~wxGISGPValueDomain(void)
{
}

void wxGISGPValueDomain::AddValue(const wxVariant& Element, wxString soNameStr)
{
	m_asoData.push_back(Element);
	m_asoNames.Add(soNameStr);
}

size_t wxGISGPValueDomain::GetCount(void)
{
	return m_asoData.size(); 
}

wxVariant wxGISGPValueDomain::GetValue(size_t nIndex)
{ 
	wxASSERT(nIndex < m_asoData.size());
	return m_asoData[nIndex]; 
}

wxString wxGISGPValueDomain::GetName(size_t nIndex)
{ 
	return m_asoNames[nIndex]; 
}

void wxGISGPValueDomain::Clear(void)
{
	m_asoData.clear();
	m_asoNames.Empty();
	m_bAltered = true;
}

wxVariant wxGISGPValueDomain::GetValueByName(wxString soNameStr)
{
	int nPos = m_asoNames.Index(soNameStr);
	wxVariant result;
	if(nPos != wxNOT_FOUND)
		result = m_asoData[nPos];
	return result;
}

int wxGISGPValueDomain::GetPosByName(wxString sName)
{
	return m_asoNames.Index(sName);
}

int wxGISGPValueDomain::GetPosByValue(wxVariant oVal)
{
	int nPos = wxNOT_FOUND;
	for(size_t i = 0; i < m_asoData.size(); ++i)
	{
		if(m_asoData[i] == oVal)
		{
			nPos = i;
			break;
		}
	}
	return nPos;
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPGxObjectDomain
///////////////////////////////////////////////////////////////////////////////

wxGISGPGxObjectDomain::wxGISGPGxObjectDomain(void) : wxGISGPValueDomain()
{
}

wxGISGPGxObjectDomain::~wxGISGPGxObjectDomain(void)
{
    for(size_t i = 0; i < m_asoData.size(); ++i)
	{
		IGxObjectFilter* pFil = (IGxObjectFilter*)m_asoData[i].GetVoidPtr();
		wxDELETE(pFil);
	}
}

void wxGISGPGxObjectDomain::AddFilter(IGxObjectFilter* pFilter)
{
	AddValue(wxVariant((void*)pFilter), pFilter->GetName());
}

IGxObjectFilter* wxGISGPGxObjectDomain::GetFilter(size_t nIndex)
{
	return (IGxObjectFilter*)GetValue(nIndex).GetVoidPtr();
}

int wxGISGPGxObjectDomain::GetPosByValue(wxVariant oVal)
{
    if(!oVal.IsNull())
    {
		wxFileName oName(oVal.GetString());
        for(size_t i = 0; i < m_asoData.size(); ++i)
        {
            IGxObjectFilter* poFilter = GetFilter(i);
            if(poFilter)
            {
                if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                {
                    return i;
                }
            }
        }
    }
	return wxNOT_FOUND;
}
///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPStringDomain
///////////////////////////////////////////////////////////////////////////////

wxGISGPStringDomain::wxGISGPStringDomain(void) : wxGISGPValueDomain()
{
}

wxGISGPStringDomain::~wxGISGPStringDomain(void)
{
}

void wxGISGPStringDomain::AddString(wxString soStr, wxString soName)
{
	if(soName.IsEmpty())
		AddValue(wxVariant(soStr), soStr);
	else
		AddValue(wxVariant(soStr), soName);
}

wxString wxGISGPStringDomain::GetString(size_t nIndex)
{
	return GetValue(nIndex).GetString();
}


