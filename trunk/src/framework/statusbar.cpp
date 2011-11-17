/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISStatusBar class.
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
#include "wxgis/framework/statusbar.h"
#include "../../art/globe_imglst14.xpm"

BEGIN_EVENT_TABLE(wxGISStatusBar, wxStatusBar)
	EVT_SIZE(wxGISStatusBar::OnSize)
	EVT_TIMER( TIMER_ID, wxGISStatusBar::OnTimer )
	EVT_RIGHT_DOWN(wxGISStatusBar::OnRightDown)
END_EVENT_TABLE()

wxGISStatusBar::wxGISStatusBar(wxWindow *parent, wxWindowID id, long style, const wxString& name, WXDWORD panesstyle) : wxStatusBar(parent, id, style, name), IStatusBar(panesstyle), m_timer(this, TIMER_ID), m_pAni(NULL), m_pProgressBar(NULL)
{
    m_MsgPos = wxNOT_FOUND;
    m_AniPos = wxNOT_FOUND;
    m_ProgressPos = wxNOT_FOUND;
    m_PositionPos = wxNOT_FOUND;
    m_ClockPos = wxNOT_FOUND;
    m_PagePositionPos = wxNOT_FOUND;
    m_SizePos = wxNOT_FOUND;
    m_CapsLockPos = wxNOT_FOUND;
    m_NumLockPos = wxNOT_FOUND;
    m_ScrollLockPos = wxNOT_FOUND;

	m_pApp = dynamic_cast<IFrameApplication*>(parent);

	std::vector<STATUSPANE> panes;
	int counter(0);
	if(panesstyle & enumGISStatusMain)
	{
		STATUSPANE data = {wxNOT_FOUND, wxSB_FLAT};
		panes.push_back(data);
		m_MsgPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusProgress)
	{
		STATUSPANE data = {100, /*wxSB_NORMAL*/wxSB_FLAT};
		panes.push_back(data);
		m_pProgressBar = new wxGISProgressor(this, wxID_ANY);
		m_pProgressBar->Hide();
		m_ProgressPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusAnimation)
	{
		STATUSPANE data = {23, wxSB_NORMAL};
		panes.push_back(data);
		//bitmap
		m_pAni = new wxGISAnimation(this, wxID_ANY, wxBitmap(globe_imglst14_xpm),14);
		m_pAni->Hide();
		m_AniPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusPosition)
	{
#ifdef __WXGTK__
		STATUSPANE data = {225, wxSB_NORMAL};
#else
		STATUSPANE data = {200, wxSB_NORMAL};
#endif
		panes.push_back(data);
        m_PositionPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusPagePosition)
	{
		STATUSPANE data = {100, wxSB_NORMAL};
		panes.push_back(data);
        m_PagePositionPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusSize)
	{
		STATUSPANE data = {80, wxSB_NORMAL};
		panes.push_back(data);
        m_SizePos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusCapsLock)
	{
		STATUSPANE data = {40, wxSB_NORMAL};
		panes.push_back(data);
        m_CapsLockPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusNumLock)
	{
		STATUSPANE data = {40, wxSB_NORMAL};
		panes.push_back(data);
        m_NumLockPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusScrollLock)
	{
		STATUSPANE data = {40, wxSB_NORMAL};
		panes.push_back(data);
        m_ScrollLockPos = counter;
		counter++;
	}
	if(panesstyle & enumGISStatusClock)
	{
		STATUSPANE data = {48, wxSB_NORMAL};
		panes.push_back(data);
		m_ClockPos = counter;
		counter++;
	}
	//STATUSPANE data = {1, wxSB_FLAT};
	//panes.push_back(data);
	//STATUSPANE data1 = {15, wxSB_FLAT};
	//panes.push_back(data1);

	int *STATUSBAR_Sizes = new int[panes.size()];
	int *STATUSBAR_Styles = new int[panes.size()];
	for(size_t i = 0; i < panes.size(); ++i)
	{
		STATUSBAR_Sizes[i] = panes[i].size;
		STATUSBAR_Styles[i] = panes[i].style;
	}
	SetFieldsCount(panes.size(), STATUSBAR_Sizes);
	SetStatusStyles(panes.size(), STATUSBAR_Styles);

	delete [] STATUSBAR_Sizes;
	delete [] STATUSBAR_Styles;

	//set text & start timer
	if(panesstyle & enumGISStatusClock)
	{
		SetMessage(wxDateTime::Now().Format(_("%H:%M")), m_ClockPos);
		m_timer.Start(30000);    // 1 minute interval
	}
}

wxGISStatusBar::~wxGISStatusBar(void)
{
	wxDELETE(m_pAni);
}


void wxGISStatusBar::OnSize(wxSizeEvent &event)
{
	wxRect	r;
	if( m_pAni && GetFieldRect(m_AniPos, r) )
	{
		r.Deflate(2);
		m_pAni->SetSize(r);
	}

	if( m_pProgressBar && GetFieldRect(m_ProgressPos, r) )
	{
		//r.Deflate(2);
		m_pProgressBar->SetSize(r);
	}
	event.Skip();
}

void wxGISStatusBar::SetMessage(const wxString& text, int i)
{
	SetStatusText(text, i);
}

wxString wxGISStatusBar::GetMessage(int i)
{
	return GetStatusText(i);
}

void wxGISStatusBar::OnTimer( wxTimerEvent& event )
{
	SetMessage(wxDateTime::Now().Format(_("%H:%M")), m_ClockPos);
}


void wxGISStatusBar::OnRightDown(wxMouseEvent& event)
{
	event.Skip();
	m_pApp->ShowToolBarMenu();
}

int wxGISStatusBar::GetPanePos(wxGISEnumStatusBarPanes nPane)
{
    switch(nPane)
    {
    case enumGISStatusMain:
        return m_MsgPos;
    case enumGISStatusAnimation:
        return m_AniPos;
    case enumGISStatusPosition:
        return m_PositionPos;
    case enumGISStatusClock:
        return m_ClockPos;
    case enumGISStatusProgress:
        return m_ProgressPos;
    case enumGISStatusPagePosition:
        return m_PagePositionPos;
    case enumGISStatusSize:
        return m_SizePos;
    case enumGISStatusCapsLock:
        return m_CapsLockPos;
    case enumGISStatusNumLock:
        return m_NumLockPos;
    case enumGISStatusScrollLock:
        return m_ScrollLockPos;
    default:
        return -1;
    }
    return -1;
}
