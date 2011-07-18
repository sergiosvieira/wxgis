/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapView class.
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
#include "wxgis/catalogui/gxmapview.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/rasterlayer.h"
#include "wxgis/framework/framework.h"
#include "wxgis/framework/messagedlg.h"
#include "wxgis/framework/application.h"
#include "wxgis/datasource/rasterop.h"

#include "wx/msgdlg.h"

BEGIN_EVENT_TABLE(wxGxMapView, wxGISMapView)
	EVT_LEFT_DOWN(wxGxMapView::OnMouseDown)
	EVT_MIDDLE_DOWN(wxGxMapView::OnMouseDown)
	EVT_RIGHT_DOWN(wxGxMapView::OnMouseDown)
	EVT_LEFT_UP(wxGxMapView::OnMouseUp)
	EVT_MIDDLE_UP(wxGxMapView::OnMouseUp)
	EVT_RIGHT_UP(wxGxMapView::OnMouseUp)
	EVT_LEFT_DCLICK(wxGxMapView::OnMouseDoubleClick)
	EVT_MIDDLE_DCLICK(wxGxMapView::OnMouseDoubleClick)
	EVT_RIGHT_DCLICK(wxGxMapView::OnMouseDoubleClick)
	EVT_MOTION(wxGxMapView::OnMouseMove)
	EVT_GXSELECTION_CHANGED(wxGxMapView::OnSelectionChanged)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxGxMapView, wxGISMapView)

wxGxMapView::wxGxMapView(void)
{
    m_nParentGxObjectID = wxNOT_FOUND;
}

wxGxMapView::wxGxMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISMapView(parent, id, pos, size), m_pStatusBar(NULL)
{
    m_nParentGxObjectID = wxNOT_FOUND;
	m_sViewName = wxString(_("Geography View"));
}

wxGxMapView::~wxGxMapView(void)
{
}
    
bool wxGxMapView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_pStatusBar = NULL;
	m_sViewName = wxString(_("Geography View"));
    return wxGISMapView::Create(parent, MAPCTRLID, pos, size, style, name);
}

bool wxGxMapView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;
	//Serialize(m_pXmlConf, false);

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pGxApplication->GetCatalog());
    m_pSelection = m_pCatalog->GetSelection();

    m_pApp = application;
    if(!m_pApp)
        return false;
	m_pStatusBar = m_pApp->GetStatusBar();

	m_pTrackCancel = new ITrackCancel();
	if(m_pStatusBar)
		m_pTrackCancel->SetProgressor(m_pStatusBar->GetAnimation());
	SetTrackCancel(m_pTrackCancel);
	return true;
}

void wxGxMapView::Deactivate(void)
{
	//Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
	wxDELETE(m_pTrackCancel);
}

bool wxGxMapView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); ++i)
	{
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(Selection->GetSelectedObjectID(i));
		IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>( pGxObject.get() );
		if(pGxDataset != NULL)
		{
			wxGISEnumDatasetType type = pGxDataset->GetType();
			switch(type)
			{
			case enumGISRasterDataset:
			case enumGISFeatureDataset:
			case enumGISContainer:
				return true;
			case enumGISTableDataset:
				break;
			}
		}
	}
	return false;
}

void wxGxMapView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	wxCHECK_RET(event.GetSelection(), "the selection pointer is NULL");
    long nLastSelID = event.GetSelection()->GetLastSelectedObjectID();
	if(m_nParentGxObjectID == nLastSelID)
		return;
    	
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nLastSelID);
	IGxDataset* pGxDataset =  dynamic_cast<IGxDataset*>(pGxObject.get());
	if(pGxDataset == NULL)
		return;

