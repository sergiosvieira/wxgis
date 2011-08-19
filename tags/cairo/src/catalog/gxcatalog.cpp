/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxevent.h"
#include "wxgis/core/globalfn.h"
#include "wxgis/core/config.h"

// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------


wxGxCatalog::wxGxCatalog(void) : m_bIsChildrenLoaded(false), m_pGxDiscConnections(NULL), m_nGlobalID(0)
{
	m_pCatalog = this;
	m_pParent = NULL;

    m_bShowHidden = false;
    m_bShowExt = true;

	m_pConf = NULL;

	RegisterObject(this); 
}

wxGxCatalog::~wxGxCatalog(void)
{
}

bool wxGxCatalog::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
	m_pParent = NULL;	
	m_pCatalog = this; 
	return true;
}

void wxGxCatalog::Detach(void)
{
	m_pPointsArray.clear();

	m_pCatalog->UnRegisterObject(m_nID);
	if(m_pConf)
	{
		wxXmlNode* pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
		if(!pNode)
			pNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog")), true);
		if(pNode)
		{
			if(pNode->HasAttribute(wxT("show_hidden")))
				pNode->DeleteAttribute(wxT("show_hidden"));
			pNode->AddAttribute(wxT("show_hidden"), wxString::Format(wxT("%u"), m_bShowHidden));
			if(pNode->HasAttribute(wxT("show_ext")))
				pNode->DeleteAttribute(wxT("show_ext"));
			pNode->AddAttribute(wxT("show_ext"), wxString::Format(wxT("%u"), m_bShowExt));
		}

		pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems")));
		if(pNode)
			wxGISConfig::DeleteNodeChildren(pNode);

		SerializePlugins(pNode, true);

		pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/objectfactories")));
		if(!pNode)
			pNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/objectfactories")), true);
		if(pNode)
		{
			wxGISConfig::DeleteNodeChildren(pNode);
			for(size_t i = m_ObjectFactoriesArray.size(); i > 0; i--)
			{
				wxXmlNode* pFactoryNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("objectfactory"));
				m_ObjectFactoriesArray[i - 1]->Serialize(pFactoryNode, true);
			}
		}
	}

	EmptyObjectFactories();
	EmptyChildren();
	EmptyDisabledChildren();
	wxDELETE(m_pConf);
}

void wxGxCatalog::Refresh(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
		m_Children[i]->Refresh();
	if(m_pCatalog)
		m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxCatalog::EmptyDisabledChildren(void)
{
	for(size_t i = 0; i < m_aRootItems.size(); ++i)
	{
		m_aRootItems[i]->Detach();
		delete m_aRootItems[i];
	}
	m_aRootItems.clear();
}

void wxGxCatalog::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		delete m_Children[i];
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxCatalog::EmptyObjectFactories(void)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
	{
		wxDELETE(m_ObjectFactoriesArray[i]);
	}
	m_ObjectFactoriesArray.clear();
}

void wxGxCatalog::Init(void)
{
	if(m_bIsChildrenLoaded)
		return;

#ifdef WXGISPORTABLE
	m_pConf = new wxGISConfig(wxString(wxT("wxCatalog")), CONFIG_DIR, true);
#else
	m_pConf = new wxGISConfig(wxString(wxT("wxCatalog")), CONFIG_DIR);
#endif

	//loads current user and when local machine items
	wxXmlNode* pObjectFactoriesNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/objectfactories")));
	LoadObjectFactories(pObjectFactoriesNode);
	pObjectFactoriesNode = m_pConf->GetConfigNode(enumGISHKLM, wxString(wxT("catalog/objectfactories")));
	LoadObjectFactories(pObjectFactoriesNode);

	//loads current user and when local machine items
	wxXmlNode* pRootItemsNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems")));
	LoadChildren(pRootItemsNode);
	pRootItemsNode = m_pConf->GetConfigNode(enumGISHKLM, wxString(wxT("catalog/rootitems")));
	LoadChildren(pRootItemsNode);
}

