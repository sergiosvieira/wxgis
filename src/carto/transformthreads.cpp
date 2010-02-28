/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Transformation Threads header.
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

#include "wxgis/carto/transformthreads.h"
#include "wxgis/geometry/algorithm.h"

wxGISFeatureTransformThread::wxGISFeatureTransformThread(wxGISFeatureDataset* pwxGISFeatureDataset, OGRCoordinateTransformation *poCT, bool bTransform, OGRPolygon* pRgn1, OGRPolygon* pRgn2, wxCriticalSection* pCritSect, OGREnvelope* pFullEnv, wxGISFeatureSet* pOGRFeatureArray, size_t &nCounter, wxGISProgressor* pProgressor, ITrackCancel* pTrackCancel) : wxThread(wxTHREAD_JOINABLE), m_nCounter(nCounter)
{
    m_pwxGISFeatureDataset = pwxGISFeatureDataset;
    m_poCT = poCT;
    m_pTrackCancel = pTrackCancel;
    m_bTransform = bTransform;
    m_pRgn1 = pRgn1;
    m_pRgn2 = pRgn2;
    m_pCritSect = pCritSect;
    m_pFullEnv = pFullEnv;
    m_pOGRFeatureArray = pOGRFeatureArray;
    m_pProgressor = pProgressor;
//??
    if(m_poCT->GetSourceCS()->IsGeographic())
        m_fSegSize = 3.0;
    else
        m_fSegSize = 130.0;
}

void *wxGISFeatureTransformThread::Entry()
{
    
    size_t nStep = m_pwxGISFeatureDataset->GetSize() < 20 ? 1 : m_pwxGISFeatureDataset->GetSize() / 20;

 	OGRFeature* poFeature;
    while((poFeature = m_pwxGISFeatureDataset->Next()) != NULL)	
    {
        if(m_pTrackCancel && !m_pTrackCancel->Continue())
            return NULL;

        OGRGeometry* pFeatureGeom = poFeature->GetGeometryRef();

        if(m_bTransform)
        {
            if(m_pRgn1 == NULL && m_pRgn2 == NULL)
            {
                if(pFeatureGeom->transform( m_poCT ) != OGRERR_NONE)
                {
                    OGRFeature::DestroyFeature(poFeature);
                    continue;
                }
            }
            else
            {

                OGREnvelope *pRgnEnv1(NULL), *pRgnEnv2(NULL);
                if(m_pRgn1)
                {
                    pRgnEnv1 = new OGREnvelope();
                    m_pRgn1->getEnvelope(pRgnEnv1);
                }
                if(m_pRgn2)
                {
                    pRgnEnv2 = new OGREnvelope();
                    m_pRgn2->getEnvelope(pRgnEnv2);
                }

                //speed fix needed fo diff types of geom
                OGRGeometry* pGeom = CheckRgnAndTransform1(pFeatureGeom, m_pRgn1, m_pRgn2, pRgnEnv1, pRgnEnv2, m_poCT);
                wxDELETE(pRgnEnv1);
                wxDELETE(pRgnEnv2);

                if(pGeom)
                    poFeature->SetGeometryDirectly(pGeom);

                //OGRGeometry* pGeom1 = CheckRgnAndTransform(pFeatureGeom, m_pRgn1, pRgnEnv1, m_poCT);
                //OGRGeometry* pGeom2 = CheckRgnAndTransform(pFeatureGeom, m_pRgn2, pRgnEnv2, m_poCT);
                //OGRGeometry *pGeom(NULL), *pGeom1(NULL), *pGeom2(NULL);

                //if(m_pRgn1)
                //    pGeom1 = pFeatureGeom->Intersection(m_pRgn1);
                //if(m_pRgn2)
                //    pGeom2 = pFeatureGeom->Intersection(m_pRgn2);

                //if(pGeom1 && pGeom1->transform( m_poCT ) != OGRERR_NONE)
                //{
                //    wxDELETE(pGeom1);
                //}  
                //if(pGeom2 && pGeom2->transform( m_poCT ) != OGRERR_NONE)
                //{
                //    wxDELETE(pGeom2);
                //}  
                //if(pGeom1 && !pGeom2)
                //    poFeature->SetGeometryDirectly(pGeom1); 
                //else if(pGeom2 && !pGeom1)
                //    poFeature->SetGeometryDirectly(pGeom2); 
                //else if(pGeom1 && pGeom2)
                //{
                //    OGRGeometryCollection* pGeometryCollection = new OGRGeometryCollection();
                //    pGeometryCollection->addGeometryDirectly(pGeom1);
                //    pGeometryCollection->addGeometryDirectly(pGeom2);
                //    poFeature->SetGeometryDirectly(pGeometryCollection); 
                //}
                else
                {
                    OGRFeature::DestroyFeature(poFeature);
                    continue;
                }
            }
        }

        OGREnvelope Env;
        poFeature->GetGeometryRef()->getEnvelope(&Env);
        m_pCritSect->Enter();

        m_pFullEnv->Merge(Env);
	    m_pOGRFeatureArray->AddFeature(poFeature);

        m_nCounter++;
        if(m_pProgressor && m_nCounter % nStep == 0)
            m_pProgressor->SetValue(m_nCounter);

        m_pCritSect->Leave();
    }
    
	return NULL;
}
void wxGISFeatureTransformThread::OnExit()
{
}

