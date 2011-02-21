/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main dataset functions.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/geometry/algorithm.h"
#include "wxgis/datasource/spvalidator.h"

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

bool Project(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRSpatialReference* pNewSpaRef, ITrackCancel* pTrackCancel)
{
    CPLErrorReset();

    if(!pFilter || !pDSet)
        return false;

    int nNewSubType = pFilter->GetSubType();
    //check multi geometry
    OGRwkbGeometryType nGeomType = pDSet->GetGeometryType();
    bool bIsMultigeom = nNewSubType == enumVecESRIShapefile && (wkbFlatten(nGeomType) == wkbUnknown || wkbFlatten(nGeomType) == wkbGeometryCollection);
    if(bIsMultigeom && nNewSubType == enumVecESRIShapefile)
    {
        wxString sErr(_("Input feature class has multi geometry but output doesn't support it!"));
        CPLString sFullErr(sErr.mb_str());
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();

    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("Projecting %s to %s"), pDSet->GetName().c_str(), sName.c_str()), -1, enumGISMessageTitle);

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

    if(!pNewSpaRef)
    {
        wxString sErr(_("Output spatial reference is not defined! OGR error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr );
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }

    OGRFeatureDefn *pDef = pDSet->GetDefiniton();
    if(!pDef)
    {
        wxString sErr(_("Error read dataset definition"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
        return false;
    }

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

    OGRCoordinateTransformation *poCT(NULL);
    if(pSrcSpaRef->IsSame(pNewSpaRef))
    {
        wxString sErr(_("The Spatial references are same! Nothing project"));
        CPLError( CE_Failure, CPLE_AppDefined, sErr.mb_str() );
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, -1, enumGISMessageErr);
        return false;
    }

    poCT = OGRCreateCoordinateTransformation( pSrcSpaRef, pNewSpaRef );
    if(!poCT)
    {
        wxString sErr(_("The unknown transformation! OGR Error: "));
        CPLString sFullErr(sErr.mb_str());
        sFullErr += CPLGetLastErrorMsg();
        CPLError( CE_Failure, CPLE_AppDefined, sFullErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wgMB2WX(sFullErr), -1, enumGISMessageErr);
        return false;
    }

    //get limits
    OGRPolygon* pRgn1 = NULL;
    OGRPolygon* pRgn2 = NULL;

    wxGISSpatialReferenceValidator GISSpaRefValidator;
    wxString sProjName;
    if(pNewSpaRef->IsProjected())
        sProjName = wgMB2WX(pNewSpaRef->GetAttrValue("PROJCS"));
    else
        sProjName = wgMB2WX(pNewSpaRef->GetAttrValue("GEOGCS"));
    if(GISSpaRefValidator.IsLimitsSet(sProjName))
    {
        LIMITS lims = GISSpaRefValidator.GetLimits(sProjName);
        if(lims.minx > lims.maxx)
        {
            OGRLinearRing ring1;
            ring1.addPoint(lims.minx,lims.miny);
            ring1.addPoint(lims.minx,lims.maxy);
            ring1.addPoint(180.0,lims.maxy);
            ring1.addPoint(180.0,lims.miny);
            ring1.closeRings();

            pRgn1 = new OGRPolygon();
            pRgn1->addRing(&ring1);
            pRgn1->flattenTo2D();

            OGRLinearRing ring2;
            ring2.addPoint(-180.0,lims.miny);
            ring2.addPoint(-180.0,lims.maxy);
            ring2.addPoint(lims.maxx,lims.maxy);
            ring2.addPoint(lims.maxx,lims.miny);
            ring2.closeRings();

            pRgn2 = new OGRPolygon();
            pRgn2->addRing(&ring2);
            pRgn2->flattenTo2D();
        }
        else
        {
            OGRLinearRing ring;
            ring.addPoint(lims.minx,lims.miny);
            ring.addPoint(lims.minx,lims.maxy);
            ring.addPoint(lims.maxx,lims.maxy);
            ring.addPoint(lims.maxx,lims.miny);
            ring.closeRings();

            pRgn1 = new OGRPolygon();
            pRgn1->addRing(&ring);
            pRgn1->flattenTo2D();
        }
        //WGS84
        OGRSpatialReference* pWGSSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);

        if(pRgn1 != NULL)
        {
            pRgn1->assignSpatialReference(pWGSSpaRef);
            pRgn1->segmentize(SEGSTEP);
        }
        if(pRgn2 != NULL)
        {
            pRgn2->assignSpatialReference(pWGSSpaRef);
            pRgn2->segmentize(SEGSTEP);
        }
        pWGSSpaRef->Dereference();

        if(!pSrcSpaRef->IsSame(pWGSSpaRef))
        {
            if(pRgn1->transformTo(pSrcSpaRef) != OGRERR_NONE)
                wxDELETE(pRgn1);
            if(pRgn2->transformTo(pSrcSpaRef) != OGRERR_NONE)
                wxDELETE(pRgn2);
        }
    }

    OGREnvelope *pRgnEnv1(NULL), *pRgnEnv2(NULL);
    if(pRgn1)
    {
        pRgnEnv1 = new OGREnvelope();
        pRgn1->getEnvelope(pRgnEnv1);
    }
    if(pRgn2)
    {
        pRgnEnv2 = new OGREnvelope();
        pRgn2->getEnvelope(pRgnEnv2);
    }


    //progress & messages
    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
       pTrackCancel->PutMessage(wxString(_("Start projecting")), -1, enumGISMessageNorm);
    }

    int nCounter(0);
    size_t nStep = pDSet->GetSize() < 10 ? 1 : pDSet->GetSize() / 10;
    if(pProgressor)
        pProgressor->SetRange(pDSet->GetSize());

    pDSet->Reset();
    OGRFeature* pFeature;
    while((pFeature = pDSet->Next()) != NULL)
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

        OGRGeometry *pGeom = pFeature->GetGeometryRef()->clone();
        if(pGeom)
        {
            if(pRgn1 == NULL && pRgn2 == NULL)
            {
                if(pGeom->transform( poCT ) != OGRERR_NONE)
                {
                    pTrackCancel->PutMessage(wxString::Format(_("Error project feature #%d"), pFeature->GetFID()), -1, enumGISMessageWarning);
                    OGRFeature::DestroyFeature(pFeature);
                    wxDELETE(pGeom);
                    continue;
                }
                else
                    pFeature->SetGeometryDirectly(pGeom);
            }
            else
            {
                OGRGeometry* pCutGeom = CheckRgnAndTransform(pGeom, pRgn1, pRgn2, pRgnEnv1, pRgnEnv2, poCT);
                if(pCutGeom)
                    pFeature->SetGeometryDirectly(pCutGeom);
                else
                {
                    pTrackCancel->PutMessage(wxString::Format(_("Error project feature #%d"), pFeature->GetFID()), -1, enumGISMessageWarning);
                    OGRFeature::DestroyFeature(pFeature);
                    wxDELETE(pGeom);
                    continue;
                }
            }
        }

        //////////////////////
        //wxFontEncoding OutputEncoding = pDstDataSet->GetEncoding();

        //OGRFeatureDefn *pFeatureDefn = pFeature->GetDefnRef();
        //if(!pFeatureDefn)
        //{
        //    OGRFeature::DestroyFeature(pFeature);
        //    wxDELETE(pGeom);
        //    continue;
        //}
        //
        //CPLErrorReset();
        //wxString sACText;

        //for(size_t i = 0; i < pFeatureDefn->GetFieldCount(); i++)
        //{
        //    OGRFieldDefn *pFieldDefn = pFeatureDefn->GetFieldDefn(i);
        //    if(pFieldDefn)
        //    {
        //        OGRFieldType nType = pFieldDefn->GetType();
        //        if(OFTString == nType)
        //        {
        //            wxString sFieldString;
        //            if(CPLIsUTF8(pFeature->GetFieldAsString(i), -1))
        //            {
        //                sFieldString = wxString::FromUTF8(pFeature->GetFieldAsString(i));
        //            }
        //            else
        //            {
        //                sFieldString = wxString(wgMB2WX(pFeature->GetFieldAsString(i)));
        //            }
        //            //wxCSConv inconv(OutputEncoding);
        //            //CPLString cplstr(pFeature->GetFieldAsString(i));
        //            //wxString sFieldString(cplstr.
        //            ////wxString sFieldString(wgMB2WX(pFeature->GetFieldAsString(i)));
        //            //wxCSConv outconv(OutputEncoding);
        //            //pFeature->SetField(i, sFieldString.mb_str(outconv));   

        //            if(OutputEncoding == wxFONTENCODING_DEFAULT)
        //                pFeature->SetField(i, wgWX2MB(sFieldString));
        //            else            
        //            {                
        //                wxCSConv outconv(OutputEncoding);
        //                pFeature->SetField(i, sFieldString.mb_str(outconv));
        //            }
        //            if(pDstDataSet->GetSubType() == enumVecDXF)
        //                sACText += sFieldString + wxT("\n");
        //        }
        //        //TODO: OFTStringList 
        //    }
        //}
        ////////////////

        OGRErr eErr = pNewDSet->CreateFeature(pFeature);
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

    wxDELETE(pRgnEnv1);
    wxDELETE(pRgnEnv2);
    wxDELETE(pRgn1);
    wxDELETE(pRgn2);

    return true;
}

