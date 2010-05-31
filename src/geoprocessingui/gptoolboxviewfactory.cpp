/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxToolboxViewFactory class.
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

#include "wxgis/geoprocessingui/gptoolboxviewfactory.h"
#include "wxgis/geoprocessingui/gptoolboxview.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxToolboxViewFactory, wxObject)


wxGxToolboxViewFactory::wxGxToolboxViewFactory(void)
{
}

wxGxToolboxViewFactory::~wxGxToolboxViewFactory(void)
{
}

wxWindow* wxGxToolboxViewFactory::CreateView(wxString sName, wxWindow* parent)
{
    if(sName.CmpNoCase(wxT("ToolboxView")) == 0)
	{
		wxGxToolboxView* pwxGxToolboxView = new wxGxToolboxView(parent);
		return static_cast<wxWindow*>(pwxGxToolboxView);
	}
	return NULL;
}