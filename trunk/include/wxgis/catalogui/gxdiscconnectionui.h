/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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

#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalog/gxdiscconnection.h"

/** \class wxGxDiscConnectionUI gxdiscconnectionui.h
    \brief A Disc Connection GxObject.
*/
class WXDLLIMPEXP_GIS_CLU wxGxDiscConnectionUI :
	public wxGxDiscConnection,
    public IGxObjectUI,
    public IGxObjectEditUI
{
public:
	wxGxDiscConnectionUI(wxString Path, wxString Name);
	virtual ~wxGxDiscConnectionUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxDiscConnection.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxDiscConnection.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
};