//OGRGeometry* wxGISFeatureTransformThread::CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv, OGRCoordinateTransformation *poCT)
//{
//    if(!pFeatureGeom)
//        return NULL;
//    if(!pRgn)
//        return NULL;
//    if(!pRgnEnv)
//        return NULL;    
//    if(!poCT)
//        return NULL;
//
//    OGREnvelope FeatureEnv;
//    pFeatureGeom->getEnvelope(&FeatureEnv);
//    if(!FeatureEnv.Intersects(*pRgnEnv))
//        return NULL;
//
//    OGRGeometry* pGeom;
//    if(pRgnEnv->Contains(FeatureEnv) )
//        pGeom = pFeatureGeom->clone();
//    else if(pFeatureGeom->Within(pRgn))
//        pGeom = pFeatureGeom->clone();
//    else
//        pGeom = pFeatureGeom->Intersection(pRgn);//speed!Intersection
//
//    if(!pGeom)
//        return NULL;
//
//    if(pGeom->getSpatialReference() == NULL)
//        pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
//
//    if(pGeom->transform( poCT ) != OGRERR_NONE)
//    {
//        wxDELETE(pGeom);
//        return NULL;
//    }  
//    return pGeom;
//}


OGRGeometry* wxGISFeatureTransformThread::CheckRgnAndTransform1(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT)
{
    if(!pFeatureGeom)
        return NULL;
    if(!poCT)
        return NULL;

    OGREnvelope FeatureEnv;
    OGRGeometry *pGeom(NULL), *pGeom1(NULL), *pGeom2(NULL);

    pFeatureGeom->getEnvelope(&FeatureEnv);

    if(pRgnEnv1 && FeatureEnv.Intersects(*pRgnEnv1))
    {
        if(pRgnEnv1->Contains(FeatureEnv) )
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else if(pFeatureGeom->Within(pRgn1))
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else
            pGeom1= Intersection(pFeatureGeom, pRgn1, pRgnEnv1);
    }
    if(pRgnEnv2 && FeatureEnv.Intersects(*pRgnEnv2))
    {
        if(pRgnEnv2->Contains(FeatureEnv) )
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else if(pFeatureGeom->Within(pRgn2))
        {
            pGeom = pFeatureGeom->clone();
            goto PROCEED;
        }
        else
            pGeom2= Intersection(pFeatureGeom, pRgn2, pRgnEnv2);
    }

    if(pGeom1 && !pGeom2)
        pGeom = pGeom1; 
    else if(pGeom2 && !pGeom1)
        pGeom = pGeom2;
    else if(pGeom1 && pGeom2)
    {
        OGRGeometryCollection* pGeometryCollection = new OGRGeometryCollection();
        pGeometryCollection->addGeometryDirectly(pGeom1);
        pGeometryCollection->addGeometryDirectly(pGeom2);
        pGeom = pGeometryCollection; 
    }


PROCEED:
    if(!pGeom)
        return NULL;

    if(pGeom->getSpatialReference() == NULL)
        pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());

    if(pGeom->transform( poCT ) != OGRERR_NONE)
    {
        wxDELETE(pGeom);
        return NULL;
    }  
    return pGeom;
}

