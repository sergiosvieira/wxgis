/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressor class. Progress of some process
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

#include "wxgis/framework/framework.h"
#include "wx/gauge.h"

class WXDLLIMPEXP_GIS_FRW wxGISProgressor : 
	public wxControl,
	public IProgressor
{
    enum
    {
        PTIMER_ID = 1013
    };
public:
	wxGISProgressor(wxWindow * parent, wxWindowID id = wxID_ANY, int range = 100, const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = /*wxNO_BORDER*/wxSTATIC_BORDER, const wxString name = wxT("GISProgressor"));
	virtual ~wxGISProgressor(void);
    //events
    void OnSize(wxSizeEvent & event);
	void OnPaint(wxPaintEvent & event);
	void OnEraseBackground(wxEraseEvent & event);
    void OnTimer( wxTimerEvent & event);
	virtual void SetYield(bool bYield = false);
	//IProgressor
	virtual bool Show(bool bShow);
    virtual void SetRange(int range);
    virtual int GetRange();
    virtual void SetValue(int value);
    virtual int GetValue();
	virtual void Play(void);
	virtual void Stop(void);
protected:
    int m_nValue;
    int m_nRange;
    bool m_bPulse;
	wxTimer m_timer;
    bool m_bYield;

	DECLARE_EVENT_TABLE()
};
