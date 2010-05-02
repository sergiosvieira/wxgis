/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
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

bool CopyRows(wxGISFeatureDataset* pSrcDataSet, wxGISFeatureDataset* pDstDataSet, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
{
    wxStopWatch sw;
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
       pTrackCancel->PutMessage(wxString::Format(_("Start CopyRows from '%s' to '%s'"), pSrcDataSet->GetPath().c_str(), pDstDataSet->GetPath().c_str()), 1000, enumGISMessageInfo);
    }
    int nCounter(0);
    size_t nStep = pSrcDataSet->GetSize() < 10 ? 1 : pSrcDataSet->GetSize() / 10;
    if(pProgressor)
    {
        pProgressor->SetRange(pSrcDataSet->GetSize());
        pProgressor->Show(true);
    }
    pSrcDataSet->Reset();
    OGRFeature* pFeature;
    while((pFeature = pSrcDataSet->Next()) != NULL)
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            OGRFeature::DestroyFeature(pFeature);
            if(pProgressor)
                pProgressor->Show(false);
            return false;
        }
        if( !bSame )
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
            if(pTrackCancel)
            {
                const char* err = CPLGetLastErrorMsg();
                wxString sErr = wxString::Format(_("Error while adding OGRFeature! OGR error: %s"), wgMB2WX(err));
                wxLogError(sErr);
                pTrackCancel->PutMessage(sErr, 1000, enumGISMessageErr);
            }
        }
        nCounter++;
        if(pProgressor && nCounter % nStep == 0)
            pProgressor->SetValue(nCounter);
    }

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);

    if(pProgressor)
        pProgressor->Show(false);
    if(pTrackCancel)
    {
        wxString sMsg;     
        double fSec = sw.Time() / 1000;
        if(fSec < 60)
        {
            double fMin = fSec / 60;
            fSec -= fMin * 60;
            sMsg = wxString::Format(_("The execution tooks %ld min %lds"), (long)fMin, (long)fSec);
        }
        else
            sMsg = wxString::Format(_("The execution tooks %lds"), (long)fSec);
//        wxString sMsg = wxString::Format(_("The execution tooks %ldms"), sw.Time());
        pTrackCancel->PutMessage(sMsg, 1000, enumGISMessageInfo);
    }
    return true;
}