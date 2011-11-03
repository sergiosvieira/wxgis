/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxDataset classes.
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

#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/datasource/table.h"
#include "wxgis/datasource/sysop.h"

//--------------------------------------------------------------
//class wxGxTableDataset
//--------------------------------------------------------------

wxGxTableDataset::wxGxTableDataset(CPLString Path, wxString Name, wxGISEnumTableDatasetType nType)
{
	m_type = nType;

	m_sName = Name;
	m_sPath = Path;
}

wxGxTableDataset::~wxGxTableDataset(void)
{
}

bool wxGxTableDataset::Delete(void)
{
	wxCriticalSectionLocker locker(m_DestructCritSect);
	wxGISTableSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISTable>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISDataset);
    }
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    if(bRet)
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

wxString wxGxTableDataset::GetBaseName(void)
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxTableDataset::Rename(wxString NewName)
{
    wxGISTableSPtr pDSet = boost::dynamic_pointer_cast<wxGISTable>(GetDataset());
    if(!pDSet)
        return false;
	if(pDSet->IsOpened())
		pDSet->Close();
	if(!pDSet->Open(0, true))
        return false;

	if(pDSet->Rename(NewName))
	{
	 //   wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	 //   PathName.SetName(NewName);
	 //   wxString sNewPath = PathName.GetFullPath();
		//m_sPath = CPLString(sNewPath.mb_str(wxConvUTF8));
		m_sPath = pDSet->GetPath();
		m_sName = NewName;		
		return true;
	}
	else
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
}

wxGISDatasetSPtr wxGxTableDataset::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
	if(m_pwxGISDataset == NULL)
	{		
        wxGISTableSPtr pwxGISTable = boost::make_shared<wxGISTable>(m_sPath, m_type);

        if(!pwxGISTable->Open(0, 0, bCache, pTrackCancel))
        {
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("%s failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
			return wxGISDatasetSPtr();
        }
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pwxGISTable);
	}
	return m_pwxGISDataset;
}

bool wxGxTableDataset::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s table dataset %s"), _("Copy"), m_sName.c_str()), -1, enumGISMessageInfo);

	wxGISTableSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISTable>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISDataset);
    }
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s table dataset %s failed!"), _("Copy"), m_sName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }
	
    //m_sPath = pDSet->GetPath();
    //m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

bool wxGxTableDataset::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s table dataset %s"), _("Move"), m_sName.c_str()), -1, enumGISMessageInfo);

	wxGISTableSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISTable>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISDataset);
    }
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s table dataset %s failed!"), _("Move"), m_sName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Move"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }

    m_sPath = pDSet->GetPath();
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

//--------------------------------------------------------------
//class wxGxFeatureDataset
//--------------------------------------------------------------

wxGxFeatureDataset::wxGxFeatureDataset(CPLString Path, wxString Name, wxGISEnumVectorDatasetType nType)
{
	m_type = nType;

	m_sName = Name;
	m_sPath = Path;
}

wxGxFeatureDataset::~wxGxFeatureDataset(void)
{
}

wxString wxGxFeatureDataset::GetCategory(void)
{
	return wxString(_("Feature class"));
}

wxString wxGxFeatureDataset::GetBaseName(void)
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxFeatureDataset::Delete(void)
{
	wxCriticalSectionLocker locker(m_DestructCritSect);
    wxGISFeatureDatasetSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISFeatureDataset>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(m_pwxGISDataset);
    }
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    if(bRet)
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

bool wxGxFeatureDataset::Rename(wxString NewName)
{
    wxGISFeatureDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(GetDataset());
    if(!pDSet)
        return false;
	if(pDSet->IsOpened())
		pDSet->Close();
	if(!pDSet->Open(0, 0, false))
        return false;

	if(pDSet->Rename(NewName))
	{
	 //   wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	 //   PathName.SetName(NewName);
	 //   wxString sNewPath = PathName.GetFullPath();
		//m_sPath = CPLString(sNewPath.mb_str(wxConvUTF8));
		m_sPath = pDSet->GetPath();
		m_sName = NewName;
		//m_pCatalog->ObjectChanged(GetID());
		return true;
	}
	else
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
}

wxGISDatasetSPtr wxGxFeatureDataset::GetDataset(bool bCache, ITrackCancel* pTrackCancel)
{
	if(m_pwxGISDataset == NULL)
	{		
        wxGISFeatureDatasetSPtr pwxGISFeatureDataset = boost::make_shared<wxGISFeatureDataset>(m_sPath, m_type);

        if(!pwxGISFeatureDataset->Open(0, 0, bCache, pTrackCancel))
        {
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("%s failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
			return wxGISDatasetSPtr();
        }

       // //TODO: Spatial Index
       // if(!pwxGISFeatureDataset->HasSpatialIndex())
       // {
       // 	bool bAskSpaInd = true;
       //     wxString name, ext;
       //     wxFileName::SplitPath(m_sPath, NULL, NULL, &name, &ext);
       //     wxString sFileName = name + wxT(".") + ext;
       //     IGISConfig*  pConfig = m_pCatalog->GetConfig();
       //     bool bCreateSpaInd = true;
       //     if(pConfig)
       //     {
       //         wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/vector")));
       //         if(pNode)
       //             bAskSpaInd = wxAtoi(pNode->GetAttribute(wxT("create_tree"), wxT("1")));
       //         else
       //         {
       //             pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/vector")), true);
       //             pNode->AddAttribute(wxT("create_tree"), wxT("1"));
       //         }
       //         if(bAskSpaInd)
       //         {
       //             //show ask dialog
       //             wxGISMessageDlg dlg(NULL, wxID_ANY, wxString::Format(_("Create spatial index for %s"), sFileName.c_str()), wxString(_("This vector datasource does not have spatial index. Spatial index allows rapid display at different resolutions.")), wxString(_("Spatial index building may take few moments.\nWould you like to create spatial index?")), wxDefaultPosition, wxSize( 400,160 ));
       //             if(dlg.ShowModal() == wxID_NO)
       //             {
       //                 bCreateSpaInd = false;
       //             }
       //             if(!dlg.GetShowInFuture())
       //             {
       //                 pNode->DeleteAttribute(wxT("create_tree"));
       //                 pNode->AddAttribute(wxT("create_tree"), wxT("0"));
       //             }
       //         }
       //     }

	      //  if(bCreateSpaInd)
	      //  {
       //         wxWindowDisabler disableAll;
       //         wxBusyInfo wait(_("Please wait, working..."));

       //         OGRErr err = pwxGISFeatureDataset->CreateSpatialIndex();
			    //if(err != OGRERR_NONE)
			    //{
       //             const char* pszerr = CPLGetLastErrorMsg();
       //             wxLogError(_("Build Spatial Index failed! OGR error: %s"), wgMB2WX(pszerr));
       //             int nRes = wxMessageBox(_("Build Spatial Index! Continue?"), _("Question"), wxICON_QUESTION | wxYES_NO);
       //             if(nRes == wxNO)
       //                 return NULL;
			    //}
	      //  }
       // }

        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pwxGISFeatureDataset);
        m_pwxGISDataset->SetSubType(m_type);
	}
	return m_pwxGISDataset;
}

