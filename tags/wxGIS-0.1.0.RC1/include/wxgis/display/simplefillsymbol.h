/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Simple Fill Symbol class. For drawing polygones
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
#include "wxgis/display/display.h"

//#define RENDERPOLYPOLY //slow??


class WXDLLIMPEXP_GIS_DSP wxSimpleFillSymbol :
	public ISymbol
{
public:
	wxSimpleFillSymbol(void);
	wxSimpleFillSymbol(wxPen Pen, wxBrush Brush);
	virtual ~wxSimpleFillSymbol(void);
	virtual void DrawPolygon(OGRPolygon* pPoly, IDisplay* pwxGISDisplay);
	virtual void DrawPolyPolygon(OGRMultiPolygon* pPoly, IDisplay* pwxGISDisplay);
	//ISymbol
	virtual void Draw(OGRGeometry* pGeometry, IDisplay* pwxGISDisplay);
};
