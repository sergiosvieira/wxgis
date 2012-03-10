/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing tool parameters domains.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxfilters.h"

/** \class wxGISGPValueDomain gpdomain.h
    \brief The domain storing variant values
*/
class WXDLLIMPEXP_GIS_GP wxGISGPValueDomain : public IGPDomain
{
public:
	wxGISGPValueDomain(void);
	virtual ~wxGISGPValueDomain (void);
	virtual void AddValue(const wxVariant& Element, wxString soNameStr);
	virtual size_t GetCount(void);
	virtual wxVariant GetValue(size_t nIndex);
	virtual wxString GetName(size_t nIndex);
    virtual void Clear(void);
	virtual bool GetAltered(void){return m_bAltered;};
	virtual void SetAltered(bool bAltered){m_bAltered = bAltered;};
	//
    virtual wxVariant GetValueByName(wxString soNameStr);
	virtual int GetPosByName(wxString sName);
	virtual int GetPosByValue(wxVariant oVal);
};

/** \class wxGISGPGxObjectDomain gpdomain.h
    \brief The domain storing GxObjectFilters
*/

class WXDLLIMPEXP_GIS_GP wxGISGPGxObjectDomain : public wxGISGPValueDomain
{
public:
    wxGISGPGxObjectDomain (void);
    virtual ~wxGISGPGxObjectDomain (void);
    virtual IGxObjectFilter* GetFilter(size_t nIndex);
	virtual void AddFilter(IGxObjectFilter* pFilter);
	virtual int GetPosByValue(wxVariant oVal);
};

inline void WXDLLIMPEXP_GIS_GP AddAllVectorFilters(wxGISGPGxObjectDomain* pDomain)
{
    for(size_t i = enumVecUnknown + 1; i < emumVecMAX; i++)
        pDomain->AddFilter(new wxGxFeatureFileFilter(wxGISEnumVectorDatasetType(i)));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecKML));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecKMZ));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecDXF));
    //pDomain->AddFilter(new wxGxFeatureFileFilter(enumVecGML));
}

inline void WXDLLIMPEXP_GIS_GP AddAllRasterFilters(wxGISGPGxObjectDomain* pDomain)
{
    for(size_t i = enumRasterUnknown + 1; i < enumRasterMAX; i++)
        pDomain->AddFilter(new wxGxRasterFilter(wxGISEnumRasterDatasetType(i)));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterTiff));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterImg));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterBmp));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterJpeg));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterPng));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterGif));
    //pDomain->AddFilter(new wxGxRasterFilter(enumRasterSAGA));
}

/** \class wxGISGPStringDomain gpdomain.h
    \brief The domain storing strings
*/
class WXDLLIMPEXP_GIS_GP wxGISGPStringDomain : public wxGISGPValueDomain
{
public:
    wxGISGPStringDomain (void);
    virtual ~wxGISGPStringDomain (void);
	virtual void AddString(wxString soStr, wxString soNameStr = wxEmptyString);
    virtual wxString GetString(size_t nIndex);
};


