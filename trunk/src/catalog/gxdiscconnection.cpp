/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2012  Bishop
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
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/format.h"

//---------------------------------------------------------------------------
// wxGxDiscConnection
//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnection, wxGxFolder)

wxGxDiscConnection::wxGxDiscConnection(void) : wxGxFolder()
{
}

wxGxDiscConnection::wxGxDiscConnection(wxGxObject *oParent, const wxString &soXmlConfPath, int nXmlId, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
    m_nXmlId = nXmlId;
    m_soXmlConfPath = soXmlConfPath;
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

bool wxGxDiscConnection::Delete(void)
{
    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_soXmlConfPath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

		while(pConnectionNode)
		{
            int nNodeXmlId = GetDecimalValue(pConnectionNode, wxT("id"), wxNOT_FOUND);
            if(nNodeXmlId == m_nXmlId)
            {
                if(pConnectionsNode->RemoveChild(pConnectionNode))
                {
                    wxDELETE(pConnectionNode);
                    break;
                }
            }
            pConnectionNode = pConnectionNode->GetNext();
		}
        return doc.Save(m_soXmlConfPath);
    }
	return false;
}

bool wxGxDiscConnection::Rename(const wxString& NewName)
{
    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_soXmlConfPath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

		while(pConnectionNode)
		{
            int nNodeXmlId = GetDecimalValue(pConnectionNode, wxT("id"), wxNOT_FOUND);
            if(nNodeXmlId == m_nXmlId)
            {
                pConnectionNode->DeleteAttribute(wxT("name"));
                pConnectionNode->AddAttribute(wxT("name"), NewName);
            }
            pConnectionNode = pConnectionNode->GetNext();
		}
        return doc.Save(m_soXmlConfPath);
    }
	return false;
}

