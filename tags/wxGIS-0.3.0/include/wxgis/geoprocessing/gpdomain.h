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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxfilters.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPGxObjectDomain
///////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GP wxGISGPGxObjectDomain : public IGPDomain
{
public:
    wxGISGPGxObjectDomain (void);
    virtual ~wxGISGPGxObjectDomain (void);
	virtual void AddFilter(IGxObjectFilter* pFilter);
    virtual IGxObjectFilter* GetFilter(size_t nIndex);
    virtual size_t GetCount(void);
    virtual void SetSel(size_t nIndex);
    virtual size_t GetSel(void);
protected:
	OBJECTFILTERS m_FilterArray;
    size_t m_nSelFilterIndex;
};


/** \class wxGISGPStringDomain gpdomain.h
    \brief The domain storing strings
*/
class WXDLLIMPEXP_GIS_GP wxGISGPStringDomain : public IGPDomain
{
public:
    wxGISGPStringDomain (void);
    virtual ~wxGISGPStringDomain (void);
	virtual void AddString(wxString soStr, wxString soInternalStr = wxEmptyString);
    virtual size_t GetCount(void);
    virtual wxString GetInternalString(size_t dIndex);
    virtual wxString GetExternalString(size_t dIndex);
    virtual wxString GetInternalString(wxString soInternalStr);
    virtual wxString GetExternalString(wxString soStr);
    virtual wxArrayString GetArrayString() const;
    virtual void SetSel(size_t nIndex);
    virtual size_t GetSel(void);
protected:
	wxArrayString m_asoData;
	wxArrayString m_asoInternalData;
    size_t m_nSelIndex;
};

inline void WXDLLIMPEXP_GIS_GP AddAllVectorFilters(wxGISGPGxObjectDomain* pDomain)
{
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile));
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab));
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif));
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecKML));
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecKMZ));
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecDXF));
    pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecGML));
}

inline void WXDLLIMPEXP_GIS_GP AddAllRasterFilters(wxGISGPGxObjectDomain* pDomain)
{
    pDomain->AddFilter(new wxGxRasterFilter(enumRasterTiff));
    pDomain->AddFilter(new wxGxRasterFilter(enumRasterImg));
    pDomain->AddFilter(new wxGxRasterFilter(enumRasterBmp));
    pDomain->AddFilter(new wxGxRasterFilter(enumRasterJpeg));
    pDomain->AddFilter(new wxGxRasterFilter(enumRasterPng));
    pDomain->AddFilter(new wxGxRasterFilter(enumRasterGif));
}
