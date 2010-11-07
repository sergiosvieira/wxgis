/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
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
	wxGxFolder* pGxFolder = dynamic_cast<wxGxFolder*>(pLocation);
	{
		if(wxFileName::IsDirWritable(pGxFolder->GetPath()) || wxFileName::IsFileWritable(pGxFolder->GetPath()))
		{
			if(pGxFolder->SearchChild(pLocation->GetFullName() + wxFileName::GetPathSeparator() + sName) == NULL)
				return enumGISSaveObjectAccept;
			else
				return enumGISSaveObjectExists;
		}
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

wxGISEnumSaveObjectResults wxGxPrjFileFilter::CanSaveObject( IGxObject* pLocation, wxString sName )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pLocation);
	if(!pContainer)
		return enumGISSaveObjectDeny;
	wxGxFolder* pGxFolder = dynamic_cast<wxGxFolder*>(pLocation);
	{
		if(wxFileName::IsDirWritable(pGxFolder->GetPath()))
		{
			if(pGxFolder->SearchChild(pLocation->GetFullName() + wxFileName::GetPathSeparator() + sName) == NULL)
				return enumGISSaveObjectAccept;
			else
				return enumGISSaveObjectExists;
		}
	}
	
	wxGxSpatialReferencesFolder* pGxSpatialReferencesFolder = dynamic_cast<wxGxSpatialReferencesFolder*>(pLocation);
	{
		if(wxFileName::IsFileWritable(pGxSpatialReferencesFolder->GetPath()))
		{
			if(pGxSpatialReferencesFolder->SearchChild(pLocation->GetFullName() + wxFileName::GetPathSeparator() + sName) == NULL)
				return enumGISSaveObjectAccept;
			else
				return enumGISSaveObjectExists;
		}
	}

	return enumGISSaveObjectDeny;
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
// wxGxShapeFileFilter
//------------------------------------------------------------

wxGxShapeFileFilter::wxGxShapeFileFilter(void)
{
}

wxGxShapeFileFilter::~wxGxShapeFileFilter(void)
{
}

bool wxGxShapeFileFilter::CanChooseObject( IGxObject* pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != enumGISFeatureDataset)
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

bool wxGxShapeFileFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != enumGISFeatureDataset)
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

wxString wxGxShapeFileFilter::GetName(void)
{
	return wxString(_("ESRI Shapefile (*.shp)"));
}

wxString wxGxShapeFileFilter::GetExt(void)
{
	return wxString(wxT("shp"));
}

wxString wxGxShapeFileFilter::GetDriver(void)
{
	return wxString(wxT("ESRI Shapefile"));
}

int wxGxShapeFileFilter::GetSubType(void)
{
    return enumVecESRIShapefile;
}

//------------------------------------------------------------
// wxGxMapInfoFilter
//------------------------------------------------------------

wxGxMapInfoFilter::wxGxMapInfoFilter(bool bIsTab)
{
    m_bIsTab = bIsTab;
}

wxGxMapInfoFilter::~wxGxMapInfoFilter(void)
{
}

wxString wxGxMapInfoFilter::GetName(void)
{
    if(m_bIsTab)
        return wxString(_("MapInfo File (*.tab)"));
    else
        return wxString(_("MapInfo File (*.mid/*.mif)"));
}

wxString wxGxMapInfoFilter::GetExt(void)
{
    if(m_bIsTab)
	    return wxString(wxT("tab"));
    else
        return wxString(wxT("mif"));
}

wxString wxGxMapInfoFilter::GetDriver(void)
{
	return wxString(wxT("MapInfo File"));
}

int wxGxMapInfoFilter::GetSubType(void)
{
    return enumVecMapinfoTab;
}

//------------------------------------------------------------
// wxGxKMLFilter
//------------------------------------------------------------

wxGxKMLFilter::wxGxKMLFilter(void)
{
}

wxGxKMLFilter::~wxGxKMLFilter(void)
{
}

wxString wxGxKMLFilter::GetName(void)
{
	return wxString(_("KML file (*.kml)"));
}

wxString wxGxKMLFilter::GetExt(void)
{
	return wxString(wxT("kml"));
}

wxString wxGxKMLFilter::GetDriver(void)
{
	return wxString(wxT("KML"));
}

int wxGxKMLFilter::GetSubType(void)
{
    return enumVecKML;
}

//------------------------------------------------------------
// wxGxDXFFilter
//------------------------------------------------------------

wxGxDXFFilter::wxGxDXFFilter(void)
{
}

wxGxDXFFilter::~wxGxDXFFilter(void)
{
}

