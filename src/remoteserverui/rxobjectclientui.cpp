/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  RxObjectUI classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

#include "wxgis/remoteserverui/rxobjectclientui.h"

//------------------------------------------------------------
// wxRxObjectContainerUI
//------------------------------------------------------------

wxRxObjectContainerUI::wxRxObjectContainerUI(void) : wxRxObjectContainer(), m_pGxPendingUI(NULL)
{
}

wxRxObjectContainerUI::~wxRxObjectContainerUI(void)
{
}

void wxRxObjectContainerUI::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	if(pChildNode)
	{
		if(pChildNode->GetName().CmpNoCase(wxT("children")) == 0 && pChildNode->HasProp(wxT("count")))
			m_nChildCount = wxAtoi(pChildNode->GetPropVal(wxT("count"), wxT("0")));
		if(m_nChildCount == 0)
		{
			m_bIsChildrenLoaded = true;
			return;
		}
		if(pChildNode->GetName().CmpNoCase(wxT("child")) == 0)
		{
			//load items
			while(pChildNode)
			{
				wxString sDst = pChildNode->GetPropVal(wxT("dst"), ERR);
				bool bAdd(true);
				for(size_t i = 0; i < m_sDstArray.GetCount(); i++)
				{
					if(sDst == m_sDstArray[i])
					{
						bAdd = false;
						break;
					}
				}

				wxString sClassName = pChildNode->GetPropVal(wxT("class"), ERR);
				if(!sClassName.IsEmpty() && bAdd)
				{
					IGxObject *pObj = dynamic_cast<IGxObject*>(wxCreateDynamicObject(sClassName));
					IRxObjectClient* pRxObjectCli = dynamic_cast<IRxObjectClient*>(pObj);
					if(pRxObjectCli)
						pRxObjectCli->Init(m_pGxRemoteServer, pChildNode);
					if(!AddChild(pObj))
                    {
						wxDELETE(pObj);
                    }
                    else
                    {
                        m_pCatalog->ObjectAdded(pObj);
                        //remove pending
                        if(m_pGxPendingUI)
                            if(DeleteChild(static_cast<IGxObject*>(m_pGxPendingUI)))
                                m_pGxPendingUI = NULL;
                    }
					m_sDstArray.Add(sDst);
				}
				pChildNode = pChildNode->GetNext();
			}

			m_bIsChildrenLoaded = m_nChildCount == m_Children.size();
		    m_pCatalog->ObjectChanged(this);
		}
	}
}

bool wxRxObjectContainerUI::LoadChildren()
{
	if(m_bIsChildrenLoaded)
		return true;
    if(!m_pGxPendingUI)
    {
        m_pGxPendingUI = new wxGxPendingUI();
        if(!AddChild(m_pGxPendingUI))
            wxDELETE(m_pGxPendingUI);
    }
	return wxRxObjectContainer::LoadChildren();
}

bool wxRxObjectContainerUI::DeleteChild(IGxObject* pChild)
{
    wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pCatalog);
    if(pCatalog)
    {
        IGxSelection* pSel = pCatalog->GetSelection();
        if(pSel)
            pSel->Unselect(pChild, IGxSelection::INIT_ALL);
    }	
	return wxRxObjectContainer::DeleteChild(pChild);
}
