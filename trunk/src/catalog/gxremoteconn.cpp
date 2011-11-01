/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnection class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/catalog/gxremoteconn.h"
#include "wxgis/core/crypt.h"
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxpostgisdataset.h"

//--------------------------------------------------------------
//class wxGxRemoteConnection
//--------------------------------------------------------------

wxGxRemoteConnection::wxGxRemoteConnection(CPLString soPath, wxString Name)
{
	m_eType = enumContRemoteConnection;
	m_bIsChildrenLoaded = false;
	m_sName = Name;
	m_sPath = soPath;
}

wxGxRemoteConnection::~wxGxRemoteConnection(void)
{
}

wxGISDatasetSPtr wxGxRemoteConnection::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
	if(m_pwxGISDataset == NULL)
	{	
		CPLErrorReset();
		wxXmlDocument doc(wxString(m_sPath,  wxConvUTF8));
		if(!doc.IsOk())
		{
			wxString sErr(_("Error open connection file!"));
			CPLString sFullErr(sErr.mb_str());
			CPLError( CE_Failure, CPLE_FileIO, sFullErr);
			return wxGISDatasetSPtr();
		}
		wxXmlNode* pRootNode = doc.GetRoot();
		if(pRootNode)
		{
			wxString sServer = pRootNode->GetAttribute(wxT("server"), wxEmptyString);
			wxString sPort = pRootNode->GetAttribute(wxT("port"), wxEmptyString);
			wxString sDatabase = pRootNode->GetAttribute(wxT("db"), wxEmptyString);
			wxString sUser = pRootNode->GetAttribute(wxT("user"), wxEmptyString);
			wxString sPass;
			Decrypt(pRootNode->GetAttribute(wxT("pass"), wxEmptyString), sPass);
			bool bIsBinaryCursor = wxString(pRootNode->GetAttribute(wxT("isbincursor"), wxT("no"))).CmpNoCase(wxString(wxT("yes"))) == 0;
			wxGISPostgresDataSourceSPtr pwxGISRemoteConn = boost::make_shared<wxGISPostgresDataSource>(sUser, sPass, sPort, sServer, sDatabase, bIsBinaryCursor);
			if(!pwxGISRemoteConn->Open())
			{
				const char* err = CPLGetLastErrorMsg();
				wxString sErr = wxString::Format(_("%s failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
				wxLogError(sErr);
				if(pTrackCancel)
					pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
				return wxGISDatasetSPtr();
			}
			m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pwxGISRemoteConn);
		}
	}
	return m_pwxGISDataset;
}

bool wxGxRemoteConnection::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    long nChildID = pChild->GetID();
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
    m_pCatalog->ObjectDeleted(nChildID);
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}

void wxGxRemoteConnection::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE( m_Children[i] );
	}
    m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxRemoteConnection::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

	if(m_pwxGISDataset == NULL)
		return;

    for(size_t i = 0; i < m_pwxGISDataset->GetSubsetsCount(); ++i)
    {
        wxGISDatasetSPtr pGISDataset = m_pwxGISDataset->GetSubset(i);
        wxGISEnumDatasetType eType = pGISDataset->GetType();
        IGxObject* pGxObject(NULL);
        switch(eType)
        {
        case enumGISFeatureDataset:
            {
                wxGxPostGISFeatureDataset* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDataset(pGISDataset->GetPath(), pGISDataset);
                pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
            }
            break;
        case enumGISTableDataset:
            {
                wxGxPostGISTableDataset* pGxPostGISTableDataset = new wxGxPostGISTableDataset(pGISDataset->GetPath(), pGISDataset);
                pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
            }
            break;
        case enumGISRasterDataset:
            break;
        default:
        case enumGISContainer:
            break;
        };

        if(pGxObject)
        {
		    bool ret_code = AddChild(pGxObject);
		    if(!ret_code)
			    wxDELETE(pGxObject);
        }
	}
	m_bIsChildrenLoaded = true;
}

wxString wxGxRemoteConnection::GetBaseName(void)
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxRemoteConnection::Delete(void)
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
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
}

bool wxGxRemoteConnection::Rename(wxString NewName)
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

bool wxGxRemoteConnection::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
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

bool wxGxRemoteConnection::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
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
