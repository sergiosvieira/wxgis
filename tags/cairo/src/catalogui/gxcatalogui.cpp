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
#include "wxgis/core/globalfn.h"

#include "../../art/mainframecat.xpm"
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
    if(m_bHasInternal)
    {
		wxGISAppConfigSPtr pConfig = GetConfig();
		if(pConfig)
		{
			wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/rootitems")));
			if(pNode)
			{
				pConfig->DeleteNodeChildren(pNode);
				SerializePlugins(pNode, true);
			}
		}

		for(size_t i = 0; i < m_ObjectFactoriesArray.size(); ++i)
			m_ObjectFactoriesArray[i]->PutCatalogRef(m_pExtCat);

	    wxDELETE(m_pSelection);
	    EmptyChildren();
    	EmptyDisabledChildren();
    }
    else
    {
		wxGISAppConfigSPtr pConfig = GetConfig();
		if(pConfig)
			pConfig->Write(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/open_last_path")), m_bOpenLastPath);

	    wxDELETE(m_pSelection);
        wxGxCatalog::Detach();
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

    if(pExtCat)
    {
        m_bHasInternal = true;
        GxObjectFactoryArray* poObjFactArr = pExtCat->GetObjectFactories();
        if(poObjFactArr)
        {
            for(size_t i = 0; i < poObjFactArr->size(); ++i)
            {
                m_ObjectFactoriesArray.push_back(poObjFactArr->at(i));
                poObjFactArr->at(i)->PutCatalogRef(this);
            }
        }

         
	    //loads current user and when local machine items

        //IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(pExtCat);
        //GxObjectArray* pGxObjectArray = pGxObjectContainer->GetChildren();
        //if(pGxObjectArray)
        //    for(size_t i = 0; i < pGxObjectArray->size(); ++i)
        //        m_Children.push_back(pGxObjectArray->at(i));

	    LoadChildren();

	    m_bShowHidden = pExtCat->GetShowHidden();
	    m_bShowExt = pExtCat->GetShowExt();
    }
    else
    {
	    LoadObjectFactories();
	    LoadChildren();

		wxGISAppConfigSPtr pConfig = GetConfig();
		if(!pConfig)
			return;

		m_bShowHidden = pConfig->ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_hidden")), false);
	    m_bShowExt = pConfig->ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/show_ext")), true);
	    m_bOpenLastPath = pConfig->ReadBool(enumGISHKCU, GetConfigName() + wxString(wxT("/catalog/open_last_path")), true);
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
    return wxIcon(mainframecat_xpm);
}


