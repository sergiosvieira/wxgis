/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISVectorPropertyPage class.
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

#include "wxgis/catalogui/vectorpropertypage.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISVectorPropertyPage, wxPanel)

BEGIN_EVENT_TABLE(wxGISVectorPropertyPage, wxPanel)
END_EVENT_TABLE()

wxGISVectorPropertyPage::wxGISVectorPropertyPage(void) : m_pDataset(NULL)
{
}

wxGISVectorPropertyPage::wxGISVectorPropertyPage(wxGxFeatureDataset* pGxDataset, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : m_pDataset(NULL)
{
    Create(pGxDataset, parent, id, pos, size, style, name);
}

wxGISVectorPropertyPage::~wxGISVectorPropertyPage()
{
    wsDELETE(m_pDataset);
}

bool wxGISVectorPropertyPage::Create(wxGxFeatureDataset* pGxDataset, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    wxPanel::Create(parent, id, pos, size, style, name);

    m_pGxDataset = pGxDataset;

    m_pDataset = dynamic_cast<wxGISFeatureDataset*>(m_pGxDataset->GetDataset(true));
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

wxPGId wxGISVectorPropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    pNewProp->SetFlag(wxPG_PROP_READONLY);
    return pNewProp;
}

wxPGId wxGISVectorPropertyPage::AppendProperty(wxPGId pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    pNewProp->SetFlag(wxPG_PROP_READONLY);
    return pNewProp;
}

wxPGId wxGISVectorPropertyPage::AppendMetadataProperty(wxString sMeta)
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

void wxGISVectorPropertyPage::FillGrid(void)
{
    //reset grid
    m_pg->Clear();

    wxString sTmp;
    //fill propertygrid
    wxPGId pid = AppendProperty( new wxPropertyCategory(_("Data Source")) );
    AppendProperty( new wxStringProperty(_("Vector"), wxPG_LABEL, m_pGxDataset->GetName()) );  

    CPLString soPath(wgWX2MB(m_pDataset->GetPath()));

    VSIStatBufL BufL;
    wxULongLong nSize(0);
    int ret = VSIStatL(soPath, &BufL);
    if(ret == 0)
    {
        nSize += BufL.st_size;
    }
    //wxULongLong nSize = wxFileName::GetSize(m_pDataset->GetPath());

    //folder
    wxFileName FName(m_pDataset->GetPath());
    AppendProperty( new wxStringProperty(_("Folder"), wxPG_LABEL, FName.GetPath()) );  

    OGRDataSource *pDataSource = m_pDataset->GetDataSource();
    if(pDataSource)
    {
        OGRSFDriver* pDrv = pDataSource->GetDriver();
        if(pDrv)
        {
            wxPGId pdriversid = AppendProperty(pid, new wxStringProperty(_("Driver"), wxPG_LABEL, wgMB2WX(pDrv->GetName()) ));  
            //TestCapability
            AppendProperty(pdriversid, new wxStringProperty(_("Create DataSource"), wxPG_LABEL, pDrv->TestCapability(ODrCCreateDataSource) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pdriversid, new wxStringProperty(_("Delete DataSource"), wxPG_LABEL, pDrv->TestCapability(ODrCDeleteDataSource) == TRUE ? _("true") : _("false")) );  
        }
        wxPGId pdssid = AppendProperty(pid, new wxStringProperty(_("DataSource"), wxPG_LABEL, wgMB2WX(pDataSource->GetName()) ));  
        AppendProperty(pdssid, new wxIntProperty(_("Layer Count"), wxPG_LABEL, pDataSource->GetLayerCount()) );  
        AppendProperty(pdssid, new wxStringProperty(_("Create DataSource"), wxPG_LABEL, pDataSource->TestCapability(ODsCCreateLayer) == TRUE ? _("true") : _("false")) );  
    }


    if(pDataSource)
    {
        AppendProperty( new wxPropertyCategory(_("Vector Information")) );
        for( int iLayer = 0; iLayer < pDataSource->GetLayerCount(); iLayer++ )
        {
            OGRLayer *poLayer = pDataSource->GetLayer(iLayer);
            //AppendProperty(new wxStringProperty(_("Name"), wxPG_LABEL, wgMB2WX(poLayer->GetName())));  
            AppendProperty(new wxStringProperty(_("Geometry type"), wxPG_LABEL, wgMB2WX(OGRGeometryTypeToName( m_pDataset->GetGeometryType() ))));  
            AppendProperty(new wxIntProperty(_("Feature count"), wxPG_LABEL, m_pDataset->GetSize() ));  

        //OGREnvelope oExt;
        //if (poLayer->GetExtent(&oExt, TRUE) == OGRERR_NONE)
        //{
        //    printf("Extent: (%f, %f) - (%f, %f)\n", 
        //           oExt.MinX, oExt.MinY, oExt.MaxX, oExt.MaxY);
        //}    

            if( CPLStrnlen(poLayer->GetFIDColumn(), 100) > 0 )
                AppendProperty(new wxStringProperty(_("FID Column"), wxPG_LABEL, wgMB2WX( poLayer->GetFIDColumn() ))); 
            if( CPLStrnlen(poLayer->GetGeometryColumn(), 100) > 0 )
                AppendProperty(new wxStringProperty(_("Geometry Column"), wxPG_LABEL, wgMB2WX( poLayer->GetGeometryColumn() ))); 

            OGRFeatureDefn *poDefn = m_pDataset->GetDefiniton();
            if(poDefn)
            {
                wxPGId pfieldsid = AppendProperty( new wxPropertyCategory(_("Fields")) );
                for( int iAttr = 0; iAttr < poDefn->GetFieldCount(); iAttr++ )
                {
                    OGRFieldDefn    *poField = poDefn->GetFieldDefn( iAttr );
                    wxString sFieldTypeName = wgMB2WX( poField->GetFieldTypeName( poField->GetType() ) );
                    wxPGId pfielid = AppendProperty(pfieldsid, new wxStringProperty(_("Name"), wxPG_LABEL, wxString::Format(wxT("%s (%s)"), wgMB2WX(poField->GetNameRef()), sFieldTypeName.c_str()) ));  
                    AppendProperty(pfielid, new wxStringProperty(_("Type"), wxPG_LABEL, sFieldTypeName ) );  
                    AppendProperty(pfielid, new wxIntProperty(_("Width"), wxPG_LABEL, poField->GetWidth()) );  
                    AppendProperty(pfielid, new wxIntProperty(_("Precision"), wxPG_LABEL, poField->GetPrecision()) ); 
                    OGRJustification Just = poField->GetJustify();
                    wxString sJust(_("Undefined"));
                    switch(Just)
                    {
                    case OJLeft:
                        sJust = wxString(_("Left"));
                        break;
                    case OJRight:
                        sJust = wxString(_("Right"));
                        break;
                    }
                    AppendProperty(pfielid, new wxStringProperty(_("Justify"), wxPG_LABEL, sJust) ); 
                    m_pg->Collapse(pfielid);

                }
            }
            //TestCapability 
            wxPGId pcapid = AppendProperty( new wxPropertyCategory(_("Layer Capability")) );
            AppendProperty(pcapid, new wxStringProperty(_("Random Read"), wxPG_LABEL, poLayer->TestCapability(OLCRandomRead) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Sequential Write"), wxPG_LABEL, poLayer->TestCapability(OLCSequentialWrite) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Random Write"), wxPG_LABEL, poLayer->TestCapability(OLCRandomWrite) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Fast Spatial Filter"), wxPG_LABEL, poLayer->TestCapability(OLCFastSpatialFilter) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Fast Feature Count"), wxPG_LABEL, poLayer->TestCapability(OLCFastFeatureCount) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Fast Get Extent"), wxPG_LABEL, poLayer->TestCapability(OLCFastGetExtent) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Fast Set Next By Index"), wxPG_LABEL, poLayer->TestCapability(OLCFastSetNextByIndex) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Create Field"), wxPG_LABEL, poLayer->TestCapability(OLCCreateField) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Delete Feature"), wxPG_LABEL, poLayer->TestCapability(OLCDeleteFeature) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Strings As UTF8"), wxPG_LABEL, poLayer->TestCapability(OLCStringsAsUTF8) == TRUE ? _("true") : _("false")) );  
            AppendProperty(pcapid, new wxStringProperty(_("Transactions"), wxPG_LABEL, poLayer->TestCapability(OLCTransactions) == TRUE ? _("true") : _("false")) );  
            //AppendProperty(pcapid, new wxStringProperty(_("Ignore Fields"), wxPG_LABEL, poLayer->TestCapability(OLCIgnoreFields) == TRUE ? _("true") : _("false")) );  
       }
    }

    AppendProperty( new wxPropertyCategory(_("Extent")));
    const OGREnvelope* pEnv = m_pDataset->GetEnvelope();
    AppendProperty( new wxFloatProperty(_("Top"), wxPG_LABEL, pEnv->MaxY));
    AppendProperty( new wxFloatProperty(_("Left"), wxPG_LABEL, pEnv->MinX));
    AppendProperty( new wxFloatProperty(_("Right"), wxPG_LABEL, pEnv->MaxX));
    AppendProperty( new wxFloatProperty(_("Bottom"), wxPG_LABEL, pEnv->MinY));

}


