/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Geometry Algorithms.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/geometry/algorithm.h"

//Sutherland-Hodgman Polygon Clipping
//Adopted from (C) 2005 by Gavin Macaulay QGIS Project

void ClipGeometryByEnvelope(OGRRawPoint* pOGRRawPoints, int *pnPointCount, const OGREnvelope &Env)
{
	bool shapeOpen = false;
	OGRRawPoint* pTmpOGRRawPoints = new OGRRawPoint[*pnPointCount];
	int nTmpPtCount(0);
	TrimFeatureToBoundary( pOGRRawPoints, *pnPointCount, &pTmpOGRRawPoints, &nTmpPtCount, enumGISPtPosRight, Env, shapeOpen );

	*pnPointCount = 0;
	TrimFeatureToBoundary( pTmpOGRRawPoints, nTmpPtCount, &pOGRRawPoints, pnPointCount, enumGISPtPosTop, Env, shapeOpen );

	nTmpPtCount = 0;
	TrimFeatureToBoundary( pOGRRawPoints, *pnPointCount, &pTmpOGRRawPoints, &nTmpPtCount, enumGISPtPosLeft, Env, shapeOpen );

	*pnPointCount = 0;
	TrimFeatureToBoundary(  pTmpOGRRawPoints, nTmpPtCount, &pOGRRawPoints, pnPointCount, enumGISPtPosBottom, Env, shapeOpen );

	wxDELETEA(pTmpOGRRawPoints);
}

void WXDLLIMPEXP_GIS_GEOM IncreaseEnvelope(OGREnvelope *pEnv, int nSize)
{
	double dWidth = (pEnv->MaxX - pEnv->MinX) * nSize / 2;
	double dHeight = (pEnv->MaxY - pEnv->MinY) * nSize / 2;
	pEnv->MinX -= dWidth;
	pEnv->MinY -= dHeight;
	pEnv->MaxX += dWidth;
	pEnv->MaxY += dHeight;
}

OGRGeometrySPtr EnvelopeToGeometry(const OGREnvelope &Env, OGRSpatialReferenceSPtr pSpaRef)
{
	if(!Env.IsInit())
		return OGRGeometrySPtr();
	OGRLinearRing ring;
	ring.addPoint(Env.MinX, Env.MinY);
	ring.addPoint(Env.MinX, Env.MaxY);
	ring.addPoint(Env.MaxX, Env.MaxY);
	ring.addPoint(Env.MaxX, Env.MinY);
    ring.closeRings();

    OGRPolygon* pRgn = new OGRPolygon();
    pRgn->addRing(&ring);
    pRgn->flattenTo2D();
	if(pSpaRef)
		pRgn->assignSpatialReference(pSpaRef->Clone());
	return OGRGeometrySPtr(static_cast<OGRGeometry*>(pRgn));
}