OGRGeometry* wxGISFeatureTransformThread::Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv)
{
    if(!pFeatureGeom || !pRgnEnv)
        return NULL;

    OGREnvelope FeatureEnv;
    pFeatureGeom->getEnvelope(&FeatureEnv);

    OGRwkbGeometryType gType = wkbFlatten(pFeatureGeom->getGeometryType());
    switch(gType)
    {
    case wkbPoint:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        return NULL;
    case wkbLineString:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        {
            wxGISAlgorithm alg;
            return alg.FastLineIntersection(pFeatureGeom, pRgn);
        }
       //return pFeatureGeom->Intersection(pRgn);
    case wkbPolygon:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        //return FastPolygonIntersection(pFeatureGeom, pRgnEnv);
        return pFeatureGeom->Intersection(pRgn);
    case wkbLinearRing:
        if(pRgnEnv && pRgnEnv->Contains(FeatureEnv) )
            return pFeatureGeom->clone();
        return pFeatureGeom->Intersection(pRgn);
    case wkbMultiPoint:
    case wkbMultiLineString:
    case wkbMultiPolygon:
    case wkbGeometryCollection:
        {
        	OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pFeatureGeom;
            OGRGeometryCollection* pNewOGRGeometryCollection = new OGRGeometryCollection();
            for(size_t i = 0; i < pOGRGeometryCollection->getNumGeometries(); i++)
            {
                OGRGeometry* pGeom = (OGRGeometry*)pOGRGeometryCollection->getGeometryRef(i);
                pGeom->assignSpatialReference(pFeatureGeom->getSpatialReference());
                OGRGeometry* pNewGeom = Intersection(pGeom, pRgn, pRgnEnv);
                if(pNewGeom)
                    pNewOGRGeometryCollection->addGeometryDirectly(pNewGeom);
            }
            if(pNewOGRGeometryCollection->getNumGeometries() == 0)
            {
                wxDELETE(pNewOGRGeometryCollection);
                return NULL;
            }
            else
                return pNewOGRGeometryCollection;
        }
        break;
    case wkbUnknown:
    case wkbNone:
        break;
    }
    return NULL;
}

