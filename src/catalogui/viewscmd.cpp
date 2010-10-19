/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Views Commands class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/catalogui/viewscmd.h"
#include "wxgis/catalogui/gxtreeview.h"

#include "../../art/views.xpm"
#include "../../art/treeview.xpm"

//	0	ContentsView states
//	1	Select All
//  2   Show/hide tree view
//  3   ?


IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogViewsCmd, wxObject)

wxGISCatalogViewsCmd::wxGISCatalogViewsCmd(void) : m_pContentsView(NULL), m_pTreeView(NULL)
{
}

wxGISCatalogViewsCmd::~wxGISCatalogViewsCmd(void)
{
}

wxIcon wxGISCatalogViewsCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxIcon(views_xpm);
		case 2:
			return wxIcon(treeview_xpm);
		case 1:
		default:
			return wxNullIcon;
	}
}

wxString wxGISCatalogViewsCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("View"));
		case 1:
			return wxString(_("Select All"));
		case 2:
			return wxString(_("Show/Hide Tree pane"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCatalogViewsCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 2:
			return wxString(_("View"));
		case 1:
			return wxString(_("Edit"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCatalogViewsCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 2:
            return m_pApp->IsApplicationWindowShown(m_pTreeView);
		case 0:
		case 1:
		default:
	        return false;
	}
	return false;
}

bool wxGISCatalogViewsCmd::GetEnabled(void)
{
	if(!m_pTreeView)
	{
		WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); i++)
			{
				wxGxTreeView* pTreeView = dynamic_cast<wxGxTreeView*>(pWinArr->at(i));
				if(pTreeView)
				{
					m_pTreeView = pTreeView;
					break;
				}
			}
		}
	}

	if(!m_pContentsView)
	{
		WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); i++)
			{
				wxGxContentView* pContentsView = dynamic_cast<wxGxContentView*>(pWinArr->at(i));
				if(pContentsView)
				{
					m_pContentsView = pContentsView;
					break;
				}
			}
		}
	}

    switch(m_subtype)
	{
		case 2:
            return m_pTreeView;
		case 0:
		case 1:
			return m_pContentsView && m_pContentsView->IsShown();
 		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCatalogViewsCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://View
			return enumGISCommandDropDown;
		case 2://Show/Hide Tree Pane
#ifdef __WXMSW__
            return enumGISCommandCheck;
#endif
        case 1://Select All
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCatalogViewsCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Select view"));
		case 1:
			return wxString(_("Select All objects"));
		case 2:
			return wxString(_("Show/Hide Tree pane"));
		default:
			return wxEmptyString;
	}
}

void wxGISCatalogViewsCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			{
                int nStyle = m_pContentsView->GetStyle() + 1;
                if(nStyle > wxGxContentView::LIST)
                    nStyle = wxGxContentView::REPORT;
                m_pContentsView->SetStyle((wxGxContentView::LISTSTYLE)(nStyle));
			}
			break;
		case 1:
            m_pContentsView->SelectAll();
            m_pContentsView->SetFocus();
			break;
		case 2:
            m_pApp->ShowApplicationWindow(m_pTreeView, !m_pApp->IsApplicationWindowShown(m_pTreeView));
		default:
			return;
	}
}

bool wxGISCatalogViewsCmd::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCatalogViewsCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Select view"));
		case 1:
			return wxString(_("Select All"));
		case 2:
			return wxString(_("Show/Hide Tree pane"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogViewsCmd::GetCount(void)
{
	return 3;
}

wxMenu* wxGISCatalogViewsCmd::GetDropDownMenu(void)
{
	switch(m_subtype)
	{
		case 0:
        {
			if(GetEnabled())
			{
                wxMenu* pMenu = new wxMenu();
                pMenu->AppendCheckItem(ID_MENUCMD + (int)wxGxContentView::LIST, _("List"));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)wxGxContentView::LARGE, _("Icons"));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)wxGxContentView::SMALL, _("Smal Icons"));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)wxGxContentView::REPORT, _("Details"));
                //check
                wxGxContentView::LISTSTYLE nStyle = m_pContentsView->GetStyle();
                pMenu->Check(ID_MENUCMD + (int)nStyle, true);
                return pMenu;
            }
            return NULL;
        }
		case 1:
		default:
			return NULL;
	}
}

void wxGISCatalogViewsCmd::OnDropDownCommand(int nID)
{
    if(GetEnabled())
    {
        m_pContentsView->SetStyle((wxGxContentView::LISTSTYLE)(nID - ID_MENUCMD));
    }
}

