/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  export geoprocessing tools.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/geoprocessing/gpexporttool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPExportTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPExportTool, wxGISGPTool)

wxGISGPExportTool::wxGISGPExportTool(void) : wxGISGPTool()
{
}

wxGISGPExportTool::~wxGISGPExportTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); i++)
        wxDELETE(m_pParamArr[i]);
}

wxString wxGISGPExportTool::GetDisplayName(void)
{
    return wxString(_("Export vector file (single)"));
}

wxString wxGISGPExportTool::GetName(void)
{
    return wxString(wxT("vexport_single"));
}

wxString wxGISGPExportTool::GetCategory(void)
{
    return wxString(_("Conversion Tools/Vector"));
}

GPParameters* wxGISGPExportTool::GetParameterInfo(void)
{
    if(m_pParamArr.empty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source feature class"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISFeatureDataset));
        pParam1->SetDomain(pDomain1);

        m_pParamArr.push_back(pParam1);

        //TODO: SQL statement

        //dst path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("dst_path"));
        pParam2->SetDisplayName(_("Destination feature class"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        pDomain2->AddFilter(new wxGxFeatureFileFilter(enumVecESRIShapefile));
        pDomain2->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoTab));
        pDomain2->AddFilter(new wxGxFeatureFileFilter(enumVecMapinfoMif));
        pDomain2->AddFilter(new wxGxFeatureFileFilter(enumVecKML));
        pDomain2->AddFilter(new wxGxFeatureFileFilter(enumVecKMZ));
        pDomain2->AddFilter(new wxGxFeatureFileFilter(enumVecDXF));
        pParam2->SetDomain(pDomain2);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_pParamArr.push_back(pParam2);

    }
    return &m_pParamArr;
}

bool wxGISGPExportTool::Validate(void)
{
    if(!m_pParamArr[1]->GetAltered())
    {
        if(m_pParamArr[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_pParamArr[0]->GetValue();
            //wxFileName Name(sPath);
            //Name.
            m_pParamArr[1]->SetValue(wxVariant(sPath, wxT("path")));
            m_pParamArr[1]->SetAltered(true);//??
        }
    }

    //check if input & output types is same!
    if(m_pParamArr[0]->GetIsValid())
    {
        if(!m_pParamArr[1]->GetHasBeenValidated())
        {
            //TODO: Maybe IGxDataset in future?
            //IGxDataset* pDset1 = m_pCatalog->SearchChild()
            wxFileName Name1(m_pParamArr[0]->GetValue());
            wxFileName Name2(m_pParamArr[1]->GetValue());
            if(Name1.GetExt() == Name2.GetExt())
            {
                m_pParamArr[1]->SetIsValid(false);
                m_pParamArr[1]->SetMessage(wxGISEnumGPMessageError, _("Cannot export to the same format"));
                return false;
            }
        }
    }
    return true;
}

bool wxGISGPExportTool::Execute(ITrackCancel* pTrackCancel)
{
    if(!Validate())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
        return false;
    }

    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pGxObjectContainer)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
        return false;
    }

    wxString sSrcPath = m_pParamArr[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISFeatureDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(pGxDataset->GetDataset(true));
    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    
    OGRFeatureDefn *pDef = pSrcDataSet->GetDefiniton();
    if(!pDef)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error reading dataset definition"), -1, enumGISMessageErr);
        //wsDELETE(pSrcDataSet);
        return false;
    }
    
    wxString sDstPath = m_pParamArr[1]->GetValue();
    wxFileName sDstFileName(sDstPath);
    wxString sPath = sDstFileName.GetPath();
    IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sPath);
    if(!pGxDstObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
        return false;
    }

    CPLString szPath = pGxDstObject->GetInternalName();
    wxString sName = sDstFileName.GetName();
    
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParamArr[1]->GetDomain());
    IGxObjectFilter* pFilter = pDomain->GetFilter(pDomain->GetSelFilter());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }
        
    bool bHasErrors = ExportFormat(pSrcDataSet, szPath, sName, pFilter, NULL, pTrackCancel);

    IGxObjectContainer* pCont = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(pCont)
    {
        IGxObject* pParentLoc = pCont->SearchChild(sPath);
        if(pParentLoc)
            pParentLoc->Refresh();
    }

    return !bHasErrors;
}

//bool wxGISGPExportTool::OnExport(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISEnumVectorDatasetType nNewSubType, ITrackCancel* pTrackCancel)
//{
//    wxGISFeatureDatasetSPtr pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
//    if(!pNewDSet)
//    {
//        if(pTrackCancel)
//            pTrackCancel->PutMessage(_("Error creating new dataset"), -1, enumGISMessageErr);
//        return false; 
//    }
//
//    if(pTrackCancel)
//        pTrackCancel->PutMessage(wxString::Format(_("Exporting %s to %s"), pDSet->GetName().c_str(), sName.c_str()), -1, enumGISMessageTitle);
//
//    //copy data
//    //TODO: Add QueryFilter
//    if(!CopyRows(pDSet, pNewDSet, NULL, pTrackCancel))
//    {
//        if(pTrackCancel)
//            pTrackCancel->PutMessage(_("Error copying data to new dataset"), -1, enumGISMessageErr);
//        //wsDELETE(pNewDSet);
//        return false; 
//    }
//
//    //wsDELETE(pNewDSet);
//    return true;
//}



    //OGRSpatialReference* pSrcSpaRef = pSrcDataSet->GetSpatialReference();
    //OGRSpatialReference* pNewSpaRef(NULL);

    //if(nNewSubType == enumVecKML)
    //    pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
    //else
    //    if(pSrcSpaRef)
    //        pNewSpaRef = pSrcSpaRef->Clone();

    //if(!pSrcSpaRef && pNewSpaRef)
    //{
    //    if(pTrackCancel)
    //        pTrackCancel->PutMessage(_("The input spatial reference is not defined!"), -1, enumGISMessageErr);

    //    wxDELETE(pNewSpaRef);
    //}

    ////set filter
    //wxGISQueryFilter* pQFilter(NULL);

    //bool bHasErrors(false);
    ////check multi geometry
    //OGRwkbGeometryType nGeomType = pSrcDataSet->GetGeometryType();
    //bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
    //if(bIsMultigeom)
    //{
    //    //select distinct ogr_geometry from elements
    //    //TODO: combain filters
    //    wxGISQueryFilter Filter(wxString(wxT("OGR_GEOMETRY='POINT'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_point"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbPoint );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_polygon"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbPolygon );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_line"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbLineString );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_mpoint"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbMultiPoint );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_mline"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbMultiLineString );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //    Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
    //    if(pSrcDataSet->SetFilter(&Filter) == OGRERR_NONE)
    //    {
    //        int nCount = pSrcDataSet->GetSize();
    //        if(nCount > 0)
    //        {
    //            wxString sNewName = sName + wxString(_("_mpolygon"));
    //            OGRFeatureDefn *pNewDef = pDef->Clone();
    //            pNewDef->SetGeomType( wkbMultiPolygon );
    //            //check overwrite for sNewName
    //            if(!OnExport(pSrcDataSet, sPath, sNewName, sExt, sDriver, pNewDef, pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //                bHasErrors = true;
    //        }
    //    }
    //}
    //else
    //{
    //    if(!OnExport(pSrcDataSet, sPath, sName, sExt, sDriver, pDef->Clone(), pNewSpaRef, (wxGISEnumVectorDatasetType)nNewSubType, pTrackCancel))
    //        bHasErrors = true;
    //}

    ////wsDELETE(pSrcDataSet);
    //wxDELETE(pNewSpaRef);
