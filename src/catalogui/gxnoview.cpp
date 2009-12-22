/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxNoView class.
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
#include "wxgis/catalogui/gxnoview.h"

BEGIN_EVENT_TABLE(wxGxNoView, wxControl)
    EVT_PAINT(wxGxNoView::OnPaint)
    EVT_SIZE(wxGxNoView::OnSize)
    EVT_ERASE_BACKGROUND(wxGxNoView::OnEraseBackground)
END_EVENT_TABLE()

bool wxGxNoView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);
	Serialize(m_pXmlConf, false);
	return true;
}

void wxGxNoView::Deactivate(void)
{
	Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

wxGxNoView::wxGxNoView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) 
						: wxControl(parent, id, pos, size, wxNO_BORDER)
{
	m_sViewName = wxString(_("NoView"));
	R = 255; G = 255; B = 230;
	IsBold = true;
	IsItalic = false;
	FontSize = 12;
}

wxGxNoView::~wxGxNoView(void)
{
}

void wxGxNoView::Serialize(wxXmlNode* pRootNode, bool bStore)
{
	if(pRootNode == NULL)
		return;
	if(bStore)
	{
		pRootNode->DeleteProperty(wxT("red"));
		pRootNode->AddProperty(wxT("red"), wxString::Format(wxT("%d"), R));
		pRootNode->DeleteProperty(wxT("green"));
		pRootNode->AddProperty(wxT("green"), wxString::Format(wxT("%d"), G));
		pRootNode->DeleteProperty(wxT("blue"));
		pRootNode->AddProperty(wxT("blue"), wxString::Format(wxT("%d"), B));
		pRootNode->DeleteProperty(wxT("bold"));
		pRootNode->AddProperty(wxT("bold"), wxString::Format(wxT("%d"), IsBold));
		pRootNode->DeleteProperty(wxT("italic"));
		pRootNode->AddProperty(wxT("italic"), wxString::Format(wxT("%d"), IsItalic));
		pRootNode->DeleteProperty(wxT("size"));
		pRootNode->AddProperty(wxT("size"), wxString::Format(wxT("%d"), FontSize));
	}
	else
	{
		R = wxAtoi(pRootNode->GetPropVal(wxT("red"), wxT("255")));
		G = wxAtoi(pRootNode->GetPropVal(wxT("green"), wxT("255")));
		B = wxAtoi(pRootNode->GetPropVal(wxT("blue"), wxT("230")));
		IsBold = wxAtoi(pRootNode->GetPropVal(wxT("bold"), wxT("1")));
		IsItalic = wxAtoi(pRootNode->GetPropVal(wxT("italic"), wxT("0")));
		FontSize = wxAtoi(pRootNode->GetPropVal(wxT("size"), wxT("10")));
	}
}

void wxGxNoView::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
	wxString s(_("Selection cannot be viewed"));
    wxSize size = GetClientSize();
    int width, height;

	wxFont font(FontSize, wxFONTFAMILY_DEFAULT , IsItalic == true ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL, 
		IsBold == true ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
    dc.SetFont(font);
    dc.GetTextExtent(s, &width, &height);
    height += 3;
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxGREY_PEN);
    dc.DrawRectangle(0, 0, size.x, size.y);

	wxBrush br(wxColour(R,G,B));
    dc.SetBrush(br);
    dc.SetPen(*wxGREY_PEN);
	int x1 = (size.x - width) / 2, y1 = (size.y - height) / 2;
    dc.DrawRectangle( x1 - 5, y1 - 2, width + 10, height + 4);

    dc.DrawText(s, x1, y1);
}

void wxGxNoView::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

void wxGxNoView::OnSize(wxSizeEvent& WXUNUSED(evt))
{
	wxControl::Refresh();
}