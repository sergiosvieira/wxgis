/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalog class.
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
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/core/config.h"
#include <wx/volume.h>
#include <wx/msgdlg.h>

// ----------------------------------------------------------------------------
// wxGxCatalog
// ----------------------------------------------------------------------------


wxGxCatalog::wxGxCatalog(void) : IGxCatalog(), m_bIsChildrenLoaded(false)
{
	m_pSelection = new wxGxSelection();
	m_pCatalog = NULL;
	m_pParent = NULL;
}

wxGxCatalog::~wxGxCatalog(void)
{
	Detach();
}

void wxGxCatalog::Detach(void)
{
	wxXmlNode* pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems")));
	if(pNode)
        wxGISConfig::DeleteNodeChildren(pNode);

	//{
	//	wxXmlNode* pChildren = pNode->GetChildren();
	//	while(pChildren)
	//	{
	//		if(pChildren->GetPropVal(wxT("name"), NONAME) == DISCCONNCAT)
	//		{
	//			SerializeDiscConnections(pChildren, true);
	//			//break;
	//		}
	//		//else
	//		//{
	//		//	SerializePlugins(pChildren, true);
	//		//	//break;
	//		//}
	//		pChildren = pChildren->GetNext();
	//	}
	//}
	//else

	SerializeDiscConnections(NULL, true);
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

	//SetShowExt(m_bShowExt);
	//SetShowHidden(m_bShowHidden);

	EmptyObjectFactories();
	wxDELETE(m_pSelection);
	EmptyChildren();
	wxDELETE(m_pConf);
}

void wxGxCatalog::Refresh(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
		m_Children[i]->Refresh();
    m_pCatalog->ObjectRefreshed(this);
}

void wxGxCatalog::EditProperties(wxWindow *parent)
{
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

void wxGxCatalog::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        if(m_pCatalog)
        {
            IGxSelection* pSel = m_pCatalog->GetSelection();
            if(pSel)
                m_pCatalog->GetSelection()->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
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

	wxXmlNode* pConfXmlNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
	if(!pConfXmlNode)
		pConfXmlNode = m_pConf->GetConfigNode(enumGISHKLM, wxString(wxT("catalog")));
	if(!pConfXmlNode)
		return;

	m_bShowHidden = wxAtoi(pConfXmlNode->GetPropVal(wxT("show_hidden"), wxT("0")));
	m_bShowExt = wxAtoi(pConfXmlNode->GetPropVal(wxT("show_ext"), wxT("1")));

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
            if(m_ObjectFactoriesArray[i]->GetName() == sName)
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
        if(!bIsEnabled)
		{
			pChildren = pChildren->GetNext();
			continue;		
		}

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

		if(sCatalogRootItemName.IsSameAs(DISCCONNCAT, false))
		{
			SerializeDiscConnections(pChildren);
		}
		else
		{
			//init plugin and add it
            wxObject *obj = wxCreateDynamicObject(sCatalogRootItemName);
			IGxObject *pGxObject = dynamic_cast<IGxObject*>(obj);
			if(pGxObject != NULL)
			{
                if(AddChild(pGxObject))
                {
                    //m_CatalogRootItemArray.Add(sCatalogRootItemName);
                    IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                    if(pGxRootObjectProperties)
                        pGxRootObjectProperties->Init(pChildren);
    				wxLogMessage(_("wxGxCatalog: Root Object %s initialize"), sCatalogRootItemName.c_str());	    			
                }
			}
			else
				wxLogError(_("wxGxCatalog: Error initializing Root Object %s"), sCatalogRootItemName.c_str());
		}
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

void wxGxCatalog::SetShowHidden(bool bShowHidden)
{
	IGxCatalog::SetShowHidden(bShowHidden);
	wxXmlNode* pConfXmlNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog")), true);
	if(pConfXmlNode->HasProp(wxT("show_hidden")))
		pConfXmlNode->DeleteProperty(wxT("show_hidden"));
	pConfXmlNode->AddProperty(wxT("show_hidden"), wxString::Format(wxT("%u"), bShowHidden));
}

void wxGxCatalog::SetShowExt(bool bShowExt)
{
	IGxCatalog::SetShowExt(bShowExt);
	wxXmlNode* pConfXmlNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog")), true);
	if(pConfXmlNode->HasProp(wxT("show_ext")))
		pConfXmlNode->DeleteProperty(wxT("show_ext"));
	pConfXmlNode->AddProperty(wxT("show_ext"), wxString::Format(wxT("%u"), bShowExt));
}

void wxGxCatalog::ObjectDeleted(IGxObject* pObject)
{	
	//if selected remove from m_Selection
	//if(m_pSelection != NULL)
	//	m_pSelection->Unselect(pObject, IGxSelection::INIT_ALL);
    
    m_pSelection->RemoveDo(pObject->GetFullName());

	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);

	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectDeleted(pObject);
	}
}