//#define LEFT  1  /* двоичное 0001 */
//#define RIGHT 2  /* двоичное 0010 */
//#define BOT   4  /* двоичное 0100 */
//#define TOP   8  /* двоичное 1000 */
//
///* вычисление кода точки 
//   r : указатель на OGREnvelope; p : указатель на OGRRawPoint */
//#define vcode(r, p) \
//	((((p)->x < (r)->MinX) ? LEFT : 0)  +  /* +1 если точка левее прямоугольника */ \
//	 (((p)->x > (r)->MaxX) ? RIGHT : 0) +  /* +2 если точка правее прямоугольника */\
//	 (((p)->y < (r)->MinY) ? BOT : 0)   +  /* +4 если точка ниже прямоугольника */  \
//	 (((p)->y > (r)->MaxY) ? TOP : 0))     /* +8 если точка выше прямоугольника */
//
//#define STEP 3.0
//
////------------------------------------------------------------------------------------
//// wxWAList
////------------------------------------------------------------------------------------
//wxWAList::wxWAList(void)
//{
//}
//
//wxWAList::~wxWAList(void)
//{
//    for(size_t i = 0; i < m_ItemsArr.size(); i++)
//    {
//        wxDELETE(m_ItemsArr[i]);
//    }
//}
//
//wxWAListItem* wxWAList::AddPoint(OGRRawPoint Point, VERTEXTYPE Type)
//{
//    wxWAListItem* pItem = new wxWAListItem();
//    pItem->m_Point = Point;
//    pItem->m_Type = Type;
//    if(GetSize() > 0)
//        m_ItemsArr[m_ItemsArr.size() - 1]->m_pNext = (long)pItem;
//    m_ItemsArr.push_back(pItem);
//    return pItem;
//}
//
//wxWAListItem* wxWAList::InsertPoint(size_t nIndex, OGRRawPoint Point, VERTEXTYPE Type)
//{
//    wxWAListItem* pItem = new wxWAListItem();
//    pItem->m_Point = Point;
//    pItem->m_Type = Type;
//    if(nIndex == 0)
//    {
//        pItem->m_pNext = (long)m_ItemsArr[0];
//    }
//    else if(nIndex == GetSize())
//    {
//        m_ItemsArr[m_ItemsArr.size() - 1]->m_pNext = (long)pItem;
//    }
//    else
//    {
//        m_ItemsArr[nIndex - 1]->m_pNext = (long)pItem;
//        pItem->m_pNext = (long)m_ItemsArr[nIndex];
//    }
//    m_ItemsArr.insert(m_ItemsArr.begin() + nIndex, pItem);
//    return pItem;
//}
//
//wxWAListItem* wxWAList::RemovePoint(wxWAListItem* pItem)
//{
//    for(size_t i = 0; i < m_ItemsArr.size(); i++)
//    {
//        if(m_ItemsArr[i] == pItem)
//        {
//            // point to new item
//            if(i > 0 && i < m_ItemsArr.size() - 1)
//                m_ItemsArr[i - 1]->m_pNext = (long)m_ItemsArr[i + 1];
//            else if(i == 0)
//                m_ItemsArr[m_ItemsArr.size() - 1]->m_pNext = NULL;
//            else if(i = m_ItemsArr.size() - 1)
//                m_ItemsArr[m_ItemsArr.size() - 2]->m_pNext = NULL;
//
//            wxWAListItem* pResItem = (wxWAListItem*)m_ItemsArr[i]->m_pNext;
//            wxDELETE(m_ItemsArr[i]);
//            m_ItemsArr.erase(m_ItemsArr.begin() + i);
//            return pResItem;
//        }
//    }
//    return NULL;
//}
//
//wxWAListItem* wxWAList::GetHead(void)
//{
//    if(GetSize() == NULL)
//        return NULL;
//    return m_ItemsArr[0];
//}
//
//wxWAListItem* wxWAList::operator[](size_t nIndex)
//{
//    if(nIndex >= GetSize())
//        return NULL;
//    return m_ItemsArr[nIndex];
//}
//
//
////--------------------------------------
//// ClipWindow
////--------------------------------------
//
//wxClipWindow::wxClipWindow(OGREnvelope* pEnv) : wxWAList()
//{
//    OGRRawPoint pt;
//    pt.x = pEnv->MinX;
//    pt.y = pEnv->MinY;
//    wxWAList::AddPoint(pt, wxVERTEX);
//    for(double i = pEnv->MinY + STEP; i < pEnv->MaxY; i += STEP)
//    {
//        pt.x = pEnv->MinX;
//        pt.y = i;
//        wxWAList::AddPoint(pt, wxVERTEX);
//    }
//    pt.x = pEnv->MinX;
//    pt.y = pEnv->MaxY;
//    wxWAList::AddPoint(pt, wxVERTEX);
//    for(double i = pEnv->MinX + STEP; i < pEnv->MaxX; i += STEP)
//    {
//        pt.x = i;
//        pt.y = pEnv->MaxY;
//        wxWAList::AddPoint(pt, wxVERTEX);
//    }
//    pt.x = pEnv->MaxX;
//    pt.y = pEnv->MaxY;
//    wxWAList::AddPoint(pt, wxVERTEX);
//    for(double i = pEnv->MaxY - STEP; i > pEnv->MinY; i -= STEP)
//    {
//        pt.x = pEnv->MaxX;
//        pt.y = i;
//        wxWAList::AddPoint(pt, wxVERTEX);
//    }
//    pt.x = pEnv->MaxX;
//    pt.y = pEnv->MinY;
//    wxWAList::AddPoint(pt, wxVERTEX);
//    for(double i = pEnv->MaxX - STEP; i > pEnv->MinX; i -= STEP)
//    {
//        pt.x = i;
//        pt.y = pEnv->MinY;
//        wxWAList::AddPoint(pt, wxVERTEX);
//    }
//    pt.x = pEnv->MinX;
//    pt.y = pEnv->MinY;
//    wxWAListItem* pItem = wxWAList::AddPoint(pt, wxVERTEX);
//    pItem->m_pNext = (long)m_ItemsArr[0];
//}
//
//wxClipWindow::~wxClipWindow(void)
//{
//}
//
//wxWAListItem* wxClipWindow::AddPoint(OGRRawPoint Point, VERTEXTYPE Type)
//{
//    for(size_t i = 0; i < m_ItemsArr.size() - 1; i++)
//    {
//        //1 MinX - x - MaxX MinY
//        if(m_ItemsArr[i]->m_Point.y == Point.y)
//        {
//            if(m_ItemsArr[i]->m_Point.x <= Point.x && m_ItemsArr[i + 1]->m_Point.x > Point.x)
//            {
//                return InsertPoint(i + 1, Point, Type);
//            }
//            if(m_ItemsArr[i]->m_Point.x > Point.x && m_ItemsArr[i + 1]->m_Point.x <= Point.x)
//            {
//                return InsertPoint(i + 1, Point, Type);
//            }
//        }
//        if(m_ItemsArr[i]->m_Point.x == Point.x)
//        {
//            if(m_ItemsArr[i]->m_Point.y <= Point.y && m_ItemsArr[i + 1]->m_Point.y > Point.y)
//            {
//                return InsertPoint(i + 1, Point, Type);
//            }
//            if(m_ItemsArr[i]->m_Point.y > Point.y && m_ItemsArr[i + 1]->m_Point.y <= Point.y)
//            {
//                return InsertPoint(i + 1, Point, Type);
//            }
//        }
//    }
//    wxASSERT(0);
//    return NULL;
//}
//
////------------------------------------------------------------------------------------
//// wxGISAlgorithm
////------------------------------------------------------------------------------------
//
//wxGISAlgorithm::wxGISAlgorithm(void)
//{
//}
//
//wxGISAlgorithm::~wxGISAlgorithm(void)
//{
//}
//
////Алгоритм Коэна — Сазерленда (англ. Cohen-Sutherland) 
//OGRGeometry* wxGISAlgorithm::FastLineIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2)
//{
//    OGREnvelope Env;
//    pGeom2->getEnvelope(&Env);
//
//    OGRLineString* pOGRLineString = (OGRLineString*)pGeom1;
//    int nPointCount = pOGRLineString->getNumPoints();
//
//    OGRRawPoint* pPoints = new OGRRawPoint[nPointCount];
//    OGRRawPoint* pNewPoints = new OGRRawPoint[nPointCount];
//    double* pZValues(NULL);
//    double* pNewZValues(NULL); 
//    if(pOGRLineString->getCoordinateDimension() > 2)
//    {
//        pZValues = new double[nPointCount];
//        pNewZValues = new double[nPointCount];
//    }
//
//    pOGRLineString->getPoints(pPoints, pZValues);
//
//    int pos = 0;    
//    OGRGeometryCollection* pNewOGRGeometryCollection = new OGRGeometryCollection();
//    for(size_t i = 0; i < nPointCount - 1; i++)
//    {
//	    int code_a = vcode(&Env, &pPoints[i]);
//	    int code_b = vcode(&Env, &pPoints[i + 1]);
//        //curve dont enter envelope
//        //wxLogDebug(wxT("x:%f y:%f code_a:%d code_b:%d"), pPoints[i].x, pPoints[i].y, code_a, code_b);
//
//        if (code_a > 0 && code_b > 0)
//        {
//            if(code_a == code_b)
//                continue;
//            else
//            {
//                OGRRawPoint pt = pPoints[i];
//                SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, &Env, code_a);
//                pNewPoints[pos] = pt;
//                if(pZValues)
//                    pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//                pos++;
//                OGRRawPoint pt1 = pPoints[i + 1];
//                SetPointOnEnvelope(&pt, &pPoints[i + 1], &pt1, &Env, code_b);
//                pNewPoints[pos] = pt1;
//                if(pZValues)
//                    pNewZValues[pos] = pZValues[i] + (pNewZValues[pos - 1] - pZValues[i]) / 2;
//                pos++;
//
//                if(pos > 1)
//                {
//                    //create & add new geom
//                    OGRLineString* pOGRNewLineString = new OGRLineString();
//                    pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
//                    pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
//                    pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
//                    pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
//                }
//                pos = 0;
//                continue;
//            }
//        }
//        
//        //curve inside envelope
//        if (!code_a && !code_b)
//        {
//            pNewPoints[pos] = pPoints[i];
//            if(pZValues)
//                pNewZValues[pos] = pZValues[i];
//            pos++;
//            continue;
//        }
//        //curve enter envelope
//        if(!code_b && code_a > 0)
//        {
//            OGRRawPoint pt = pPoints[i];
//            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, &Env, code_a);
//
//            pNewPoints[pos] = pt;
//            if(pZValues)
//                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//            pos++;
//            continue;
//        }
//        //curve exit envelope
//        if(!code_a && code_b > 0)
//        {
//            //add point a
//            pNewPoints[pos] = pPoints[i];
//            if(pZValues)
//                pNewZValues[pos] = pZValues[i];
//            pos++;
//
//            //add point on intesection a ---|--- b
//            OGRRawPoint pt = pPoints[i + 1];
//            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, &Env, code_b);
//
//            pNewPoints[pos] = pt;
//            if(pZValues)
//                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//            pos++;
//            if(pos > 1)
//            {
//                //create & add new geom
//                OGRLineString* pOGRNewLineString = new OGRLineString();
//                pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
//                pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
//                pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
//                pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
//            }
//            pos = 0;
//            continue;
//        }
//    }
//    //last point
//    int code = vcode(&Env, &pPoints[nPointCount - 1]);
//    if(code == 0)
//    {
//        pNewPoints[pos] = pPoints[nPointCount - 1];
//        if(pZValues)
//            pNewZValues[pos] = pZValues[nPointCount - 1];
//        pos++;
//    }
//
//    if(pos > 1)
//    {
//
//        OGRLineString* pOGRNewLineString = new OGRLineString();
//        pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
//        pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
//        pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
//        pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
//    }
//
//    wxDELETEA(pPoints);
//    wxDELETEA(pZValues);
//    wxDELETEA(pNewPoints);
//    wxDELETEA(pNewZValues);
//
//    if(pNewOGRGeometryCollection->getNumGeometries() == 0)
//    {
//        wxDELETE(pNewOGRGeometryCollection);
//        return NULL;
//    }
//    
//    if(pNewOGRGeometryCollection->getNumGeometries() == 1)
//    {
//        OGRGeometry* pRetGeom = pNewOGRGeometryCollection->getGeometryRef(0);
//        pNewOGRGeometryCollection->removeGeometry(0, 0);
//        wxDELETE(pNewOGRGeometryCollection);
//        return pRetGeom;
//    }
//
//    return pNewOGRGeometryCollection;
//}
//
////  Алгоритм Вейлера-Азертона (Weiler-Atherton)
//OGRGeometry* wxGISAlgorithm::FastPolyIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2)
//{
//    OGREnvelope Env;
//    pGeom2->getEnvelope(&Env);
//
//    OGRPolygon* pPoly = (OGRPolygon*)pGeom1;
//    OGRLineString* pOGRLineString = pPoly->getExteriorRing();
//    OGRGeometry* pExtRingGeom = PolyIntersection(&Env, pOGRLineString);
//    if(!pExtRingGeom)
//        return NULL;
//    OGRwkbGeometryType Type = wkbFlatten(pExtRingGeom->getGeometryType());
//    bool bIsRing = (Type == wkbLinearRing) || (Type == wkbLineString);
//    OGRGeometry* pOutGeom(NULL);
//    OGRPolygon* pNewPoly(NULL);
//    OGRGeometryCollection* pNewOGRGeometryCollection(NULL);
//    if(bIsRing)
//    {
//        OGRLinearRing* pExtRing = (OGRLinearRing*)pExtRingGeom;
//        if(!pExtRing)
//            return NULL;
//
//        pNewPoly = new OGRPolygon();
//        pNewPoly->setCoordinateDimension(pGeom1->getCoordinateDimension());
//        pNewPoly->assignSpatialReference(pGeom1->getSpatialReference());
//        pNewPoly->addRingDirectly(pExtRing);
//        pOutGeom = pNewPoly;
//    }
//    else
//    {
//        pNewOGRGeometryCollection = new OGRGeometryCollection();
//        OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pExtRingGeom;
//        while(pOGRGeometryCollection->getNumGeometries() > 0)
//        {
//            pNewPoly = new OGRPolygon();
//            pNewPoly->setCoordinateDimension(pGeom1->getCoordinateDimension());
//            pNewPoly->assignSpatialReference(pGeom1->getSpatialReference());
//            pNewPoly->addRingDirectly((OGRLinearRing*)pOGRGeometryCollection->getGeometryRef(0));
//            pOGRGeometryCollection->removeGeometry(0, 0);
//            pNewOGRGeometryCollection->addGeometryDirectly(pNewPoly);
//        }
//        wxDELETE(pOGRGeometryCollection);
//        pOutGeom = pNewOGRGeometryCollection;
//    }
//
//    int nCount = pPoly->getNumInteriorRings();
//    for(size_t i = 0; i < nCount; i++)
//    {        
//        pOGRLineString = pPoly->getInteriorRing(i);
//        //change for holes!!!
//        OGRGeometry* pIntRingGeom = PolyIntersection(&Env, pOGRLineString);
//        if(pIntRingGeom == NULL)
//            continue;
//        Type = wkbFlatten(pIntRingGeom->getGeometryType());
//        bIsRing = (Type == wkbLinearRing) || (Type == wkbLineString);
//        if(bIsRing)
//        {
//            if(pNewPoly)
//                pNewPoly->addRingDirectly((OGRLinearRing*)pIntRingGeom);
//            if(pNewOGRGeometryCollection)
//            {
//                for(size_t i = 0; i < pNewOGRGeometryCollection->getNumGeometries(); i++)
//                {
//                    OGRGeometry* pPolyGeom = pNewOGRGeometryCollection->getGeometryRef(i);
//                    if(pPolyGeom->Contains(pIntRingGeom))
//                    {
//                        OGRPolygon* pPoly = (OGRPolygon*)pPolyGeom;
//                        pPoly->addRingDirectly((OGRLinearRing*)pIntRingGeom);
//                        break;
//                    }
//                }
//            }
//        }
//        else
//        {
//            OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pExtRingGeom;
//            while(pOGRGeometryCollection->getNumGeometries() > 0)
//            {
//                if(pNewPoly)
//                    pNewPoly->addRingDirectly((OGRLinearRing*)pOGRGeometryCollection->getGeometryRef(0));
//                if(pNewOGRGeometryCollection)
//                {
//                    for(size_t i = 0; i < pNewOGRGeometryCollection->getNumGeometries(); i++)
//                    {
//                        OGRGeometry* pPolyGeom = pNewOGRGeometryCollection->getGeometryRef(i);
//                        if(pPolyGeom->Contains(pOGRGeometryCollection->getGeometryRef(0)))
//                        {
//                            OGRPolygon* pPoly = (OGRPolygon*)pPolyGeom;
//                            pPoly->addRingDirectly((OGRLinearRing*)pOGRGeometryCollection->getGeometryRef(0));
//                            break;
//                        }
//                    }
//                }
//                pOGRGeometryCollection->removeGeometry(0, 0);
//            }
//            wxDELETE(pOGRGeometryCollection);
//       }
//    } 
//    return pOutGeom;
//}
//
//OGRGeometry* wxGISAlgorithm::PolyIntersection(OGREnvelope* pEnv, OGRLineString* pOGRLineString)
//{
//    int nPointCount = pOGRLineString->getNumPoints();
//
//    OGRRawPoint* pPoints = new OGRRawPoint[nPointCount];
//    OGRRawPoint* pNewPoints = new OGRRawPoint[nPointCount * 10];
//    double* pZValues(NULL);
//    double* pNewZValues(NULL); 
//    if(pOGRLineString->getCoordinateDimension() > 2)
//    {
//        pZValues = new double[nPointCount];
//        pNewZValues = new double[nPointCount * 10];
//    }
//
//    wxClipWindow Win(pEnv);
//    wxWAList PolyData;
//
//    pOGRLineString->getPoints(pPoints, pZValues);
//
//    for(size_t i = 0; i < nPointCount - 1; i++)
//    {
//	    int code_a = vcode(pEnv, &pPoints[i]);
//	    int code_b = vcode(pEnv, &pPoints[i + 1]);
//        //curve dont enter envelope
//        //wxLogDebug(wxT("x:%f y:%f code_a:%d code_b:%d"), pPoints[i].x, pPoints[i].y, code_a, code_b);
//
//        if (code_a > 0 && code_b > 0)
//        {
//            if(code_a == code_b)
//                continue;
//            else
//            {
//                OGRRawPoint pt = pPoints[i];
//                SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, pEnv, code_a);
//
//                wxWAListItem* pItem = Win.AddPoint(pt, wxENTER);
//                wxWAListItem* pPolyItem = PolyData.AddPoint(pt, wxENTER);
//                pPolyItem->m_pData = (long)pItem;
//                pItem->m_pData = (long)pPolyItem;
//                //if(pZValues)
//                //    pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//
//                OGRRawPoint pt1 = pPoints[i + 1];
//                SetPointOnEnvelope(&pt, &pPoints[i + 1], &pt1, pEnv, code_b);
//
//                wxWAListItem* pItem1 = Win.AddPoint(pt1, wxEXIT);
//                wxWAListItem* pPolyItem1 = PolyData.AddPoint(pt1, wxEXIT);
//                pPolyItem1->m_pData = (long)pItem1;
//                pItem1->m_pData = (long)pPolyItem1;
//                //if(pZValues)
//                //    pNewZValues[pos] = pZValues[i] + (pNewZValues[pos - 1] - pZValues[i]) / 2;
//                continue;
//            }
//        }
//
//        //curve inside envelope
//        if (!code_a && !code_b)
//        {
//            //pNewPoints[pos] = pPoints[i];
//            //if(pZValues)
//            //    pNewZValues[pos] = pZValues[i];
//            //pos++;
//            PolyData.AddPoint(pPoints[i], wxVERTEX);
//            continue;
//        }
//        //curve enter envelope
//        if(!code_b && code_a > 0)
//        {
//            OGRRawPoint pt = pPoints[i];
//            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, pEnv, code_a);
//
//            wxWAListItem* pItem = Win.AddPoint(pt, wxENTER);
//            wxWAListItem* pPolyItem = PolyData.AddPoint(pt, wxENTER);
//            pPolyItem->m_pData = (long)pItem;
//            pItem->m_pData = (long)pPolyItem;
//
//            //pNewPoints[pos] = pt;
//            //if(pZValues)
//            //    pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//            //pos++;
//            continue;
//        }
//        //curve exit envelope
//        if(!code_a && code_b > 0)
//        {
//            //add point a
//            PolyData.AddPoint(pPoints[i], wxVERTEX);
//            //if(pZValues)
//            //    pNewZValues[pos] = pZValues[i];
//
//            //add point on intesection a ---|--- b
//            OGRRawPoint pt = pPoints[i + 1];
//            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, pEnv, code_b);
//
//            wxWAListItem* pItem = Win.AddPoint(pt, wxEXIT);
//            wxWAListItem* pPolyItem = PolyData.AddPoint(pt, wxEXIT);
//            pPolyItem->m_pData = (long)pItem;
//            pItem->m_pData = (long)pPolyItem;
//
//            //pNewPoints[pos] = pt;
//            //if(pZValues)
//            //    pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
//            continue;
//        }
//    }
//
//    //last point
//    int code = vcode(pEnv, &pPoints[nPointCount - 1]);
//    if(code == 0)
//    {
//        PolyData.AddPoint(pPoints[nPointCount - 1], wxVERTEX);
//        //if(pZValues)
//        //    pNewZValues[pos] = pZValues[nPointCount - 1];
//    }
//
//        
//    OGRGeometryCollection* pNewOGRGeometryCollection = new OGRGeometryCollection();
//
//    while(PolyData.GetSize() > 2)
//    {
//        int pos = 0;    
//        wxWAListItem* pPolyItem = PolyData.GetHead();
//        if(pPolyItem == NULL)
//            goto EXIT;
//
//        while(pPolyItem != NULL && pPolyItem->m_pNext != NULL)
//        {       
//            if(pPolyItem->m_Point.x == pNewPoints[0].x && pPolyItem->m_Point.y == pNewPoints[0].y)
//            {
//                //create new poly
//                if(pos > 2)
//                {
//                    OGRLinearRing* pOGRNewLinearRing = new OGRLinearRing();
//                    pOGRNewLinearRing->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
//                    pOGRNewLinearRing->assignSpatialReference(pOGRLineString->getSpatialReference());
//                    pOGRNewLinearRing->setPoints(pos, pNewPoints, pNewZValues);
//                    pOGRNewLinearRing->closeRings();
//                    pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLinearRing);
//                }
//                pos = 0;
//            }
//
//
//            switch(pPolyItem->m_Type)
//            {
//            case wxENTER:
//            case wxVERTEX:
//                pNewPoints[pos] = pPolyItem->m_Point;
//                pos++;
//                //remove point
//                pPolyItem = PolyData.RemovePoint(pPolyItem);
//                continue;
//                ////pNewPoints[pos] = PolyData[i].pt;
//                //continue;
//            case wxEXIT:
//                {
//                    wxWAListItem* pEnvItem = (wxWAListItem*)pPolyItem->m_pData;
//                    //dont doubled
//                    pEnvItem = pEnvItem->GetNext();
//                    pNewPoints[pos] = pPolyItem->m_Point;
//                    pos++;
//                    PolyData.RemovePoint(pPolyItem);
//                    while(pEnvItem->m_Type != wxENTER)
//                    {
//                        pNewPoints[pos] = pEnvItem->m_Point;
//                        pos++;
//                        pEnvItem = pEnvItem->GetNext();
//                    }
//                    pPolyItem = (wxWAListItem*)pEnvItem->m_pData;
//                }
//                continue;
//            }
//        }
//
//        if(pPolyItem)
//        {
//            pNewPoints[pos] = pPolyItem->m_Point;
//            pos++;
//            if(pPolyItem->m_Type == wxEXIT)
//            {
//                wxWAListItem* pEnvItem = (wxWAListItem*)pPolyItem->m_pData;
//                //dont doubled
//                pEnvItem = pEnvItem->GetNext();
//                while(pEnvItem->m_Type != wxENTER)
//                {
//                    pNewPoints[pos] = pEnvItem->m_Point;
//                    pos++;
//                    pEnvItem = pEnvItem->GetNext();
//                }
//            }
//            PolyData.RemovePoint(pPolyItem);
//        }
//
//        if(pos > 2)
//        {
//            OGRLinearRing* pOGRNewLinearRing = new OGRLinearRing();
//            pOGRNewLinearRing->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
//            pOGRNewLinearRing->assignSpatialReference(pOGRLineString->getSpatialReference());
//            pOGRNewLinearRing->setPoints(pos, pNewPoints, pNewZValues);
//            pOGRNewLinearRing->closeRings();
//            pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLinearRing);
//        }
//    }
//
//EXIT:
//    wxDELETEA(pPoints);
//    wxDELETEA(pZValues);
//    wxDELETEA(pNewPoints);
//    wxDELETEA(pNewZValues);
//
//    if(pNewOGRGeometryCollection->getNumGeometries() == 0)
//    {
//        wxDELETE(pNewOGRGeometryCollection);
//        return NULL;
//    }
//    
//    if(pNewOGRGeometryCollection->getNumGeometries() == 1)
//    {
//        OGRGeometry* pRetGeom = pNewOGRGeometryCollection->getGeometryRef(0);
//        pNewOGRGeometryCollection->removeGeometry(0, 0);
//        wxDELETE(pNewOGRGeometryCollection);
//        return pRetGeom;
//    }
//
//    return pNewOGRGeometryCollection;
//}
//
//void wxGISAlgorithm::SetPointOnEnvelope(OGRRawPoint* a, OGRRawPoint* b, OGRRawPoint* c, OGREnvelope* r, int code)
//{
//    /* если c левее r, то передвигаем c на прямую x = r->x_min
//    если c правее r, то передвигаем c на прямую x = r->x_max */
//    if (code & LEFT) {
//        c->y += (a->y - b->y) * (r->MinX - c->x) / (a->x - b->x);
//        c->x = r->MinX;
//    } else if (code & RIGHT) {
//        c->y += (a->y - b->y) * (r->MaxX - c->x) / (a->x - b->x);
//        c->x = r->MaxX;
//    }
//    /* если c ниже r, то передвигаем c на прямую y = r->y_min
//    если c выше r, то передвигаем c на прямую y = r->y_max */
//    if (code & BOT) {
//        c->x += (a->x - b->x) * (r->MinY - c->y) / (a->y - b->y);
//        c->y = r->MinY;
//    } else if (code & TOP) {
//        c->x += (a->x - b->x) * (r->MaxY - c->y) / (a->y - b->y);
//        c->y = r->MaxY;
//    }
//}
//
//OGRRawPoint* wxGISAlgorithm::Crossing(OGRRawPoint p11, OGRRawPoint p12, OGRRawPoint p21, OGRRawPoint p22 )
//{
//    // знаменатель
//    double Z  = (p12.y-p11.y)*(p21.x-p22.x)-(p21.y-p22.y)*(p12.x-p11.x);
//    // числитель 1
//    double Ca = (p12.y-p11.y)*(p21.x-p11.x)-(p21.y-p11.y)*(p12.x-p11.x);
//    // числитель 2
//    double Cb = (p21.y-p11.y)*(p21.x-p22.x)-(p21.y-p22.y)*(p21.x-p11.x);
//
//    // если знаменатель = 0, прямые параллельны
//    if( Z == 0 )
//        return NULL;
//    // если числители и знаменатель = 0, прямые совпадают
//    if( (Z == 0) && (Ca == 0) && (Cb == 0) )
//        return NULL;
//
//    double Ua = Ca/Z;
//    double Ub = Cb/Z;
//
//    if( (0 <= Ua) && (Ua <= 1) && (0 <= Ub) && (Ub <= 1) )
//    {
//        OGRRawPoint* pt = new OGRRawPoint;
//        pt->x = p11.x + (p12.x - p11.x) * Ub;
//        pt->y = p11.y + (p12.y - p11.y) * Ub;
//        return pt;
//        
//    }    
//    else // иначе точка пересечения за пределами отрезков
//        return NULL;
//}
//
//   //GEOSGeom hIniGeosGeom = pGeom->exportToGEOS();
//    //hIniGeosGeom
//    //GEOSGeom hThisGeosGeom = NULL;
//    //GEOSGeom hOtherGeosGeom = NULL;
//    //GEOSGeom hGeosProduct = NULL;
//    //OGRGeometry *poOGRProduct = NULL;
//
//    //hThisGeosGeom = exportToGEOS();
//    //hOtherGeosGeom = poOtherGeom->exportToGEOS();
//    //if( hThisGeosGeom != NULL && hOtherGeosGeom != NULL )
//    //{
//    //    hGeosProduct = GEOSIntersection( hThisGeosGeom, hOtherGeosGeom );
//    //    GEOSGeom_destroy( hThisGeosGeom );
//    //    GEOSGeom_destroy( hOtherGeosGeom );
//
//    //    if( hGeosProduct != NULL )
//    //    {
//    //        poOGRProduct = OGRGeometryFactory::createFromGEOS(hGeosProduct);
//    //        GEOSGeom_destroy( hGeosProduct );
//    //    }
//    //}
//
//    //return poOGRProduct;