wxString wxGxDXFFilter::GetName(void)
{
	return wxString(_("AutoCAD DXF file (*.dxf)"));
}

wxString wxGxDXFFilter::GetExt(void)
{
	return wxString(wxT("dxf"));
}

wxString wxGxDXFFilter::GetDriver(void)
{
	return wxString(wxT("DXF"));
}

int wxGxDXFFilter::GetSubType(void)
{
    return enumVecDXF;
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
    if(pObject->GetCategory() == wxString(_("Folder Connection")))
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

wxGxRasterFilter::wxGxRasterFilter(void)
{
}

wxGxRasterFilter::~wxGxRasterFilter(void)
{
}

bool wxGxRasterFilter::CanChooseObject( IGxObject* pObject )
{
	IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pObject);
	if(!pGxDataset)
		return false;
    if(pGxDataset->GetType() != enumGISRasterDataset)
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
    if(pGxDataset->GetType() != enumGISRasterDataset)
		return false;
    if(pGxDataset->GetSubType() != GetSubType())
		return false;
    return true;
}

//------------------------------------------------------------
// wxGxTiffFilter
//------------------------------------------------------------

wxGxTiffFilter::wxGxTiffFilter(void)
{
}

wxGxTiffFilter::~wxGxTiffFilter(void)
{
}

wxString wxGxTiffFilter::GetName(void)
{
	return wxString(_("GeoTIFF (*.tif, *.tiff)"));
}

wxString wxGxTiffFilter::GetExt(void)
{
	return wxString(wxT("tif"));
}

wxString wxGxTiffFilter::GetDriver(void)
{
	return wxString(wxT("GTiff"));
}

int wxGxTiffFilter::GetSubType(void)
{
    return enumRasterTiff;
}

//------------------------------------------------------------
// wxGxImgFilter
//------------------------------------------------------------

wxGxImgFilter::wxGxImgFilter(void)
{
}

wxGxImgFilter::~wxGxImgFilter(void)
{
}

wxString wxGxImgFilter::GetName(void)
{
	return wxString(_("Erdas IMAGINE image (*.img)"));
}

wxString wxGxImgFilter::GetExt(void)
{
	return wxString(wxT("img"));
}

wxString wxGxImgFilter::GetDriver(void)
{
	return wxString(wxT("HFA"));
}

int wxGxImgFilter::GetSubType(void)
{
    return enumRasterImg;
}

//------------------------------------------------------------
// wxGxBmpFilter
//------------------------------------------------------------

wxGxBmpFilter::wxGxBmpFilter(void)
{
}

wxGxBmpFilter::~wxGxBmpFilter(void)
{
}

wxString wxGxBmpFilter::GetName(void)
{
	return wxString(_("Windows Device Independent Bitmap (*.bmp)"));
}

wxString wxGxBmpFilter::GetExt(void)
{
	return wxString(wxT("bmp"));
}

wxString wxGxBmpFilter::GetDriver(void)
{
	return wxString(wxT("BMP"));
}

int wxGxBmpFilter::GetSubType(void)
{
    return enumRasterBmp;
}

//------------------------------------------------------------
// wxGxJpegFilter
//------------------------------------------------------------

wxGxJpegFilter::wxGxJpegFilter(void)
{
}

wxGxJpegFilter::~wxGxJpegFilter(void)
{
}

wxString wxGxJpegFilter::GetName(void)
{
	return wxString(_("JPEG image (*.jpeg, *.jfif, *.jpg, *.jpe)"));
}

wxString wxGxJpegFilter::GetExt(void)
{
	return wxString(wxT("jpg"));
}

wxString wxGxJpegFilter::GetDriver(void)
{
	return wxString(wxT("JPEG"));
}

int wxGxJpegFilter::GetSubType(void)
{
    return enumRasterJpeg;
}

//------------------------------------------------------------
// wxGxPngFilter
//------------------------------------------------------------

wxGxPngFilter::wxGxPngFilter(void)
{
}

wxGxPngFilter::~wxGxPngFilter(void)
{
}

wxString wxGxPngFilter::GetName(void)
{
	return wxString(_("Portable Network Graphics (*.png)"));
}

wxString wxGxPngFilter::GetExt(void)
{
	return wxString(wxT("png"));
}

wxString wxGxPngFilter::GetDriver(void)
{
	return wxString(wxT("PNG"));
}

int wxGxPngFilter::GetSubType(void)
{
    return enumRasterPng;
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
    if(poGxTextFile->GetPath().Lower().Find(m_soExt) != wxNOT_FOUND)
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
    if(poGxTextFile->GetPath().Lower().Find(m_soExt) != wxNOT_FOUND)
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

