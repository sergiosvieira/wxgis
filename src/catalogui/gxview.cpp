/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxView class.
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalog/gxcatalog.h"

//-----------------------------------------------
// wxGxView
//-----------------------------------------------

wxGxView::wxGxView(void)
{
}

wxGxView::~wxGxView(void)
{
}

bool wxGxView::Activate(wxGxApplication* application, IGxCatalog* Catalog, wxXmlNode* pConf)
{ 
	m_pCatalog = Catalog; 
	m_pApplication = application; 
	m_pXmlConf = pConf;
	return true; 
}

void wxGxView::Deactivate(void)
{
	m_pCatalog = NULL;
	m_pApplication = NULL;
	m_pXmlConf = NULL;
}

bool wxGxView::Applies(IGxSelection* Selection)
{ 
	return NULL == Selection ? false : true; 
};

wxString wxGxView::GetName(void)
{
	return m_sViewName;
}