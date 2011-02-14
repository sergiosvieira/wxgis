/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main dataset functions.
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

#include "wxgis/geoprocessing/gpvector.h"
#include "wxgis/datasource/sysop.h"

bool CopyRows(wxGISFeatureDatasetSPtr pSrcDataSet, wxGISFeatureDatasetSPtr pDstDataSet, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    OGRSpatialReference* pSrsSRS = pSrcDataSet->GetSpatialReference();
    OGRSpatialReference* pDstSRS = pDstDataSet->GetSpatialReference();
    OGRCoordinateTransformation *poCT(NULL);
    bool bSame = pSrsSRS == NULL || pDstSRS == NULL || pSrsSRS->IsSame(pDstSRS);
    if( !bSame )
        poCT = OGRCreateCoordinateTransformation( pSrsSRS, pDstSRS );

    //progress & messages
    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
       pTrackCancel->PutMessage(wxString::Format(_("Start CopyRows from '%s' to '%s'"), pSrcDataSet->GetPath().c_str(), pDstDataSet->GetPath().c_str()), -1, enumGISMessageNorm);
    }
    int nCounter(0);
    size_t nStep = pSrcDataSet->GetSize() < 10 ? 1 : pSrcDataSet->GetSize() / 10;
    if(pProgressor)
        pProgressor->SetRange(pSrcDataSet->GetSize());

    pSrcDataSet->Reset();
    OGRFeature* pFeature;
    while((pFeature = pSrcDataSet->Next()) != NULL)
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            OGRFeature::DestroyFeature(pFeature);

            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.mb_str());
            CPLError( CE_Warning, CPLE_AppDefined, sFullErr );

            if(pTrackCancel)
                pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
            return false;
        }
        if( !bSame && poCT )
        {
            OGRGeometry *pGeom = pFeature->GetGeometryRef()->clone();
            if(pGeom)
            {
                OGRErr eErr = pGeom->transform(poCT);
                if(eErr == OGRERR_NONE)
                    pFeature->SetGeometryDirectly(pGeom);
                else
                    wxDELETE(pGeom);
            }
        }
        //set encoding
        //wxFontEncoding InputEncoding = pSrcDataSet->GetEncoding();
        wxFontEncoding OutputEncoding = pDstDataSet->GetEncoding();

        //if(1)//OutputEncoding != InputEncoding)
        //{
        OGRFeatureDefn *pFeatureDefn = pFeature->GetDefnRef();
        if(!pFeatureDefn)
        {
            OGRFeature::DestroyFeature(pFeature);
            continue;
        }
        
        CPLErrorReset();
        wxString sACText;

        for(size_t i = 0; i < pFeatureDefn->GetFieldCount(); i++)
        {
            OGRFieldDefn *pFieldDefn = pFeatureDefn->GetFieldDefn(i);
            if(pFieldDefn)
            {
                OGRFieldType nType = pFieldDefn->GetType();
                if(OFTString == nType)
                {
                    wxString sFieldString;
                    if(CPLIsUTF8(pFeature->GetFieldAsString(i), -1))
                    {
                        sFieldString = wxString::FromUTF8(pFeature->GetFieldAsString(i));
                    }
                    else
                    {
                        sFieldString = wxString(wgMB2WX(pFeature->GetFieldAsString(i)));
                    }
                    //wxCSConv inconv(OutputEncoding);
                    //CPLString cplstr(pFeature->GetFieldAsString(i));
                    //wxString sFieldString(cplstr.
                    ////wxString sFieldString(wgMB2WX(pFeature->GetFieldAsString(i)));
                    //wxCSConv outconv(OutputEncoding);
                    //pFeature->SetField(i, sFieldString.mb_str(outconv));   

                    if(OutputEncoding == wxFONTENCODING_DEFAULT)
                        pFeature->SetField(i, wgWX2MB(sFieldString));
                    else            
                    {                
                        wxCSConv outconv(OutputEncoding);
                        pFeature->SetField(i, sFieldString.mb_str(outconv));
                    }
                    if(pDstDataSet->GetSubType() == enumVecDXF)
                        sACText += sFieldString + wxT("\n");
                }
                //TODO: OFTStringList 
            }
        }
        //}

        if(pDstDataSet->GetSubType() == enumVecDXF)
        {
        //    //LABEL(f:"Arial, Helvetica", s:12pt, t:"Hello World!")
            wxString sStyleLabel = wxString::Format(wxT("LABEL(f:\"Arial, Helvetica\", s:12pt, t:\"%s\")"), sACText.c_str());
            pFeature->SetStyleString(wgWX2MB(sStyleLabel));
        }

        OGRErr eErr = pDstDataSet->CreateFeature(pFeature);
        if(eErr != OGRERR_NONE)
        {
            wxString sErr(_("Error create feature! OGR error: "));
            CPLString sFullErr(sErr.mb_str());
            sFullErr += CPLGetLastErrorMsg();
            CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
            if(pTrackCancel)
                pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        }
        nCounter++;
        if(pProgressor && nCounter % nStep == 0)
            pProgressor->SetValue(nCounter);
    }

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);

    return true;
}

