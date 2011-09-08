/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
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
#include "wxgis/catalog/gxfile.h"
#include "wxgis/datasource/sysop.h"
#include "cpl_vsi.h"


//--------------------------------------------------------------
//class wxGxFile
//--------------------------------------------------------------

wxGxFile::wxGxFile(CPLString Path, wxString Name)
{
	m_sName = Name;
	m_sPath = Path;
}

wxGxFile::~wxGxFile(void)
{
}

wxString wxGxFile::GetBaseName(void)
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxFile::Delete(void)
{
	wxCriticalSectionLocker locker(m_DestructCritSect);
    if(DeleteFile(m_sPath))
	{
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Delete failed! GDAL error: %s, file '%s'"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
}

bool wxGxFile::Rename(wxString NewName)
{
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(ClearExt(NewName));

	wxString sNewPath = PathName.GetFullPath();
    CPLString szNewPath = sNewPath.mb_str(wxConvUTF8);
    if(RenameFile(m_sPath, szNewPath))
	{
        m_sPath = szNewPath;
		m_sName = NewName;
		m_pCatalog->ObjectChanged(GetID());
		return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
	return false;
}

bool wxGxFile::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString(_("Copy file ")) + m_sName, -1, enumGISMessageInfo);

	CPLString szFileName = CPLGetBasename(m_sPath);
	CPLString szNewDestFileName = GetUniqPath(m_sPath, szDestPath, szFileName);
    if(!CopyFile(szNewDestFileName, m_sPath, pTrackCancel))
        return false;
    
    m_sPath = szNewDestFileName;
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

bool wxGxFile::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString(_("Move file ")) + m_sName, -1, enumGISMessageInfo);

	CPLString szFileName = CPLGetBasename(m_sPath);
	CPLString szNewDestFileName = GetUniqPath(m_sPath, szDestPath, szFileName);
    if(!MoveFile(szNewDestFileName, m_sPath, pTrackCancel))
        return false;
    
    m_sPath = szNewDestFileName;
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

//--------------------------------------------------------------
//class wxGxPrjFile
//--------------------------------------------------------------

wxGxPrjFile::wxGxPrjFile(CPLString Path, wxString Name, wxGISEnumPrjFileType nType) : wxGxFile(Path, Name)
{
    m_Type = nType;
	m_pOGRSpatialReference = boost::make_shared<OGRSpatialReference>();
}

wxGxPrjFile::~wxGxPrjFile(void)
{
}

OGRSpatialReferenceSPtr wxGxPrjFile::GetSpatialReference(void)
{
	OGRErr err = OGRERR_NONE;
	if(m_pOGRSpatialReference->Validate() != OGRERR_NONE)
	{
		char **papszLines = CSLLoad( m_sPath );

		switch(m_Type)
		{
		case enumESRIPrjFile:
			err = m_pOGRSpatialReference->importFromESRI(papszLines);
			break;
		case enumSPRfile:
            {
                char *pszWKT, *pszWKT2;
                pszWKT = CPLStrdup(papszLines[0]);
                for(int i = 1; papszLines[i] != NULL; ++i )
                {
                    int npapszLinesSize = CPLStrnlen(papszLines[i], 1000);
                    int npszWKTSize = CPLStrnlen(pszWKT, 8000);
                    int nDestSize = npszWKTSize + npapszLinesSize + 1;
                    pszWKT = (char *)CPLRealloc(pszWKT, npszWKTSize + npapszLinesSize + 1 );
                    CPLStrlcat( pszWKT, papszLines[i], nDestSize );
                }
                pszWKT2 = pszWKT;
                err = m_pOGRSpatialReference->importFromWkt( &pszWKT2 );//.importFromWkt(papszLines);
                CPLFree( pszWKT );
            }
			break;
		default:
			break;
		}
        CSLDestroy( papszLines );
	}
    if(err != OGRERR_NONE)
    	return OGRSpatialReferenceSPtr();

    //err = m_OGRSpatialReference.importFromProj4("+proj=bonne +a=6371000 +es=0 +lon_0=0 +lat_1=60 +units=m +no_defs");
    //0x04e3c368 "+proj=bonne +ellps=sphere +lon_0=0 +lat_1=60 +units=m +no_defs "
    //0x04e3c368 "+proj=aitoff +lon_0=0 +lat_1=60 +x_0=0 +y_0=0 +a=6371000 +b=6371000 +units=m +no_defs "
	//if(m_OGRSpatialReference.GetEPSGGeogCS() == -1)
	//	m_OGRSpatialReference.SetWellKnownGeogCS("sphere");//WGS84

 //   err = m_OGRSpatialReference.Fixup();
	//err = m_OGRSpatialReference.Validate();
    //m_OGRSpatialReference.set
    //+over
	if(err == OGRERR_NONE)
	{
		//char *pszProj4Defn = NULL;
		//m_OGRSpatialReference.exportToProj4( &pszProj4Defn );
  //      CPLFree( pszProj4Defn );
		return m_pOGRSpatialReference;
	}
	else
	{
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGxPrjFile: GDAL error: %s"), wxString(err, wxConvUTF8).c_str());
		wxLogError(sErr);
	}
	return OGRSpatialReferenceSPtr();
}

//--------------------------------------------------------------
//class wxGxTextFile
//--------------------------------------------------------------

wxGxTextFile::wxGxTextFile(CPLString Path, wxString Name) : wxGxFile(Path, Name)
{
}

wxGxTextFile::~wxGxTextFile(void)
{
}