OGRGeometry* CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT)
{
    if(!pFeatureGeom)
        return NULL;
    if(!poCT)
        return NULL;

    OGREnvelope FeatureEnv;
    OGRGeometry *pGeom(NULL), *pGeom1(NULL), *pGeom2(NULL);

    pFeatureGeom->getEnvelope(&FeatureEnv);

    if(pRgnEnv1 && FeatureEnv.Intersects(*pRgnEnv1))
    {
        if(pRgnEnv1->Contains(FeatureEnv) )
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else if(pFeatureGeom->Within(pRgn1))
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else
            pGeom1 = Intersection(pFeatureGeom, pRgn1, pRgnEnv1);
    }
    if(pRgnEnv2 && FeatureEnv.Intersects(*pRgnEnv2))
    {
        if(pRgnEnv2->Contains(FeatureEnv) )
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else if(pFeatureGeom->Within(pRgn2))
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else
            pGeom2= Intersection(pFeatureGeom, pRgn2, pRgnEnv2);
    }

    if(pGeom1 && !pGeom2)
        pGeom = pGeom1; 
    else if(pGeom2 && !pGeom1)
        pGeom = pGeom2;
    else if(pGeom1 && pGeom2)
    {
        OGRGeometryCollection* pGeometryCollection = new OGRGeometryCollection();
        pGeometryCollection->addGeometryDirectly(pGeom1);
        pGeometryCollection->addGeometryDirectly(pGeom2);
        pGeom = pGeometryCollection; 
    }


PROCEED:
    if(!pGeom)
        return NULL;

    if(pGeom->getSpatialReference() == NULL)
        pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());

    if(pGeom->transform( poCT ) != OGRERR_NONE)
    {
        wxDELETE(pGeom);
        return NULL;
    }  
    return pGeom;
}