//    wxBusyCursor wait;
	if(m_pStatusBar)
		m_pTrackCancel->SetProgressor(m_pStatusBar->GetProgressor());
	m_pTrackCancel->Reset();
	wxGISDatasetSPtr pwxGISDataset = pGxDataset->GetDataset(m_pTrackCancel);
	if(m_pStatusBar)
		m_pTrackCancel->SetProgressor(m_pStatusBar->GetAnimation());

	if(pwxGISDataset == NULL)
		return;

    m_nParentGxObjectID = pGxObject->GetID();    

	wxGISEnumDatasetType type = pwxGISDataset->GetType();
    std::vector<wxGISLayerSPtr> paLayers;

	switch(type)
	{
	case enumGISFeatureDataset:
		{
		wxGISFeatureLayerSPtr pGISFeatureLayer = boost::make_shared<wxGISFeatureLayer>(pwxGISDataset);
		paLayers.push_back(boost::static_pointer_cast<wxGISLayer>(pGISFeatureLayer));
		}
		break;
	case enumGISRasterDataset:
        //CheckOverviews(pwxGISDataset, pGxObject->GetName());
		//pwxGISLayers.push_back(new wxGISRasterLayer(pwxGISDataset));
        //pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISDataset->GetName());
		break;
	case enumGISContainer:
        for(size_t i = 0; i < pwxGISDataset->GetSubsetsCount(); ++i)
        {
            wxGISDatasetSPtr pwxGISSubDataset = pwxGISDataset->GetSubset(i);
            if(!pwxGISSubDataset)
                continue;
            wxGISEnumDatasetType subtype = pwxGISSubDataset->GetType();
	        switch(subtype)
	        {
	        case enumGISFeatureDataset:
				{
				wxGISFeatureLayerSPtr pGISFeatureLayer = boost::make_shared<wxGISFeatureLayer>(pwxGISSubDataset);
				paLayers.push_back(boost::static_pointer_cast<wxGISLayer>(pGISFeatureLayer));
				}
				break;
	        case enumGISRasterDataset:
          //      CheckOverviews(pwxGISSubDataset, pGxObject->GetName());
		        //pwxGISLayers.push_back(new wxGISRasterLayer(pwxGISSubDataset));
          //      pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISSubDataset->GetName());
		        break;
            }
        }
		break;
	default:
		break;
	}

    Clear();

    for(size_t i = 0; i < paLayers.size(); ++i)
    {
	    if(paLayers[i] && paLayers[i]->IsValid())
		    AddLayer(paLayers[i]);
    }

    SetFullExtent();
}

void wxGxMapView::OnMouseMove(wxMouseEvent& event)
{
	if(m_pGISDisplay)
	{
		double dX(event.m_x), dY(event.m_y);
		m_pGISDisplay->DC2World(&dX, &dY);
        int nPanePos = m_pStatusBar->GetPanePos(enumGISStatusPosition);
		m_pStatusBar->SetMessage(wxString::Format(_("X: %.4f  Y: %.4f"), dX, dY), nPanePos);
	}

	if(m_pApp)
		m_pApp->OnMouseMove(event);
	event.Skip();
}

void wxGxMapView::OnMouseDown(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDown(event);
	event.Skip();
}

void wxGxMapView::OnMouseUp(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseUp(event);
	event.Skip();
}

void wxGxMapView::OnMouseDoubleClick(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDoubleClick(event);
	event.Skip();
}

typedef struct OvrProgressData
{
    IStatusBar* pStatusBar;
    IProgressor* pProgressor;
    wxString sMessage;
} *LPOVRPROGRESSDATA;

int CPL_STDCALL OvrProgress( double dfComplete, const char *pszMessage, void *pData)
{
    LPOVRPROGRESSDATA pOvrData = (LPOVRPROGRESSDATA)pData;
    if( pszMessage != NULL )
        pOvrData->pStatusBar->SetMessage(wgMB2WX(pszMessage));
    else if( pOvrData != NULL && !pOvrData->sMessage.IsEmpty() )
        pOvrData->pStatusBar->SetMessage(pOvrData->sMessage);
    else
        pOvrData->pStatusBar->SetMessage(_("Building overviews"));

    if(pOvrData->pProgressor)
        pOvrData->pProgressor->SetValue((int) (dfComplete*100));

    if(wxGetKeyState(WXK_SHIFT) || wxGetKeyState(WXK_ALT) || wxGetKeyState(WXK_CONTROL))
        return 1;

    bool bKeyState = wxGetKeyState(WXK_ESCAPE);
    return bKeyState == true ? 0 : 1;
}

