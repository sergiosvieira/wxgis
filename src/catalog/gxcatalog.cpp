/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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
#include "wxgis/core/config.h"

// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------


wxGxCatalog::wxGxCatalog(void) : IGxCatalog(), m_bIsChildrenLoaded(false), m_pGxDiscConnections(NULL)
{
	m_pCatalog = NULL;
	m_pParent = NULL;

    m_bShowHidden = false;
    m_bShowExt = true;
}

wxGxCatalog::~wxGxCatalog(void)
{
}

void wxGxCatalog::Detach(void)
{
    wxXmlNode* pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
	if(!pNode)
        pNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog")), true);
	if(pNode)
    {
	    if(pNode->HasProp(wxT("show_hidden")))
		    pNode->DeleteProperty(wxT("show_hidden"));
	    pNode->AddProperty(wxT("show_hidden"), wxString::Format(wxT("%u"), m_bShowHidden));
	    if(pNode->HasProp(wxT("show_ext")))
		    pNode->DeleteProperty(wxT("show_ext"));
	    pNode->AddProperty(wxT("show_ext"), wxString::Format(wxT("%u"), m_bShowExt));
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

	EmptyObjectFactories();
	EmptyChildren();
	EmptyDisabledChildren();
	wxDELETE(m_pConf);
}

void wxGxCatalog::Refresh(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
		m_Children[i]->Refresh();
	if(m_pCatalog)
		m_pCatalog->ObjectRefreshed(this);
}

bool wxGxCatalog::AddChild(IGxObject* pChild)
{
	if(pChild == NULL)
		return false;
	pChild->Attach(this, this);
	m_Children.push_back(pChild);
	return true;
}

bool wxGxCatalog::DeleteChild(IGxObject* pChild)
{
	if(pChild == NULL)
		return false;
	GxObjectArray::iterator pos = std::find(m_Children.begin(), m_Children.end(), pChild);
	if(pos != m_Children.end())
	{
		pChild->Detach();
		delete pChild;
		m_Children.erase(pos);
	}
	return true;
}

void wxGxCatalog::EmptyDisabledChildren(void)
{
	for(size_t i = 0; i < m_aRootItems.size(); i++)
	{
		m_aRootItems[i]->Detach();
		delete m_aRootItems[i];
	}
	m_aRootItems.clear();
}

void wxGxCatalog::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		delete m_Children[i];
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxCatalog::EmptyObjectFactories(void)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); i++)
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

void wxGxCatalog::LoadObjectFactories(wxXmlNode* pNode)
{
	if(pNode == NULL)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sName = pChildren->GetPropVal(wxT("factory_name"), wxT(""));

		for(size_t i = 0; i < m_ObjectFactoriesArray.size(); i++)
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

void wxGxCatalog::LoadChildren(wxXmlNode* pNode)
{
	if(!pNode)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sCatalogRootItemName = pChildren->GetPropVal(wxT("name"), NONAME);
        bool bIsEnabled = wxAtoi(pChildren->GetPropVal(wxT("is_enabled"), wxT("1")));
        bool bContin = false;

		for(size_t i = 0; i < m_CatalogRootItemArray.Count(); i++)
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

bool wxGxCatalog::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	for(size_t i = 0; i < m_ObjectFactoriesArray.size(); i++)
	{
        if(m_ObjectFactoriesArray[i]->GetEnabled())
			if(!m_ObjectFactoriesArray[i]->GetChildren(sParentDir, pFileNames, pObjArray))
				return false;
	}
	return true;
}

void wxGxCatalog::ObjectDeleted(IGxObject* pObject)
{
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectDeleted(pObject);
	}
}

void  wxGxCatalog::ObjectAdded(IGxObject* pObject)
{
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectAdded(pObject);
	}
}

void  wxGxCatalog::ObjectChanged(IGxObject* pObject)
{
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectChanged(pObject);
	}
}

void  wxGxCatalog::ObjectRefreshed(IGxObject* pObject)
{
	//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectRefreshed(pObject);
	}
}

long wxGxCatalog::Advise(wxObject* pObject)
{
	IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(pObject);
	if(pGxCatalogEvents == NULL)
		return -1;
	return IConnectionPointContainer::Advise(pObject);
}

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


void wxGxCatalog::DisconnectFolder(wxString sPath, bool bSelect)
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

void wxGxCatalog::SerializePlugins(wxXmlNode* pNode, bool bStore)
{
	if(bStore)
	{
        for(size_t i = 0; i < m_Children.size(); i++)
		{
            IGxRootObjectProperties* pProps = dynamic_cast<IGxRootObjectProperties*>(m_Children[i]);
            if(!pProps)
                continue;
            wxXmlNode* pConfigNode = pProps->GetProperties();
            if(!pConfigNode)
                continue;

            pNode->AddChild(pConfigNode);
        }
        for(size_t i = 0; i < m_aRootItems.size(); i++)
		{
            IGxRootObjectProperties* pProps = dynamic_cast<IGxRootObjectProperties*>(m_aRootItems[i]);
            if(!pProps)
                continue;
            wxXmlNode* pConfigNode = pProps->GetProperties();
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
        for(size_t i = 0; i < m_aRootItems.size(); i++)
        {
            if(m_aRootItems[i] == pRootItem)
            {
                m_aRootItems.erase(m_aRootItems.begin() + i);
                break;
            }
        }
        m_Children.push_back(pRootItem);
        ObjectAdded(pRootItem);
    }
    else        //disable
    {
        for(size_t i = 0; i < m_Children.size(); i++)
        {
            if(m_Children[i] == pRootItem)
            {
                m_Children.erase(m_Children.begin() + i);
                ObjectDeleted(pRootItem);
                break;
            }
        }
        m_aRootItems.push_back(pRootItem);
    }
}