bool ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();

    wxGISFeatureDatasetSPtr pNewDSet = CreateVectorLayer(sPath, sName, sExt, sDriver, pDef, pNewSpaRef);
    if(!pNewDSet)
    {
        wxString sErr(_("Error creating new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }

    //copy data
    if(!CopyRows(pDSet, pNewDSet, pQFilter, pTrackCancel))
    {
        wxString sErr(_("Error copying data to a new dataset! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_FileIO, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }
    return true;
}

bool ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    if(!pFilter || !pDSet)
        return false;

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Exporting %s to %s"), pDSet->GetName().c_str(), sName.c_str()), -1, enumGISMessageTitle);
    OGRSpatialReference* pSrcSpaRef = pDSet->GetSpatialReference();
    if(!pSrcSpaRef)
    {
        wxString sErr(_("Input spatial reference is not defined! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }

    OGRSpatialReference* pNewSpaRef(NULL);
    if(nNewSubType == enumVecKML || nNewSubType == enumVecKMZ)
        pNewSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);
    else
        pNewSpaRef = pSrcSpaRef->Clone();

    OGRFeatureDefn *pDef = pDSet->GetDefiniton();
    if(!pDef)
    {
        wxString sErr(_("Error read dataset definition"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
        return false;
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
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_point")), sExt);                
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbPoint );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='POLYGON'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            int nCount = pDSet->GetSize();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_polygon")), sExt); 
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbPolygon );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='LINESTRING'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            int nCount = pDSet->GetSize();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_line")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbLineString );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOINT'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            int nCount = pDSet->GetSize();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_mpoint")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbMultiPoint );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTILINESTRING'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            int nCount = pDSet->GetSize();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_mline")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbMultiLineString );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
            }
        }
        Filter.SetWhereClause(wxString(wxT("OGR_GEOMETRY='MULTIPOLYGON'")));
        if(pDSet->SetFilter(&Filter) == OGRERR_NONE)
        {
            int nCount = pDSet->GetSize();
            if(nCount > 0)
            {
                wxString sNewName = CheckUniqName(sPath, sName + wxString(_("_mpolygon")), sExt);
                OGRFeatureDefn *pNewDef = pDef->Clone();
                pNewDef->SetGeomType( wkbMultiPolygon );
                if( !ExportFormat(pDSet, sPath, sNewName, pFilter, pNewDef, pNewSpaRef, pQFilter, pTrackCancel) )
                    return false;
            }
        }
    }
    else
    {
        if( !ExportFormat(pDSet, sPath, sName, pFilter, pDef, pNewSpaRef, pQFilter, pTrackCancel) )
            return false;
    }
    return true;
}