OGRGeometry* Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv)
{
    if(!pFeatureGeom || !pRgnEnv)
        return NULL;

    OGREnvelope FeatureEnv;
    pFeatureGeom->getEnvelope(&FeatureEnv);

    OGRwkbGeometryType gType = wkbFlatten(pFeatureGeom->getGeometryType());
    switch(gType)
    {
    case wkbPoint:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        return NULL;
    case wkbLineString:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
#ifdef FAST_BISHOP_INTERSECTION
        {
            wxGISAlgorithm alg;
            return alg.FastLineIntersection(pFeatureGeom, pRgn);
        }
#else
        return pFeatureGeom->Intersection(pRgn);
#endif
    case wkbPolygon:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
#ifdef FAST_BISHOP_INTERSECTION
        {
            wxGISAlgorithm alg;
            return alg.FastPolyIntersection(pFeatureGeom, pRgn);
        }
#else
        return pFeatureGeom->Intersection(pRgn);
#endif
    case wkbLinearRing:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        return pFeatureGeom->Intersection(pRgn);
    case wkbMultiPoint:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRMultiPoint();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbMultiLineString:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRMultiLineString();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbMultiPolygon:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRMultiPolygon();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbGeometryCollection:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRGeometryCollection();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbUnknown:
    case wkbNone:
        break;
    }
    return NULL;
}
