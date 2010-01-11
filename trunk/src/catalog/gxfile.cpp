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

		switch(m_Type)
		{
		case enumESRIPrjFile:
			{
			char **papszLines = CSLLoad( wgWX2MB(m_sPath) );
			err = m_OGRSpatialReference.importFromESRI(papszLines);
			CSLDestroy( papszLines );
			}
			break;
		case enumSRMLfile:
			{
   //             wxString Data;
   //             char* pData(NULL);
   //             OGRSpatialReference SpaRef;
   //             OGRErr err = SpaRef.importFromEPSG(3752);//28407
   //             if(err == OGRERR_NONE)
   //             {
   //                 SpaRef.exportToWkt(&pData);
   //                 Data = wgMB2WX(pData);  
   //                 wxLogDebug(Data);
   //             }
			//err = m_OGRSpatialReference.importFromWkt(&pData);
   //         CPLFree(pData);


			FILE *fp;
			fp = VSIFOpenL( wgWX2MB(m_sPath), "rb" );
			if( fp == NULL )
				return NULL;

			VSIFSeekL( fp, 0, SEEK_END );
			int nFileLen = VSIFTellL( fp );
			VSIRewindL( fp );

			char *pszRawData = (char *) CPLMalloc(nFileLen/* + 1*/);
			if( (int)VSIFReadL( pszRawData, 1, nFileLen, fp ) != nFileLen )
			{
				CPLFree( pszRawData );
				pszRawData = NULL;

				wxLogError(wxString::Format(_("Read of file %s failed."), m_sPath.c_str() ));
				return NULL;
			}

			//pszRawData[nFileLen] = '\0';
			VSIFCloseL( fp );
			fp = NULL;	
			wxString sStr(pszRawData, *wxConvCurrent, nFileLen);
			err = m_OGRSpatialReference.importFromXML(wgWX2MB(sStr));
			CPLFree(pszRawData);
			}
			break;
		default:
			break;
		}
	}
	if(err == OGRERR_NONE)
		return &m_OGRSpatialReference;
	return NULL;
}
