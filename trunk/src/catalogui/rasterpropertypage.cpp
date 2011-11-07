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

#include "gdal_rat.h"

#define STAT_TXT _("Statistics")
#define OVR_TXT _("Overviews")

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterPropertyPage, wxPanel)

BEGIN_EVENT_TABLE(wxGISRasterPropertyPage, wxPanel)
    EVT_BUTTON( ID_PPCTRL, wxGISRasterPropertyPage::OnPropertyGridButtonClick )
	EVT_PROCESS_FINISH( wxGISRasterPropertyPage::OnFinish )
END_EVENT_TABLE()

wxGISRasterPropertyPage::wxGISRasterPropertyPage(void)
{
	m_nCounter = 0;
	m_nCookie = wxNOT_FOUND;
	m_pToolManagerUI = NULL;
}

wxGISRasterPropertyPage::wxGISRasterPropertyPage(wxGxRasterDataset* pGxDataset, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_nCounter = 0;
	m_nCookie = wxNOT_FOUND;
	m_pToolManagerUI = NULL;
    Create(pGxDataset, pCatalog, parent, id, pos, size, style, name);
}

wxGISRasterPropertyPage::~wxGISRasterPropertyPage()
{
	if(m_nCookie != wxNOT_FOUND && m_pToolManagerUI)
		m_pToolManagerUI->Unadvise(m_nCookie);
}

bool wxGISRasterPropertyPage::Create(wxGxRasterDataset* pGxDataset, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create(parent, id, pos, size, style, name))
		return false;

    m_pGxDataset = pGxDataset;
    m_pCatalog = pCatalog;

    m_pDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(m_pGxDataset->GetDataset(false));
    if(!m_pDataset)
        return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    FillGrid();

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

