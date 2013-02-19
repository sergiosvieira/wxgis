/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectFactory class. Base abstract class for all factories
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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
#include "wxgis/catalog/gxobjectfactory.h"
#include "wxgis/core/format.h"

//-----------------------------------------------------------------------------
// wxGxObjectFactory
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGxObjectFactory, wxObject)

wxGxObjectFactory::wxGxObjectFactory(void)
{
}

wxGxObjectFactory::~wxGxObjectFactory(void)
{
}

void wxGxObjectFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
{
    if(bStore)
    {
        if(pConfig->HasAttribute(wxT("factory_name")))
            pConfig->DeleteAttribute(wxT("factory_name"));
        pConfig->AddAttribute(wxT("factory_name"), GetClassName());  
        if(pConfig->HasAttribute(wxT("is_enabled")))
            pConfig->DeleteAttribute(wxT("is_enabled"));
        SetBoolValue(pConfig, wxT("is_enabled"), m_bIsEnabled);    
    }
    else
    {
        m_bIsEnabled = GetBoolValue(pConfig, wxT("is_enabled"), true);
    }
}

