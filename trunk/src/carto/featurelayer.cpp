/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  FeatureLayer header.
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
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/simplerenderer.h"
#include "wxgis/carto/spvalidator.h"
#include "wxgis/framework/application.h"
#include "wxgis/carto/transformthreads.h"
#include <wx/stopwatch.h>

#define STEP 3.0

wxGISFeatureLayer::wxGISFeatureLayer(wxGISDataset* pwxGISDataset) : wxGISLayer(), m_pwxGISFeatureDataset(NULL), m_pFeatureRenderer(NULL), m_pQuadTree(NULL), m_pSpatialReference(NULL), m_bIsFeaturesLoaded(false)
{
	m_pwxGISFeatureDataset = dynamic_cast<wxGISFeatureDataset*>(pwxGISDataset);
	if(m_pwxGISFeatureDataset)
	{
		m_pwxGISFeatureDataset->Reference();
		m_pFeatureRenderer = new wxGISSimpleRenderer();
//        m_pSpatialReference = ;
		//pre load features
//        LoadFeatures();
		SetSpatialReference(m_pwxGISFeatureDataset->GetSpatialReference());
	}
}

wxGISFeatureLayer::~wxGISFeatureLayer(void)
{
	wxDELETE(m_pFeatureRenderer);

    UnloadFeatures();

	wsDELETE(m_pwxGISFeatureDataset);
    wxDELETE(m_pSpatialReference);
}