//

//
//#define poscode(r, p) \
//	((((p).x < (r).MinX) ? LEFT : 0)  +  /* +1 если точка левее прямоугольника */ \
//	 (((p).x > (r).MaxX) ? RIGHT : 0) +  /* +2 если точка правее прямоугольника */\
//	 (((p).y < (r).MinY) ? BOT : 0)   +  /* +4 если точка ниже прямоугольника */  \
//	 (((p).y > (r).MaxY) ? TOP : 0))     /* +8 если точка выше прямоугольника */
//

//OGRRawPoint GetPointOnEnvelope(OGREnvelope &Env, OGRRawPoint &Pt1, OGRRawPoint &Pt2)
//{
//	int code_a = poscode(Env, Pt1);
//	int code_b = poscode(Env, Pt2);
//
//    //curve inside envelope
//    if(code_a == code_b || !code_a && !code_b)
//       return OGRRawPoint();
//    //curve enter envelope
//    if (code_a > 0 && code_b > 0 || !code_b && code_a > 0)
//		return GetPointOnEnvelope(Env, Pt1, Pt2, code_a);
//    //curve exit envelope
//    if(!code_a && code_b > 0)
//        return GetPointOnEnvelope(Env, Pt1, Pt2, code_b);
//	return OGRRawPoint();
//}
//
//
//OGRRawPoint GetPointOnEnvelope(OGREnvelope &Env, OGRRawPoint &Pt1, OGRRawPoint &Pt2, int code)
//{
//	OGRRawPoint c;
//    if (code & enumGISPtPosLeft) 
//	{
//        c.y += (Pt1.y - Pt2.y) * (Env.MinX - c.x) / (Pt1.x - Pt2.x);
//        c.x = Env.MinX;
//    } 
//	else if (code & enumGISPtPosRight) 
//	{
//        c.y += (Pt1.y - Pt2.y) * (Env.MaxX - c.x) / (Pt1.x - Pt2.x);
//        c.x = Env.MaxX;
//    }
//    if (code & enumGISPtPosBottom) 
//	{
//        c.x += (Pt1.x - Pt2.x) * (Env.MinY - c.y) / (Pt1.y - Pt2.y);
//        c.y = Env.MinY;
//    } 
//	else if (code & enumGISPtPosTop) 
//	{
//        c.x += (Pt1.x - Pt2.x) * (Env.MaxY - c.y) / (Pt1.y - Pt2.y);
//        c.y = Env.MaxY;
//    }
//	return c;
//}