/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISScreenDisplayPlus class. For gdi+
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
#include "wxgis/display/screendisplayplus.h"

#if wxUSE_GRAPHICS_CONTEXT	

//-----------------------------------------------------
// wxGISScreenDisplay
//-----------------------------------------------------


wxGISScreenDisplayPlus::wxGISScreenDisplayPlus(void) : wxGISScreenDisplay()
{
}

wxGISScreenDisplayPlus::~wxGISScreenDisplayPlus(void)
{
}

void wxGISScreenDisplayPlus::DrawPoint(wxCoord x, wxCoord y)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawPoint(x, y);
}

void wxGISScreenDisplayPlus::DrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawLines(n, points, xoffset, yoffset);
}

void wxGISScreenDisplayPlus::DrawCircle(wxCoord x, wxCoord y, wxCoord radius)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawCircle(x, y, radius);
}

void wxGISScreenDisplayPlus::DrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fill_style)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawPolygon(n, points, xoffset, yoffset, fill_style);
}

void wxGISScreenDisplayPlus::DrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fill_style)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawPolyPolygon(n, count, points, xoffset, yoffset, fill_style);
}

void wxGISScreenDisplayPlus::DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawRectangle(x, y, width, height);
}

void wxGISScreenDisplayPlus::DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent)
{
	wxGCDC GDC(m_dc);
	GDC.SetPen(m_dc.GetPen());
	GDC.SetBrush(m_dc.GetBrush());
	GDC.SetFont(m_dc.GetFont());
	GDC.DrawBitmap(bitmap, x, y, transparent);
}

#endif
