/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/catalog/gxspatreffolder.h"
#include "wxgis/datasource/datasource.h"

//------------------------------------------------------------
// wxGxObjectFilter
//------------------------------------------------------------

wxGxObjectFilter::wxGxObjectFilter(void)
{
}

wxGxObjectFilter::~wxGxObjectFilter(void)
{
}

bool wxGxObjectFilter::CanChooseObject( IGxObject* pObject )
{
	return true;
}

bool wxGxObjectFilter::CanDisplayObject( IGxObject* pObject )
{
	return true;
}

wxGISEnumSaveObjectResults wxGxObjectFilter::CanSaveObject( IGxObject* pLocation, wxString sName )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pLocation);
	if(!pContainer)
		return enumGISSaveObjectDeny;
	if(pContainer->CanCreate(GetType(), GetSubType()))
	{
		if(pContainer->SearchChild(pLocation->GetFullName() + wxFileName::GetPathSeparator() + sName) == NULL)
			return enumGISSaveObjectAccept;
		else
			return enumGISSaveObjectExists;
	}	
	return enumGISSaveObjectDeny;
}

wxString wxGxObjectFilter::GetName(void)
{
	return wxString(_("Any items (*.*)"));
}

wxString wxGxObjectFilter::GetExt(void)
{
	return wxEmptyString;
}

wxString wxGxObjectFilter::GetDriver(void)
{
	return wxEmptyString;
}

int wxGxObjectFilter::GetSubType(void)
{
    return 0;
}

wxGISEnumDatasetType wxGxObjectFilter::GetType(void)
{
    return enumGISAny;
}

//------------------------------------------------------------
// wxGxPrjFileFilter
//------------------------------------------------------------

wxGxPrjFileFilter::wxGxPrjFileFilter(void)
{
}

wxGxPrjFileFilter::~wxGxPrjFileFilter(void)
{
}

bool wxGxPrjFileFilter::CanChooseObject( IGxObject* pObject )
{
	wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pObject);
	if(pGxPrjFile)
		return true;
	else
		return false;
}

bool wxGxPrjFileFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	wxGxPrjFile* pGxPrjFile = dynamic_cast<wxGxPrjFile*>(pObject);
	if(pGxPrjFile)
		return true;
	else
		return false;
}

wxString wxGxPrjFileFilter::GetName(void)
{
	return wxString(_("Coordinate Systems (*.prj, *.spr)"));
}

wxString wxGxPrjFileFilter::GetExt(void)
{
	return wxString(wxT("spr"));
}


//------------------------------------------------------------
// wxGxDatasetFilter
//------------------------------------------------------------

wxGxDatasetFilter::wxGxDatasetFilter(wxGISEnumDatasetType nType)
{
    m_nType = nType;
}

wxGxDatasetFilter::~wxGxDatasetFilter(void)
{
}

bool wxGxDatasetFilter::CanChooseObject( IGxObject* pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != m_nType)
		return false;
    return true;
}

bool wxGxDatasetFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != m_nType)
		return false;
    return true;
}

wxString wxGxDatasetFilter::GetName(void)
{
    switch(m_nType)
    {
    case enumGISRasterDataset:
        return wxString(_("Raster (*.img, *.tif, etc.)"));
    case enumGISFeatureDataset:
        return wxString(_("Vector (*.shp, *.tab, etc.)"));
    }
    return wxEmptyString;
}

//------------------------------------------------------------
// wxGxFeatureFileFilter
//------------------------------------------------------------

wxGxFeatureFileFilter::wxGxFeatureFileFilter(wxGISEnumVectorDatasetType nSubType)
{
    m_nSubType = nSubType;
}

wxGxFeatureFileFilter::~wxGxFeatureFileFilter(void)
{
}

bool wxGxFeatureFileFilter::CanChooseObject( IGxObject* pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(GetSubType() == enumVecUnknown)
        return true;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxFeatureFileFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(GetSubType() == enumVecUnknown)
        return true;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

wxString wxGxFeatureFileFilter::GetName(void)
{
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
        return wxString(_("ESRI Shapefile (*.shp)"));
    case enumVecMapinfoTab:
        return wxString(_("MapInfo File (*.tab)"));
    case enumVecMapinfoMif:
        return wxString(_("MapInfo File (*.mid/*.mif)"));
    case enumVecKML:
 	    return wxString(_("KML file (*.kml)"));
    case enumVecKMZ:
 	    return wxString(_("KML file (*.kmz)"));
    case enumVecDXF:
	    return wxString(_("AutoCAD DXF file (*.dxf)"));
	//case emumVecPostGIS:
    default:
	    return wxString(_("Any feature classes (*.*)"));
    }
}

wxString wxGxFeatureFileFilter::GetExt(void)
{
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
	    return wxString(wxT("shp"));
    case enumVecMapinfoTab:
	    return wxString(wxT("tab"));
    case enumVecMapinfoMif:
        return wxString(wxT("mif"));
    case enumVecKML:
        return wxString(wxT("kml"));
    case enumVecKMZ:
        return wxString(wxT("kmz"));
    case enumVecDXF:
        return wxString(wxT("dxf"));
	//case emumVecPostGIS:
    default:
        return wxEmptyString;
    }
}

