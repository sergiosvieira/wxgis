/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxTasksView class.
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

#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/catalogui/gxview.h"

#include <wx/html/htmlwin.h>

//////////////////////////////////////////////////////////////////
// wxGxTaskPanel
//////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GPU wxGxTaskPanel :
	public wxPanel
{
public:
	wxGxTaskPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL|wxBORDER_RAISED );
    ~wxGxTaskPanel(void);
    //events
    virtual void OnExpand(wxCommandEvent & event);
protected:
    wxImageList m_ImageList;
    bool m_bExpand;
    wxHtmlWindow* m_pHtmlWindow;
    wxBoxSizer* m_bMainSizer;
    wxBitmap m_ExpandBitmapBW, m_ExpandBitmap, m_ExpandBitmapBWRotated, m_ExpandBitmapRotated;
    wxBitmapButton* m_bpExpandButton;

    DECLARE_EVENT_TABLE();
};

//////////////////////////////////////////////////////////////////
// wxGxTasksView
//////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_GPU wxGxTasksView :
	public wxScrolledWindow, 
	public wxGxView
{
public:
	wxGxTasksView(wxWindow* parent, wxWindowID id = TASKSVIEWCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL|wxBORDER_SUNKEN|wxVSCROLL );
	virtual ~wxGxTasksView(void);
//IGxView
	virtual bool Activate(IGxApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
protected:

DECLARE_EVENT_TABLE()
};