void wxGxCatalog::LoadObjectFactories(wxXmlNode* const pNode)
{
	if(pNode == NULL)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sName = pChildren->GetAttribute(wxT("factory_name"), wxT(""));

		for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
		{
            if(m_ObjectFactoriesArray[i]->GetClassName() == sName)
			{
				sName.Empty();
				break;
			}
		}

		if(!sName.IsEmpty())
		{
			wxObject *obj = wxCreateDynamicObject(sName);
			IGxObjectFactory *Factory = dynamic_cast<IGxObjectFactory*>(obj);
			if(Factory != NULL)
			{
				Factory->PutCatalogRef(this);
				Factory->Serialize(pChildren, false);
				m_ObjectFactoriesArray.push_back( Factory );
				wxLogMessage(_("wxGxCatalog: ObjectFactory %s initialize"), sName.c_str());
			}
			else
				wxLogError(_("wxGxCatalog: Error initializing ObjectFactory %s"), sName.c_str());
		}
		pChildren = pChildren->GetNext();
	}
}

void wxGxCatalog::LoadChildren(wxXmlNode* const pNode)
{
	if(!pNode)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sCatalogRootItemName = pChildren->GetAttribute(wxT("name"), NONAME);
        bool bIsEnabled = wxAtoi(pChildren->GetAttribute(wxT("is_enabled"), wxT("1"))) != 0;
        bool bContin = false;

		for(size_t i = 0; i < m_CatalogRootItemArray.Count(); ++i)
		{
			if(m_CatalogRootItemArray[i].IsSameAs(sCatalogRootItemName, false))
			{
				pChildren = pChildren->GetNext();
                bContin = true;
				break;
			}
		}

        if(bContin)
            continue;

        m_CatalogRootItemArray.Add(sCatalogRootItemName);

		//init plugin and add it
        wxObject *obj = wxCreateDynamicObject(sCatalogRootItemName);
		IGxObject *pGxObject = dynamic_cast<IGxObject*>(obj);
		if(pGxObject != NULL)
		{
            if(bIsEnabled)
            {
                if(AddChild(pGxObject))
                {
                    //m_CatalogRootItemArray.Add(sCatalogRootItemName);
                    IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                    if(pGxRootObjectProperties)
                    {
                        pGxRootObjectProperties->Init(pChildren);
                        pGxRootObjectProperties->SetEnabled(bIsEnabled);
                    }
                    wxGxDiscConnections* pGxDiscConnections = dynamic_cast<wxGxDiscConnections*>(pGxObject);
                    if(pGxDiscConnections)
                        m_pGxDiscConnections = pGxDiscConnections;

				    wxLogMessage(_("wxGxCatalog: Root Object %s initialize"), sCatalogRootItemName.c_str());
                }
            }
            else
            {
                pGxObject->Attach(this, this);
                IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                if(pGxRootObjectProperties)
                {
                    pGxRootObjectProperties->Init(pChildren);
                    pGxRootObjectProperties->SetEnabled(bIsEnabled);
                }
                m_aRootItems.push_back(pGxObject);
            }
		}
		else
			wxLogError(_("wxGxCatalog: Error initializing Root Object %s"), sCatalogRootItemName.c_str());

		pChildren = pChildren->GetNext();
	}
	m_bIsChildrenLoaded = true;
}

bool wxGxCatalog::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
	{
        if(m_ObjectFactoriesArray[i]->GetEnabled())
			if(!m_ObjectFactoriesArray[i]->GetChildren(sParentDir, pFileNames, ObjArray))
				return false;
	}
	return true;
}

void wxGxCatalog::ObjectDeleted(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_DELETED, nObjectID);
	PostEvent(event);
}

void  wxGxCatalog::ObjectAdded(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_ADDED, nObjectID);
	PostEvent(event);
}

void  wxGxCatalog::ObjectChanged(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_CHANGED, nObjectID);
	PostEvent(event);
}

void  wxGxCatalog::ObjectRefreshed(long nObjectID)
{
	wxGxCatalogEvent event(wxGXOBJECT_REFRESHED, nObjectID);
	PostEvent(event);
}
//
//void wxGxCatalog::PostEvent(wxGxCatalogEvent &event)
//{
//	for(size_t i = 0; i < m_pPointsArray.size(); ++i)
//	{
//		if(m_pPointsArray[i] != NULL)
//			m_pPointsArray[i]->AddPendingEvent(event);
//	}
//}

IGxObject* wxGxCatalog::ConnectFolder(wxString sPath, bool bSelect)
{
    if(m_pGxDiscConnections)
    {
        IGxObject* pAddedObj = m_pGxDiscConnections->ConnectFolder(sPath);
        if(pAddedObj)
            return pAddedObj;
    }
    return NULL;
}

