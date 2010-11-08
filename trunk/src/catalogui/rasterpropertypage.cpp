/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRasterPropertyPage class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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

#include "wxgis/catalogui/rasterpropertypage.h"

#include "gdal_rat.h"

//#define PREF_THUMBNAIL_HEIGHT       64
//WX_PG_IMPLEMENT_PROPERTY_CLASS(wxTextAndButtonProperty, wxStringProperty, wxString, const wxString&, TextCtrl)
//
//wxTextAndButtonProperty::wxTextAndButtonProperty( const wxString& label, const wxString& name, const wxString& value, wxWindowID buttonid, const wxString& buttonCaption ) : wxStringProperty(label, name, value), m_button(NULL)
//{
//    m_buttonid = buttonid;
//    m_buttonCaption = buttonCaption;
//
//}
//
//wxTextAndButtonProperty::~wxTextAndButtonProperty ()
//{
//    wxDELETE(m_button);
//}
//
//wxSize wxTextAndButtonProperty::OnMeasureImage( int item ) const
//{
//    return wxSize(0, 0);
//    //if ( item == -1 )
//        //return wxPG_DEFAULT_IMAGE_SIZE;
//
//    //return wxSize(PREF_THUMBNAIL_HEIGHT,PREF_THUMBNAIL_HEIGHT);
//}
//
//void wxTextAndButtonProperty::OnCustomPaint( wxDC& dc, const wxRect& rect, wxPGPaintData& pd )
//{
//    if(!m_button)
//    {
//        wxWindow* pWnd = GetGrid()->GetPanel();
//        m_button = new wxButton(pWnd, m_buttonid, m_buttonCaption);
//        m_button->Show(true);
//    }
//
//    //if ( index >= 0 )
//    //{
//    //    LoadThumbnails(index);
//
//    // Is this a measure item call?
//    //if ( rect.x < 0 )
//    //{
//    //    //// Variable height
//    //    ////pd.m_drawnHeight = PREF_THUMBNAIL_HEIGHT;
//    //    //wxBitmap* pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail2;
//    //    //if ( pBitmap )
//    //    //    pd.m_drawnHeight = pBitmap->GetHeight();
//    //    //else
//    //    //    pd.m_drawnHeight = 16;
//    //    return;
//    //}
//
//    //wxPoint pt = rect.GetRightTop();
//    //wxSize sz = m_button->GetSize();
//    //pt.x -= sz.GetWidth();
//    //m_button->Move(pt.x, pt.y);
//
//    //    // Draw the thumbnail
//
//    //    wxBitmap* pBitmap;
//
//    //    if ( pd.m_choiceItem >= 0 )
//    //        pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail2;
//    //    else
//    //        pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail1;
//
//    //    if ( pBitmap )
//    //    {
//    //        dc.DrawBitmap ( *pBitmap, rect.x, rect.y, FALSE );
//
//    //        // Tell the caller how wide we drew.
//    //        pd.m_drawnWidth = pBitmap->GetWidth();
//
//    //        return;
//    //    }
//    //}
//
//    //// No valid file - just draw a white box.
//    //dc.SetBrush ( *wxWHITE_BRUSH );
//    //dc.DrawRectangle ( rect );
//}

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterPropertyPage, wxPanel)

wxGISRasterPropertyPage::wxGISRasterPropertyPage(void) : m_pDataset(NULL)
{
}

wxGISRasterPropertyPage::wxGISRasterPropertyPage(wxGISDataset* pDataset, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Create(pDataset, parent, id, pos, size, style, name);
}

wxGISRasterPropertyPage::~wxGISRasterPropertyPage()
{
    wsDELETE(m_pDataset);
}

