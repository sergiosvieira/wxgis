/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  extended tooltip class.
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

/*
#define TIMER_BALOON 1014

class WXDLLIMPEXP_GIS_FRW wxGISBaloonTip : public wxFrame
{
public:
    wxGISBaloonTip(wxString sTitle, wxIcon Icon, wxString sMessage);
    virtual ~wxGISBaloonTip() { delete timer; }

    // painting bg
    virtual void OnPaint(wxPaintEvent& event);
    // timer to close window
    virtual void OnTimerTick(wxTimerEvent & event);
    // click on the baloon
    virtual void OnClick(wxMouseEvent & event){ };
    // click esc
    virtual void OnEscape(wxKeyEvent & event)
    { 
        if(event.GetKeyCode() == WXK_SHIFT || event.GetKeyCode() == WXK_ALT || event.GetKeyCode() == WXK_CONTROL)
            return;
        if(event.GetKeyCode() == WXK_ESCAPE)
            Close();
    };
    // click close
    virtual void OnClose(wxCommandEvent & event)
    {    
        Close();
    };

    // display the baloon and run the timer
    virtual void ShowBaloon(unsigned int iTimeout);
    virtual void Close(void);
private:
    wxTimer * timer;

    DECLARE_EVENT_TABLE();

};
*/