bool wxGxFeatureDataset::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s feature dataset %s"), _("Copy"), m_sName.c_str()), -1, enumGISMessageInfo);

    wxGISFeatureDatasetSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISFeatureDataset>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(m_pwxGISDataset);
    }
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s feature dataset %s failed!"), _("Copy"), m_sName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }

    m_sPath = pDSet->GetPath();
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

bool wxGxFeatureDataset::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s feature dataset %s"), _("Move"), m_sName.c_str()), -1, enumGISMessageInfo);

    wxGISFeatureDatasetSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISFeatureDataset>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(m_pwxGISDataset);
    }
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s feature dataset %s failed!"), _("Move"), m_sName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Move"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }

    m_sPath = pDSet->GetPath();
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

//--------------------------------------------------------------
//class wxGxRasterDataset
//--------------------------------------------------------------

wxGxRasterDataset::wxGxRasterDataset(CPLString Path, wxString Name, wxGISEnumRasterDatasetType nType)
{
	m_type = nType;

	m_sName = Name;
	m_sPath = Path;
}

wxGxRasterDataset::~wxGxRasterDataset(void)
{
}

bool wxGxRasterDataset::Delete(void)
{
	wxCriticalSectionLocker locker(m_DestructCritSect);
    wxGISRasterDatasetSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISRasterDataset>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(m_pwxGISDataset);
    }
    
    if(!pDSet)
        return false;

    bool bRet = pDSet->Delete();
    if(bRet)
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
wxString wxGxRasterDataset::GetBaseName(void)
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxRasterDataset::Rename(wxString NewName)
{
    wxGISRasterDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(GetDataset());
    if(!pDSet)
        return false;
	if(pDSet->IsOpened())
		pDSet->Close();
	if(!pDSet->Open(true))
        return false;

	if(pDSet->Rename(NewName))
	{
	 //   wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	 //   PathName.SetName(NewName);
	 //   wxString sNewPath = PathName.GetFullPath();
		//m_sPath = CPLString(sNewPath.mb_str(wxConvUTF8));
		m_sPath = pDSet->GetPath();
		m_sName = NewName;
		//m_pCatalog->ObjectChanged(GetID());
		return true;
	}
	else
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
}

wxString wxGxRasterDataset::GetCategory(void)
{
	switch(m_type)
	{
	case enumRasterUnknown:
	default:
		return wxString(_("Raster"));
	}
}

wxGISDatasetSPtr wxGxRasterDataset::GetDataset(bool bCached, ITrackCancel* pTrackCancel)
{
	if(m_pwxGISDataset == NULL)
	{	
        wxGISRasterDatasetSPtr pwxGISRasterDataset = boost::make_shared<wxGISRasterDataset>(m_sPath, m_type);

        if(!pwxGISRasterDataset->Open(true))
        {
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("%s failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);

			return wxGISDatasetSPtr();
        }
        
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pwxGISRasterDataset);
	}
	return m_pwxGISDataset;
}

bool wxGxRasterDataset::Copy(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s raster dataset %s"), _("Copy"), m_sName.c_str()), -1, enumGISMessageInfo);

    wxGISRasterDatasetSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISRasterDataset>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(m_pwxGISDataset);
    }
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s raster dataset %s failed!"), _("Copy"), m_sName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }

    m_sPath = pDSet->GetPath();
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}

bool wxGxRasterDataset::Move(CPLString szDestPath, ITrackCancel* pTrackCancel)
{
    if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("%s raster dataset %s"), _("Move"), m_sName.c_str()), -1, enumGISMessageInfo);

    wxGISRasterDatasetSPtr pDSet;
 	if(m_pwxGISDataset == NULL)
    {
        pDSet = boost::make_shared<wxGISRasterDataset>(m_sPath, m_type);
        m_pwxGISDataset = boost::static_pointer_cast<wxGISDataset>(pDSet);
    }
    else
    {
        pDSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(m_pwxGISDataset);
    }
    
    if(!pDSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("%s raster dataset %s failed!"), _("Move"), m_sName.c_str()), -1, enumGISMessageErr);
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("%s failed! GDAL error: %s, file '%s'"), _("Move"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }

    m_sPath = pDSet->GetPath();
    m_sName = wxString(CPLGetFilename(m_sPath), wxConvUTF8);

    return true;
}