bool wxGISRasterPropertyPage::Create(wxGISDataset* pDataset, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    wxPanel::Create(parent, id, pos, size, style, name);

    m_pDataset = dynamic_cast<wxGISRasterDataset*>(pDataset);
    if(!m_pDataset)
        return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_THEME_BORDER | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    wxString sTmp;
    //fill propertygrid
    wxPGId pid = AppendProperty( new wxPropertyCategory(_("Data Source")) );
    AppendProperty( new wxStringProperty(_("Raster"), wxPG_LABEL, m_pDataset->GetName()) );  
    //folder
    wxFileName FName(m_pDataset->GetPath());
    AppendProperty( new wxStringProperty(_("Folder"), wxPG_LABEL, FName.GetPath()) );  
    //file list
    GDALDataset* poGDALDataset = m_pDataset->GetMainRaster();
    if(!poGDALDataset)
        poGDALDataset = m_pDataset->GetRaster();
    if(poGDALDataset)
    {
        char** papszFileList = poGDALDataset->GetFileList();
        if( CSLCount(papszFileList) < 2 )
        {
            AppendProperty( new wxStringProperty(_("Files"), wxPG_LABEL, _("none associated")) );  
        }
        else
        {
            wxPGId pfilesid = AppendProperty(pid, new wxPropertyCategory(_("Files")) );  
            for(int i = 1; papszFileList[i] != NULL; i++ )
		    {
			    wxString sFileName = wgMB2WX(papszFileList[i]);
                wxFileName FName(sFileName);
                AppendProperty(pfilesid, new wxStringProperty(_("File"), wxPG_LABEL, FName.GetFullName()) );  
		    }
        }
        CSLDestroy( papszFileList );

        GDALDriver* pDrv = poGDALDataset->GetDriver();
	    sTmp = sTmp.Format(wxT("%s(%s)"), wgMB2WX(pDrv->GetMetadataItem( GDAL_DMD_LONGNAME )), wgMB2WX(pDrv->GetDescription()) );
        AppendProperty(pid, new wxStringProperty(_("Driver"), wxPG_LABEL, sTmp) );  
    }


    AppendProperty( new wxPropertyCategory(_("Raster Information")) );
    //Columns and Rows
    int nW = m_pDataset->GetWidth();
    int nH = m_pDataset->GetHeight();
    sTmp = sTmp.Format(wxT("%d, %d"), nW, nH);
    AppendProperty( new wxStringProperty(_("Columns and Rows"), wxPG_LABEL, sTmp) ); 

    //Number of bands
    AppendProperty( new wxIntProperty(_("Number of bands"), wxPG_LABEL, m_pDataset->GetBandCount()) ); 
    if(poGDALDataset)
    {
    //Cell size (X, Y)
        double adfGeoTransform[6] = {0};
        if( poGDALDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
        {
            sTmp = sTmp.Format(wxT("%.6g, %.6g"), fabs(adfGeoTransform[1]), fabs(adfGeoTransform[5]) );
            AppendProperty( new wxStringProperty(_("Cell size (X, Y)"), wxPG_LABEL, sTmp) ); 
            AppendProperty( new wxStringProperty(_("GeoTransform"), wxPG_LABEL, wxString::Format(wxT("%.16g, %.16g, %.16g | %.16g, %.16g, %.16g"), adfGeoTransform[0], adfGeoTransform[1], adfGeoTransform[2], adfGeoTransform[3], adfGeoTransform[4], adfGeoTransform[5])));
        }
    }
    //Pyramids
    bool bHasOvr = m_pDataset->HasOverviews();
    if(bHasOvr)
        AppendProperty( new wxStringProperty(_("Overviews"), wxPG_LABEL, _("Present")) ); 
    else
    {
        wxPGId pyrprop = AppendProperty( new wxStringProperty(_("Overviews"), wxPG_LABEL, _("Absent")) );
        m_pg->SetPropertyEditor(pyrprop, wxPG_EDITOR(TextCtrlAndButton));
    }
    //Statistics
    wxPGId pstatprop = AppendProperty( new wxStringProperty(_("Statistics"), wxPG_LABEL, _("Absent")) ); 
    m_pg->SetPropertyEditor(pstatprop, wxPG_EDITOR(TextCtrlAndButton));
    //Histogram
    //wxPGId phistprop = AppendProperty( new wxTextAndButtonProperty(_("Histogram"), wxPG_LABEL, _("Absent")) ); 
    ////m_pg->SetPropertyEditor(phistprop, wxPG_EDITOR(TextCtrlAndButton));

    AppendProperty( new wxPropertyCategory(_("Extent")));
    const OGREnvelope* pEnv = m_pDataset->GetEnvelope();
    AppendProperty( new wxFloatProperty(_("Top"), wxPG_LABEL, pEnv->MaxY));
    AppendProperty( new wxFloatProperty(_("Left"), wxPG_LABEL, pEnv->MinX));
    AppendProperty( new wxFloatProperty(_("Right"), wxPG_LABEL, pEnv->MaxX));
    AppendProperty( new wxFloatProperty(_("Bottom"), wxPG_LABEL, pEnv->MinY));

    if(poGDALDataset)
    {
        
        char** papszMetadata = poGDALDataset->GetMetadata();
        if( CSLCount(papszMetadata) > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("Metadata")) );
            for(int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTmp = wgMB2WX(papszMetadata[i]);
                AppendMetadataProperty(sTmp);
            }
        }

	    /* -------------------------------------------------------------------- */
	    /*      Report "IMAGE_STRUCTURE" metadata.                              */
	    /* -------------------------------------------------------------------- */
        papszMetadata = poGDALDataset->GetMetadata("IMAGE_STRUCTURE");
        if( CSLCount(papszMetadata) > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("Image Structure Metadata")) );
            for(int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTmp = wgMB2WX(papszMetadata[i]);
                AppendMetadataProperty(sTmp);
            }
        }

	    /* -------------------------------------------------------------------- */
	    /*      Report subdatasets.                                             */
	    /* -------------------------------------------------------------------- */
        papszMetadata = poGDALDataset->GetMetadata("SUBDATASETS");
        if( CSLCount(papszMetadata) > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("Subdatasets")) );
            for(int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTmp = wgMB2WX(papszMetadata[i]);
                AppendMetadataProperty(sTmp);
            }
        }

	    /* -------------------------------------------------------------------- */
	    /*      Report geolocation.                                             */
	    /* -------------------------------------------------------------------- */
        papszMetadata = poGDALDataset->GetMetadata("GEOLOCATION");
        if( CSLCount(papszMetadata) > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("Geolocation")) );
            for(int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTmp = wgMB2WX(papszMetadata[i]);
                AppendMetadataProperty(sTmp);
            }
        }

	    /* -------------------------------------------------------------------- */
	    /*      Report RPCs                                                     */
	    /* -------------------------------------------------------------------- */
        papszMetadata = poGDALDataset->GetMetadata("RPC");
        if( CSLCount(papszMetadata) > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("RPC Metadata")) );
            for(int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTmp = wgMB2WX(papszMetadata[i]);
                AppendMetadataProperty(sTmp);
            }
        }

        /* -------------------------------------------------------------------- */
        /*      Report GCPs.                                                    */
        /* -------------------------------------------------------------------- */
        if( poGDALDataset->GetGCPCount() > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("GCP data")) );
            OGRSpatialReference* poSRS = NULL;
            char* pszGCPProjection = (char*)poGDALDataset->GetGCPProjection();
            if (pszGCPProjection)
            {
                poSRS = new OGRSpatialReference();
                if( poSRS->importFromWkt(&pszGCPProjection ) == CE_None )
                {
                    OSRDestroySpatialReference(poSRS);
                    poSRS = NULL;
                }
            }
            else
            {
                poSRS = m_pDataset->GetSpatialReference();
            }
            if(poSRS)
            {
                const char *pszName;
                if(poSRS->IsProjected())
                {
                    pszName = poSRS->GetAttrValue("PROJCS");
                    AppendProperty( new wxStringProperty(_("GCP Projection"), wxPG_LABEL, wgMB2WX(pszName)) );
                }
                else if(poSRS->IsGeographic())
                {
                    pszName = poSRS->GetAttrValue("GEOGCS");
                    AppendProperty( new wxStringProperty(_("GCP Projection"), wxPG_LABEL, wgMB2WX(pszName)) );
                }
                else
                    AppendProperty( new wxStringProperty(_("GCP Projection"), wxPG_LABEL, _("Undefined")) );

                OSRDestroySpatialReference(poSRS);
            }

            for(size_t i = 0; i < poGDALDataset->GetGCPCount(); i++ )
            {
                const GDAL_GCP *psGCP = poGDALDataset->GetGCPs( ) + i;
                AppendProperty( new wxStringProperty(wxString::Format(wxT("GCP[%03d]"), i + 1), wxPG_LABEL, wxString::Format(_("Id='%s', Info='%s', (%.6g,%.6g) -> (%.6g,%.6g,%.6g)"), wgMB2WX(psGCP->pszId), wgMB2WX(psGCP->pszInfo), psGCP->dfGCPPixel, psGCP->dfGCPLine, psGCP->dfGCPX, psGCP->dfGCPY, psGCP->dfGCPZ)) );
            }
        }

        /* ==================================================================== */
        /*      Loop over bands.                                                */
        /* ==================================================================== */        
     
        wxPGId pstatid = AppendProperty( new wxPropertyCategory(_("Statistics details")) );
               
        for(int nBand = 0; nBand < poGDALDataset->GetRasterCount(); nBand++ )
        {
            wxPGId pbandid = AppendProperty(pstatid, new wxPropertyCategory(wxString::Format(_("Band %d"), nBand + 1)) );

            double      adfCMinMax[2], dfNoData;
            int         bGotMin, bGotMax, bGotNodata, bSuccess;
            int         nBlockXSize, nBlockYSize, nMaskFlags;
            double      dfMin, dfMax, dfMean, dfStdDev;
            GDALColorTable*	hTable;
            CPLErr      eErr;

            GDALRasterBand* pBand = poGDALDataset->GetRasterBand(nBand + 1);

		    pBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
            AppendProperty( new wxStringProperty(_("Block"), wxPG_LABEL, wxString::Format(wxT("%d x %d"), nBlockXSize, nBlockYSize)));
            AppendProperty( new wxStringProperty(_("Type"), wxPG_LABEL, wgMB2WX(GDALGetDataTypeName(pBand->GetRasterDataType())) ));
            AppendProperty( new wxStringProperty(_("ColorInterp"), wxPG_LABEL, wgMB2WX(GDALGetColorInterpretationName(pBand->GetColorInterpretation())) ));

		    wxString sDescription = wgMB2WX(pBand->GetDescription());
            if(!sDescription.IsEmpty())
                AppendProperty( new wxStringProperty(_("Description"), wxPG_LABEL, sDescription ));

            //Statistics
            if(pBand->GetStatistics(false, false, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
            {
                AppendProperty( new wxFloatProperty(_("Min"), wxPG_LABEL, dfMin ));
                AppendProperty( new wxFloatProperty(_("Max"), wxPG_LABEL, dfMax ));
                AppendProperty( new wxFloatProperty(_("Mean"), wxPG_LABEL, dfMean ));
                AppendProperty( new wxFloatProperty(_("Std Dev"), wxPG_LABEL, dfStdDev ));
            }

            ////Histogram
            //int nBucketCount, *panHistogram = NULL;
            //if(pBand->GetDefaultHistogram(&dfMin, &dfMax, &nBucketCount, &panHistogram, false, GDALTermProgress, NULL ) == CE_None )
            //{
            //    CPLFree( panHistogram );
            //}
            //else

            //Checksum
            //   wxLogDebug( wxT("  Checksum=%d"), GDALChecksumImage(pBand, 0, 0, nXSize, nYSize));

		    dfNoData = pBand->GetNoDataValue(&bGotNodata );
            if( bGotNodata )
            {
                AppendProperty( new wxFloatProperty(_("NoData Value"), wxPG_LABEL, dfNoData ));
            }

            //Band Overviews
		    if( pBand->GetOverviewCount() > 0 )
            {
                wxPGId povrid = AppendProperty(pbandid, new wxStringProperty(_("Overviews"), wxPG_LABEL, pBand->HasArbitraryOverviews() == true ? _("arbitrary") : wxEmptyString ) );
                
                for(int iOverview = 0; iOverview < pBand->GetOverviewCount(); iOverview++ )
                {
                    const char *pszResampling = NULL;
                    
                    GDALRasterBand*	pOverview = pBand->GetOverview( iOverview );
                    wxString sOvrDesc = wxString::Format(wxT("%d x %d"), pOverview->GetXSize(), pOverview->GetYSize());
                    pszResampling = pOverview->GetMetadataItem("RESAMPLING", "" );
                    if( pszResampling != NULL && EQUALN(pszResampling, "AVERAGE_BIT2", 12) )
                        sOvrDesc += wxT( ", *" );
                    else
                        sOvrDesc += wgMB2WX(pszResampling);

                    AppendProperty(povrid, new wxStringProperty(wxString::Format(_("Overview %d"), iOverview + 1), wxPG_LABEL, sOvrDesc ));
                }
		    }	
	
            nMaskFlags = pBand->GetMaskFlags();
            if( (nMaskFlags & (GMF_NODATA|GMF_ALL_VALID)) == 0 )
            {
                GDALRasterBand* pMaskBand = pBand->GetMaskBand() ;
                
                wxString sMaskStr;
                if( nMaskFlags & GMF_PER_DATASET )
                    sMaskStr += wxT("PER_DATASET" );
                if( nMaskFlags & GMF_ALPHA )
                {
                    if(sMaskStr.Len() > 0)
                        sMaskStr += wxT(", ");
                    sMaskStr += wxT("ALPHA" );
                }
                if( nMaskFlags & GMF_NODATA )
                {
                    if(sMaskStr.Len() > 0)
                        sMaskStr += wxT(", ");
                    sMaskStr += wxT("NODATA" );
                }
                if( nMaskFlags & GMF_ALL_VALID )
                {
                    if(sMaskStr.Len() > 0)
                        sMaskStr += wxT(", ");
                    sMaskStr += wxT("ALL_VALID" );
                }

                wxPGId pmfid = AppendProperty(pbandid, new wxStringProperty(_("Mask Flags"), wxPG_LABEL, sMaskStr ));

                if( pMaskBand != NULL && pMaskBand->GetOverviewCount() > 0 )
                {
                    wxPGId pombid = AppendProperty(pmfid, new wxPropertyCategory(_("Overviews of mask band")) );

                    for( int nOverview = 0; nOverview < pMaskBand->GetOverviewCount(); nOverview++ )
                    {
                        GDALRasterBand*	pOverview = pMaskBand->GetOverview( nOverview );
                        AppendProperty(pombid, new wxStringProperty(wxString::Format(_("Overview %d"), nOverview + 1), wxPG_LABEL, wxString::Format(wxT("%dx%d"), pOverview->GetXSize(), pOverview->GetYSize() ))); 
                    }
                }
            }

            wxString sUnitType = wgMB2WX( pBand->GetUnitType() );
		    if( sUnitType.Len() > 0 )
            {
                AppendProperty(pbandid, new wxStringProperty(_("Unit Type"), wxPG_LABEL, sUnitType));
            }

		    char **papszCategories = pBand->GetCategoryNames();
            if( papszCategories != NULL )
            {
                wxPGId pcatid = AppendProperty(pbandid, new wxIntProperty(_("Categories"), wxPG_LABEL, CSLCount(papszCategories) ) );
                for(int  i = 0; papszCategories[i] != NULL; i++ )
                    AppendProperty(pcatid, new wxStringProperty(wxString::Format(wxT("%3d"), i), wxPG_LABEL, wgMB2WX(papszCategories[i]) ) );
            }

            double fScale = pBand->GetScale( &bSuccess );
            if(bSuccess && fScale != 1.0)
                AppendProperty(pbandid, new wxFloatProperty(_("Scale"), wxPG_LABEL, fScale ) );
            double fOffset = pBand->GetOffset( &bSuccess );
            if(bSuccess && fOffset != 0.0)
                AppendProperty(pbandid, new wxFloatProperty(_("Offset"), wxPG_LABEL, fOffset ) );

		    papszMetadata = pBand->GetMetadata();
            if( CSLCount(papszMetadata) > 0 )
            {
                AppendProperty(pbandid, new wxPropertyCategory(_("Metadata") ) );
                for( int i = 0; papszMetadata[i] != NULL; i++ )
                {
                    AppendMetadataProperty( wgMB2WX(papszMetadata[i]) );
                }
            }

            papszMetadata = pBand->GetMetadata( "IMAGE_STRUCTURE" );
            if( CSLCount(papszMetadata) > 0 )
            {
                AppendProperty(pbandid, new wxPropertyCategory(_("Image Structure Metadata") ) );
                for( int i = 0; papszMetadata[i] != NULL; i++ )
                {
                    AppendMetadataProperty( wgMB2WX(papszMetadata[i]) );
                }
            }

		    if( pBand->GetColorInterpretation() == GCI_PaletteIndex && (hTable = pBand->GetColorTable()) != NULL )
            {
                wxPGId pcpid = AppendProperty(pbandid, new wxStringProperty(_("Color Table"), wxPG_LABEL, wxString::Format(_("%s with %d entries"), wgMB2WX(GDALGetPaletteInterpretationName(hTable->GetPaletteInterpretation())), hTable->GetColorEntryCount()) ));

			    for(int	i = 0; i < hTable->GetColorEntryCount(); i++ )
                {
                    GDALColorEntry	sEntry;
				    hTable->GetColorEntryAsRGB(i, &sEntry );

                    AppendProperty(pcpid, new wxStringProperty(wxString::Format(_("%3d"), i), wxPG_LABEL, wxString::Format(_("%d,%d,%d,%d"), sEntry.c1, sEntry.c2, sEntry.c3, sEntry.c4 )));
                }
                m_pg->Collapse(pcpid);
            }

		    if( pBand->GetDefaultRAT() != NULL )
            {
			    const GDALRasterAttributeTable* pRAT = (const GDALRasterAttributeTable*)pBand->GetDefaultRAT();
                
                char *pszXMLText = CPLSerializeXMLTree( pRAT->Serialize() );
                m_pg->AppendIn(pbandid, new wxLongStringProperty(_("Raster Attribute Table"), wxPG_LABEL, wgMB2WX(pszXMLText)));
            }
        }
    }

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

wxPGId wxGISRasterPropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    pNewProp->SetFlag(wxPG_PROP_READONLY);
    return pNewProp;
}

