/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCatalogUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/core/config.h"
#include "wxgis/framework/application.h"

#include "../../art/mainframe.xpm"
#include "../../art/process_working_16.xpm"
#include "../../art/process_working_48.xpm"

wxGxCatalogUI::wxGxCatalogUI(bool bFast) : wxGxCatalog()
{
	m_pSelection = new wxGxSelection();

    m_bOpenLastPath = true;
    m_bHasInternal = false;

	m_ImageListSmall.Create(16, 16);
	m_ImageListLarge.Create(48, 48);
    if(bFast)
        return;
    m_ImageListSmall.Add(wxBitmap(process_working_16_xpm));
    m_ImageListLarge.Add(wxBitmap(process_working_48_xpm));
}

wxGxCatalogUI::~wxGxCatalogUI()
{
}

void wxGxCatalogUI::Detach(void)
{
    if(!m_bHasInternal)
    {
        wxXmlNode* pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog")));
	    if(!pNode)
            pNode = m_pConf->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog")), true);
	    if(pNode)
        {
	        if(pNode->HasProp(wxT("open_last_path")))
		        pNode->DeleteProperty(wxT("open_last_path"));
	        pNode->AddProperty(wxT("open_last_path"), wxString::Format(wxT("%u"), m_bOpenLastPath));
        }

	    wxDELETE(m_pSelection);
        wxGxCatalog::Detach();
    }
    else
    {
	    wxXmlNode* pNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems")));
	    if(pNode)
            wxGISConfig::DeleteNodeChildren(pNode);

        SerializePlugins(pNode, true);

        for(size_t i = 0; i < m_ObjectFactoriesArray.size(); i++)
            m_ObjectFactoriesArray[i]->PutCatalogRef(m_pExtCat);

	    wxDELETE(m_pSelection);
	    EmptyChildren();
    	EmptyDisabledChildren();
    	wxDELETE(m_pConf);
    }
}

void wxGxCatalogUI::EditProperties(wxWindow *parent)
{
    //show options
    IApplication* pApp = GetApplication();
    if(pApp)
        pApp->OnAppOptions();
}

void wxGxCatalogUI::Init(IGxCatalog* pExtCat)
{
	if(m_bIsChildrenLoaded)
		return;

    m_pExtCat = pExtCat;
    #ifdef WXGISPORTABLE
	    m_pConf = new wxGISConfig(wxString(wxT("wxCatalogUI")), CONFIG_DIR, true);
    #else
	    m_pConf = new wxGISConfig(wxString(wxT("wxCatalogUI")), CONFIG_DIR);
    #endif

    if(pExtCat)
    {
        m_bHasInternal = true;
        GxObjectFactoryArray* poObjFactArr = pExtCat->GetObjectFactories();
        if(poObjFactArr)
        {
            for(size_t i = 0; i < poObjFactArr->size(); i++)
            {
                m_ObjectFactoriesArray.push_back(poObjFactArr->at(i));
                poObjFactArr->at(i)->PutCatalogRef(this);
            }
        }

         
	    //loads current user and when local machine items

        //IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pExtCat);
        //GxObjectArray* pGxObjectArray = pGxObjectContainer->GetChildren();
        //if(pGxObjectArray)
        //    for(size_t i = 0; i < pGxObjectArray->size(); i++)
        //        m_Children.push_back(pGxObjectArray->at(i));

	    wxXmlNode* pRootItemsNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems")));
	    LoadChildren(pRootItemsNode);
	    pRootItemsNode = m_pConf->GetConfigNode(enumGISHKLM, wxString(wxT("catalog/rootitems")));
	    LoadChildren(pRootItemsNode);

	    m_bShowHidden = pExtCat->GetShowHidden();
	    m_bShowExt = pExtCat->GetShowExt();
    }
    else
    {
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

	    wxXmlNode* pConfXmlNode = m_pConf->GetConfigNode(wxString(wxT("catalog")), false, true);
	    if(!pConfXmlNode)
		    return;

	    m_bShowHidden = wxAtoi(pConfXmlNode->GetPropVal(wxT("show_hidden"), wxT("0")));
	    m_bShowExt = wxAtoi(pConfXmlNode->GetPropVal(wxT("show_ext"), wxT("1")));
	    m_bOpenLastPath = wxAtoi(pConfXmlNode->GetPropVal(wxT("open_last_path"), wxT("1")));
    }
}

void wxGxCatalogUI::ObjectDeleted(long nObjectID)
{
    m_pSelection->RemoveDo(nObjectID);
    wxGxCatalog::ObjectDeleted(nObjectID);
}

IGxObject* wxGxCatalogUI::ConnectFolder(wxString sPath, bool bSelect)
{
    if(m_pGxDiscConnections)
    {
        IGxObject* pAddedObj = m_pGxDiscConnections->ConnectFolder(sPath);
        if(pAddedObj && bSelect)
        {
            m_pSelection->Select(pAddedObj->GetID(), false, IGxSelection::INIT_ALL);
            return pAddedObj;
        }
    }
    return NULL;
}

void wxGxCatalogUI::DisconnectFolder(CPLString sPath)
{
    if(m_pGxDiscConnections)
        m_pGxDiscConnections->DisconnectFolder(sPath);
}

void wxGxCatalogUI::SetLocation(wxString sPath)
{
    IGxObject* pObj = SearchChild(sPath);
	if(pObj)
		m_pSelection->Select(pObj->GetID(), false, IGxSelection::INIT_ALL);
	else
		ConnectFolder(sPath);
}

void wxGxCatalogUI::Undo(int nPos)
{
    if(m_pSelection->CanUndo())
    {
        long nID = m_pSelection->Undo(nPos);
        if(nID != wxNOT_FOUND && GxObjectMap[nID] != NULL)
		{
			m_pSelection->Select(nID, false, IGxSelection::INIT_ALL);
		}
    }
}

void wxGxCatalogUI::Redo(int nPos)
{
    if(m_pSelection->CanRedo())
    {
        long nID = m_pSelection->Redo(nPos);
        if(nID != wxNOT_FOUND && GxObjectMap[nID] != NULL)
		{
			m_pSelection->Select(nID, false, IGxSelection::INIT_ALL);
		}
    }
}

wxIcon wxGxCatalogUI::GetLargeImage(void)
{
    return wxNullIcon;
}

wxIcon wxGxCatalogUI::GetSmallImage(void)
{
    return wxIcon(mainframe_xpm);
}


