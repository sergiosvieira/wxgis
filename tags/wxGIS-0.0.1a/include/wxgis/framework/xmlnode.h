/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISXmlNode class. Some useful xml functions
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008  Bishop
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

#include "wxgis/framework/framework.h"

class WXDLLIMPEXP_GIS_FRW wxGISXmlNode :
	public wxXmlNode
{
public:
	wxGISXmlNode(const wxXmlNode& node);
	virtual ~wxGISXmlNode(void);
	static wxXmlNode* GetChildNodeByName(wxXmlNode* pNode, wxString sName)
	{
		wxXmlNode* pChild = pNode->GetChildren();
		while(pChild)
		{
			if(pChild->GetName() == sName)
				return pChild;
			pChild = pChild->GetNext();
		}
		return NULL;
	}
	virtual wxXmlNode* GetChildNodeByName(wxString sName)
	{
		wxXmlNode* pChild = GetChildren();
		while(pChild)
		{
			if(pChild->GetName() == sName)
				return pChild;
			pChild = pChild->GetNext();
		}
		return NULL;
	}
	static void DeleteNodeChildren(wxXmlNode* pNode);
};
