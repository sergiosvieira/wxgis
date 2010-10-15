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
#pragma once

#include "wxgis/display/screendisplay.h"
#define wxUSE_GRAPHICS_CONTEXT 0
#if wxUSE_GRAPHICS_CONTEXT	

//-----------------------------------------------------
// wxGISScreenDisplay
//-----------------------------------------------------


class WXDLLIMPEXP_GIS_DSP wxGISScreenDisplayPlus : 
    public wxGISScreenDisplay	
{
public:
	//wxGISScreenPlusDisplay
	wxGISScreenDisplayPlus(void);
	virtual ~wxGISScreenDisplayPlus(void);
	//IDisplay
	virtual void DrawPolygon(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE);
	virtual void DrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE);
	virtual void DrawPoint(wxCoord x, wxCoord y);
	virtual void DrawLines(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0);
	virtual void DrawCircle(wxCoord x, wxCoord y, wxCoord radius);
	virtual void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	virtual void DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent = false);
};

#endif
