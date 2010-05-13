/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Views Commands class.
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
#include "wxgis/catalogui/viewscmd.h"
#include "../../art/views16.xpm"

//	0	ContentsView states
//	1	Select All
//  2   ?


IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogViewsCmd, wxObject)

wxGISCatalogViewsCmd::wxGISCatalogViewsCmd(void) : m_pContentsView(NULL)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(views16_xpm));
}

wxGISCatalogViewsCmd::~wxGISCatalogViewsCmd(void)
{
}

wxIcon wxGISCatalogViewsCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_ImageList.GetIcon(0);
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
		default:
			return wxEmptyString;
	}
}

wxString wxGISCatalogViewsCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("View"));
		case 1:
			return wxString(_("Edit"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCatalogViewsCmd::GetChecked(void)
{
	return false;
}

bool wxGISCatalogViewsCmd::GetEnabled(void)
{
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
	if(!m_pContentsView)
        return false;
	switch(m_subtype)
	{
		case 0:
		case 1:
			return m_pContentsView->IsShown();
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
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogViewsCmd::GetCount(void)
{
	return 2;
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

