/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISStatusBar class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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

#include "wxgis/framework/animation.h"
#include "wxgis/framework/progressor.h"

#include <wx/statusbr.h> 

class WXDLLIMPEXP_GIS_FRW wxGISStatusBar :
	public wxStatusBar,
	public IStatusBar
{
    enum
    {
        TIMER_ID = 1011
    };
public:
	wxGISStatusBar(wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString& name = wxT("statusBar"), WXDWORD panelsstyle = enumGISStatusMain | enumGISStatusProgress | enumGISStatusAnimation | enumGISStatusPosition | enumGISStatusClock);
	virtual ~wxGISStatusBar(void);
	void OnSize(wxSizeEvent &event);
	virtual void SetMessage(const wxString& text, int i = 0);
	virtual wxString GetMessage(int i = 0);
    virtual int GetPanelPos(wxGISEnumStatusBarPanels nPanel);
	virtual IProgressor* GetAnimation(void)
	{
		if(m_Panels & enumGISStatusAnimation)
			return static_cast<IProgressor*>(m_pAni);
		return NULL;
	};
	virtual IProgressor* GetProgressor(void)//???
	{
		if(m_Panels & enumGISStatusProgress)
			return static_cast<IProgressor*>(m_pProgressBar);
		return NULL;
	};
	//events
	void OnRightDown(wxMouseEvent& event);
    void OnTimer( wxTimerEvent & event);

typedef struct _statuspanel
	{
		int size;
		long style;
	}STATUSPANEL;

protected:
	wxTimer m_timer;
	wxGISAnimation* m_pAni;
    wxGISProgressor* m_pProgressBar;
	int m_MsgPos, m_AniPos, m_ProgressPos, m_PositionPos, m_ClockPos, m_PagePositionPos, m_SizePos, m_CapsLockPos, m_NumLockPos, m_ScrollLockPos;
	IFrameApplication* m_pApp;

	DECLARE_EVENT_TABLE()
};
