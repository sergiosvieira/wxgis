/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
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
#include "wxgis/catalog/gxfile.h"
#include "../../art/sr_16(2).xpm"
#include "../../art/sr_48.xpm"
#include "cpl_vsi.h"

//--------------------------------------------------------------
//class wxGxFile
//--------------------------------------------------------------

wxGxFile::wxGxFile(wxString Path, wxString Name)
{
	m_sName = Name;
	m_sPath = Path;
}

wxGxFile::~wxGxFile(void)
{
}


//--------------------------------------------------------------
//class wxGxPrjFile
//--------------------------------------------------------------

wxGxPrjFile::wxGxPrjFile(wxString Path, wxString Name, wxGISEnumPrjFileType Type) : wxGxFile(Path, Name)
{
    m_Type = Type;
}

wxGxPrjFile::~wxGxPrjFile(void)
{
}

wxIcon wxGxPrjFile::GetLargeImage(void)
{
	return wxIcon(sr_48_xpm);
}

wxIcon wxGxPrjFile::GetSmallImage(void)
{
	return wxIcon(sr_16_2_xpm);
}

bool wxGxPrjFile::Delete(void)
{
	//if(wxFileName::Rmdir(m_sPath))
	//{
	//	IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
	//	if(pGxObjectContainer == NULL)
	//		return false;
	//	return pGxObjectContainer->DeleteChild(this);		
	//}
	//else
		return false;	
}

bool wxGxPrjFile::Rename(wxString NewName)
{
	//rename ?
	m_sName = NewName; 
	m_pCatalog->ObjectChanged(this);
	return true;
}

void wxGxPrjFile::EditProperties(wxWindow *parent)
{
}

OGRSpatialReference* wxGxPrjFile::GetSpatialReference(void)
{
	OGRErr err = OGRERR_NONE;
	if(m_OGRSpatialReference.Validate() != OGRERR_NONE)
	{
		char **papszLines = CSLLoad( wgWX2MB(m_sPath) );

		switch(m_Type)
		{
		case enumESRIPrjFile:
			err = m_OGRSpatialReference.importFromESRI(papszLines);
			break;
		case enumSRMLfile:
			err = m_OGRSpatialReference.importFromWkt(papszLines);
			break;
		default:
			break;
		}
		//CSLDestroy( papszLines );
	}
    //err = m_OGRSpatialReference.importFromProj4("+proj=bonne +a=6371000 +es=0 +lon_0=0 +lat_1=60 +units=m +no_defs");
    //0x04e3c368 "+proj=bonne +ellps=sphere +lon_0=0 +lat_1=60 +units=m +no_defs "
    //0x04e3c368 "+proj=aitoff +lon_0=0 +lat_1=60 +x_0=0 +y_0=0 +a=6371000 +b=6371000 +units=m +no_defs "
	//if(m_OGRSpatialReference.GetEPSGGeogCS() == -1)
	//	m_OGRSpatialReference.SetWellKnownGeogCS("sphere");//WGS84

    //err = m_OGRSpatialReference.Fixup();
	//err = m_OGRSpatialReference.Validate();
    //m_OGRSpatialReference.set
    //+over
	if(err == OGRERR_NONE)
	{
		char *pszProj4Defn = NULL;
		m_OGRSpatialReference.exportToProj4( &pszProj4Defn );
        CPLFree( pszProj4Defn );
		return &m_OGRSpatialReference;
	}
	else
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGxPrjFile: OGR error #%d: %s"), err, wgMB2WX(err));
		wxLogError(sErr);
	}
	return NULL;
}
