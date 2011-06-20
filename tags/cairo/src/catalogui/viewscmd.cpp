/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Views Commands class.
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
#include "wxgis/catalogui/viewscmd.h"
#include "wxgis/catalogui/gxtreeview.h"

#include "../../art/views.xpm"
#include "../../art/treeview.xpm"
#include "../../art/select_all.xpm"

//	0	ContentsView states
//	1	Select All
//  2   Show/hide tree view
//  3   ?


IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogViewsCmd, wxObject)

wxGISCatalogViewsCmd::wxGISCatalogViewsCmd(void) : m_pTreeView(NULL)
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
			if(!m_IconViews.IsOk())
				m_IconViews = wxIcon(views_xpm);
			return m_IconViews;
		case 1:
			if(!m_IconSelAll.IsOk())
				m_IconSelAll = wxIcon(select_all_xpm);
			return m_IconSelAll;
		case 2:
			if(!m_IconTreeView.IsOk())
				m_IconTreeView = wxIcon(treeview_xpm);
			return m_IconTreeView;
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
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
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

	if(m_apContentsWin.empty())
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); i++)
			{
				IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(pWinArr->at(i));
                if(pGxContentsView)
                    m_apContentsWin.push_back(pWinArr->at(i));
			}
		}
	}

    switch(m_subtype)
	{
		case 2:
            return m_pTreeView;
		case 0:
		case 1:
            for(size_t i = 0; i < m_apContentsWin.size(); i++)
                if(m_apContentsWin[i]->IsShown())
                    return true;
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
            for(size_t i = 0; i < m_apContentsWin.size(); i++)
			{
                if(m_apContentsWin[i]->IsShown())
                {
                    IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(m_apContentsWin[i]);
                    if(pGxContentsView)
                    {
                        if(pGxContentsView->CanSetStyle())
                        {
                            int nStyle = pGxContentsView->GetStyle() + 1;
                            if(nStyle > enumGISCVList)
                                nStyle = enumGISCVReport;
                            pGxContentsView->SetStyle((wxGISEnumContentsViewStyle)(nStyle));
                        }
                    }
                }
            }
			break;
		case 1:
            for(size_t i = 0; i < m_apContentsWin.size(); i++)
			{
                if(m_apContentsWin[i]->IsShown())
                {
                    IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(m_apContentsWin[i]);
                    if(pGxContentsView)
                    {
                        pGxContentsView->SelectAll();
                        m_apContentsWin[i]->SetFocus();
                    }
                }
            }
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
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVList, _("List"));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVLarge, _("Icons"));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVSmall, _("Smal Icons"));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVReport, _("Details"));
                //check
                for(size_t i = 0; i < m_apContentsWin.size(); i++)
			    {
                    if(m_apContentsWin[i]->IsShown())
                    {
                        IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(m_apContentsWin[i]);
                        if(pGxContentsView)
                        {
                            wxGISEnumContentsViewStyle nStyle = pGxContentsView->GetStyle();
                            pMenu->Check(ID_MENUCMD + (int)nStyle, true);
                        }
                    }
                }
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
        for(size_t i = 0; i < m_apContentsWin.size(); i++)
	    {
            if(m_apContentsWin[i]->IsShown())
            {
                IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(m_apContentsWin[i]);
                if(pGxContentsView)
                    pGxContentsView->SetStyle((wxGISEnumContentsViewStyle)(nID - ID_MENUCMD));
            }
        }
    }
}