wxPGId wxGISRasterPropertyPage::AppendProperty(wxPGId pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    pNewProp->SetFlag(wxPG_PROP_READONLY);
    return pNewProp;
}

wxPGId wxGISRasterPropertyPage::AppendMetadataProperty(wxString sMeta)
{
    int nPos = sMeta.Find('=');
    if(nPos == wxNOT_FOUND)
        return AppendProperty( new wxStringProperty(_("Item"), wxPG_LABEL, sMeta) );
    else
    {
        wxString sName = sMeta.Left(nPos);
        wxString sVal = sMeta.Right(sMeta.Len() - nPos - 1);
        //clean
        wxString sCleanVal;
        for(size_t i = 0; i < sVal.Len(); i++)
        {
            char c = sVal[i];
            if(sVal[i] > 31 && sVal[i] != 127)
                sCleanVal += sVal[i];
        }
        if(sCleanVal.Len() > 500)
            return m_pg->Append( new wxLongStringProperty(sName, wxPG_LABEL, sCleanVal) );//??
        else
            return AppendProperty( new wxStringProperty(sName, wxPG_LABEL, sCleanVal) );
    }
}


	////for(int nBand = 0; nBand < m_poDataset->GetRasterCount(); nBand++ )
 ////   {
 ////       double      dfMin, dfMax, adfCMinMax[2], dfNoData;
 ////       int         bGotMin, bGotMax, bGotNodata, bSuccess;
 ////       int         nBlockXSize, nBlockYSize, nMaskFlags;
 ////       double      dfMean, dfStdDev;
 ////       GDALColorTable*	hTable;
 ////       CPLErr      eErr;

	////	GDALRasterBand* pBand = m_poDataset->GetRasterBand(nBand + 1);

	////	pBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
	////	wxLogDebug( wxT( "Band %d Block=%dx%d Type=%s, ColorInterp=%s"), nBand + 1, nBlockXSize, nBlockYSize, wgMB2WX(GDALGetDataTypeName(pBand->GetRasterDataType())), wgMB2WX(GDALGetColorInterpretationName(pBand->GetColorInterpretation())));

	////	wxString sDescription = wgMB2WX(pBand->GetDescription());
 ////       wxLogDebug( wxT( "  Description = %s"), sDescription.c_str());

	////	dfMin = pBand->GetMinimum(&bGotMin);
	////	dfMax = pBand->GetMaximum(&bGotMax);
 ////       if( bGotMin || bGotMax )
 ////       {
 ////           if( bGotMin )
 ////               wxLogDebug( wxT( "Min=%.3f "), dfMin );
 ////           if( bGotMax )
 ////               wxLogDebug( wxT( "Max=%.3f "), dfMax );

	////		pBand->ComputeRasterMinMax(FALSE, adfCMinMax );
 ////           wxLogDebug( wxT("  Computed Min/Max=%.3f,%.3f"), adfCMinMax[0], adfCMinMax[1] );
 ////       }

 ////       eErr = pBand->GetStatistics(TRUE, TRUE, &dfMin, &dfMax, &dfMean, &dfStdDev );
 ////       if( eErr == CE_None )
 ////       {
 ////           wxLogDebug( wxT("  Minimum=%.3f, Maximum=%.3f, Mean=%.3f, StdDev=%.3f"), dfMin, dfMax, dfMean, dfStdDev );
 ////       }

 ////       //if( bReportHistograms )
 ////       //{
 ////       //    int nBucketCount, *panHistogram = NULL;

 ////       //    eErr = GDALGetDefaultHistogram( hBand, &dfMin, &dfMax,
 ////       //                                    &nBucketCount, &panHistogram,
 ////       //                                    TRUE, GDALTermProgress, NULL );
 ////       //    if( eErr == CE_None )
 ////       //    {
 ////       //        int iBucket;

 ////       //        printf( "  %d buckets from %g to %g:\n  ",
 ////       //                nBucketCount, dfMin, dfMax );
 ////       //        for( iBucket = 0; iBucket < nBucketCount; iBucket++ )
 ////       //            printf( "%d ", panHistogram[iBucket] );
 ////       //        printf( "\n" );
 ////       //        CPLFree( panHistogram );
 ////       //    }
 ////       //}

 ////       //wxLogDebug( wxT("  Checksum=%d"), GDALChecksumImage(pBand, 0, 0, nXSize, nYSize));

	////	dfNoData = pBand->GetNoDataValue(&bGotNodata );
 ////       if( bGotNodata )
 ////       {
 ////           wxLogDebug( wxT("  NoData Value=%.18g"), dfNoData );
 ////       }

	////	if( pBand->GetOverviewCount() > 0 )
 ////       {
	////		wxString sOut(wxT("  Overviews: " ));
 ////           for(int iOverview = 0; iOverview < pBand->GetOverviewCount(); iOverview++ )
 ////           {
 ////               const char *pszResampling = NULL;

 ////               if( iOverview != 0 )
 ////                   sOut += wxT( ", " );

	////			GDALRasterBand*	pOverview = pBand->GetOverview( iOverview );
	////			sOut += wxString::Format(wxT("%dx%d"), pOverview->GetXSize(), pOverview->GetYSize());

	////			pszResampling = pOverview->GetMetadataItem("RESAMPLING", "" );
 ////               if( pszResampling != NULL && EQUALN(pszResampling, "AVERAGE_BIT2", 12) )
 ////                   sOut += wxT( "*" );
 ////           }
 ////           wxLogDebug(sOut);

 ////  //         sOut = wxT( "  Overviews checksum: " );
 ////  //         for(int iOverview = 0; iOverview < pBand->GetOverviewCount(); iOverview++ )
	////		//{
 ////  //             if( iOverview != 0 )
 ////  //                 sOut += wxT( ", " );

	////		//	GDALRasterBand*	pOverview = pBand->GetOverview( iOverview );
	////		//	sOut += GDALChecksumImage(pOverview, 0, 0, pOverview->GetXSize(), pOverview->GetYSize());
 ////  //         }
 ////  //         wxLogDebug(sOut);
	////	}

	//	if( pBand->HasArbitraryOverviews() )
 //       {
 //          wxLogDebug( wxT("  Overviews: arbitrary" ));
 //       }

	//	nMaskFlags = pBand->GetMaskFlags();
 //       if( (nMaskFlags & (GMF_NODATA|GMF_ALL_VALID)) == 0 )
 //       {
	//		GDALRasterBand* pMaskBand = pBand->GetMaskBand() ;

 //           wxLogDebug( wxT("  Mask Flags: " ));
 //           if( nMaskFlags & GMF_PER_DATASET )
 //               wxLogDebug( wxT("PER_DATASET " ));
 //           if( nMaskFlags & GMF_ALPHA )
 //               wxLogDebug( wxT("ALPHA " ));
 //           if( nMaskFlags & GMF_NODATA )
 //               wxLogDebug( wxT("NODATA " ));
 //           if( nMaskFlags & GMF_ALL_VALID )
 //              wxLogDebug( wxT("ALL_VALID " ));

	//		if( pMaskBand != NULL && pMaskBand->GetOverviewCount() > 0 )
 //           {
 //               int		iOverview;

 //               wxLogDebug( wxT("  Overviews of mask band: " ));
 //               for( int nOverview = 0; nOverview < pMaskBand->GetOverviewCount(); nOverview++ )
 //               {
 //                   GDALRasterBand*	pOverview;

 //                   if( nOverview != 0 )
 //                       wxLogDebug( wxT(", " ));

	//				pOverview = pMaskBand->GetOverview( nOverview );
 //                   wxLogDebug( wxT("%dx%d"), pOverview->GetXSize(), pOverview->GetYSize());
 //               }
 //           }
 //       }

	//	if( strlen(pBand->GetUnitType()) > 0 )
 //       {
	//		wxLogDebug( wxT("  Unit Type: %s"),wgMB2WX( pBand->GetUnitType()) );
 //       }

	//	char **papszCategories = pBand->GetCategoryNames();
 //       if( papszCategories != NULL )
 //       {
 //           int i;
 //           wxLogDebug( wxT("  Categories:" ));
 //           for( i = 0; papszCategories[i] != NULL; i++ )
 //               wxLogDebug( wxT("    %3d: %s"), i, wgMB2WX(papszCategories[i]) );
 //       }

	//	if( pBand->GetScale( &bSuccess ) != 1.0 || pBand->GetOffset( &bSuccess ) != 0.0 )
 //           wxLogDebug( wxT("  Offset: %.15g,   Scale:%.15g"), pBand->GetOffset( &bSuccess ), pBand->GetScale( &bSuccess ) );

	//	papszMetadata = pBand->GetMetadata();
 //       if( CSLCount(papszMetadata) > 0 )
 //       {
 //           wxLogDebug( wxT("  Metadata:" ));
 //           for( int i = 0; papszMetadata[i] != NULL; i++ )
 //           {
 //               wxLogDebug( wxT("    %s"), wgMB2WX(papszMetadata[i]) );
 //           }
 //       }

 //       papszMetadata = pBand->GetMetadata( "IMAGE_STRUCTURE" );
 //       if( CSLCount(papszMetadata) > 0 )
 //       {
 //           wxLogDebug( wxT("  Image Structure Metadata:" ));
 //           for( int i = 0; papszMetadata[i] != NULL; i++ )
 //           {
 //               wxLogDebug( wxT("    %s"), wgMB2WX(papszMetadata[i]));
 //           }
 //       }

	//	if( pBand->GetColorInterpretation() == GCI_PaletteIndex && (hTable = pBand->GetColorTable()) != NULL )
 //       {
 //           int			i;

	//		wxLogDebug( wxT("  Color Table (%s with %d entries)"), wgMB2WX(GDALGetPaletteInterpretationName(hTable->GetPaletteInterpretation())), hTable->GetColorEntryCount() );

	//		for( i = 0; i < hTable->GetColorEntryCount(); i++ )
 //           {
 //               GDALColorEntry	sEntry;

	//			hTable->GetColorEntryAsRGB(i, &sEntry );
 //               wxLogDebug( wxT("  %3d: %d,%d,%d,%d"), i, sEntry.c1, sEntry.c2, sEntry.c3, sEntry.c4 );
 //           }
 //       }

	//	if( pBand->GetDefaultRAT() != NULL )
 //       {
	//		const GDALRasterAttributeTable* pRAT = (const GDALRasterAttributeTable*)pBand->GetDefaultRAT();
	//		GDALRasterAttributeTable* pRATn = (GDALRasterAttributeTable*)pRAT;
	//		pRATn->DumpReadable();
 //       }
	//}

 //   //CPLCleanupTLS();
