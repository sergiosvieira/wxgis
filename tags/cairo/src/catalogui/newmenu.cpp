/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISNewMenu class.
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
#include "wxgis/catalogui/newmenu.h"
#include "wxgis/catalogui/gxapplication.h"

//#include "../../art/mainframe.xpm"

//----------------------------------------------------------------------
// wxGISNewMenu
//----------------------------------------------------------------------

wxGISNewMenu::wxGISNewMenu(const wxString& sName, const wxString& sCaption, wxGISEnumCommandBars type, const wxString& title, long style) : wxGISMenu(sName, sCaption, type, title, style)
{
	m_pCatalog = NULL;
	m_ConnectionPointSelectionCookie = wxNOT_FOUND;
}

wxGISNewMenu::~wxGISNewMenu(void)
{
    for(size_t i = 0; i < m_delitems.size(); i++)
    {
        wxMenuItem* pItem = Remove(m_delitems[i]);
        wxDELETE(pItem);
    }
    m_delitems.clear();
}

wxIcon wxGISNewMenu::GetBitmap(void)
{
	return wxNullIcon;
}

wxString wxGISNewMenu::GetCaption(void)
{
	return wxString(_("New"));
}

wxString wxGISNewMenu::GetCategory(void)
{
	return wxString(_("[Menus]"));
}

bool wxGISNewMenu::GetChecked(void)
{
	return false;
}

bool wxGISNewMenu::GetEnabled(void)
{
	return true;
}

wxString wxGISNewMenu::GetMessage(void)
{
	return wxString(_("New menu"));
}

wxGISEnumCommandKind wxGISNewMenu::GetKind(void)
{
	return enumGISCommandMenu;
}

void wxGISNewMenu::OnClick(void)
{
}

bool wxGISNewMenu::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
    return true;
}

wxString wxGISNewMenu::GetTooltip(void)
{
	return wxString(_("New menu"));
}

unsigned char wxGISNewMenu::GetCount(void)
{
	return 1;
}

//void wxGISNewMenu::UnInit(void)
//{
//	if(m_ConnectionPointSelectionCookie != -1)
//		m_pConnectionPointSelection->Unadvise(m_ConnectionPointSelectionCookie);
//}
//
//void wxGISNewMenu::Init(void)
//{
//    IGxApplication* pGxApp = dynamic_cast<wxGxApplication*>(m_pApp);
//    wxMenu* pMenu = static_cast<wxMenu*>(this);
//    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
//    m_pConnectionPointSelection = dynamic_cast<IConnectionPointContainer*>( pGxCatalogUI->GetSelection() );
//    if(m_pConnectionPointSelection != NULL)
//	    m_ConnectionPointSelectionCookie = m_pConnectionPointSelection->Advise(pMenu);
//
//    wxMenuItem* pItem = Append(ID_MENUCMD - 1, wxT(" "), wxEmptyString, wxITEM_NORMAL);
//    pItem->Enable(false);
//    m_delitems.push_back(pItem);
//}
//
void wxGISNewMenu::Update(IGxSelection* Selection)
//void wxGISNewMenu::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
	if(!m_pCatalog)
    {
        IGxApplication* pGxApplication = dynamic_cast<IGxApplication*>(m_pApp);
        if(pGxApplication)
            m_pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApplication->GetCatalog());
    }

    for(size_t i = 0; i < m_delitems.size(); i++)
    {
        wxMenuItem* pItem = Remove(m_delitems[i]);
        wxDELETE(pItem);
    }

    m_delitems.clear();

    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(Selection->GetLastSelectedObjectID());
    IGxObjectUI* pGxObjUI = dynamic_cast<IGxObjectUI*>(pGxObject.get());
    if(pGxObjUI)
    {
        wxMenu* pCmdMenu = dynamic_cast<wxMenu*>(m_pApp->GetCommandBar( pGxObjUI->NewMenu() ));
        if(pCmdMenu)
        {
            wxMenuItemList& pLst = pCmdMenu->GetMenuItems();
            wxMenuItemList::iterator iter;
            for (iter = pLst.begin(); iter != pLst.end(); ++iter)
            {
                //MyListElement *current = *iter;  
                wxMenuItem* pAddItem = *iter;
                wxMenuItem* pItem;
                if(pAddItem->IsSubMenu())
                    pItem = Append(pAddItem->GetId(), pAddItem->GetItemLabel(), pAddItem->GetSubMenu());
                else
                    pItem = Append(pAddItem->GetId(), pAddItem->GetItemLabel(), wxEmptyString, pAddItem->GetKind());

                //wxMenuItem* pItem = Append(*iter);
                m_delitems.push_back(pItem);
            }
            return;
        }
    }
    wxMenuItem* pItem = Append(ID_MENUCMD - 1, wxT(" "), wxEmptyString, wxITEM_NORMAL);
    pItem->Enable(false);
    m_delitems.push_back(pItem);
}

