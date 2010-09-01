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
#include "wxgis/catalogui/gxmapview.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/rasterlayer.h"
#include "wxgis/framework/framework.h"
#include "wxgis/framework/messagedlg.h"

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
END_EVENT_TABLE()

wxGxMapView::wxGxMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISMapView(parent, id, pos, size), m_pStatusBar(NULL)
{
	m_sViewName = wxString(_("Geography View"));
}

wxGxMapView::~wxGxMapView(void)
{
}

bool wxGxMapView::Activate(IGxApplication* application, wxXmlNode* pConf)
{
	wxGxView::Activate(application, pConf);
	//Serialize(m_pXmlConf, false);

    m_pCatalog = application->GetCatalog();
    m_pSelection = m_pCatalog->GetSelection();
    m_pApp = dynamic_cast<IApplication*>(application);
    if(!m_pApp)
        return false;
	m_pStatusBar = m_pApp->GetStatusBar();
	GetTrackCancel()->SetProgressor(m_pStatusBar->GetAnimation());
	//m_pAni = static_cast<wxGISAnimation*>(m_pStatusBar->GetAnimation());

	//wxGISMapView::SetTrackCancel();
	return true;
}

void wxGxMapView::Deactivate(void)
{
	//Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

bool wxGxMapView::Applies(IGxSelection* Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); i++)
	{
		IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>( Selection->GetSelectedObjects(i) );
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

void wxGxMapView::OnSelectionChanged(IGxSelection* Selection, long nInitiator)
{
    IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();
	if(m_pParentGxObject == pGxObj)
		return;
    	
	IGxDataset* pGxDataset =  dynamic_cast<IGxDataset*>(pGxObj);
	if(pGxDataset == NULL)
		return;

//    wxBusyCursor wait;
	wxGISDataset* pwxGISDataset = pGxDataset->GetDataset();
	if(pwxGISDataset == NULL)
		return;

    m_pParentGxObject = pGxObj;
    //the pOGRLayer will live while IGxObject live. IGxObject( from IGxSelection ) store IwxGISDataset, and destroy it then catalog destroyed
    pwxGISDataset->Dereference();

	wxGISEnumDatasetType type = pwxGISDataset->GetType();
    std::vector<wxGISLayer*> pwxGISLayers;
	switch(type)
	{
	case enumGISFeatureDataset:
		pwxGISLayers.push_back(new wxGISFeatureLayer(pwxGISDataset));
        pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISDataset->GetName());
		break;
	case enumGISRasterDataset:
        CheckOverviews(pwxGISDataset, pGxObj->GetName());
		pwxGISLayers.push_back(new wxGISRasterLayer(pwxGISDataset));
        pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISDataset->GetName());
		break;
	case enumGISContainer:
        for(size_t i = 0; i < pwxGISDataset->GetSubsetsCount(); i++)
        {
            wxGISDataset* pwxGISSubDataset = pwxGISDataset->GetSubset(i);
            pwxGISSubDataset->Dereference();
            if(!pwxGISSubDataset)
                continue;
            wxGISEnumDatasetType subtype = pwxGISSubDataset->GetType();
	        switch(subtype)
	        {
	        case enumGISFeatureDataset:
		        pwxGISLayers.push_back(new wxGISFeatureLayer(pwxGISSubDataset));
                pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISSubDataset->GetName());
		        break;
	        case enumGISRasterDataset:
                CheckOverviews(pwxGISSubDataset, pGxObj->GetName());
		        pwxGISLayers.push_back(new wxGISRasterLayer(pwxGISSubDataset));
                pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISSubDataset->GetName());
		        break;
            }
        }
		break;
	default:
		break;
	}

    ClearLayers();

    for(size_t i = 0; i < pwxGISLayers.size(); i++)
    {
	    if(pwxGISLayers[i] && pwxGISLayers[i]->IsValid())
		    AddLayer(pwxGISLayers[i]);
    }

#ifdef __WXGTK__
	wxMilliSleep(200);
#endif

    SetFullExtent();
}

void wxGxMapView::OnMouseMove(wxMouseEvent& event)
{
	IDisplayTransformation* pDisplayTransformation = pGISScreenDisplay->GetDisplayTransformation();
	if(pDisplayTransformation)
	{
		wxPoint* pDCPoint = new wxPoint(event.m_x, event.m_y);
		OGRRawPoint* pGeoPoints = pDisplayTransformation->TransformCoordDC2World(pDCPoint, 1);
        int nPanePos = m_pStatusBar->GetPanePos(enumGISStatusPosition);
		m_pStatusBar->SetMessage(wxString::Format(_("X: %.4f  Y: %.4f"), pGeoPoints->x, pGeoPoints->y), nPanePos);//_("X: %u  Y: %u")
		delete [] pDCPoint;
		delete [] pGeoPoints;
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

void wxGxMapView::CheckOverviews(wxGISDataset* pwxGISDataset, wxString soFileName)
{
 	wxGISRasterDataset *pwxGISRasterDataset = dynamic_cast<wxGISRasterDataset*>(pwxGISDataset);
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
                bAskCreateOvr = wxAtoi(pNode->GetPropVal(wxT("ask_create_ovr"), wxT("1")));
                sCompress = pNode->GetPropVal(wxT("ovr_compress"), wxT("NONE"));
                sResampleMethod = pNode->GetPropVal(wxT("ovr_resample"), wxT("GAUSS"));
                //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"
                bCreateOverviews = wxAtoi(pNode->GetPropVal(wxT("create_ovr"), wxT("1")));
            }
            else
            {
                pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")), true);
                pNode->AddProperty(wxT("create_ovr"), wxT("1"));
                pNode->AddProperty(wxT("ask_create_ovr"), wxT("1"));
                pNode->AddProperty(wxT("ovr_compress"), wxT("NONE"));
                pNode->AddProperty(wxT("ovr_resample"), wxT("NONE"));//wxT("GAUSS"));
            }
            CPLSetConfigOption( "COMPRESS_OVERVIEW", wgWX2MB(sCompress) );//LZW "DEFLATE"
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
                    pNode->DeleteProperty(wxT("ask_create_ovr"));
                    pNode->AddProperty(wxT("ask_create_ovr"), wxT("0"));
                    pNode->DeleteProperty(wxT("create_ovr"));
                    pNode->AddProperty(wxT("create_ovr"), wxString::Format(wxT("%d"), bCreateOverviews));
                }
            }
        }

        if(bCreateOverviews)
        {
            int nSize = MIN(pwxGISRasterDataset->GetHeight(), pwxGISRasterDataset->GetWidth());
            int anOverviewList[25] = {0};
            int nLevel(1);
            int nLevelCount(0);
            while(1)
            {
                nSize /= 2;
                if(nSize < 20)
                    break;
                nLevel *= 2;
                if(nLevel != 2)
                {
                    anOverviewList[nLevelCount] = nLevel;
                    nLevelCount++;
                }
            }
	        //int anOverviewList[8] = { 4, 8, 16, 32, 64, 128, 256, 512 };

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
                wxLogError(_("BuildOverviews failed! OGR error: %s"), wgMB2WX(pszErr));
                wxMessageBox(_("Build Overviews failed!"), _("Error"), wxICON_ERROR | wxOK );
		    }
            else
                pwxGISRasterDataset->SetHasOverviews(true);
        }
    }
}
