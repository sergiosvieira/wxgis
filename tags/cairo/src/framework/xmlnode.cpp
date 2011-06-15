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
#include "wxgis/framework/xmlnode.h"

wxGISXmlNode::wxGISXmlNode(const wxXmlNode& node) : wxXmlNode(node)
{
}

wxGISXmlNode::~wxGISXmlNode(void)
{
}

void wxGISXmlNode::DeleteNodeChildren(wxXmlNode* pNode)
{
	wxXmlNode* pChild = pNode->GetChildren();
	while(pChild)
	{
		wxXmlNode* pDelChild = pChild;
		pChild = pChild->GetNext();
		pNode->RemoveChild(pDelChild);
		wxDELETE(pDelChild);
	}
}
