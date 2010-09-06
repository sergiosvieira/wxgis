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
    if(nIndex >= GetFilterCount())
        return NULL;
    return m_FilterArray[nIndex];
}

size_t wxGISGPGxObjectDomain::GetFilterCount(void)
{
    return m_FilterArray.size();
}

 void wxGISGPGxObjectDomain::SetSelFilter(size_t nIndex)
 {
     m_nSelFilterIndex = nIndex;
 }

 size_t wxGISGPGxObjectDomain::GetSelFilter(void)
 {
     return m_nSelFilterIndex;
 }

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPStringDomain
///////////////////////////////////////////////////////////////////////////////

wxGISGPStringDomain::wxGISGPStringDomain(void) : m_nSelFilterIndex(0)
{
}

wxGISGPStringDomain::~wxGISGPStringDomain(void)
{
}

void wxGISGPStringDomain::AddString(wxString soStr)
{
    m_asoData.Add(soStr);
}

size_t wxGISGPStringDomain::GetStringCount(void)
{
    return m_asoData.GetCount();
}

wxString wxGISGPStringDomain::GetString(size_t dIndex)
{
    return m_asoData[dIndex];
}

wxArrayString wxGISGPStringDomain::GetArrayString() const
{
    return m_asoData;
}

void wxGISGPStringDomain::SetSelString(size_t nIndex)
{
     m_nSelFilterIndex = nIndex;
}

size_t wxGISGPStringDomain::GetSelString(void)
{
     return m_nSelFilterIndex;
}