void wxGISFeatureLayer::Draw(wxGISEnumDrawPhase DrawPhase, ICachedDisplay* pDisplay, ITrackCancel* pTrackCancel)
{
	IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
	if(!pDisplayTransformation)
		return;

    RECTARARRAY* pInvalidrectArray = pDisplay->GetInvalidRect();
    OGREnvelope Envs[10];
    size_t EnvCount = pInvalidrectArray->size();
    if(EnvCount > 10)
        EnvCount = 10;

    if(EnvCount == 0)
    {
        Envs[0] = pDisplayTransformation->GetVisibleBounds();
        EnvCount = 1;
    }
    else
    {
        for(size_t i = 0; i < EnvCount; i++)
        {
            wxRect Rect = pInvalidrectArray->operator[](i);            
            Rect.Inflate(1,1);
            Envs[i] = pDisplayTransformation->TransformRect(Rect);
        }
    }

    for(size_t i = 0; i < EnvCount; i++)
    {
	    //1. get envelope
	    OGREnvelope Env = Envs[i];//pDisplayTransformation->GetVisibleBounds();
	    bool bSetFilter(false);
	    if(fabs(m_PreviousDisplayEnv.MaxX - Env.MaxX) > DELTA || fabs(m_PreviousDisplayEnv.MaxY - Env.MaxY) > DELTA || fabs(m_PreviousDisplayEnv.MinX - Env.MinX) > DELTA || fabs(m_PreviousDisplayEnv.MinY - Env.MinY) > DELTA)
		    bSetFilter = m_FullEnv.Intersects(Env);

	    //2. set spatial filter
	    pDisplay->StartDrawing(GetCacheID());
	    if(m_pFeatureRenderer && m_pFeatureRenderer->CanRender(m_pwxGISFeatureDataset))
	    {
		    //3. get features set
		    if(bSetFilter)
		    {
                if(!m_pQuadTree)
                    continue;

			    int count(0);
			    CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
			    OGRFeature** pFeatureArr = (OGRFeature**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
		        wxGISFeatureSet GISFeatureSet(count);
			    for(size_t i = 0; i < count; i++)
			    {
				    if(pTrackCancel && !pTrackCancel->Continue())
					    break;
				    GISFeatureSet.AddFeature(pFeatureArr[i]);
			    }
			    wxDELETEA( pFeatureArr );
		        m_pFeatureRenderer->Draw(&GISFeatureSet, DrawPhase, pDisplay, pTrackCancel);
		    }
		    else
                m_pFeatureRenderer->Draw(&m_OGRFeatureArray, DrawPhase, pDisplay, pTrackCancel);
		    //4. send it to renderer
		    m_PreviousDisplayEnv = Env;
	    }
    }
	//5. clear a spatial filter		
	pDisplay->FinishDrawing();
}

OGRSpatialReference* wxGISFeatureLayer::GetSpatialReference(void)
{
	return m_pSpatialReference;
}

const OGREnvelope* wxGISFeatureLayer::GetEnvelope(void)
{
	return &m_FullEnv;
}

bool wxGISFeatureLayer::IsValid(void)
{
	return m_pwxGISFeatureDataset == NULL ? false : true;
}

void wxGISFeatureLayer::Empty(void)
{
	for(size_t i = 0; i < m_OGRFeatureArray.GetSize(); i++)
		OGRFeature::DestroyFeature( m_OGRFeatureArray[i] );
	m_OGRFeatureArray.Clear();
}

void wxGISFeatureLayer::CreateQuadTree(OGREnvelope* pEnv)
{
    DeleteQuadTree();
    CPLRectObj Rect = {pEnv->MinX, pEnv->MinY, pEnv->MaxX, pEnv->MaxY};
    m_pQuadTree = CPLQuadTreeCreate(&Rect, GetFeatureBoundsFunc);
}

void wxGISFeatureLayer::DeleteQuadTree(void)
{
	if(m_pQuadTree)
    {
		CPLQuadTreeDestroy(m_pQuadTree);
        m_pQuadTree = NULL;
    }
}

void wxGISFeatureLayer::LoadFeatures(void)
{
    if(!IsValid() && m_bIsFeaturesLoaded)
        return;
    UnloadFeatures();
    m_FullEnv.MinX = m_FullEnv.MaxX = m_FullEnv.MinY = m_FullEnv.MaxY = 0;

    if(!m_pwxGISFeatureDataset->Open())
    {
		const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("wxGISFeatureLayer: Open failed! OGR error: %s"), wgMB2WX(err));
		wxLogError(sErr);
		wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
        return;
    }

    bool bTransform(false);
    OGRPolygon* pRgn1 = NULL;
    OGRPolygon* pRgn2 = NULL;

	OGRSpatialReference* pDatasetSpaRef = m_pwxGISFeatureDataset->GetSpatialReference();
    //get cut poly
	if(m_pSpatialReference && pDatasetSpaRef)
    {
        if(!m_pSpatialReference->IsSame(pDatasetSpaRef))
        {
            //get envelope
            wxGISSpatialReferenceValidator GISSpaRefValidator;
            wxString sProjName;
            if(m_pSpatialReference->IsProjected())
                sProjName = wgMB2WX(m_pSpatialReference->GetAttrValue("PROJCS")); 
            else
                sProjName = wgMB2WX(m_pSpatialReference->GetAttrValue("GEOGCS")); 
            if(GISSpaRefValidator.IsLimitsSet(sProjName))
            {
                LIMITS lims = GISSpaRefValidator.GetLimits(sProjName);
                if(lims.minx > lims.maxx)
                {
                    OGRLinearRing ring1;
                    ring1.addPoint(lims.minx,lims.miny);
                    ring1.addPoint(lims.minx,lims.maxy);
                    ring1.addPoint(180.0,lims.maxy);
                    ring1.addPoint(180.0,lims.miny);
                    ring1.closeRings();

                    pRgn1 = new OGRPolygon();
                    pRgn1->addRing(&ring1);	
                    pRgn1->flattenTo2D();

                    OGRLinearRing ring2;
                    ring2.addPoint(-180.0,lims.miny);
                    ring2.addPoint(-180.0,lims.maxy);
                    ring2.addPoint(lims.maxx,lims.maxy);
                    ring2.addPoint(lims.maxx,lims.miny);
                    ring2.closeRings();

                    pRgn2 = new OGRPolygon();
                    pRgn2->addRing(&ring2);	
                    pRgn2->flattenTo2D();
                }
                else
                {
                    OGRLinearRing ring;
                    ring.addPoint(lims.minx,lims.miny);
                    ring.addPoint(lims.minx,lims.maxy);
                    ring.addPoint(lims.maxx,lims.maxy);
                    ring.addPoint(lims.maxx,lims.miny);
                    ring.closeRings();

                    pRgn1 = new OGRPolygon();
                    pRgn1->addRing(&ring);	
                    pRgn1->flattenTo2D();
                }
                //WGS84     
                OGRSpatialReference* pWGSSpaRef = new OGRSpatialReference(SRS_WKT_WGS84);

                if(pRgn1 != NULL)
                {
                    pRgn1->assignSpatialReference(pWGSSpaRef);
                    pRgn1->segmentize(STEP);
                }
                if(pRgn2 != NULL)
                {
                    pRgn2->assignSpatialReference(pWGSSpaRef);
                    pRgn2->segmentize(STEP);
                }
                pWGSSpaRef->Dereference();
                
                if(!pDatasetSpaRef->IsSame(pWGSSpaRef))
                {
                    if(pRgn1->transformTo(pDatasetSpaRef) != OGRERR_NONE)
                        wxDELETE(pRgn1);
                    if(pRgn2->transformTo(pDatasetSpaRef) != OGRERR_NONE)
                        wxDELETE(pRgn2);
                }
            }
            //create cut poly
            bTransform = true;
        }
    }

    IApplication* pApp = ::GetApplication();
    IStatusBar* pStatusBar = pApp->GetStatusBar();  
    wxGISProgressor* pProgressor = dynamic_cast<wxGISProgressor*>(pStatusBar->GetProgressor());
    if(pProgressor)
    {
        pProgressor->Show(true);
        pProgressor->SetRange(m_pwxGISFeatureDataset->GetSize() * 2);
    }

    
    OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pDatasetSpaRef, m_pSpatialReference );

    m_pwxGISFeatureDataset->Reset();
    //strart multithreaded transform
    int CPUCount = wxThread::GetCPUCount();
    std::vector<wxGISFeatureTransformThread*> threadarray;
    wxCriticalSection CritSect;
    size_t nCounter(0);

    wxStopWatch sw;

    for(int i = 0; i < CPUCount; i++)
    {        
        wxGISFeatureTransformThread *thread = new wxGISFeatureTransformThread(m_pwxGISFeatureDataset, poCT, bTransform, pRgn1, pRgn2, &CritSect, &m_FullEnv, &m_OGRFeatureArray, nCounter, pProgressor, NULL);
        thread->Create();
        thread->Run();
        threadarray.push_back(thread);
    }

    for(size_t i = 0; i < threadarray.size(); i++)
    {
        wgDELETE(threadarray[i], Wait());
    }

    wxLogMessage(wxT("The long running function took %ldms to execute"), sw.Time());

              
    OCTDestroyCoordinateTransformation(poCT);
    wxDELETE(pRgn1);
    wxDELETE(pRgn2);

    CreateQuadTree(&m_FullEnv);

    size_t nStep = m_pwxGISFeatureDataset->GetSize() < 20 ? 1 : m_pwxGISFeatureDataset->GetSize() / 20;

    for(size_t i = 0; i < m_OGRFeatureArray.GetSize(); i++)
    {
	    if(m_pQuadTree)
		    CPLQuadTreeInsert(m_pQuadTree, m_OGRFeatureArray[i]);

        nCounter++;
        if(pProgressor && nCounter % nStep == 0)
            pProgressor->SetValue(nCounter);
    }

    if(pProgressor)
        pProgressor->Show(false);
    pStatusBar->SetMessage(_("Done"));
    m_bIsFeaturesLoaded = true;

}

