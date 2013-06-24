/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxXMLConnectionStorage class.
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

#include "wxgis/catalog/gxxmlconnstor.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/format.h"

//----------------------------------------------------------------------------
// wxGxXMLConnectionStorage
//----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGxXMLConnectionStorage, wxGxObjectContainer)

wxGxXMLConnectionStorage::wxGxXMLConnectionStorage() : wxGxObjectContainer()
{
}

wxGxXMLConnectionStorage::~wxGxXMLConnectionStorage(void)
{
}

void wxGxXMLConnectionStorage::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    //reread conn.xml file
    wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    if(event.GetPath().GetFullName().CmpNoCase(m_sXmlStorageName) == 0)
        LoadConnectionsStorage();

}

void wxGxXMLConnectionStorage::LoadConnectionsStorage(void)
{
    wxCriticalSectionLocker locker(m_oCritSect);

    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_sXmlStoragePath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();

        //check version
        int nVer = GetDecimalValue(pConnectionsNode, wxT("ver"), 1);
        if(nVer < GetStorageVersion())
        {
            DestroyChildren();
            wxGxCatalogBase* pGxCatalog = GetGxCatalog();
            if(pGxCatalog)
                pGxCatalog->ObjectChanged(GetId());
            CreateConnectionsStorage();
            return;
        }

 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

        wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
        std::map<long, bool> mnIds;

        wxGxObjectList::const_iterator iter;
        for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
        {
            wxGxObject *current = *iter;
             mnIds[current->GetId()] = false;
        }

		while(pConnectionNode)
		{
            bool bFoundKey = false;

            for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
            {
                wxGxObject *current = *iter;
                if(!current)
                    continue;

                if(IsObjectExist(current, pConnectionNode))
                {
                    bFoundKey = true;
                    mnIds[current->GetId()] = true;
                    break;
                }
            }

            if(!bFoundKey)
            {
                wxGxObject* pNewGxObject = CreateChildGxObject(pConnectionNode);
                //ObjectAdded event
                if(pGxCatalog)
                    pGxCatalog->ObjectAdded(pNewGxObject->GetId());
                mnIds[pNewGxObject->GetId()] = true;
            }

			pConnectionNode = pConnectionNode->GetNext();
		}

        if(mnIds.empty())//delete all items
        {
            if(GetChildren().GetCount() != 0)
            {
                DestroyChildren();
                if(pGxCatalog)
                    pGxCatalog->ObjectChanged(GetId());
            }
        }
        else
        {
            //delete items
            for(std::map<long, bool>::const_iterator i = mnIds.begin(); i != mnIds.end(); ++i)
            {
                if(pGxCatalog && i->second == false)
                {
                    if(DestroyChild(pGxCatalog->GetRegisterObject(i->first)))
                        pGxCatalog->ObjectDeleted(i->first);
                }
            }
        }
    }
    else
    {
        CreateConnectionsStorage();
    }
}
