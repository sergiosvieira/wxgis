/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing tool parameters domains.
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

#include "wxgis/geoprocessing/gpdomain.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPGxObjectDomain
///////////////////////////////////////////////////////////////////////////////

wxGISGPGxObjectDomain::wxGISGPGxObjectDomain(void) : m_nSelFilterIndex(0)
{
}

wxGISGPGxObjectDomain::~wxGISGPGxObjectDomain(void)
{
    for(size_t i = 0; i < m_FilterArray.size(); i++)
        wxDELETE(m_FilterArray[i]);
}

void wxGISGPGxObjectDomain::AddFilter(IGxObjectFilter* pFilter)
{
    m_FilterArray.push_back(pFilter);
}

IGxObjectFilter* wxGISGPGxObjectDomain::GetFilter(size_t nIndex)
{
    if(nIndex >= GetCount())
        return NULL;
    return m_FilterArray[nIndex];
}

size_t wxGISGPGxObjectDomain::GetCount(void)
{
    return m_FilterArray.size();
}

 void wxGISGPGxObjectDomain::SetSel(size_t nIndex)
 {
     m_nSelFilterIndex = nIndex;
 }

 size_t wxGISGPGxObjectDomain::GetSel(void)
 {
     return m_nSelFilterIndex;
 }

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPStringDomain
///////////////////////////////////////////////////////////////////////////////

wxGISGPStringDomain::wxGISGPStringDomain(void) : m_nSelIndex(0)
{
}

wxGISGPStringDomain::~wxGISGPStringDomain(void)
{
}

void wxGISGPStringDomain::AddString(wxString soStr, wxString soInternalStr)
{
    m_asoData.Add(soStr);
    if(soInternalStr.IsEmpty())
        soInternalStr = soStr;
    m_asoInternalData.Add(soInternalStr);
}

size_t wxGISGPStringDomain::GetCount(void)
{
    return m_asoData.GetCount();
}

wxString wxGISGPStringDomain::GetExternalString(size_t dIndex)
{
    return m_asoData[dIndex];
}

wxString wxGISGPStringDomain::GetInternalString(size_t dIndex)
{
    return m_asoInternalData[dIndex];
}

wxArrayString wxGISGPStringDomain::GetArrayString() const
{
    return m_asoData;
}

void wxGISGPStringDomain::SetSel(size_t nIndex)
{
     m_nSelIndex = nIndex;
}

size_t wxGISGPStringDomain::GetSel(void)
{
     return m_nSelIndex;
}

wxString wxGISGPStringDomain::GetInternalString(wxString soInternalStr)
{
    int nPos = m_asoInternalData.Index(soInternalStr);
    if(nPos == wxNOT_FOUND)
        return wxEmptyString;
    return m_asoData[nPos];
}

wxString wxGISGPStringDomain::GetExternalString(wxString soStr)
{
    int nPos = m_asoData.Index(soStr);
    if(nPos == wxNOT_FOUND)
        return wxEmptyString;
    return m_asoInternalData[nPos];
}