void wxGISFeatureLayer::UnloadFeatures(void)
{
    if(!m_bIsFeaturesLoaded)
        return;
    DeleteQuadTree();
    Empty();
    m_bIsFeaturesLoaded = false;
}

void wxGISFeatureLayer::SetSpatialReference(OGRSpatialReference* pSpatialReference)
{
    if(NULL == pSpatialReference)
        return;
    if(m_pSpatialReference && m_pSpatialReference->IsSame(pSpatialReference))
        return;
    wxDELETE(m_pSpatialReference);
    m_pSpatialReference = pSpatialReference->Clone();
    LoadFeatures();
}


	//while(nIndex + 1 > m_OGRFeatureArray.size()) //m_pOGRLayer->GetFeatureCount()**
	//{
	//	size_t count(0); 
	//	OGRFeature *poFeature;
	//	while( (count < CACHE_SIZE) && (poFeature = m_poLayer->GetNextFeature()) != NULL )
	//	{
	//		AddFeature(poFeature);
	//		count++;
	//	}
	//}
	//return m_OGRFeatureArray[nIndex];

	//if(m_OGRFeatureArray.size() < GetSize())
	//{
	//	OGRFeature* poFeature;
	//	while((poFeature = m_poLayer->GetNextFeature()) != NULL )
	//	{
	//		if(pTrackCancel && !pTrackCancel->Continue())
	//			return NULL;
	//		AddFeature(poFeature);
	//	}
	//}

