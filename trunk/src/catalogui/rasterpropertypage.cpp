/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRasterPropertyPage class.
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

#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/geoprocessingui/geoprocessingui.h"

#include "gdal_rat.h"

#define STAT_TXT _("Statistics")
#define OVR_TXT _("Overviews")

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterPropertyPage, wxPanel)

BEGIN_EVENT_TABLE(wxGISRasterPropertyPage, wxPanel)
    EVT_BUTTON( ID_PPCTRL, wxGISRasterPropertyPage::OnPropertyGridButtonClick )
END_EVENT_TABLE()

wxGISRasterPropertyPage::wxGISRasterPropertyPage(void)/* : m_pDataset(NULL)*/
{
}

wxGISRasterPropertyPage::wxGISRasterPropertyPage(wxGxRasterDataset* pGxDataset, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)/* : m_pDataset(NULL)*/
{
    Create(pGxDataset, pCatalog, parent, id, pos, size, style, name);
}

wxGISRasterPropertyPage::~wxGISRasterPropertyPage()
{
    //wsDELETE(m_pDataset);
}

bool wxGISRasterPropertyPage::Create(wxGxRasterDataset* pGxDataset, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    wxPanel::Create(parent, id, pos, size, style, name);

    m_pGxDataset = pGxDataset;
    m_pCatalog = pCatalog;

    m_pDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(m_pGxDataset->GetDataset(true));
    if(!m_pDataset)
        return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_THEME_BORDER | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    FillGrid();

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

void wxGISRasterPropertyPage::FillGrid(void)
{
    //reset grid
    m_pg->Clear();

    m_pDataset->Open(true);

    wxString sTmp;
    //fill propertygrid
    wxPGId pid = AppendProperty( new wxPropertyCategory(_("Data Source")) );
    AppendProperty( new wxStringProperty(_("Raster"), wxPG_LABEL, m_pGxDataset->GetName()) );  

    CPLString soPath(m_pDataset->GetPath());

    VSIStatBufL BufL;
    wxULongLong nSize(0);
    int ret = VSIStatL(soPath, &BufL);
    if(ret == 0)
    {
        nSize += BufL.st_size;
    }
    //wxULongLong nSize = wxFileName::GetSize(m_pDataset->GetPath());

    //folder
	//convert from internal representation UTF8 to wxConvCurrent

	wxString sPath(CPLGetPath(soPath), wxConvLocal);
    AppendProperty( new wxStringProperty(_("Folder"), wxPG_LABEL, sPath) );  
    //file list
    GDALDataset* poGDALDataset = m_pDataset->GetMainRaster();
    if(!poGDALDataset)
        poGDALDataset = m_pDataset->GetRaster();
    if(poGDALDataset)
    {
        char** papszFileList = poGDALDataset->GetFileList();
        if( CSLCount(papszFileList) < 2 )
        {
            AppendProperty( new wxStringProperty(_("Files"), wxPG_LABEL, _("None associated")) );  
        }
        else
        {
            wxPGId pfilesid = AppendProperty(pid, new wxPropertyCategory(_("Files")) );  
            for(int i = 1; papszFileList[i] != NULL; i++ )
		    {
                ret = VSIStatL(papszFileList[i], &BufL);
                if(ret == 0)
                {
                    nSize += BufL.st_size;
                }
                //TODO: conv file name to zip coding
			    wxString sFileName(CPLGetFilename(papszFileList[i]), wxConvLocal);

                AppendProperty(pfilesid, new wxStringProperty(_("File"), wxPG_LABEL, sFileName) );  
		    }
        }
        CSLDestroy( papszFileList );

        //size        
        AppendProperty(pid, new wxStringProperty(_("Total size"), wxPG_LABEL, wxFileName::GetHumanReadableSize(nSize)) );  

        GDALDriver* pDrv = poGDALDataset->GetDriver();
	    sTmp = sTmp.Format(wxT("%s(%s)"), wgMB2WX(pDrv->GetMetadataItem( GDAL_DMD_LONGNAME )), wgMB2WX(pDrv->GetDescription()) );
        AppendProperty(pid, new wxStringProperty(_("Driver"), wxPG_LABEL, sTmp) );  
//char **papszMetadata = poDriver->GetMetadata();
//    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
//        printf( "Driver %s supports Create() method.\n", pszFormat );
//    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
//        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );
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
    wxPGId pyrprop;
    if(bHasOvr)
        pyrprop = m_pg->Append( new wxStringProperty(OVR_TXT, wxPG_LABEL, _("Present (click to rebuild)")) );
    else
        pyrprop = m_pg->Append( new wxStringProperty(OVR_TXT, wxPG_LABEL, _("Absent (click to build)")) );
    m_pg->SetPropertyEditor(pyrprop, wxPG_EDITOR(TextCtrlAndButton));

    //Statistics
    bool bHasStats = m_pDataset->HasStatistics();
    wxPGId pstatprop;
    if(bHasStats)
        pstatprop = m_pg->Append( new wxStringProperty(STAT_TXT, wxPG_LABEL, _("Present (click to rebuild)")) );
    else
        pstatprop = m_pg->Append( new wxStringProperty(STAT_TXT, wxPG_LABEL, _("Absent (click to build)")) );
    m_pg->SetPropertyEditor(pstatprop, wxPG_EDITOR(TextCtrlAndButton));

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
            if(pBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
            {
                AppendProperty( new wxFloatProperty(_("Min"), wxPG_LABEL, dfMin ));
                AppendProperty( new wxFloatProperty(_("Max"), wxPG_LABEL, dfMax ));
                AppendProperty( new wxFloatProperty(_("Mean"), wxPG_LABEL, dfMean ));
                AppendProperty( new wxFloatProperty(_("Std Dev"), wxPG_LABEL, dfStdDev ));
            }

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
                CPLFree( pszXMLText );
            }
        }
    }
    //CPLCleanupTLS();
}


