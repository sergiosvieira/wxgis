/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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
    public IGxObjectEditUI,
    public IGxDropTarget
{
public:
	wxGxDiscConnectionUI(CPLString Path, wxString Name, wxIcon SmallIco, wxIcon LargeIco, wxIcon SmallIcoDsbl, wxIcon LargeIcoDsbl);
	virtual ~wxGxDiscConnectionUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void){return wxString(wxT("wxGxDiscConnection.ContextMenu"));};
	virtual wxString NewMenu(void){return wxString(wxT("wxGxDiscConnection.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
    //IGxDropTarget
    virtual wxDragResult CanDrop(wxDragResult def);
    virtual bool Drop(const wxArrayString& filenames, bool bMove);
protected:
	wxIcon m_Conn16, m_Conn48;
	wxIcon m_ConnDsbld16, m_ConnDsbld48;
};