void wxGxMapView::CheckOverviews(wxGISDatasetSPtr pwxGISDataset, wxString soFileName)
{
/*    wxGISRasterDatasetSPtr pwxGISRasterDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(pwxGISDataset);
    if(!pwxGISRasterDataset)
        return;
   //pyramids
    if(!pwxGISRasterDataset->HasOverviews())
    {
    	bool bAskCreateOvr = true;
        IGISConfig*  pConfig = m_pCatalog->GetConfig();

        bool bCreateOverviews = true;
        wxString sResampleMethod(wxT("GAUSS"));
        if(pConfig)
        {
            wxString sCompress;
            wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")));
            if(pNode)
            {
                bAskCreateOvr = wxAtoi(pNode->GetAttribute(wxT("ask_create_ovr"), wxT("1")));
                sCompress = pNode->GetAttribute(wxT("ovr_compress"), wxT("NONE"));
                sResampleMethod = pNode->GetAttribute(wxT("ovr_resample"), wxT("GAUSS"));
                //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"
                bCreateOverviews = wxAtoi(pNode->GetAttribute(wxT("create_ovr"), wxT("1")));
            }
            else
            {
                pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")), true);
                pNode->AddAttribute(wxT("create_ovr"), wxT("1"));
                pNode->AddAttribute(wxT("ask_create_ovr"), wxT("1"));
                pNode->AddAttribute(wxT("ovr_compress"), wxT("NONE"));
                pNode->AddAttribute(wxT("ovr_resample"), wxT("NONE"));//wxT("GAUSS"));
            }
            CPLSetConfigOption( "COMPRESS_OVERVIEW", wgWX2MB(sCompress) );//LZW "DEFLATE" NONE
            if(bAskCreateOvr)
            {
                //show ask dialog
                wxGISMessageDlg dlg(this->GetParent()->GetParent(), wxID_ANY, wxString::Format(_("Create pyramids for %s (%d x %d)"), soFileName.c_str(), pwxGISRasterDataset->GetWidth(), pwxGISRasterDataset->GetHeight()), wxString(_("This raster datasource does not have pyramids. Pyramids allow rapid display at different resolutions.")), wxString(_("Pyramids building may take few moments.\nWould you like to create pyramids?")), wxDefaultPosition, wxSize( 400,160 ));

                if(dlg.ShowModal() == wxID_NO)
                    bCreateOverviews = false;
                else
                    bCreateOverviews = true;

                if(!dlg.GetShowInFuture())
                {
                    pNode->DeleteAttribute(wxT("ask_create_ovr"));
                    pNode->AddAttribute(wxT("ask_create_ovr"), wxT("0"));
                    pNode->DeleteAttribute(wxT("create_ovr"));
                    pNode->AddAttribute(wxT("create_ovr"), wxString::Format(wxT("%d"), bCreateOverviews));
                }
            }
        }

        if(bCreateOverviews)
        {
            int anOverviewList[25] = {0};
            int nLevelCount = GetOverviewLevels(pwxGISRasterDataset, anOverviewList);

            wxString sProgressMsg = wxString::Format(_("Creating pyramids for : %s (%d bands)"), soFileName.c_str(), pwxGISRasterDataset->GetRaster()->GetRasterCount());
            IStatusBar* pStatusBar = m_pApp->GetStatusBar();
            IProgressor* pProgressor = pStatusBar->GetProgressor();
            if(pProgressor)
                pProgressor->Show(true);

            OvrProgressData Data = {pStatusBar, pProgressor, sProgressMsg};
            GDALDataset* pDSet = pwxGISRasterDataset->GetRaster();
            if(!pDSet)
                return;

            if(pwxGISRasterDataset->GetSubType() == enumRasterImg)
            {
                CPLSetConfigOption( "USE_RRD", "YES" );
                CPLSetConfigOption( "HFA_USE_RRD", "YES" );
            }
            else
                CPLSetConfigOption( "USE_RRD", "NO" );

	        CPLErr eErr = pDSet->BuildOverviews( wgWX2MB(sResampleMethod), nLevelCount, anOverviewList, 0, NULL, OvrProgress, (void*)&Data );

            if(pProgressor)
                pProgressor->Show(false);
            pStatusBar->SetMessage(_("Done"));

		    if(eErr != CE_None)
		    {
                const char* pszErr = CPLGetLastErrorMsg();
                wxLogError(_("BuildOverviews failed! GDAL error: %s"), wgMB2WX(pszErr));
                wxMessageBox(_("Build Overviews failed!"), _("Error"), wxICON_ERROR | wxOK );
		    }
            else
                pwxGISRasterDataset->SetHasOverviews(true);
        }
    }
	*/
}
