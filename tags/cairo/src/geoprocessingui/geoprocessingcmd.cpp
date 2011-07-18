/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Geoprocessing Main Commands class.
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

#include "wxgis/geoprocessingui/geoprocessingcmd.h"
#include "wxgis/geoprocessingui/gptoolboxview.h"
#include "wxgis/datasource/sysop.h"

#include "../../art/export.xpm"
#include "../../art/toolview.xpm"

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/gxcontdialog.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/framework/progressdlg.h"

#include "wx/busyinfo.h"

//	0	Export
//  1   Show/hide toolbox pane
//  2   ?

IMPLEMENT_DYNAMIC_CLASS(wxGISGeoprocessingCmd, wxObject)

wxGISGeoprocessingCmd::wxGISGeoprocessingCmd(void) : m_pToolboxView(NULL)
{
}

wxGISGeoprocessingCmd::~wxGISGeoprocessingCmd(void)
{
}

wxIcon wxGISGeoprocessingCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			if(!m_IconGPMenu.IsOk())
				m_IconGPMenu = wxIcon(export_xpm);
			return m_IconGPMenu;
		case 1:
			if(!m_IconToolview.IsOk())
				m_IconToolview = wxIcon(toolview_xpm);
			return m_IconToolview;
		default:
			return wxNullIcon;
	}
}

wxString wxGISGeoprocessingCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Export"));
		case 1:
			return wxString(_("Show/Hide &Toolbox pane"));
		default:
		return wxEmptyString;
	}
}

wxString wxGISGeoprocessingCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Geoprocessing"));
		case 1:
			return wxString(_("View"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISGeoprocessingCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 1:
            return m_pApp->IsApplicationWindowShown(m_pToolboxView);
		case 0:
		default:
	        return false;
	}
	return false;
}

bool wxGISGeoprocessingCmd::GetEnabled(void)
{
	if(!m_pToolboxView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); ++i)
			{
				wxGxToolboxView* pwxGxToolboxView = dynamic_cast<wxGxToolboxView*>(pWinArr->at(i));
				if(pwxGxToolboxView)
				{
					m_pToolboxView = pwxGxToolboxView;
					break;
				}
			}
		}
	}

    switch(m_subtype)
	{
		case 0://Export
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pCatalog->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); ++i)
                {
                    IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxDataset* pDSet = dynamic_cast<IGxDataset*>(pGxObject.get());
                    if(pDSet)
                        return true;
                }
			}
			return false;
        }
		case 1://Show/Hide Toolbox pane
            return m_pToolboxView != NULL;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISGeoprocessingCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 1://Show/hide toolbox pane
            return enumGISCommandCheck;
		case 0://Export
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISGeoprocessingCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Export item to another format"));
		case 1:
			return wxString(_("Show/Hide Toolbox pane"));
		default:
			return wxEmptyString;
	}
}

void wxGISGeoprocessingCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			{
                //1. get the GxObject's list
                std::vector<IGxDataset*> DatasetArray;
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
                    wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pCatalog->GetSelection();
                    for(size_t i = 0; i < pSel->GetCount(); ++i)
                    {
                        IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                        IGxDataset* pGxDSet = dynamic_cast<IGxDataset*>(pGxObject.get());
                        IGxObjectContainer* pObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxDSet);
                        if(pObjectContainer)
                        {
							wxBusyCursor wait;
                            if(!pObjectContainer->HasChildren())
                                continue;
                            GxObjectArray* pArr = pObjectContainer->GetChildren();
                            if(pArr != NULL)
                            {
                                for(size_t i = 0; i < pArr->size(); ++i)
                                {
                                    pGxDSet = dynamic_cast<IGxDataset*>(pArr->operator[](i));
                                    if(pGxDSet)
                                        DatasetArray.push_back(pGxDSet);
                                }
                            }
                        }
                        else if(pGxDSet)
                            DatasetArray.push_back(pGxDSet);
                    }
                }

                wxWindow* pWnd = dynamic_cast<wxWindow*>(m_pApp);
                //2. if single GxObject progress in status bar
                if(DatasetArray.size() == 1)
                {
                    IGxObject* pGxSrcObj = dynamic_cast<IGxObject*>(DatasetArray[0]);
                    wxString sName = pGxSrcObj->GetName();
                    sName = ClearExt(sName);

                    IGxObject* pGxParentObj = pGxSrcObj->GetParent();
                    wxString sStartLoc;
                    if(pGxParentObj)
                        sStartLoc = pGxParentObj->GetFullName();

                    IGxCatalog *pExtCat = NULL;
                    if(pGxApp)
                        pExtCat = pGxApp->GetCatalog();
                    wxGxObjectDialog dlg(pWnd, pExtCat, wxID_ANY, _("Select output"));
                    IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                    dlg.SetName(sName);
				    dlg.SetAllowMultiSelect(false);
				    dlg.SetAllFilters(false);

                    wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[0]->GetSubType();
                    if(nSubType != enumVecKML)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKML), true);
                    if(nSubType != enumVecKMZ)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKMZ), true);
                    if(nSubType != enumVecESRIShapefile)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile), false);
                    if(nSubType != enumVecMapinfoTab)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab), false);
                    if(nSubType != enumVecMapinfoMif)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif), false);
                    if(nSubType != enumVecDXF)
                        dlg.AddFilter(new wxGxFeatureFileFilter(enumVecDXF), false);

				    dlg.SetButtonCaption(_("Export"));
				    //dlg.SetStartingLocation(_("Coordinate Systems"));
                    dlg.SetOverwritePrompt(true);
                    if(!sStartLoc.IsEmpty())
                        dlg.SetStartingLocation(sStartLoc);
                    if(dlg.ShowModalSave() == wxID_OK)
                    {
                        wxWindowDisabler disableAll;
                        wxBusyInfo wait(_("Please wait, working..."));

                        IGxObjectFilter* pFilter = dlg.GetCurrentFilter();
                        if(!pFilter)
                        {
                            wxLogError(_("Null IGxObjectFilter returned"));
                            return;
                        }

                        CPLString sPath = dlg.GetInternalPath();
                        wxString sCatalogPath = dlg.GetPath();
                        wxString sName = dlg.GetName();

                        wxGISFeatureDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(DatasetArray[0]->GetDataset());
                        if(!pDSet)
                        {
                            wxMessageBox(wxString(_("The dataset is empty")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                            wxLogError(_("Null wxGISDataset returned"));
                            return;
                        }

						if(pDSet->IsOpened() && pDSet->IsReadOnly())
							pDSet->Close();
						if(!pDSet->IsOpened())
							if(!pDSet->Open())
								return;

                        ITrackCancel TrackCancel;
                        IStatusBar* pStatusBar = m_pApp->GetStatusBar();                            
                        TrackCancel.SetProgressor(pStatusBar->GetProgressor());
                        if( !ExportFormat(pDSet, sPath, sName, pFilter, NULL, &TrackCancel) )
                            wxMessageBox(wxString(_("While exporting some error occured!")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);

                        //add new IGxObject's
                        IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                        if(pGxApp)
                        {
                            IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
                            if(pCont)
                            {
                                IGxObject* pParentLoc = pCont->SearchChild(sCatalogPath);
                                if(pParentLoc)
                                    pParentLoc->Refresh();
                            }
                        }
                    }
                }
                //3. if not single GxObject progress in special dialog
                else
                {
                    IGxCatalog *pExtCat = NULL;
                    if(pGxApp)
                        pExtCat = pGxApp->GetCatalog();
                    wxGxContainerDialog dlg(pWnd, pExtCat, wxID_ANY, _("Select output"));
				    dlg.SetAllFilters(false);
                    dlg.ShowExportFormats(true);
                    dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKML), true);
                    dlg.AddFilter(new wxGxFeatureFileFilter(enumVecKMZ), true);
                    dlg.AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile), false);
                    dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab), false);
                    dlg.AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif), false);
                    dlg.AddFilter(new wxGxFeatureFileFilter(enumVecDXF), false);

                    dlg.AddShowFilter(new wxGxFolderFilter());
                    dlg.ShowCreateButton(true);
                    if(dlg.ShowModal() == wxID_OK)
                    {
                        IGxObjectFilter* pFilter = dlg.GetCurrentFilter();
                        if(!pFilter)
                        {
                            wxMessageBox(wxString(_("Null IGxObjectFilter returned")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                            wxLogError(_("Null IGxObjectFilter returned"));
                            return;
                        }

                        CPLString sPath = dlg.GetInternalPath();
                        wxString sCatalogPath = dlg.GetPath();

                        //create multiexport dialog
                        ITrackCancel TrackCancel;
                        wxGISProgressDlg ProgressDlg(&TrackCancel, NULL/*pWnd*/);
                        wxWindowDisabler disableAll(&ProgressDlg);
                        ProgressDlg.Show(true);
//                        ProgressDlg.ShowModal(); test purpose
                        IProgressor* pProgr1 = ProgressDlg.GetProgressor1();
                        pProgr1->SetRange(DatasetArray.size());

                        TrackCancel.SetProgressor(ProgressDlg.GetProgressor2());

                        for(size_t i = 0; i < DatasetArray.size(); ++i)
                        {
                            pProgr1->SetValue(i);
                            if(!TrackCancel.Continue())
                                break;

                            IGxObject* pGxSrcObj = dynamic_cast<IGxObject*>(DatasetArray[i]);
                            wxString sName = pGxSrcObj->GetName();
                            sName = ClearExt(sName);

                            wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[i]->GetSubType();
                            //if types is same skip exporting
                            if(nSubType == pFilter->GetSubType())
                                continue;

                            wxGISFeatureDatasetSPtr pDSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(DatasetArray[i]->GetDataset());
                            if(!pDSet)
                            {
                                ProgressDlg.SetText1(wxString::Format(_("The %d dataset is empty"), i));
                                continue;
                            }
							if(!pDSet->IsOpened())
								if(!pDSet->Open())
									return;


                            if( !ExportFormat(pDSet, sPath, sName, pFilter, NULL, &TrackCancel) )
                            {
                                const char* err = CPLGetLastErrorMsg();
                                ProgressDlg.SetText1(wgMB2WX(err));
                            }
                            else
                                ProgressDlg.SetText1(wxString::Format(_("Export successful (%s.%s)!"), sName.c_str(), pFilter->GetExt().c_str()));
                        }

                        //add new IGxObject's
                        IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                        if(pGxApp)
                        {
                            IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
                            if(pCont)
                            {
                                IGxObject* pParentLoc = pCont->SearchChild(sCatalogPath);
                                if(pParentLoc)
                                    pParentLoc->Refresh();
                            }
                        }
                    }
                }
                return;
			}
			break;
		case 1:
            m_pApp->ShowApplicationWindow(m_pToolboxView, !m_pApp->IsApplicationWindowShown(m_pToolboxView));
		default:
			return;
	}
}

