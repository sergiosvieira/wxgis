/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Geoprocessing Main Commands class.
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

#include "wxgis/geoprocessingui/geoprocessingcmd.h"

#include "../../art/gp_menu_16.xpm"

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/gxcontdialog.h"
#include "wxgis/catalogui/gxfilters.h"
#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/framework/progressdlg.h"

#include "wx/busyinfo.h"

//
//#include "wxgis/framework/progressor.h"
//#include "wxgis/datasource/datasource.h"
//#include <wx/dirdlg.h>
//#include <wx/file.h>
//#include "wxgis/catalog/gxfile.h"
//#include "wxgis/carto/mapview.h"


//	0	Export
//  1   ?

IMPLEMENT_DYNAMIC_CLASS(wxGISGeoprocessingCmd, wxObject)

wxGISGeoprocessingCmd::wxGISGeoprocessingCmd(void)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(gp_menu_16_xpm));
}

wxGISGeoprocessingCmd::~wxGISGeoprocessingCmd(void)
{
}

wxIcon wxGISGeoprocessingCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_ImageList.GetIcon(0);
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
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISGeoprocessingCmd::GetChecked(void)
{
	return false;
}

bool wxGISGeoprocessingCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Export
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
				IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxDataset* pDSet = dynamic_cast<IGxDataset*>(pSel->GetSelectedObjects(i));
                    if(pDSet)
                        return true;
                }
			}
			return false;
		}
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISGeoprocessingCmd::GetKind(void)
{
	switch(m_subtype)
	{
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
					IGxSelection* pSel = pGxApp->GetCatalog()->GetSelection();
                    for(size_t i = 0; i < pSel->GetCount(); i++)
                    {
                        IGxDataset* pGxDSet = dynamic_cast<IGxDataset*>(pSel->GetSelectedObjects(i));
                        IGxObjectContainer* pObjectContainer = dynamic_cast<IGxObjectContainer*>(pGxDSet);
                        if(pObjectContainer)
                        {
                            GxObjectArray* pArr = pObjectContainer->GetChildren();
                            if(pArr != NULL)
                            {
                                for(size_t i = 0; i < pArr->size(); i++)
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
                    wxFileName FName(sName);
                    FName.ClearExt();
                    sName = FName.GetName();

                    IGxObject* pGxParentObj = pGxSrcObj->GetParent();
                    wxString sStartLoc;
                    if(pGxParentObj)
                        sStartLoc = pGxParentObj->GetFullName();
                    
                    wxGxObjectDialog dlg(pWnd, wxID_ANY, _("Select output")); 
                    dlg.SetName(sName);
				    dlg.SetAllowMultiSelect(false);
				    dlg.SetAllFilters(false);

                    wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[0]->GetSubType();
                    if(nSubType != enumVecKML)
                        dlg.AddFilter(new wxGxKMLFilter(), true);
                    if(nSubType != enumVecESRIShapefile)
                        dlg.AddFilter(new wxGxShapeFileFilter(), false);
                    if(nSubType != enumVecMapinfoTab)
                        dlg.AddFilter(new wxGxMapInfoFilter(true), false);
                    if(nSubType != enumVecMapinfoTab)
                        dlg.AddFilter(new wxGxMapInfoFilter(false), false);
                    if(nSubType != enumVecDXF)
                        dlg.AddFilter(new wxGxDXFFilter(), false);

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
                        wxString sDriver = pFilter->GetDriver();
                        wxString sExt = pFilter->GetExt();
                        int nNewSubType = pFilter->GetSubType();
                        wxString sPath = dlg.GetPath();
                        wxString sName = dlg.GetName();

                        wxGISFeatureDataset* pDSet = dynamic_cast<wxGISFeatureDataset*>(DatasetArray[0]->GetDataset());
                        if(!pDSet)
                        {
                            wxMessageBox(wxString(_("The dataset is empty")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                            wxLogError(_("Null wxGISDataset returned"));
                            return; 
                        }

                        OGRFeatureDefn *pDef = pDSet->GetDefiniton();
                        if(!pDef)
                        {
                            wxMessageBox(wxString(_("Error read dataset definition")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                            wxLogError(_("Error read dataset definition"));
                            wsDELETE(pDSet);
                            return; 
                        }

                        OGRSpatialReference* pSrcSpaRef = pDSet->GetSpatialReference();
                        OGRSpatialReference* pNewSpaRef(NULL);
                        if(nNewSubType == enumVecKML)
                            pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
                        else
                            if(pSrcSpaRef)
                                pNewSpaRef = pSrcSpaRef->Clone();

                        if(!pSrcSpaRef && pNewSpaRef)
                        {
                            wxMessageBox(wxString(_("The input spatial reference is not defined!")), wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                            wxLogError(_("The input spatial reference is not defined!"));
                            goto EXIT; 
                        }

                        //check multi geometry
                        OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
                        bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
                        if(bIsMultigeom)
                        {
                            wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                            {
                                int nCount = pDSet->GetSize();
                                if(nCount > 0)
                                {
                                    wxString sNewName = sName + wxString(_("_point"));
                                    OGRFeatureDefn *pNewDef = pDef->Clone();
                                    pNewDef->SetGeomType( wkbPoint );
                                    //check overwrite for sNewName
                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                    {
                                        wxLogError(m_sLastError);
                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
                                            goto EXIT;
                                    }
                                }
                            }
                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                            {
                                int nCount = pDSet->GetSize();
                                if(nCount > 0)
                                {
                                    wxString sNewName = sName + wxString(_("_polygon"));
                                    OGRFeatureDefn *pNewDef = pDef->Clone();
                                    pNewDef->SetGeomType( wkbPolygon );
                                    //check overwrite for sNewName
                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                    {
                                        wxLogError(m_sLastError);
                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
                                            goto EXIT;
                                    }
                                }
                            }
                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                            {
                                int nCount = pDSet->GetSize();
                                if(nCount > 0)
                                {
                                    wxString sNewName = sName + wxString(_("_line"));
                                    OGRFeatureDefn *pNewDef = pDef->Clone();
                                    pNewDef->SetGeomType( wkbLineString );
                                    //check overwrite for sNewName
                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                    {
                                        wxLogError(m_sLastError);
                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
                                            goto EXIT;
                                    }
                                }
                            }
                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                            {
                                int nCount = pDSet->GetSize();
                                if(nCount > 0)
                                {
                                    wxString sNewName = sName + wxString(_("_mpoint"));
                                    OGRFeatureDefn *pNewDef = pDef->Clone();
                                    pNewDef->SetGeomType( wkbMultiPoint );
                                    //check overwrite for sNewName
                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                    {
                                        wxLogError(m_sLastError);
                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
                                            goto EXIT;
                                    }
                                }
                            }
                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                            {
                                int nCount = pDSet->GetSize();
                                if(nCount > 0)
                                {
                                    wxString sNewName = sName + wxString(_("_mline"));
                                    OGRFeatureDefn *pNewDef = pDef->Clone();
                                    pNewDef->SetGeomType( wkbMultiLineString );
                                    //check overwrite for sNewName
                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                    {
                                        wxLogError(m_sLastError);
                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
                                            goto EXIT;
                                    }
                                }
                            }
                            Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
                            if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                            {
                                int nCount = pDSet->GetSize();
                                if(nCount > 0)
                                {
                                    wxString sNewName = sName + wxString(_("_mpolygon"));
                                    OGRFeatureDefn *pNewDef = pDef->Clone();
                                    pNewDef->SetGeomType( wkbMultiPolygon );
                                    //check overwrite for sNewName
                                    if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                    {
                                        wxLogError(m_sLastError);
                                        wxString sQuestion = m_sLastError + wxString(_("/nContinue?"));
                                        if(wxMessageBox(sQuestion, wxString(_("Question")), wxCENTRE | wxICON_QUESTION | wxYES_NO, pWnd) == wxNO)
                                            goto EXIT;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if(!OnExport(pDSet, sPath, sName, sExt, sDriver, pDef->Clone(), pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                            {
                                wxMessageBox(m_sLastError, wxString(_("Error")), wxCENTRE | wxICON_ERROR | wxOK, pWnd);
                                wxLogError(m_sLastError);
                            }
                        }
EXIT:
                        wsDELETE(pDSet);
                        wxDELETE(pNewSpaRef);

                        //add new IGxObject's
                        IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                        if(pGxApp)
                        {
                            IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
                            if(pCont)
                            {
                                IGxObject* pParentLoc = pCont->SearchChild(sPath);
                                if(pParentLoc)
                                    pParentLoc->Refresh();
                            }
                        }
                        return;
                    }
                }
                //3. if not single GxObject progress in special dialog
                else
                {
                    wxGxContainerDialog dlg(pWnd, wxID_ANY, _("Select output")); 
				    dlg.SetAllFilters(false);
                    dlg.ShowExportFormats(true);
                    dlg.AddFilter(new wxGxKMLFilter(), true);
                    dlg.AddFilter(new wxGxShapeFileFilter(), false);
                    dlg.AddFilter(new wxGxMapInfoFilter(true), false);
                    dlg.AddFilter(new wxGxMapInfoFilter(false), false);
                    dlg.AddFilter(new wxGxDXFFilter(), false);

                    dlg.AddShowFilter(new wxGxFolderFilter());
                    //dlg.ShowCreateButton(true);
                    if(dlg.ShowModal() == wxID_OK)
                    {
                        IGxObjectFilter* pFilter = dlg.GetCurrentFilter();
                        if(!pFilter)
                        {
                            wxLogError(_("Null IGxObjectFilter returned"));
                            return; 
                        }
                        wxString sDriver = pFilter->GetDriver();
                        wxString sExt = pFilter->GetExt();
                        int nNewSubType = pFilter->GetSubType();
                        wxString sPath = dlg.GetPath();

                        //create multiexport dialog
                        wxGISProgressDlg ProgressDlg(NULL/*pWnd*/);
                        wxWindowDisabler disableAll(&ProgressDlg);
                        ProgressDlg.Show(true);
//                        ProgressDlg.ShowModal(); test purpose
                        IProgressor* pProgr1 = ProgressDlg.GetProgressor1();
                        pProgr1->SetRange(DatasetArray.size());

                        ITrackCancel TrackCancel;
                        TrackCancel.SetProgressor(ProgressDlg.GetProgressor2());

                        for(size_t i = 0; i < DatasetArray.size(); i++)
                        {
                            pProgr1->SetValue(i);
                            if(!TrackCancel.Continue())
                                break;

                            IGxObject* pGxSrcObj = dynamic_cast<IGxObject*>(DatasetArray[i]);
                            wxString sName = pGxSrcObj->GetName();
                            wxFileName FName(sName);
                            FName.ClearExt();
                            sName = FName.GetName();

                            wxGISEnumVectorDatasetType nSubType = (wxGISEnumVectorDatasetType)DatasetArray[i]->GetSubType();
                            //if types is same scip exporting
                            if(nSubType == nNewSubType)
                                continue;

                            wxGISFeatureDataset* pDSet = dynamic_cast<wxGISFeatureDataset*>(DatasetArray[i]->GetDataset());
                            if(!pDSet)
                            {
                                ProgressDlg.SetText1(wxString::Format(_("The %d dataset is empty"), i));
                                continue;
                            }

                            OGRFeatureDefn *pDef = pDSet->GetDefiniton();
                            if(!pDef)
                            {
                                ProgressDlg.SetText1(wxString::Format(_("Error read %d dataset definition"), i));
                                wsDELETE(pDSet);
                                continue;
                            }

                            OGRSpatialReference* pSrcSpaRef = pDSet->GetSpatialReference();
                            OGRSpatialReference* pNewSpaRef(NULL);
                            if(nNewSubType == enumVecKML)
                                pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
                            else
                                if(pSrcSpaRef)
                                    pNewSpaRef = pSrcSpaRef->Clone();

                            if(!pSrcSpaRef && pNewSpaRef)
                            {
                                ProgressDlg.SetText1(wxString(_("The input spatial reference is not defined!")));
                                wsDELETE(pDSet);
                                wxDELETE(pNewSpaRef);
                                continue;
                            }

                            //check multi geometry
                            OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
                            bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
                            if(bIsMultigeom)
                            {
                                wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
                                if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                                {
                                    int nCount = pDSet->GetSize();
                                    if(nCount > 0)
                                    {
                                        wxString sNewName = sName + wxString(_("_point"));
                                        OGRFeatureDefn *pNewDef = pDef->Clone();
                                        pNewDef->SetGeomType( wkbPoint );
                                        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                                        //check overwrite for sNewName
                                        if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, &TrackCancel))
                                        {
                                            wxLogError(m_sLastError);
                                            ProgressDlg.SetText1(m_sLastError);
                                        }
                                    }
                                }
                                Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
                                if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                                {
                                    int nCount = pDSet->GetSize();
                                    if(nCount > 0)
                                    {
                                        wxString sNewName = sName + wxString(_("_polygon"));
                                        OGRFeatureDefn *pNewDef = pDef->Clone();
                                        pNewDef->SetGeomType( wkbPolygon );
                                        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                                        //check overwrite for sNewName
                                        if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                        {
                                            wxLogError(m_sLastError);
                                            ProgressDlg.SetText1(m_sLastError);
                                        }
                                    }
                                }
                                Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
                                if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                                {
                                    int nCount = pDSet->GetSize();
                                    if(nCount > 0)
                                    {
                                        wxString sNewName = sName + wxString(_("_line"));
                                        OGRFeatureDefn *pNewDef = pDef->Clone();
                                        pNewDef->SetGeomType( wkbLineString );
                                        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                                        //check overwrite for sNewName
                                        if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                        {
                                            wxLogError(m_sLastError);
                                            ProgressDlg.SetText1(m_sLastError);
                                        }
                                    }
                                }
                                Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
                                if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                                {
                                    int nCount = pDSet->GetSize();
                                    if(nCount > 0)
                                    {
                                        wxString sNewName = sName + wxString(_("_mpoint"));
                                        OGRFeatureDefn *pNewDef = pDef->Clone();
                                        pNewDef->SetGeomType( wkbMultiPoint );
                                        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                                        //check overwrite for sNewName
                                        if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                        {
                                            wxLogError(m_sLastError);
                                            ProgressDlg.SetText1(m_sLastError);
                                        }
                                    }
                                }
                                Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
                                if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                                {
                                    int nCount = pDSet->GetSize();
                                    if(nCount > 0)
                                    {
                                        wxString sNewName = sName + wxString(_("_mline"));
                                        OGRFeatureDefn *pNewDef = pDef->Clone();
                                        pNewDef->SetGeomType( wkbMultiLineString );
                                        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                                        //check overwrite for sNewName
                                        if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                        {
                                            wxLogError(m_sLastError);
                                            ProgressDlg.SetText1(m_sLastError);
                                        }
                                    }
                                }
                                Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
                                if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
                                {
                                    int nCount = pDSet->GetSize();
                                    if(nCount > 0)
                                    {
                                        wxString sNewName = sName + wxString(_("_mpolygon"));
                                        OGRFeatureDefn *pNewDef = pDef->Clone();
                                        pNewDef->SetGeomType( wkbMultiPolygon );
                                        ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sNewName + wxT(".") + sExt).c_str()));
                                        //check overwrite for sNewName
                                        if(!OnExport(pDSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType))
                                        {
                                            wxLogError(m_sLastError);
                                            ProgressDlg.SetText1(m_sLastError);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                ProgressDlg.SetText1(wxString::Format(_("Exporting %s to %s"), sName.c_str(), wxString(sName + wxT(".") + sExt).c_str()));
                                if(!OnExport(pDSet, sPath, sName, sExt, sDriver, pDef->Clone(), pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, &TrackCancel))
                                {
                                    ProgressDlg.SetText1(m_sLastError);
                                    wxLogError(m_sLastError);
                                }
                            }
                            wsDELETE(pDSet);
                            wxDELETE(pNewSpaRef);
                        }
                        //ProgressDlg.Show(false);
                        //pWnd->Raise();//SetFocus();

                        //add new IGxObject's
                        IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
                        if(pGxApp)
                        {
                            IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
                            if(pCont)
                            {
                                IGxObject* pParentLoc = pCont->SearchChild(sPath);
                                if(pParentLoc)
                                    pParentLoc->Refresh();
                            }
                        }
                        return;
                    }
                }
			}
			break;
		default:
			return;
	}
}

bool wxGISGeoprocessingCmd::OnCreate(IApplication* pApp)
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
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISGeoprocessingCmd::GetCount(void)
{
	return 1;
}

bool wxGISGeoprocessingCmd::OnExport(wxGISFeatureDataset* pDSet, wxString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISEnumVectorDatasetType nNewSubType)
{
    wxGISFeatureDataset* pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
    if(!pNewDSet)
    {
        m_sLastError = wxString(_("Error create new dataset"));
        return false; 
    }

    IStatusBar* pStatusBar = m_pApp->GetStatusBar();
    ITrackCancel TrackCancel;
    if(pStatusBar)
    {                            
        TrackCancel.SetProgressor(pStatusBar->GetProgressor());
        pStatusBar->SetMessage(wxString::Format(_("Exporting %s to %s"), pDSet->GetName().c_str(), sName.c_str()));
    }

    //copy data
    if(!CopyRows(pDSet, pNewDSet, NULL, &TrackCancel))
    {
        m_sLastError = wxString(_("Error copy data to new dataset"));
        wsDELETE(pNewDSet);
        return false; 
    }

    wsDELETE(pNewDSet);

    ////add new IGxObject
    //IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
    //if(pGxApp)
    //{
    //    IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(pGxApp->GetCatalog());
    //    if(pCont)
    //    {
    //        IGxObjectContainer* pParentLoc = dynamic_cast<IGxObjectContainer*>(pCont->SearchChild(sPath));
    //        if(pParentLoc)
    //        {
    //            wxString sFullPath = sPath + wxFileName::GetPathSeparator() + sName + wxT(".") + sExt;
    //            IGxObject* pNewGxObj = static_cast<IGxObject*>(new wxGxShapefileDataset(sFullPath, sName, (wxGISEnumVectorDatasetType)nNewSubType));
    //            if(pParentLoc->AddChild(pNewGxObj))
    //                pGxApp->GetCatalog()->ObjectAdded(pNewGxObj);
    //        }
    //    }
    //}
    if(pStatusBar)
        pStatusBar->SetMessage(_("Done"));
    return true;
}

bool wxGISGeoprocessingCmd::OnExport(wxGISFeatureDataset* pDSet, wxString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISEnumVectorDatasetType nNewSubType, ITrackCancel* pTrackCancel)
{
    wxGISFeatureDataset* pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
    if(!pNewDSet)
    {
        m_sLastError = wxString(_("Error create new dataset"));
        return false; 
    }

    //copy data
    if(!CopyRows(pDSet, pNewDSet, NULL, pTrackCancel))
    {
        m_sLastError = wxString(_("Error copy data to new dataset"));
        wsDELETE(pNewDSet);
        return false; 
    }

    wsDELETE(pNewDSet);

    return true;
}