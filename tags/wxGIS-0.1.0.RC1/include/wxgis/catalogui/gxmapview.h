/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapView class.
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/carto/mapview.h"

class WXDLLIMPEXP_GIS_CLU wxGxMapView :
	public wxGISMapView,
	public wxGxView,
	public IGxSelectionEvents
{
public:
//	wxGxMapView(void){};
	wxGxMapView(wxWindow* parent, wxWindowID id = MAPCTRLID, const wxPoint& pos = wxDefaultPosition, 
						 const wxSize& size = wxDefaultSize);
	virtual ~wxGxMapView(void);
    virtual void CheckOverviews(wxGISDataset* pwxGISDataset, wxString soFileName);
    //events
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseDoubleClick(wxMouseEvent& event);
//IGxView
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(IGxSelection* Selection);
//IGxSelectionEvents
	virtual void OnSelectionChanged(IGxSelection* Selection, long nInitiator);
private:
	IGxSelection* m_pSelection;
    IGxCatalog* m_pCatalog;
	IGxObject* m_pParentGxObject;
	IStatusBar* m_pStatusBar;
	IApplication* m_pApp;

	DECLARE_EVENT_TABLE()
};
