/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxView class.
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
#pragma once
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/catalog.h"

//-----------------------------------------------
// wxGxView
//-----------------------------------------------

class WXDLLIMPEXP_GIS_CLU wxGxView :
    public IGxView
{
public:	
	wxGxView(void);
	virtual ~wxGxView(void);
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("view")) = 0;
	virtual bool Activate(IFrameApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(IGxSelection* Selection);
	virtual void Refresh(void){};
	virtual wxString GetViewName(void);
	virtual wxIcon GetViewIcon(void);
	virtual void SetViewIcon(wxIcon Icon);
    virtual void BeginRename(long nObjectID = wxNOT_FOUND){};
protected:
	wxString m_sViewName;
	//IGxCatalog* m_pCatalog;
	IGxApplication* m_pGxApplication;
	wxXmlNode* m_pXmlConf;
    wxIcon m_Icon;
};

int GxObjectCompareFunction(IGxObject* pObject1, IGxObject* pObject2, long sortData);

