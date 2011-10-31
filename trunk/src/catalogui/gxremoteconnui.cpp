/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRemoteConnectionUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/catalogui/gxremoteconnui.h"
#include "wxgis/catalogui/remoteconndlg.h"
#include "wxgis/catalogui/gxpostgisdatasetui.h"
#include "wxgis/core/globalfn.h"

//--------------------------------------------------------------
//class wxGxRemoteConnectionUI
//--------------------------------------------------------------

wxGxRemoteConnectionUI::wxGxRemoteConnectionUI(CPLString soPath, wxString Name, wxIcon LargeIconConn, wxIcon SmallIconConn, wxIcon LargeIconDisconn, wxIcon SmallIconDisconn, wxIcon LargeIconFeatureClass, wxIcon SmallIconFeatureClass, wxIcon LargeIconTable, wxIcon SmallIconTable) : wxGxRemoteConnection(soPath, Name)
{
    m_oLargeIconConn = LargeIconConn;
    m_oSmallIconConn = SmallIconConn;
    m_oLargeIconDisconn = LargeIconDisconn;
    m_oSmallIconDisconn = SmallIconDisconn;
    m_oLargeIconFeatureClass = LargeIconFeatureClass;
    m_oSmallIconFeatureClass = SmallIconFeatureClass;
    m_oLargeIconTable = LargeIconTable;
    m_oSmallIconTable = SmallIconTable;
}

wxGxRemoteConnectionUI::~wxGxRemoteConnectionUI(void)
{
}

wxIcon wxGxRemoteConnectionUI::GetLargeImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oLargeIconConn;
    else
        return m_oLargeIconDisconn;
}

wxIcon wxGxRemoteConnectionUI::GetSmallImage(void)
{
    if(m_pwxGISDataset && m_pwxGISDataset->IsOpened())
        return m_oSmallIconConn;
    else
        return m_oSmallIconDisconn;
}

void wxGxRemoteConnectionUI::EditProperties(wxWindow *parent)
{
	wxGISRemoteConnDlg dlg(m_sPath, parent);
	if(dlg.ShowModal() == wxID_OK)
	{
		m_sPath = dlg.GetPath();
		m_sName = dlg.GetName();
		m_pCatalog->ObjectChanged(GetID());
	}
}

bool wxGxRemoteConnectionUI::Invoke(wxWindow* pParentWnd)
{
    //EditProperties(pParentWnd);
    wxBusyCursor cur;
    //connect
    GetDataset();
    m_pCatalog->ObjectChanged(GetID());

    return true;
}

void wxGxRemoteConnectionUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

	if(m_pwxGISDataset == NULL)
		return;

    IFrameApplication* pFApp = dynamic_cast<IFrameApplication*>(GetApplication());
    IProgressor* pProgressor(NULL);
    if(pFApp)
    {
        IStatusBar* pStatusBar = pFApp->GetStatusBar();
        if(pStatusBar)
            pProgressor = pStatusBar->GetProgressor();
    }

    if(pProgressor)
    {
        pProgressor->SetRange(m_pwxGISDataset->GetSubsetsCount());
        pProgressor->Show(true);
    }

    for(size_t i = 0; i < m_pwxGISDataset->GetSubsetsCount(); ++i)
    {
        if(pProgressor)
            pProgressor->SetValue(i);

        wxGISDatasetSPtr pGISDataset = m_pwxGISDataset->GetSubset(i);
        wxGISEnumDatasetType eType = pGISDataset->GetType();
        IGxObject* pGxObject(NULL);
        switch(eType)
        {
        case enumGISFeatureDataset:
            {
                wxGxPostGISFeatureDatasetUI* pGxPostGISFeatureDataset = new wxGxPostGISFeatureDatasetUI(m_sPath, pGISDataset, m_oLargeIconFeatureClass, m_oSmallIconFeatureClass);
                pGxObject = static_cast<IGxObject*>(pGxPostGISFeatureDataset);
            }
            break;
        case enumGISTableDataset:
            {
                wxGxPostGISTableDatasetUI* pGxPostGISTableDataset = new wxGxPostGISTableDatasetUI(m_sPath, pGISDataset, m_oLargeIconTable, m_oSmallIconTable);
                pGxObject = static_cast<IGxObject*>(pGxPostGISTableDataset);
            }
            break;
        case enumGISRasterDataset:
            break;
        default:
        case enumGISContainer:
            break;
        };

        if(pGxObject)
        {
		    bool ret_code = AddChild(pGxObject);
		    if(!ret_code)
			    wxDELETE(pGxObject);
        }
	}

    if(pProgressor)
        pProgressor->Show(false);

	m_bIsChildrenLoaded = true;
}