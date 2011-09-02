/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferencePropertyPage class.
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

#include "wxgis/catalogui/spatrefpropertypage.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISSpatialReferencePropertyPage, wxPanel)

wxGISSpatialReferencePropertyPage::wxGISSpatialReferencePropertyPage(void)
{
}

wxGISSpatialReferencePropertyPage::wxGISSpatialReferencePropertyPage(OGRSpatialReferenceSPtr poSRS, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    Create(poSRS, parent, id, pos, size, style, name);
}

wxGISSpatialReferencePropertyPage::~wxGISSpatialReferencePropertyPage()
{
}

bool wxGISSpatialReferencePropertyPage::Create(OGRSpatialReferenceSPtr poSRS, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    wxPanel::Create(parent, id, pos, size, style, name);

    //if(!poSRS)
    //    return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pg = new wxPropertyGrid(this, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    //fill propertygrid
    AppendProperty( new wxPropertyCategory(_("Coordinate System")) );
    if(poSRS)
    {
        poSRS->AutoIdentifyEPSG();
        if(poSRS->IsProjected())
        {
            FillProjected(poSRS);
            AppendProperty( new wxPropertyCategory(_("Geographic Coordinate System")) );
            OGRSpatialReferenceSPtr poGeogCS = OGRSpatialReferenceSPtr(poSRS->CloneGeogCS());
            FillGeographic(poGeogCS);
        }
        else if(poSRS->IsGeographic())
        {
            FillGeographic(poSRS);
        }
        else if(poSRS->IsLocal())
        {
            FillLoclal(poSRS);
        }
        //else if(pSpaRef->IsVertical())
        //{
        //}
        else
            FillUndefined();
        //OSRDestroySpatialReference(poSRS);
    }
    else
    {
        FillUndefined();
    }

    bMainSizer->Add( m_pg, 1, wxEXPAND | wxALL, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

void wxGISSpatialReferencePropertyPage::FillUndefined(void)
{
    m_pg->Append( new wxStringProperty(_("Name"), wxT("Name_Undefined"), _("Undefined")) );
}

void wxGISSpatialReferencePropertyPage::FillProjected(OGRSpatialReferenceSPtr poSRS)
{
    const char *pszName = poSRS->GetAttrValue("PROJCS");
    //make bold!  wxPG_BOLD_MODIFIED
    AppendProperty( new wxStringProperty(_("Name"), wxT("Name_Projected"), wxString(pszName, wxConvLocal)) );

    //EPSG
    const char *pszAfCode = poSRS->GetAuthorityCode("PROJCS");
    const char *pszAfName = poSRS->GetAuthorityName("PROJCS");
	if(pszAfName || pszAfCode)
		AppendProperty( new wxStringProperty(wxString(pszAfName, wxConvLocal), wxPG_LABEL, wxString(pszAfCode, wxConvLocal))  );
	//

    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Projection")) );
    const char *pszNameProj = poSRS->GetAttrValue("PROJECTION");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameProj"), wxString(pszNameProj, wxConvLocal)) );
    wxPGProperty* pidparm = AppendProperty(pid, new wxPropertyCategory(_("Parameters")) );

    //ogr_srs_api.h
    AppendProjParam(pidparm, SRS_PP_CENTRAL_MERIDIAN, poSRS);
    AppendProjParam(pidparm, SRS_PP_SCALE_FACTOR, poSRS);
    AppendProjParam(pidparm, SRS_PP_STANDARD_PARALLEL_1, poSRS);
    AppendProjParam(pidparm, SRS_PP_STANDARD_PARALLEL_2, poSRS);
    AppendProjParam(pidparm, SRS_PP_PSEUDO_STD_PARALLEL_1, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_CENTER, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_CENTER, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_ORIGIN, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_ORIGIN, poSRS);
    AppendProjParam(pidparm, SRS_PP_FALSE_EASTING, poSRS);
    AppendProjParam(pidparm, SRS_PP_FALSE_NORTHING, poSRS);
    AppendProjParam(pidparm, SRS_PP_AZIMUTH, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_POINT_1, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_POINT_1, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_POINT_2, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_POINT_2, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_POINT_3, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_POINT_3, poSRS);
    AppendProjParam(pidparm, SRS_PP_RECTIFIED_GRID_ANGLE, poSRS);
    AppendProjParam(pidparm, SRS_PP_LANDSAT_NUMBER, poSRS);
    AppendProjParam(pidparm, SRS_PP_PATH_NUMBER, poSRS);
    AppendProjParam(pidparm, SRS_PP_PERSPECTIVE_POINT_HEIGHT, poSRS);
    AppendProjParam(pidparm, SRS_PP_SATELLITE_HEIGHT, poSRS);
    AppendProjParam(pidparm, SRS_PP_FIPSZONE, poSRS);
    AppendProjParam(pidparm, SRS_PP_ZONE, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_1ST_POINT, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_1ST_POINT, poSRS);
    AppendProjParam(pidparm, SRS_PP_LATITUDE_OF_2ND_POINT, poSRS);
    AppendProjParam(pidparm, SRS_PP_LONGITUDE_OF_2ND_POINT, poSRS);

    AppendProperty( new wxPropertyCategory(_("Linear unit")) );
    char *pszUnitName = NULL;
    double fUnit = poSRS->GetLinearUnits(&pszUnitName);
    AppendProperty( new wxStringProperty(_("Name"), wxT("UnitName"), wxString(pszUnitName, wxConvLocal)) );
    AppendProperty( new wxFloatProperty(_("Meters per unit"), wxPG_LABEL, fUnit) );

}

void wxGISSpatialReferencePropertyPage::FillGeographic(OGRSpatialReferenceSPtr poSRS)
{
    const char *pszName = poSRS->GetAttrValue("GEOGCS");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameGeographic"), wxString(pszName, wxConvLocal)) );
    //EPSG
    const char *pszAfCode = poSRS->GetAuthorityCode("GEOGCS");
    const char *pszAfName = poSRS->GetAuthorityName("GEOGCS");
	if(pszAfName || pszAfCode)
	    AppendProperty( new wxStringProperty(wxString(pszAfName, wxConvLocal), wxPG_LABEL, wxString(pszAfCode, wxConvLocal))  );

    wxPGProperty* pid = AppendProperty( new wxPropertyCategory(_("Datum")) );
    const char *pszNameDatum = poSRS->GetAttrValue("DATUM");
    AppendProperty( new wxStringProperty(_("Name"), wxT("NameDatum"), wxString(pszNameDatum, wxConvLocal)) );
    wxPGProperty* pidsph = AppendProperty(pid, new wxPropertyCategory(_("Spheroid")) );
    const char *pszNameSph = poSRS->GetAttrValue("SPHEROID");
    AppendProperty(pidsph, new wxStringProperty(_("Name"), wxT("NameSph"), wxString(pszNameSph, wxConvLocal)) );
    AppendProperty(pidsph, new wxFloatProperty(_("Semimajor Axis"), wxPG_LABEL, poSRS->GetSemiMajor(NULL)) );
    AppendProperty(pidsph, new wxFloatProperty(_("Semiminor Axis"), wxPG_LABEL, poSRS->GetSemiMinor(NULL)) );
    AppendProperty(pidsph, new wxFloatProperty(_("Inverse Flattering"), wxPG_LABEL, poSRS->GetInvFlattening(NULL)) );
    pid = AppendProperty( new wxPropertyCategory(_("Angular unit")) );

    char *pszUnitName = NULL;
    double fUnit = poSRS->GetAngularUnits(&pszUnitName);
    AppendProperty( new wxStringProperty(_("Name"), wxT("UnitName"), wxString(pszUnitName, wxConvLocal)) );
    AppendProperty( new wxFloatProperty(_("Radians per unit"), wxPG_LABEL, fUnit) );

    pid = AppendProperty( new wxPropertyCategory(_("Prime Meridian")));

    char *pszMerName = NULL;
    double fMerLon = poSRS->GetPrimeMeridian(&pszMerName);
    AppendProperty(pid, new wxStringProperty(_("Name"), wxT("MerName"), wxString(pszMerName, wxConvLocal)));
    AppendProperty(pid, new wxFloatProperty(_("Longitude"), wxPG_LABEL, fMerLon));
}

void wxGISSpatialReferencePropertyPage::FillLoclal(OGRSpatialReferenceSPtr poSRS)
{
    AppendProperty(new wxStringProperty(_("Name"), wxT("NameLoclal"), _("Undefined")));
}

void wxGISSpatialReferencePropertyPage::AppendProjParam(wxPGProperty* pid, const char *pszName, OGRSpatialReferenceSPtr poSRS)
{
    if(poSRS->FindProjParm(pszName) != wxNOT_FOUND)
    {
        double fParam = poSRS->GetNormProjParm(pszName);
        AppendProperty(pid, new wxFloatProperty(wxString(pszName, wxConvLocal), wxPG_LABEL, fParam));
    }
}

wxPGProperty* wxGISSpatialReferencePropertyPage::AppendProperty(wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->Append(pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}

wxPGProperty* wxGISSpatialReferencePropertyPage::AppendProperty(wxPGProperty* pid, wxPGProperty* pProp)
{
    wxPGProperty* pNewProp = m_pg->AppendIn(pid, pProp);
    pNewProp->ChangeFlag(wxPG_PROP_READONLY, 1);
    return pNewProp;
}