bool wxGISGeoprocessingCmd::OnCreate(IFrameApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISGeoprocessingCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Export item"));
		case 1:
			return wxString(_("Show/Hide Toolbox pane"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISGeoprocessingCmd::GetCount(void)
{
	return 2;
}

//bool wxGISGeoprocessingCmd::OnExport(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISEnumVectorDatasetType nNewSubType)
//{
//    wxGISFeatureDatasetSPtr pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
//    if(!pNewDSet)
//    {
//        m_sLastError = wxString(_("Error creating new dataset"));
//        return false;
//    }
//
//    IStatusBar* pStatusBar = m_pApp->GetStatusBar();
//    ITrackCancel TrackCancel;
//    IProgressor* pProgressor;
//    if(pStatusBar)
//    {
//        pProgressor = pStatusBar->GetProgressor();
//        if(pProgressor)
//        {
//            TrackCancel.SetProgressor(pProgressor);
//            pProgressor->Show(true);
//        }
//        pStatusBar->SetMessage(wxString::Format(_("Exporting %s to %s"), pDSet->GetName().c_str(), sName.c_str()));
//    }
//
//    //copy data
//    if(!CopyRows(pDSet, pNewDSet, NULL, &TrackCancel))
//    {
//        m_sLastError = wxString(_("Error copying data to a new dataset"));
//        return false;
//    }
//
//
//    ////add new IGxObject
//    //IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
//    //if(pGxApp)
//    //{
//    //    IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
//    //    if(pCont)
//    //    {
//    //        IGxObjectContainer* pParentLoc = dynamic_cast<IGxObjectContainer*>(pCont->SearchChild(sPath));
//    //        if(pParentLoc)
//    //        {
//    //            wxString sFullPath = sPath + wxFileName::GetPathSeparator() + sName + wxT(".") + sExt;
//    //            IGxObject* pNewGxObj = static_cast<IGxObject*>(new wxGxShapefileDataset(sFullPath, sName, (wxGISEnumVectorDatasetType)nNewSubType));
//    //            if(pParentLoc->AddChild(pNewGxObj))
//    //                pGxApp->GetCatalog()->ObjectAdded(pNewGxObj);
//    //        }
//    //    }
//    //}
//    if(pStatusBar)
//        pStatusBar->SetMessage(_("Done"));
//    if(pProgressor)
//        pProgressor->Show(false);
//    return true;
//}
//
//bool wxGISGeoprocessingCmd::OnExport(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISEnumVectorDatasetType nNewSubType, ITrackCancel* pTrackCancel)
//{
//    wxGISFeatureDatasetSPtr pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
//    if(!pNewDSet)
//    {
//        m_sLastError = wxString(_("Error creating new dataset"));
//        return false;
//    }
//
//    //copy data
//    if(!CopyRows(pDSet, pNewDSet, NULL, pTrackCancel))
//    {
//        m_sLastError = wxString(_("Error copying data to a new dataset"));
//        return false;
//    }
//    return true;
//}

//                        OGRSpatialReference* pSrcSpaRef = pDSet->GetSpatialReference();
//                        OGRSpatialReference* pNewSpaRef(NULL);
//                        if(nNewSubType == enumVecKML)
//                            pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
//                        else
//                            if(pSrcSpaRef)
//                                pNewSpaRef = pSrcSpaRef->Clone();
//
//                         //check multi geometry
//                        OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
//                        bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
//
//
//                        if(!pSrcSpaRef && pNewSpaRef)
//                        {
//                            wxMessageBox(wxString(_("Input spatial reference is not defined")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
//                            wxLogError(_("Input spatial reference is not defined"));
//                            goto EXIT;
//                        }
//
//                        if(bIsMultigeom)
//                        {
//                            wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
//                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
//                            {
//                                int nCount = pDSet->GetSize();
//                                if(nCount > 0)
//                                {
//                                    wxString sNewName = sName + wxString(_("_point"));
//                                    OGRFeatureDefn *pNewDef = pDef->Clone();
//                                    pNewDef->SetGeomType( wkbPoint );
//                                    //check overwrite for sNewName
//                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                                    {
//                                        wxLogError(m_sLastError);
//                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
//                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
//                                            goto EXIT;
//                                    }
//                                }
//                            }
//                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
//                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
//                            {
//                                int nCount = pDSet->GetSize();
//                                if(nCount > 0)
//                                {
//                                    wxString sNewName = sName + wxString(_("_polygon"));
//                                    OGRFeatureDefn *pNewDef = pDef->Clone();
//                                    pNewDef->SetGeomType( wkbPolygon );
//                                    //check overwrite for sNewName
//                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                                    {
//                                        wxLogError(m_sLastError);
//                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
//                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
//                                            goto EXIT;
//                                    }
//                                }
//                            }
//                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
//                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
//                            {
//                                int nCount = pDSet->GetSize();
//                                if(nCount > 0)
//                                {
//                                    wxString sNewName = sName + wxString(_("_line"));
//                                    OGRFeatureDefn *pNewDef = pDef->Clone();
//                                    pNewDef->SetGeomType( wkbLineString );
//                                    //check overwrite for sNewName
//                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                                    {
//                                        wxLogError(m_sLastError);
//                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
//                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
//                                            goto EXIT;
//                                    }
//                                }
//                            }
//                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
//                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
//                            {
//                                int nCount = pDSet->GetSize();
//                                if(nCount > 0)
//                                {
//                                    wxString sNewName = sName + wxString(_("_mpoint"));
//                                    OGRFeatureDefn *pNewDef = pDef->Clone();
//                                    pNewDef->SetGeomType( wkbMultiPoint );
//                                    //check overwrite for sNewName
//                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                                    {
//                                        wxLogError(m_sLastError);
//                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
//                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
//                                            goto EXIT;
//                                    }
//                                }
//                            }
//                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
//                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
//                            {
//                                int nCount = pDSet->GetSize();
//                                if(nCount > 0)
//                                {
//                                    wxString sNewName = sName + wxString(_("_mline"));
//                                    OGRFeatureDefn *pNewDef = pDef->Clone();
//                                    pNewDef->SetGeomType( wkbMultiLineString );
//                                    //check overwrite for sNewName
//                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                                    {
//                                        wxLogError(m_sLastError);
//                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
//                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
//                                            goto EXIT;
//                                    }
//                                }
//                            }
//                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
//                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
//                            {
//                                int nCount = pDSet->GetSize();
//                                if(nCount > 0)
//                                {
//                                    wxString sNewName = sName + wxString(_("_mpolygon"));
//                                    OGRFeatureDefn *pNewDef = pDef->Clone();
//                                    pNewDef->SetGeomType( wkbMultiPolygon );
//                                    //check overwrite for sNewName
//                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                                    {
//                                        wxLogError(m_sLastError);
//                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
//                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
//                                            goto EXIT;
//                                    }
//                                }
//                            }
//                        }
//                        else
//                        {
//                            if(!OnExport(pDSet, sPath, sName, sExt, sDriver, pDef->Clone(), pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
//                            {
//                                wxMessageBox(m_sLastError, wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
//                                wxLogError(m_sLastError);
//                            }
//                        }
//EXIT:
////                        wxDELETE(pNewSpaRef);

                        //    OGRSpatialReference* pSrcSpaRef = pDSet->GetSpatialReference();
                        //    OGRSpatialReference* pNewSpaRef(NULL);
                        //    if(nNewSubType == enumVecKML)
                        //        pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
                        //    else
                        //        if(pSrcSpaRef)
                        //            pNewSpaRef = pSrcSpaRef->Clone();

                        //    if(!pSrcSpaRef && pNewSpaRef)
                        //    {
                        //        ProgressDlg.SetText1(wxString(_("Input spatial reference is not defined")));
                        //        wxDELETE(pNewSpaRef);
                        //        continue;
                        //    }

                        //    //check multi geometry
                        //    OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
                        //    bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
                        //    if(bIsMultigeom)
                        //    {
                        //        wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
                        //        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                        //        {
                        //            int nCount = pDSet->GetSize();
                        //            if(nCount > 0)
                        //            {
                        //                wxString sNewName = sName + wxString(_("_point"));
                        //                OGRFeatureDefn *pNewDef = pDef->Clone();
                        //                pNewDef->SetGeomType( wkbPoint );
                        //                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                        //                //check overwrite for sNewName
                        //                sNewName = CheckUniqName(sPath, sNewName, sExt);
                        //                if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, &TrackCancel))
                        //                {
                        //                    wxLogError(m_sLastError);
                        //                    ProgressDlg.SetText1(m_sLastError);
                        //                }
                        //            }
                        //        }
                        //        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
                        //        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                        //        {
                        //            int nCount = pDSet->GetSize();
                        //            if(nCount > 0)
                        //            {
                        //                wxString sNewName = sName + wxString(_("_polygon"));
                        //                OGRFeatureDefn *pNewDef = pDef->Clone();
                        //                pNewDef->SetGeomType( wkbPolygon );
                        //                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                        //                //check overwrite for sNewName
                        //                sNewName = CheckUniqName(sPath, sNewName, sExt);
                        //                if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                        //                {
                        //                    wxLogError(m_sLastError);
                        //                    ProgressDlg.SetText1(m_sLastError);
                        //                }
                        //            }
                        //        }
                        //        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
                        //        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                        //        {
                        //            int nCount = pDSet->GetSize();
                        //            if(nCount > 0)
                        //            {
                        //                wxString sNewName = sName + wxString(_("_line"));
                        //                OGRFeatureDefn *pNewDef = pDef->Clone();
                        //                pNewDef->SetGeomType( wkbLineString );
                        //                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                        //                //check overwrite for sNewName
                        //                sNewName = CheckUniqName(sPath, sNewName, sExt);
                        //                if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                        //                {
                        //                    wxLogError(m_sLastError);
                        //                    ProgressDlg.SetText1(m_sLastError);
                        //                }
                        //            }
                        //        }
                        //        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
                        //        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                        //        {
                        //            int nCount = pDSet->GetSize();
                        //            if(nCount > 0)
                        //            {
                        //                wxString sNewName = sName + wxString(_("_mpoint"));
                        //                OGRFeatureDefn *pNewDef = pDef->Clone();
                        //                pNewDef->SetGeomType( wkbMultiPoint );
                        //                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                        //                //check overwrite for sNewName
                        //                sNewName = CheckUniqName(sPath, sNewName, sExt);
                        //                if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                        //                {
                        //                    wxLogError(m_sLastError);
                        //                    ProgressDlg.SetText1(m_sLastError);
                        //                }
                        //            }
                        //        }
                        //        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
                        //        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                        //        {
                        //            int nCount = pDSet->GetSize();
                        //            if(nCount > 0)
                        //            {
                        //                wxString sNewName = sName + wxString(_("_mline"));
                        //                OGRFeatureDefn *pNewDef = pDef->Clone();
                        //                pNewDef->SetGeomType( wkbMultiLineString );
                        //                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                        //                //check overwrite for sNewName
                        //                sNewName = CheckUniqName(sPath, sNewName, sExt);
                        //                if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                        //                {
                        //                    wxLogError(m_sLastError);
                        //                    ProgressDlg.SetText1(m_sLastError);
                        //                }
                        //            }
                        //        }
                        //        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
                        //        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                        //        {
                        //            int nCount = pDSet->GetSize();
                        //            if(nCount > 0)
                        //            {
                        //                wxString sNewName = sName + wxString(_("_mpolygon"));
                        //                OGRFeatureDefn *pNewDef = pDef->Clone();
                        //                pNewDef->SetGeomType( wkbMultiPolygon );
                        //                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                        //                //check overwrite for sNewName
                        //                sNewName = CheckUniqName(sPath, sNewName, sExt);
                        //                if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                        //                {
                        //                    wxLogError(m_sLastError);
                        //                    ProgressDlg.SetText1(m_sLastError);
                        //                }
                        //            }
                        //        }
                        //    }
                        //    else
                        //    {
                        //        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sName + wxT(".") + sExt).c_str()));
                        //        sName = CheckUniqName(sPath, sName, sExt);
                        //        if(!OnExport(pDSet, sPath, sName, sExt, sDriver, pDef->Clone(), pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, &TrackCancel))
                        //        {
                        //            ProgressDlg.SetText1(m_sLastError);
                        //            wxLogError(m_sLastError);
                        //        }
                        //    }
                        //    //wsDELETE(pDSet);
                        //    wxDELETE(pNewSpaRef);
                        //}
                        ////ProgressDlg.Show(false);
                        ////pWnd->Raise();//SetFocus();