void  wxGxCatalog::ObjectAdded(IGxObject* pObject)
{
	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectAdded(pObject);
	}
}

void  wxGxCatalog::ObjectChanged(IGxObject* pObject)
{
	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
	for(size_t i = 0; i < m_pPointsArray.size(); i++)
	{
		IGxCatalogEvents* pGxCatalogEvents = dynamic_cast<IGxCatalogEvents*>(m_pPointsArray[i]);
		if(pGxCatalogEvents != NULL)
			pGxCatalogEvents->OnObjectChanged(pObject);
	}
}

void  wxGxCatalog::ObjectRefreshed(IGxObject* pObject)
{
	wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
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

void wxGxCatalog::SerializeDiscConnections(wxXmlNode* pNode, bool bStore)
{
	if(bStore)
	{
		pNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems/rootitem")), false);
		pNode->AddProperty(wxT("name"), DISCCONNCAT);
		pNode->AddProperty(wxT("scan_once"), wxT("1"));
		for(std::map<wxString, IGxObject*>::const_iterator IT = m_DiscConnections.begin(); IT != m_DiscConnections.end(); ++IT)
		{
			wxGxDiscConnection* pConn = dynamic_cast<wxGxDiscConnection*>(IT->second);
			if(pConn)
			{
				wxXmlNode* pDiscConn = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
				pDiscConn->AddProperty(wxT("name"), pConn->GetName());
				pDiscConn->AddProperty(wxT("path"), pConn->GetPath());
			}
		}
	}
	else
	{
		short bScanOnce = wxAtoi(pNode->GetPropVal(wxT("scan_once"), wxT("0")));
		if(bScanOnce == 0)
		{
            wxLogMessage(_("wxGxCatalog: Procceed scan disc connections"));
            wxArrayString arr;
#ifdef WIN32
			arr = wxFSVolumeBase::GetVolumes(wxFS_VOL_MOUNTED, wxFS_VOL_REMOVABLE | wxFS_VOL_REMOTE);
#else
            //linux paths
            wxStandardPaths stp;
            arr.Add(wxT("/"));
            arr.Add(stp.GetUserConfigDir());
            arr.Add(stp.GetDataDir());
#endif		
            for(size_t i = 0; i < arr.size(); i++)
			{
				if(m_DiscConnections[arr[i]] != NULL)
					continue;
				wxGxDiscConnection* pwxGxDiscConnection = new wxGxDiscConnection(arr[i], arr[i], m_bShowHidden);
				IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
				if(AddChild(pGxObject))
                {
					m_DiscConnections[arr[i]] = pGxObject;
                    wxLogMessage(_("wxGxCatalog: Add new disc connection [%s]"), pGxObject->GetName().c_str());
                }
			}
		}
		else
		{
			wxXmlNode* pDiscConn = pNode->GetChildren();
			while(pDiscConn)
			{
				wxString sName = pDiscConn->GetPropVal(wxT("name"), NONAME);
				wxString sPath = pDiscConn->GetPropVal(wxT("path"), ERR);
				if(m_DiscConnections[sPath] != NULL)
					sPath = ERR;
				if(sPath != ERR)
				{
					wxGxDiscConnection* pwxGxDiscConnection = new wxGxDiscConnection(sPath, sName, m_bShowHidden);
					IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
					if(AddChild(pGxObject))
                    {
						m_DiscConnections[sPath] = pGxObject;
                        wxLogMessage(_("wxGxCatalog: Add disc connection [%s]"), sName.c_str());
                    }
				}
				pDiscConn = pDiscConn->GetNext();
			}
		}
	}
}

IGxObject* wxGxCatalog::ConnectFolder(wxString sPath)
{
    IGxObject* pReturnObj(NULL);
	if(m_DiscConnections[sPath] != NULL)
		pReturnObj = m_DiscConnections[sPath];
    else if(wxDir::Exists(sPath)) 	//check if path is valid
	{
	    wxGxDiscConnection* pwxGxDiscConnection = new wxGxDiscConnection(sPath, sPath, m_bShowHidden);
	    pReturnObj = static_cast<IGxObject*>(pwxGxDiscConnection);
	    if(AddChild(pReturnObj))
	    {
		    m_DiscConnections[sPath] = pReturnObj;
		    ObjectAdded(pReturnObj);
	    }
        else
		    pReturnObj = this;
    }
    else
    {
		//wxMessageBox(_("The directory is not exist!"), _("Error"), wxOK | wxICON_ERROR );
		pReturnObj = this;
    }
    m_pSelection->Select(pReturnObj, false, IGxSelection::INIT_ALL);
	return pReturnObj;
}

IGxObject* wxGxCatalog::SearchChild(wxString sPath)
{
    IGxObject* pOutObj = IGxObjectContainer::SearchChild(sPath);
    if(pOutObj)
        return pOutObj;
    //try to connect parents obj
    wxFileName oName(sPath);
    //if(oName.IsDir())
    //    return NULL;
    IGxObjectContainer* poObjCont = dynamic_cast<IGxObjectContainer*>(ConnectFolder(oName.GetPath()));
    if(poObjCont == this)
        return NULL;
    if(!poObjCont)
        return poObjCont;
    return poObjCont->SearchChild(sPath);
}


void wxGxCatalog::DisconnectFolder(wxString sPath)
{
	if(m_DiscConnections[sPath] != NULL)
	{
        IGxObject* pGxObj = m_DiscConnections[sPath]->GetParent();
		ObjectDeleted(m_DiscConnections[sPath]);
		DeleteChild(m_DiscConnections[sPath]);
        m_pSelection->Select(pGxObj, false, IGxSelection::INIT_ALL);
		m_DiscConnections[sPath] = NULL;
	}
}

void wxGxCatalog::SetLocation(wxString sPath)
{
	IGxObject* pObj = SearchChild(sPath);
	if(pObj)
		m_pSelection->Select(pObj, false, IGxSelection::INIT_ALL);
	else
		ConnectFolder(sPath);
}

void wxGxCatalog::Undo(int nPos)
{
    if(m_pSelection->CanUndo())
    {
        wxString sPath = m_pSelection->Undo(nPos);
        if(!sPath.IsEmpty())
            SetLocation(sPath);
    }
}

void wxGxCatalog::Redo(int nPos)
{
    if(m_pSelection->CanRedo())
    {
        wxString sPath = m_pSelection->Redo(nPos);
        if(!sPath.IsEmpty())
           SetLocation(sPath);
    }
}

wxString wxGxCatalog::ConstructFullName(IGxObject* pObject)
{
	wxString sParentPath = pObject->GetParent()->GetFullName();
	if(sParentPath.IsEmpty())
		return pObject->GetName();
	else
	{
		if(sParentPath[sParentPath.Len() - 1] == wxFileName::GetPathSeparator())
			return sParentPath + pObject->GetName();
		else
			return sParentPath + wxFileName::GetPathSeparator() + pObject->GetName();
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
            //wxObject* pObj = dynamic_cast<wxObject*>(m_Children[i]);
            //if(!pObj)
            //    continue;
            //wxClassInfo* pClInfo = pObj->GetClassInfo();
            //if(!pClInfo)
            //    continue;            

    		//pNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems/rootitem")), false);
	    	//pNode->AddProperty(wxT("name"), pClInfo->GetClassName());
	    	//pNode->AddProperty(wxT("is_enabled"), wxT("1"));
            //wxXmlProperty* pProp = pConfigNode->GetProperties();
            //while(pProp)
            //{
            //    pNode->AddProperty(pProp);
            //    pProp = pProp->GetNext();
            //}
            //pNode->SetProperties(pConfigNode->GetProperties());
            //pNode->SetChildren(pConfigNode->GetChildren());

            //const wxXmlNode& node = *pConfigNode;
            //wxXmlNode* pNewNode = new wxXmlNode(node);
            pNode->AddChild(pConfigNode);
        }
    }
}