//OGRGeometry* wxGISFeatureTransformThread::FastPolygonIntersection(OGRGeometry* pGeom, OGREnvelope* pEnv)
//{
//    OGRPolygon* pPoly = (OGRPolygon*)pGeom;
//    OGRGeometry* pOGRLineString = FastLineIntersection(pPoly->getExteriorRing(), pEnv);
//    if(pOGRLineString == NULL)
//        return NULL;
//
//    OGRPolygon* pNewPoly = new OGRPolygon();
//    pNewPoly->addRingDirectly((OGRLinearRing*)pOGRLineString);
//    for(size_t i = 0; i < pPoly->getNumInteriorRings(); i++)
//    {
//        pOGRLineString = FastLineIntersection(pPoly->getInteriorRing(i), pEnv);
//        if(pOGRLineString == NULL)
//            continue;
//        pNewPoly->addRingDirectly((OGRLinearRing*)pOGRLineString);
//    }
//    return pNewPoly;
//}
//
//OGRGeometry* wxGISFeatureTransformThread::FastLineIntersection(OGRGeometry* pGeom, OGREnvelope* pEnv)
//{
//    OGREnvelope Env;
//    Env.MaxY = pEnv->MaxY + 0.5;
//    Env.MaxX = pEnv->MaxX + 0.5;
//    Env.MinY = pEnv->MinY - 0.5;
//    Env.MinX = pEnv->MinX - 0.5;
//
//    OGRLineString* pOGRLineString = (OGRLineString*)pGeom->clone();
//    //pOGRLineString->segmentize(m_fSegSize);
//    int nPointCount = pOGRLineString->getNumPoints();
//
//    OGRRawPoint* pPoints = new OGRRawPoint[nPointCount];
//    OGRRawPoint* pNewPoints = new OGRRawPoint[nPointCount * 3];
//    double* pZValues(NULL);
//    double* pNewZValues(NULL); 
//    if(pOGRLineString->getCoordinateDimension() > 2)
//    {
//        pZValues = new double[nPointCount];
//        pNewZValues = new double[nPointCount * 3];
//    }
//
//    pOGRLineString->getPoints(pPoints, pZValues);
//    int pos = 0;
//
//    int bWithin = 0; // -1 outside; 1 inside
//    OGRRawPoint EnvPt1;
//    EnvPt1.x = Env.MinX;
//    EnvPt1.y = Env.MinY;
//    OGRRawPoint EnvPt2;
//    EnvPt2.x = Env.MinX;
//    EnvPt2.y = Env.MaxY;
//    OGRRawPoint EnvPt3;
//    EnvPt3.x = Env.MaxX;
//    EnvPt3.y = Env.MaxY;
//    OGRRawPoint EnvPt4;
//    EnvPt4.x = Env.MaxX;
//    EnvPt4.y = Env.MinY;
//
//    for(size_t i = 0; i < nPointCount; i++)
//    {
//        if(Env.MaxX > pPoints[i].x && Env.MinX < pPoints[i].x && Env.MaxY > pPoints[i].y && Env.MinY < pPoints[i].y)
//        {
//            if(bWithin == -1)
//            {
//                OGRRawPoint* pPT = NULL;
//                if(Env.MaxX < pPoints[i - 1].x)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt3, EnvPt4);
//                else if(Env.MinX > pPoints[i - 1].x)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt1, EnvPt2);
//                else if(Env.MaxY < pPoints[i - 1].y)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt2, EnvPt3);
//                else if(Env.MinY > pPoints[i - 1].y)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt4, EnvPt1);
//
//                if(pPT)  
//                {
//                    pNewPoints[pos] = *pPT;
//                    if(pZValues)
//                        pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                    pos++;
//                    wxDELETE(pPT);
//                }
//            }
//            pNewPoints[pos] = pPoints[i];
//            if(pZValues)
//                pNewZValues[pos] = pZValues[i];
//            pos++;
//            bWithin = 1;
//        }
//        else
//        {
//            if(bWithin == 1)
//            {
//                OGRRawPoint* pPT = NULL;
//                if(Env.MaxX < pPoints[i].x)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt3, EnvPt4);
//                else if(Env.MinX > pPoints[i].x)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt1, EnvPt2);
//                else if(Env.MaxY < pPoints[i].y)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt2, EnvPt3);
//                else if(Env.MinY > pPoints[i].y)
//                    pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt4, EnvPt1);
//
//                if(pPT)  
//                {
//                    pNewPoints[pos] = *pPT;
//                    if(pZValues)
//                        pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                    pos++;
//                    wxDELETE(pPT);
//                }
//            }
//            else
//            {
//                if(i != 0)
//                {
//                    if((Env.MaxX < pPoints[i].x && Env.MinX > pPoints[i - 1].x) || (Env.MaxX < pPoints[i - 1].x && Env.MinX > pPoints[i].x))
//                    {
//                        OGRRawPoint* pPT = NULL;
//                        pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt3, EnvPt4);
//                        if(pPT)
//                        {
//                            pNewPoints[pos] = *pPT;
//                            if(pZValues)
//                                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                            pos++;
//                            wxDELETE(pPT);
//                        }
//                        pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt1, EnvPt2);
//                        if(pPT)
//                        {
//                            pNewPoints[pos] = *pPT;
//                            if(pZValues)
//                                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                            pos++;
//                            wxDELETE(pPT);
//                        }
//                    }
////                    else if((pEnv->MaxX < pPoints[i].x && pEnv->MinX > pPoints[i - 1].x) || (pEnv->MaxX < pPoints[i - 1].x && pEnv->MinX > pPoints[i].x))
//                    else if((Env.MaxY < pPoints[i].y && Env.MinY > pPoints[i - 1].y) || (Env.MaxY < pPoints[i - 1].y && Env.MinY > pPoints[i].y))
//                    {
//                        OGRRawPoint* pPT = NULL;
//                        pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt3, EnvPt4);
//                        if(pPT)
//                        {
//                            pNewPoints[pos] = *pPT;
//                            if(pZValues)
//                                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                            pos++;
//                            wxDELETE(pPT);
//                        }
//                        pPT = Crossing(pPoints[i - 1], pPoints[i], EnvPt1, EnvPt2);
//                        if(pPT)
//                        {
//                            pNewPoints[pos] = *pPT;
//                            if(pZValues)
//                                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                            pos++;
//                            wxDELETE(pPT);
//                        }
//                    }
//                }
//            }
//            bWithin = -1;
//        }
//    }
//    wxDELETEA(pPoints);
//    wxDELETEA(pZValues);
//    if(!pos)
//    {
//        wxDELETEA(pNewPoints);
//        wxDELETEA(pNewZValues);
//        wxDELETE(pOGRLineString);
//        return NULL;
//    }
//    pOGRLineString->setPoints(pos, pNewPoints, pNewZValues);
//    wxDELETEA(pNewPoints);
//    wxDELETEA(pNewZValues);
//    pOGRLineString->segmentize(m_fSegSize);
//    return pOGRLineString;
//}
