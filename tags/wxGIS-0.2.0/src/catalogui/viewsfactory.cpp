/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISViewsFactory class.
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
#include "wxgis/catalogui/viewsfactory.h"
#include "wxgis/catalogui/gxnoview.h"
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalogui/gxtableview.h"
#include "wxgis/catalogui/gxmapview.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISViewsFactory, wxObject)


wxGISViewsFactory::wxGISViewsFactory(void)
{
}

wxGISViewsFactory::~wxGISViewsFactory(void)
{
}

wxWindow* wxGISViewsFactory::CreateView(wxString sName, wxWindow* parent)
{
	if(sName.CmpNoCase(wxT("NoView")) == 0)
	{
		wxGxNoView* pwxGxNoView = new wxGxNoView(parent, wxID_ANY);
		return static_cast<wxWindow*>(pwxGxNoView);//wxStaticCast(pwxGxNoView, wxWindow);//
	}
	if(sName.CmpNoCase(wxT("ContentsView")) == 0)
	{
		wxGxContentView* pwxGxContentView = new wxGxContentView(parent);
		return static_cast<wxWindow*>(pwxGxContentView);//wxStaticCast(pwxGxContentView, wxWindow);//
	}
	if(sName.CmpNoCase(wxT("GeographicView")) == 0)
	{
		wxGxMapView* pwxGxMapView = new wxGxMapView(parent);
		return static_cast<wxWindow*>(pwxGxMapView);//wxStaticCast(pwxGxMapView, wxWindow);//
	}
	if(sName.CmpNoCase(wxT("TableView")) == 0)
	{
		wxGxTableView* pwxGxTableView = new wxGxTableView(parent);
		return static_cast<wxWindow*>(pwxGxTableView);//wxStaticCast(pwxGxTableView, wxWindow);//
	}
	return NULL;
}