wxString wxGxFeatureFileFilter::GetDriver(void)
{
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
	    return wxString(wxT("ESRI Shapefile"));
    case enumVecMapinfoTab:
    case enumVecMapinfoMif:
	    return wxString(wxT("MapInfo File"));
    case enumVecKML:
    case enumVecKMZ:
	    return wxString(wxT("LIBKML"));
    case enumVecDXF:
	    return wxString(wxT("DXF"));
	//case emumVecPostGIS:
    default:
        return wxEmptyString;
    }
}

int wxGxFeatureFileFilter::GetSubType(void)
{
    return m_nSubType;
}

//------------------------------------------------------------
// wxGxFolderFilter
//------------------------------------------------------------

wxGxFolderFilter::wxGxFolderFilter(void)
{
}

wxGxFolderFilter::~wxGxFolderFilter(void)
{
}

bool wxGxFolderFilter::CanChooseObject( IGxObject* pObject )
{
	wxGxFolder* pGxFolder = dynamic_cast<wxGxFolder*>(pObject);
	if(!pGxFolder)
		return false;
    if(pObject->GetCategory() == wxString(_("Folder")))
        return true;
    if(pObject->GetCategory() == wxString(_("Folder connection")))
        return true;
    return false;
}

bool wxGxFolderFilter::CanDisplayObject( IGxObject* pObject )
{
	//IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	//if(pContainer)
	//	return true;
    if(pObject->GetCategory() == wxString(wxT("Root")))
        return true;
    if(pObject->GetCategory() == wxString(_("Folder connections")))
        return true;
	//wxGxFolder* pGxFolder = dynamic_cast<wxGxFolder*>(pObject);
	//if(!pGxFolder)
	//	return false;
    if(pObject->GetCategory() == wxString(_("Folder")))
        return true;
    if(pObject->GetCategory() == wxString(_("Folder connection")))
        return true;
    return false;
}

wxString wxGxFolderFilter::GetName(void)
{
	return wxString(_("Folder"));
}

//------------------------------------------------------------
// wxGxRasterFilter
//------------------------------------------------------------

wxGxRasterFilter::wxGxRasterFilter(wxGISEnumRasterDatasetType nSubType)
{
    m_nSubType = nSubType;
}

wxGxRasterFilter::~wxGxRasterFilter(void)
{
}

bool wxGxRasterFilter::CanChooseObject( IGxObject* pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxRasterFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != GetType())
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

wxString wxGxRasterFilter::GetName(void)
{
    switch(m_nSubType)
    {
    case enumRasterBmp:
	    return wxString(_("Windows Device Independent Bitmap (*.bmp)"));
    case enumRasterTiff:
	    return wxString(_("GeoTIFF (*.tif, *.tiff)"));
    case enumRasterJpeg:
	    return wxString(_("JPEG image (*.jpeg, *.jfif, *.jpg, *.jpe)"));
    case enumRasterImg:
	    return wxString(_("Erdas IMAGINE image (*.img)"));
    case enumRasterPng:
	    return wxString(_("Portable Network Graphics (*.png)"));
    case enumRasterGif:
	    return wxString(_("Graphics Interchange Format (*.gif)"));
    }
}

wxString wxGxRasterFilter::GetExt(void)
{
    switch(m_nSubType)
    {
    case enumRasterBmp:
	    return wxString(wxT("bmp"));
    case enumRasterTiff:
	    return wxString(wxT("tif"));
    case enumRasterJpeg:
	    return wxString(wxT("jpg"));
    case enumRasterImg:
	    return wxString(wxT("img"));
    case enumRasterPng:
	    return wxString(wxT("png"));
    case enumRasterGif:
	    return wxString(wxT("gif"));
    }
}

wxString wxGxRasterFilter::GetDriver(void)
{
    switch(m_nSubType)
    {
    case enumRasterBmp:
        return wxString(wxT("BMP"));
    case enumRasterTiff:
	    return wxString(wxT("GTiff"));
    case enumRasterJpeg:
	    return wxString(wxT("JPEG"));
    case enumRasterImg:
	    return wxString(wxT("HFA"));
    case enumRasterPng:
	    return wxString(wxT("PNG"));
    case enumRasterGif:
	    return wxString(wxT("GIF"));
    }
}

int wxGxRasterFilter::GetSubType(void)
{
    return m_nSubType;
}


//------------------------------------------------------------
// wxGxTextFilter
//------------------------------------------------------------

wxGxTextFilter::wxGxTextFilter(wxString soName, wxString soExt)
{
    m_soName = soName;
    m_soExt = soExt;
}

wxGxTextFilter::~wxGxTextFilter(void)
{
}

bool wxGxTextFilter::CanChooseObject( IGxObject* pObject )
{
	wxGxTextFile* poGxTextFile = dynamic_cast<wxGxTextFile*>(pObject);
	if(!poGxTextFile)
		return false;    
    if(EQUAL(CPLGetExtension(poGxTextFile->GetInternalName()), m_soExt.mb_str()))
		return true;
	return false;
}

bool wxGxTextFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	wxGxTextFile* poGxTextFile = dynamic_cast<wxGxTextFile*>(pObject);
	if(!poGxTextFile)
		return false;
    if(EQUAL(CPLGetExtension(poGxTextFile->GetInternalName()), m_soExt.mb_str()))
		return true;
	return false;
}

wxString wxGxTextFilter::GetName(void)
{
	return m_soName;
}

wxString wxGxTextFilter::GetExt(void)
{
	return m_soExt;
}

