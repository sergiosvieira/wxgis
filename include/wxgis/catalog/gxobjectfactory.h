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
#pragma once

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"

/** \class wxGxObjectFactory gxobjectfactory.h
    \brief A base class for GxObject factory.
*/
class WXDLLIMPEXP_GIS_CLT wxGxObjectFactory :
	public wxObject
{
	DECLARE_ABSTRACT_CLASS(wxGxObjectFactory)
public:
	wxGxObjectFactory(void);
	virtual ~wxGxObjectFactory(void);
    virtual void Serialize(wxXmlNode* const pConfig, bool bStore);
    virtual wxString GetClassName(void) const {return GetClassInfo()->GetClassName();};
    virtual wxString GetName(void) const = 0;
	virtual bool GetChildren(wxGxObject* pParent, char** &pFileNames, wxArrayLong & pChildrenIds) = 0;     
    virtual bool GetEnabled(void) const {return m_bIsEnabled;};
    virtual void SetEnabled(bool bIsEnabled){m_bIsEnabled = bIsEnabled;};
protected:
    bool m_bIsEnabled;
};

