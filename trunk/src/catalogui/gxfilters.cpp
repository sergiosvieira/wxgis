/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDialog filters of GxObjects to show.
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

#include "wxgis/catalogui/gxfilters.h"
#include "wxgis/catalog/gxfile.h"
#include "wxgis/catalog/gxspatreffolder.h"
#include "wxgis/catalog/gxdataset.h"

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
	return wxString(_("Coordinate Systems (*.prj, *.srml)"));
}

//------------------------------------------------------------
// wxGxRasterDatasetFilter
//------------------------------------------------------------

wxGxRasterDatasetFilter::wxGxRasterDatasetFilter(void)
{
}

wxGxRasterDatasetFilter::~wxGxRasterDatasetFilter(void)
{
}

bool wxGxRasterDatasetFilter::CanChooseObject( IGxObject* pObject )
{
	wxGxRasterDataset* pGxRasterDataset = dynamic_cast<wxGxRasterDataset*>(pObject);
	if(pGxRasterDataset)
		return true;
	else
		return false;
}

bool wxGxRasterDatasetFilter::CanDisplayObject( IGxObject* pObject )
{
	IGxObjectContainer* pContainer = dynamic_cast<IGxObjectContainer*>(pObject);
	if(pContainer)
		return true;
	wxGxRasterDataset* pGxRasterDataset = dynamic_cast<wxGxRasterDataset*>(pObject);
	if(pGxRasterDataset)
		return true;
	else
		return false;
}

wxString wxGxRasterDatasetFilter::GetName(void)
{
	return wxString(_("Rasters (*.img, *.tif, etc.)"));
}