void wxGISRasterPropertyPage::OnPropertyGridButtonClick ( wxCommandEvent& )
{
    wxPGProperty* prop = m_pg->GetSelectedProperty();
    if (!prop )
        return;
    wxString sName = m_pg->GetPropertyName(prop);        
    wxString sToolName;
    if(sName.Cmp(STAT_TXT) == 0)//comp stats tool
        sToolName = wxString(wxT("comp_stats"));
    else if(sName.Cmp(OVR_TXT) == 0)//create ovr tool
        sToolName = wxString(wxT("create_ovr"));
    else
        return;

    //create/get tool mngr
    IGxObjectContainer* pRootContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pRootContainer)
        return;
    IToolManagerUI* pToolManagerUI = dynamic_cast<IToolManagerUI*>(pRootContainer->SearchChild(_("Toolboxes")));
    if(!pToolManagerUI)
    {
        //error msg
        wxMessageBox(_("Error find IToolManagerUI interface!\nThe wxGxRootToolbox Catalog root item\nis not loaded or not inherited from IToolManagerUI interface.\nCannnot continue."), _("Error"), wxICON_ERROR | wxOK );
        return; 
    }


	IGPTool* pTool = pToolManagerUI->GetGPTool(sToolName);
	if(!pTool)
	{
        //error msg
		wxMessageBox(wxString::Format(_("Error find %s tool!\nCannnot continue."), sToolName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return; 
	}
    GPParameters* pParams = pTool->GetParameterInfo();
    if(pParams->size() == 0)
	{
        //error msg
		wxMessageBox(wxString::Format(_("Wrong tool (%s)!\nCannnot continue."), sToolName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return; 
	}
    IGPParameter* pParam = pParams->operator[](0);
    pParam->SetValue(wxVariant(m_pGxDataset->GetFullName()));

	pToolManagerUI->OnPrepareTool(this, pTool, static_cast<IGPCallBack*>(this), true);
    m_pDataset->Close();
}

void wxGISRasterPropertyPage::OnFinish(bool bHasErrors, IGPTool* pTool)
{
    if(!bHasErrors)
        FillGrid();
}

    ////Histogram in new tab!
    //wxPGId phistprop = m_pg->Append( new wxStringProperty(_("Histogram"), wxPG_LABEL, _("Absent (click to build)")) ); 
    //m_pg->SetPropertyEditor(phistprop, wxPG_EDITOR(TextCtrlAndButton));
            ////Histogram
            //int nBucketCount, *panHistogram = NULL;
            //if(pBand->GetDefaultHistogram(&dfMin, &dfMax, &nBucketCount, &panHistogram, false, GDALTermProgress, NULL ) == CE_None )
            //{
            //    CPLFree( panHistogram );
            //}
            //else
