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
#pragma once

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/datasource/rasterdataset.h"

#include "wx/propgrid/propgrid.h"

//class wxTextAndButtonProperty : public wxStringProperty
//{
//    WX_PG_DECLARE_PROPERTY_CLASS(wxTextAndButtonProperty)
//public:
//    wxTextAndButtonProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString, wxWindowID buttonid = wxID_ANY, const wxString& buttonCaption = wxEmptyString );
//    virtual ~wxTextAndButtonProperty ();
//
//    //void OnCustomPaint( wxDC& dc, const wxRect& rect, wxPGPaintData& pd );
//
//    WX_PG_DECLARE_CUSTOM_PAINT_METHODS()
//protected:
//    wxButton* m_button;
//    wxWindowID m_buttonid;
//    wxString m_buttonCaption;
//};



class WXDLLIMPEXP_GIS_CLU wxGISRasterPropertyPage : 
    public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterPropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 45
	};

public:
    wxGISRasterPropertyPage(void);
    wxGISRasterPropertyPage(wxGISDataset* pDataset, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
	~wxGISRasterPropertyPage();
    virtual bool Create(wxGISDataset* pDataset, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
    virtual wxString GetPageName(void){return wxString(_("Raster"));};
    wxPGId AppendProperty(wxPGProperty* pProp);
    wxPGId AppendProperty(wxPGId pid, wxPGProperty* pProp);
    wxPGId AppendMetadataProperty(wxString sMeta);
protected:
    wxGISRasterDataset* m_pDataset;
    wxPropertyGrid* m_pg;
};