wxPGProperty* wxGISRasterPropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISRasterPropertyPage::AppendProperty(wxPGProperty* pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISRasterPropertyPage::AppendMetadataProperty(wxString sMeta)
{
    int nPos = sMeta.Find('=');
    if(nPos == wxNOT_FOUND)
		return AppendProperty( new wxStringProperty(_("Item"), wxString::Format(wxT("Item_%d"), ++m_nCounter), sMeta) );
    else
    {
        wxString sName = sMeta.Left(nPos);
        wxString sVal = sMeta.Right(sMeta.Len() - nPos - 1);
        //clean
        wxString sCleanVal;
        for(size_t i = 0; i < sVal.Len(); ++i)
        {
            char c = sVal[i];
            if(sVal[i] > 31 && sVal[i] != 127)
                sCleanVal += sVal[i];
        }
        if(sCleanVal.Len() > 500)
            return m_pg->Append( new wxLongStringProperty(sName, wxString::Format(wxT("%s_%d"), sName.c_str(), ++m_nCounter), sCleanVal) );//??
        else
            return AppendProperty( new wxStringProperty(sName, wxString::Format(wxT("%s_%d"), sName.c_str(), ++m_nCounter), sCleanVal) );
    }
}

void wxGISRasterPropertyPage::FillGrid(void)
{
    m_pDataset->Open(true);

    wxString sTmp;
    //fill propertygrid
    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Data Source")) );
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

	wxString sPath(CPLGetPath(soPath), wxConvUTF8);
    AppendProperty( new wxStringProperty(_("Folder"), wxPG_LABEL, sPath) );  
    //file list
    GDALDataset* poGDALDataset = m_pDataset->GetMainRaster();
    if(!poGDALDataset)
        poGDALDataset = m_pDataset->GetRaster();
    if(poGDALDataset)
    {
        char** papszFileList = m_pDataset->GetFileList();//poGDALDataset->GetFileList();
        if( !papszFileList || CSLCount(papszFileList) == 0 )
        {
            AppendProperty( new wxStringProperty(_("Files"), wxPG_LABEL, _("None associated")) );  
        }
        else
        {
            wxPGProperty* pfilesid = AppendProperty(pid, new wxPropertyCategory(_("Files")) );  
            for(int i = 0; papszFileList[i] != NULL; ++i )
		    {
                ret = VSIStatL(papszFileList[i], &BufL);
                if(ret == 0)
                {
                    nSize += BufL.st_size;
                }
		        wxString sFileName = GetConvName(papszFileList[i]);
				AppendProperty(pfilesid, new wxStringProperty(wxString::Format(_("File %d"), i), wxPG_LABEL, sFileName) );  
		    }
        }
        CSLDestroy( papszFileList );

        //size        
        AppendProperty(pid, new wxStringProperty(_("Total size"), wxPG_LABEL, wxFileName::GetHumanReadableSize(nSize)) );  

        GDALDriver* pDrv = poGDALDataset->GetDriver();
		sTmp = sTmp.Format(wxT("%s(%s)"), wxString(pDrv->GetMetadataItem( GDAL_DMD_LONGNAME ), wxConvLocal).c_str(), wxString(pDrv->GetDescription(), wxConvLocal).c_str() );
        AppendProperty(pid, new wxStringProperty(_("Driver"), wxPG_LABEL, sTmp) );  
//char **papszMetadata = poDriver->GetMetadata();
//    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
//        printf( "Driver %s supports Create() method.\n", pszFormat );
//    if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
//        printf( "Driver %s supports CreateCopy() method.\n", pszFormat );
    }


    AppendProperty( new wxPropertyCategory(_("Information")) );
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
    wxPGProperty* pyrprop;
    if(bHasOvr)
        pyrprop = m_pg->Append( new wxStringProperty(OVR_TXT, wxPG_LABEL, _("Present (click to rebuild)")) );
    else
        pyrprop = m_pg->Append( new wxStringProperty(OVR_TXT, wxPG_LABEL, _("Absent (click to build)")) );
    m_pg->SetPropertyEditor(pyrprop, wxPG_EDITOR(TextCtrlAndButton));

    //Statistics
    bool bHasStats = m_pDataset->HasStatistics();
    wxPGProperty* pstatprop;
    if(bHasStats)
        pstatprop = m_pg->Append( new wxStringProperty(STAT_TXT, wxPG_LABEL, _("Present (click to rebuild)")) );
    else
        pstatprop = m_pg->Append( new wxStringProperty(STAT_TXT, wxPG_LABEL, _("Absent (click to build)")) );
    m_pg->SetPropertyEditor(pstatprop, wxPG_EDITOR(TextCtrlAndButton));

    AppendProperty( new wxPropertyCategory(_("Extent")));
    OGREnvelope Env = m_pDataset->GetEnvelope();
    AppendProperty( new wxFloatProperty(_("Top"), wxPG_LABEL, Env.MaxY));
    AppendProperty( new wxFloatProperty(_("Left"), wxPG_LABEL, Env.MinX));
    AppendProperty( new wxFloatProperty(_("Right"), wxPG_LABEL, Env.MaxX));
    AppendProperty( new wxFloatProperty(_("Bottom"), wxPG_LABEL, Env.MinY));

    if(poGDALDataset)
    {
        
        char** papszMetadata = poGDALDataset->GetMetadata();
        if( CSLCount(papszMetadata) > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("Metadata")) );
            for(int i = 0; papszMetadata[i] != NULL; ++i )
            {
                sTmp = wxString(papszMetadata[i], wxConvLocal);
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
            for(int i = 0; papszMetadata[i] != NULL; ++i )
            {
                sTmp = wxString(papszMetadata[i], wxConvLocal);
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
            for(int i = 0; papszMetadata[i] != NULL; ++i )
            {
                sTmp = wxString(papszMetadata[i], wxConvLocal);
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
            for(int i = 0; papszMetadata[i] != NULL; ++i )
            {
                sTmp = wxString(papszMetadata[i], wxConvLocal);
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
            for(int i = 0; papszMetadata[i] != NULL; ++i )
            {
                sTmp = wxString(papszMetadata[i], wxConvLocal);
                AppendMetadataProperty(sTmp);
            }
        }

        /* -------------------------------------------------------------------- */
        /*      Report GCPs.                                                    */
        /* -------------------------------------------------------------------- */
        if( poGDALDataset->GetGCPCount() > 0 )
        {
            AppendProperty( new wxPropertyCategory(_("GCP data")) );
            OGRSpatialReferenceSPtr poSRS;
            char* pszGCPProjection = (char*)poGDALDataset->GetGCPProjection();
            if (pszGCPProjection)
            {
				poSRS = boost::make_shared<OGRSpatialReference>();
                if( poSRS->importFromWkt(&pszGCPProjection ) == CE_None )
					poSRS.reset();
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
                    AppendProperty( new wxStringProperty(_("GCP Projection"), wxPG_LABEL, wxString(pszName, wxConvLocal)) );
                }
                else if(poSRS->IsGeographic())
                {
                    pszName = poSRS->GetAttrValue("GEOGCS");
                    AppendProperty( new wxStringProperty(_("GCP Projection"), wxPG_LABEL, wxString(pszName, wxConvLocal)) );
                }
                else
                    AppendProperty( new wxStringProperty(_("GCP Projection"), wxPG_LABEL, _("Undefined")) );
            }

            for(size_t i = 0; i < poGDALDataset->GetGCPCount(); ++i )
            {
                const GDAL_GCP *psGCP = poGDALDataset->GetGCPs( ) + i;
                AppendProperty( new wxStringProperty(wxString::Format(wxT("GCP[%03d]"), i + 1), wxPG_LABEL, wxString::Format(_("Id='%s', Info='%s', (%.6g,%.6g) -> (%.6g,%.6g,%.6g)"), wxString(psGCP->pszId, wxConvLocal), wxString(psGCP->pszInfo, wxConvLocal), psGCP->dfGCPPixel, psGCP->dfGCPLine, psGCP->dfGCPX, psGCP->dfGCPY, psGCP->dfGCPZ)) );
            }
        }

        /* ==================================================================== */
        /*      Loop over bands.                                                */
        /* ==================================================================== */        
     
        wxPGProperty* pstatid = AppendProperty( new wxPropertyCategory(_("Statistics details")) );
               
        for(int nBand = 0; nBand < poGDALDataset->GetRasterCount(); ++nBand )
        {
            wxPGProperty* pbandid = AppendProperty(pstatid, new wxPropertyCategory(wxString::Format(_("Band %d"), nBand + 1)) );

            int         bSuccess;
            int         nBlockXSize, nBlockYSize, nMaskFlags;
            double      dfMin, dfMax, dfMean, dfStdDev;
            GDALColorTable*	hTable;

            GDALRasterBand* pBand = poGDALDataset->GetRasterBand(nBand + 1);

		    pBand->GetBlockSize(&nBlockXSize, &nBlockYSize);
			AppendProperty( new wxStringProperty(_("Block"), wxString::Format(wxT("Block_%d"), nBand), wxString::Format(wxT("%d x %d"), nBlockXSize, nBlockYSize)));
            AppendProperty( new wxStringProperty(_("Type"), wxString::Format(wxT("Type_%d"), nBand), wxString(GDALGetDataTypeName(pBand->GetRasterDataType()), wxConvLocal) ));
            AppendProperty( new wxStringProperty(_("ColorInterp"), wxString::Format(wxT("ColorInterp_%d"), nBand), wxString(GDALGetColorInterpretationName(pBand->GetColorInterpretation()), wxConvLocal) ));

		    wxString sDescription = wxString(pBand->GetDescription(), wxConvLocal);
            if(!sDescription.IsEmpty())
                AppendProperty( new wxStringProperty(_("Description"), wxString::Format(wxT("Description_%d"), nBand), sDescription ));

            //Statistics
            if(pBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
            {
                AppendProperty( new wxFloatProperty(_("Min"), wxString::Format(wxT("Min_%d"), nBand), dfMin ));
                AppendProperty( new wxFloatProperty(_("Max"), wxString::Format(wxT("Max_%d"), nBand), dfMax ));
                AppendProperty( new wxFloatProperty(_("Mean"), wxString::Format(wxT("Mean_%d"), nBand), dfMean ));
                AppendProperty( new wxFloatProperty(_("Std Dev"), wxString::Format(wxT("Std Dev_%d"), nBand), dfStdDev ));
            }

            //Checksum
            //   wxLogDebug( wxT("  Checksum=%d"), GDALChecksumImage(pBand, 0, 0, nXSize, nYSize));

            if(m_pDataset->HasNoData(nBand + 1) )
            {
                AppendProperty( new wxFloatProperty(_("NoData Value"), wxString::Format(wxT("NoData Value_%d"), nBand + 1), m_pDataset->GetNoData(nBand + 1) ));
            }

            //Band Overviews
		    if( pBand->GetOverviewCount() > 0 )
            {
				wxPGProperty* povrid = AppendProperty(pbandid, new wxStringProperty(_("Overviews"), wxString::Format(wxT("Overviews_%d"), nBand), pBand->HasArbitraryOverviews() != 0 ? _("arbitrary") : wxEmptyString ) );
                
                for(int iOverview = 0; iOverview < pBand->GetOverviewCount(); iOverview++ )
                {
                    const char *pszResampling = NULL;
                    
                    GDALRasterBand*	pOverview = pBand->GetOverview( iOverview );
                    wxString sOvrDesc = wxString::Format(wxT("%d x %d"), pOverview->GetXSize(), pOverview->GetYSize());
                    pszResampling = pOverview->GetMetadataItem("RESAMPLING", "" );
                    if( pszResampling != NULL && EQUALN(pszResampling, "AVERAGE_BIT2", 12) )
                        sOvrDesc += wxT( ", *" );
                    else
                        sOvrDesc += wxString(pszResampling, wxConvLocal);

					wxString sPropName = wxString::Format(_("Overview %d"), iOverview + 1);
					AppendProperty(povrid, new wxStringProperty(sPropName, wxString::Format(wxT("%s_%d"), sPropName.c_str(), nBand), sOvrDesc ));
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

                wxPGProperty* pmfid = AppendProperty(pbandid, new wxStringProperty(_("Mask Flags"), wxString::Format(wxT("Mask Flags_%d"), nBand), sMaskStr ));

                if( pMaskBand != NULL && pMaskBand->GetOverviewCount() > 0 )
                {
                    wxPGProperty* pombid = AppendProperty(pmfid, new wxPropertyCategory(_("Overviews of mask band")) );

                    for( int nOverview = 0; nOverview < pMaskBand->GetOverviewCount(); nOverview++ )
                    {
                        GDALRasterBand*	pOverview = pMaskBand->GetOverview( nOverview );
						wxString sPropName = wxString::Format(_("Overview %d"), nOverview + 1);
						AppendProperty(pombid, new wxStringProperty(sPropName, wxString::Format(wxT("%s_%d"), sPropName.c_str(), nBand), wxString::Format(wxT("%dx%d"), pOverview->GetXSize(), pOverview->GetYSize() ))); 
                    }
                }
            }

            wxString sUnitType = wxString( pBand->GetUnitType(), wxConvLocal );
		    if( sUnitType.Len() > 0 )
            {
                AppendProperty(pbandid, new wxStringProperty(_("Unit Type"), wxString::Format(wxT("Unit Type_%d"), nBand), sUnitType));
            }

		    char **papszCategories = pBand->GetCategoryNames();
            if( papszCategories != NULL )
            {
                wxPGProperty* pcatid = AppendProperty(pbandid, new wxIntProperty(_("Categories"), wxString::Format(wxT("Categories_%d"), nBand), CSLCount(papszCategories) ) );
                for(int  i = 0; papszCategories[i] != NULL; ++i )
				{
					wxString sSubPropName = wxString::Format(wxT("%3d"), i);
					AppendProperty(pcatid, new wxStringProperty(sSubPropName, wxString::Format(wxT("%s_%d"), sSubPropName.c_str(), nBand), wxString(papszCategories[i], wxConvLocal) ) );
				}
            }

            double fScale = pBand->GetScale( &bSuccess );
            if(bSuccess && fScale != 1.0)
                AppendProperty(pbandid, new wxFloatProperty(_("Scale"), wxString::Format(wxT("Scale_%d"), nBand), fScale ) );
            double fOffset = pBand->GetOffset( &bSuccess );
            if(bSuccess && fOffset != 0.0)
                AppendProperty(pbandid, new wxFloatProperty(_("Offset"), wxString::Format(wxT("Offset_%d"), nBand), fOffset ) );

		    papszMetadata = pBand->GetMetadata();
            if( CSLCount(papszMetadata) > 0 )
            {
                AppendProperty(pbandid, new wxPropertyCategory(_("Metadata") ) );
                for( int i = 0; papszMetadata[i] != NULL; ++i )
                {
                    AppendMetadataProperty( wxString(papszMetadata[i], wxConvLocal) );
                }
            }

            papszMetadata = pBand->GetMetadata( "IMAGE_STRUCTURE" );
            if( CSLCount(papszMetadata) > 0 )
            {
                AppendProperty(pbandid, new wxPropertyCategory(_("Image Structure Metadata") ) );
                for( int i = 0; papszMetadata[i] != NULL; ++i )
                {
                    AppendMetadataProperty( wxString(papszMetadata[i], wxConvLocal) );
                }
            }

		    if( pBand->GetColorInterpretation() == GCI_PaletteIndex && (hTable = pBand->GetColorTable()) != NULL )
            {
				wxPGProperty* pcpid = AppendProperty(pbandid, new wxStringProperty(_("Color Table"), wxString::Format(wxT("Color Table_%d"), nBand), wxString::Format(_("%s with %d entries"), wxString(GDALGetPaletteInterpretationName(hTable->GetPaletteInterpretation()), wxConvLocal).c_str(), hTable->GetColorEntryCount()) ));

			    for(int	i = 0; i < hTable->GetColorEntryCount(); ++i )
                {
                    GDALColorEntry	sEntry;
				    hTable->GetColorEntryAsRGB(i, &sEntry );

					wxString sPropString = wxString::Format(_("%3d"), i);
					AppendProperty(pcpid, new wxStringProperty(sPropString, wxString::Format(wxT("%s_%d"), sPropString.c_str(), nBand), wxString::Format(_("%d,%d,%d,%d"), sEntry.c1, sEntry.c2, sEntry.c3, sEntry.c4 )));
                }
                m_pg->Collapse(pcpid);
            }

		    if( pBand->GetDefaultRAT() != NULL )
            {
			    const GDALRasterAttributeTable* pRAT = (const GDALRasterAttributeTable*)pBand->GetDefaultRAT();
                
                char *pszXMLText = CPLSerializeXMLTree( pRAT->Serialize() );
                m_pg->AppendIn(pbandid, new wxLongStringProperty(_("Raster Attribute Table"), wxString::Format(wxT("Raster Attribute Table_%d"), nBand), wxString(pszXMLText, wxConvLocal)));
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
    m_pToolManagerUI = dynamic_cast<IToolManagerUI*>(pRootContainer->SearchChild(_("Toolboxes")));
    if(!m_pToolManagerUI)
    {
        //error msg
        wxMessageBox(_("Error find IToolManagerUI interface!\nThe wxGxRootToolbox Catalog root item\nis not loaded or not inherited from IToolManagerUI interface.\nCannnot continue."), _("Error"), wxICON_ERROR | wxOK );
        return; 
    }


	IGPToolSPtr pTool = m_pToolManagerUI->GetGPTool(sToolName);
	if(!pTool)
	{
        //error msg
		wxMessageBox(wxString::Format(_("Error find %s tool!\nCannnot continue."), sToolName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return; 
	}
    GPParameters Params = pTool->GetParameterInfo();
	if(Params.IsEmpty())
	{
        //error msg
		wxMessageBox(wxString::Format(_("Wrong tool (%s)!\nCannnot continue."), sToolName.c_str()), _("Error"), wxICON_ERROR | wxOK );
        return; 
	}
	IGPParameter* pParam = Params[0];
    pParam->SetValue(wxVariant(m_pGxDataset->GetFullName()));

	if(m_nCookie == wxNOT_FOUND)
		m_nCookie = m_pToolManagerUI->Advise(this);

	m_pToolManagerUI->PrepareTool(this, pTool, true);
    m_pDataset->Close();
}

void wxGISRasterPropertyPage::OnFinish(wxGISProcessEvent& event)
{
	if(!event.GetHasErrors())
	{
	    //reset grid
		m_pg->Clear();
        FillGrid();
	}
}

    ////Histogram in new tab!
    //wxPGProperty* phistprop = m_pg->Append( new wxStringProperty(_("Histogram"), wxPG_LABEL, _("Absent (click to build)")) ); 
    //m_pg->SetPropertyEditor(phistprop, wxPG_EDITOR(TextCtrlAndButton));
            ////Histogram
            //int nBucketCount, *panHistogram = NULL;
            //if(pBand->GetDefaultHistogram(&dfMin, &dfMax, &nBucketCount, &panHistogram, false, GDALTermProgress, NULL ) == CE_None )
            //{
            //    CPLFree( panHistogram );
            //}
            //else
