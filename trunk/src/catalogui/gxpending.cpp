/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPendingUI class. Show pending item in tree or content view
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/catalogui/gxpending.h"
#include "wxgis/catalogui/gxcatalogui.h"

//---------------------------------------------------------------------
// wxGxPendingUI
//---------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPendingUI, wxGxObject)

BEGIN_EVENT_TABLE(wxGxPendingUI, wxGxObject)
  EVT_TIMER(TIMER_ID, wxGxPendingUI::OnTimer)
END_EVENT_TABLE()

wxGxPendingUI::wxGxPendingUI(wxImageList *pImageListSmall, wxImageList  *pImageListLarge, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObject(oParent, soName, soPath), m_timer(this, TIMER_ID), m_nCurrentImage(0), m_pImageListSmall(pImageListSmall), m_pImageListLarge(pImageListLarge)
{
    m_timer.Start(150);
}

wxGxPendingUI::~wxGxPendingUI(void)
{
}

/*
bool wxGxPendingUI::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
    IGxObject::Attach(pParent, pCatalog);
    wxGxCatalogUI* pGxCatalogUI = dynamic_cast<wxGxCatalogUI*>(pCatalog);
    if(pGxCatalogUI)
    {
        m_pImageListLarge = pGxCatalogUI->GetPendingImageList(true);
        m_pImageListSmall = pGxCatalogUI->GetPendingImageList(false);
    }
    // start update thread
	m_pThread = new wxPendingUpdateThread(this);
	if(!CreateAndRunThread(m_pThread, wxT("wxPendingUpdateThread"), wxT("PendingUpdate")))
		return false;
	return true;
}

void wxGxPendingUI::Detach(void)
{
    // stop update thread
	OnStopPending();
    IGxObject::Detach();
}

void wxGxPendingUI::OnStopPending(void)
{
    if(m_pThread)
        m_pThread->Delete();
	m_pThread = NULL;
}
*/
wxIcon wxGxPendingUI::GetLargeImage(void)
{
    if(m_pImageListLarge)
        return m_pImageListLarge->GetIcon(m_nCurrentImage);
    else
        return wxNullIcon;
}

wxIcon wxGxPendingUI::GetSmallImage(void)
{
    if(m_pImageListSmall)
        return m_pImageListSmall->GetIcon(m_nCurrentImage);
    else
        return wxNullIcon;
}

void wxGxPendingUI::OnTimer( wxTimerEvent& event )
{
    m_nCurrentImage++;
    if(m_nCurrentImage >= m_pImageListSmall->GetImageCount())
        m_nCurrentImage = 0;
    wxGIS_GXCATALOG_EVENT(ObjectChanged);
}

/*
void wxGxPendingUI::OnUpdate(void)
{
    m_nCurrentImage++;
    if(m_nCurrentImage >= m_pImageListSmall->GetImageCount())
        m_nCurrentImage = 0;
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
}
*/