IGxObject* wxGxCatalog::SearchChild(wxString sPath)
{
    IGxObject* pOutObj = IGxObjectContainer::SearchChild(sPath);
    if(pOutObj)
        return pOutObj;

    wxFileName oName(sPath);
    //the container is exist but item is absent
    pOutObj = IGxObjectContainer::SearchChild(oName.GetPath());
    if(pOutObj)
        return NULL;
    //try connect path & search child
    IGxObjectContainer* poObjCont = dynamic_cast<IGxObjectContainer*>(ConnectFolder(oName.GetPath(), false));
    if(poObjCont == this)
        return NULL;
    if(!poObjCont)
        return poObjCont;
    return poObjCont->SearchChild(sPath);
}


void wxGxCatalog::DisconnectFolder(CPLString sPath)
{
    if(m_pGxDiscConnections)
        m_pGxDiscConnections->DisconnectFolder(sPath);
}

wxString wxGxCatalog::ConstructFullName(IGxObject* pObject)
{
	wxString sParentPath = pObject->GetParent()->GetFullName();
    wxString sName;
    if(m_bShowExt)
        sName = pObject->GetName();
    else
        sName = pObject->GetBaseName();

	if(sParentPath.IsEmpty())
		return sName;
	else
	{
		if(sParentPath[sParentPath.Len() - 1] == wxFileName::GetPathSeparator())
			return sParentPath + sName;
		else
			return sParentPath + wxFileName::GetPathSeparator() + sName;
	}
}

void wxGxCatalog::SerializePlugins(wxXmlNode* const pNode, bool bStore)
{
	if(bStore)
	{
        for(size_t i = 0; i < m_Children.size(); ++i)
		{
            IGxRootObjectProperties* pProps = dynamic_cast<IGxRootObjectProperties*>(m_Children[i]);
            if(!pProps)
                continue;
            wxXmlNode* pConfigNode = pProps->GetAttributes();
            if(!pConfigNode)
                continue;

            pNode->AddChild(pConfigNode);
        }
        for(size_t i = 0; i < m_aRootItems.size(); ++i)
		{
            IGxRootObjectProperties* pProps = dynamic_cast<IGxRootObjectProperties*>(m_aRootItems[i]);
            if(!pProps)
                continue;
            wxXmlNode* pConfigNode = pProps->GetAttributes();
            if(!pConfigNode)
                continue;

            pNode->AddChild(pConfigNode);
        }
    }
}

void wxGxCatalog::EnableRootItem(IGxObject* pRootItem, bool bEnable)
{
    IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pRootItem);
    pGxRootObjectProperties->SetEnabled(bEnable);

    if(bEnable) //enable
    {
        for(size_t i = 0; i < m_aRootItems.size(); ++i)
        {
            if(m_aRootItems[i] == pRootItem)
            {
                m_aRootItems.erase(m_aRootItems.begin() + i);
                break;
            }
        }
        m_Children.push_back(pRootItem);
        ObjectAdded(pRootItem->GetID());
    }
    else        //disable
    {
        for(size_t i = 0; i < m_Children.size(); ++i)
        {
            if(m_Children[i] == pRootItem)
            {
                m_Children.erase(m_Children.begin() + i);
                ObjectDeleted(pRootItem->GetID());
                break;
            }
        }
        m_aRootItems.push_back(pRootItem);
    }
}

void wxGxCatalog::RegisterObject(IGxObject* pObj)
{
	wxCriticalSectionLocker locker(m_RegCritSect);
	pObj->SetID(m_nGlobalID);
	GxObjectMap[m_nGlobalID] = pObj;
	m_nGlobalID++;
}

void wxGxCatalog::UnRegisterObject(long nID)
{
	wxCriticalSectionLocker locker(m_RegCritSect);
	GxObjectMap[nID] = NULL;
}

IGxObjectSPtr wxGxCatalog::GetRegisterObject(long nID)
{
	wxCriticalSectionLocker locker(m_RegCritSect);
	if(wxNOT_FOUND == nID)
		return IGxObjectSPtr();

	if(GxObjectMap[nID] != NULL)
	{
		//if(GxObjectMap[nID]->GetID() != nID)
		//	return IGxObjectSPtr();
		GxObjectMap[nID]->Lock();
		return IGxObjectSPtr(GxObjectMap[nID], GxObjectDeleter);
	}
	return IGxObjectSPtr();
}