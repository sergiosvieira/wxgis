/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Algorithm src.
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

#include "wxgis/geometry/algorithm.h"

#define LEFT  1  /* двоичное 0001 */
#define RIGHT 2  /* двоичное 0010 */
#define BOT   4  /* двоичное 0100 */
#define TOP   8  /* двоичное 1000 */

/* вычисление кода точки 
   r : указатель на OGREnvelope; p : указатель на OGRRawPoint */
#define vcode(r, p) \
	((((p)->x < (r)->MinX) ? LEFT : 0)  +  /* +1 если точка левее прямоугольника */ \
	 (((p)->x > (r)->MaxX) ? RIGHT : 0) +  /* +2 если точка правее прямоугольника */\
	 (((p)->y < (r)->MinY) ? BOT : 0)   +  /* +4 если точка ниже прямоугольника */  \
	 (((p)->y > (r)->MaxY) ? TOP : 0))     /* +8 если точка выше прямоугольника */


wxGISAlgorithm::wxGISAlgorithm(void)
{
}

wxGISAlgorithm::~wxGISAlgorithm(void)
{
}

//Алгоритм Коэна — Сазерленда (англ. Cohen-Sutherland) 
OGRGeometry* wxGISAlgorithm::FastLineIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2)
{
    OGREnvelope Env;
    pGeom2->getEnvelope(&Env);

    OGRLineString* pOGRLineString = (OGRLineString*)pGeom1;
    int nPointCount = pOGRLineString->getNumPoints();

    OGRRawPoint* pPoints = new OGRRawPoint[nPointCount];
    OGRRawPoint* pNewPoints = new OGRRawPoint[nPointCount];
    double* pZValues(NULL);
    double* pNewZValues(NULL); 
    if(pOGRLineString->getCoordinateDimension() > 2)
    {
        pZValues = new double[nPointCount];
        pNewZValues = new double[nPointCount];
    }

    pOGRLineString->getPoints(pPoints, pZValues);

    int pos = 0;    
    OGRGeometryCollection* pNewOGRGeometryCollection = new OGRGeometryCollection();
    for(size_t i = 0; i < nPointCount - 1; i++)
    {
	    int code_a = vcode(&Env, &pPoints[i]);
	    int code_b = vcode(&Env, &pPoints[i + 1]);
        //curve dont enter envelope
        //wxLogDebug(wxT("x:%f y:%f code_a:%d code_b:%d"), pPoints[i].x, pPoints[i].y, code_a, code_b);

        if (code_a > 0 && code_b > 0)
            continue;
        //curve inside envelope
        if (!code_a && !code_b)
        {
            pNewPoints[pos] = pPoints[i];
            if(pZValues)
                pNewZValues[pos] = pZValues[i];
            pos++;
            continue;
        }
        //curve enter envelope
        if(!code_b && code_a > 0)
        {
            OGRRawPoint pt = pPoints[i];
            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, &Env, code_a);

            pNewPoints[pos] = pt;
            if(pZValues)
                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
            pos++;
            continue;
        }
        //curve exit envelope
        if(!code_a && code_b > 0)
        {
            OGRRawPoint pt = pPoints[i + 1];
            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, &Env, code_b);

            pNewPoints[pos] = pt;
            if(pZValues)
                pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
            pos++;
            if(pos > 1)
            {
                //create & add new geom
                OGRLineString* pOGRNewLineString = new OGRLineString();
                pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
                pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
                pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
                pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
            }
            pos = 0;
            continue;
        }
    }

    if(pos > 1)
    {
        OGRLineString* pOGRNewLineString = new OGRLineString();
        pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
        pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
        pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
        pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
    }

    wxDELETEA(pPoints);
    wxDELETEA(pZValues);
    wxDELETEA(pNewPoints);
    wxDELETEA(pNewZValues);

    if(pNewOGRGeometryCollection->getNumGeometries() == 0)
    {
        wxDELETE(pNewOGRGeometryCollection);
        return NULL;
    }
    
    if(pNewOGRGeometryCollection->getNumGeometries() == 1)
    {
        OGRGeometry* pRetGeom = pNewOGRGeometryCollection->getGeometryRef(0);
        pNewOGRGeometryCollection->removeGeometry(0, 0);
        wxDELETE(pNewOGRGeometryCollection);
        return pRetGeom;
    }

    return pNewOGRGeometryCollection;
}

//  Алгоритм Вейлера-Азертона (Weiler-Atherton)
OGRGeometry* wxGISAlgorithm::FastPolyIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2)
{
    OGREnvelope Env;
    pGeom2->getEnvelope(&Env);

    OGRPolygon* pPoly = (OGRPolygon*)pGeom1;
    OGRLineString* pOGRLineString = pPoly->getExteriorRing();
    OGRLinearRing* pExtRing = PolyIntersection(&Env, pOGRLineString);
    if(!pExtRing)
        return NULL;
    OGRPolygon* pNewPoly = new OGRPolygon();
    pNewPoly->setCoordinateDimension(pGeom1->getCoordinateDimension());
    pNewPoly->assignSpatialReference(pGeom1->getSpatialReference());
    pNewPoly->addRingDirectly(pExtRing);

    int nCount = pPoly->getNumInteriorRings();
    for(size_t i = 0; i < nCount; i++)
    {
        pOGRLineString = pPoly->getInteriorRing(i);
        OGRLinearRing* pIntRing = PolyIntersection(&Env, pOGRLineString);
        if(!pIntRing)
            continue;
        pNewPoly->addRingDirectly(pIntRing);
    } 
    return pNewPoly;
}

OGRLinearRing* wxGISAlgorithm::PolyIntersection(OGREnvelope* pEnv, OGRLineString* pOGRLineString)
{
    int nPointCount = pOGRLineString->getNumPoints();

    OGRRawPoint* pPoints = new OGRRawPoint[nPointCount];
    OGRRawPoint* pNewPoints = new OGRRawPoint[nPointCount];
    double* pZValues(NULL);
    double* pNewZValues(NULL); 
    if(pOGRLineString->getCoordinateDimension() > 2)
    {
        pZValues = new double[nPointCount];
        pNewZValues = new double[nPointCount];
    }

    wxClipWindow win(pEnv);
    std::vector<wxClipWindow::CLIPVERT> PolyData;

    pOGRLineString->getPoints(pPoints, pZValues);

    for(size_t i = 0; i < nPointCount - 1; i++)
    {
	    int code_a = vcode(pEnv, &pPoints[i]);
	    int code_b = vcode(pEnv, &pPoints[i + 1]);
        //curve dont enter envelope
        //wxLogDebug(wxT("x:%f y:%f code_a:%d code_b:%d"), pPoints[i].x, pPoints[i].y, code_a, code_b);

        if (code_a > 0 && code_b > 0)
            continue;
        //curve inside envelope
        if (!code_a && !code_b)
        {
            //pNewPoints[pos] = pPoints[i];
            //if(pZValues)
            //    pNewZValues[pos] = pZValues[i];
            //pos++;
            wxClipWindow::CLIPVERT data = {pPoints[i], -1, wxClipWindow::wxVERTEX};
            PolyData.push_back(data);
            continue;
        }
        //curve enter envelope
        if(!code_b && code_a > 0)
        {
            OGRRawPoint pt = pPoints[i];
            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, pEnv, code_a);

            int nInd = win.AddPoint(&pt, PolyData.size(), wxClipWindow::wxENTER);

            wxClipWindow::CLIPVERT data = {pt, nInd, wxClipWindow::wxENTER};
            PolyData.push_back(data);

            //pNewPoints[pos] = pt;
            //if(pZValues)
            //    pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
            //pos++;
            continue;
        }
        //curve exit envelope
        if(!code_a && code_b > 0)
        {
            OGRRawPoint pt = pPoints[i + 1];
            SetPointOnEnvelope(&pPoints[i], &pPoints[i + 1], &pt, pEnv, code_b);

            int nInd = win.AddPoint(&pt, PolyData.size(), wxClipWindow::wxEXIT);

            wxClipWindow::CLIPVERT data = {pt, nInd, wxClipWindow::wxEXIT};
            PolyData.push_back(data);


            //pNewPoints[pos] = pt;
            //if(pZValues)
            //    pNewZValues[pos] = pZValues[i] + (pZValues[i - 1] - pZValues[i]) / 2;
            //pos++;
            //if(pos > 1)
            //{
            //    //create & add new geom
            //    //OGRLineString* pOGRNewLineString = new OGRLineString();
            //    //pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
            //    //pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
            //    //pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
            //    //pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
            //}
            //pos = 0;
            continue;
        }
    }

    //if(pos > 1)
    //{
    //    //OGRLineString* pOGRNewLineString = new OGRLineString();
    //    //pOGRNewLineString->setCoordinateDimension(pOGRLineString->getCoordinateDimension());
    //    //pOGRNewLineString->assignSpatialReference(pOGRLineString->getSpatialReference());
    //    //pOGRNewLineString->setPoints(pos, pNewPoints, pNewZValues);
    //    //pNewOGRGeometryCollection->addGeometryDirectly(pOGRNewLineString);
    //}

    int pos = 0;    
    for(size_t i = 0; i < PolyData.size(); i++)
    {       
        if(PolyData[i].pt == pNewPoints[0])
        {
            //create new poly
        }

        //remove point

        switch(PolyData[i].Type)
        {
        case wxClipWindow::wxVERTEX:
            pNewPoints[pos] = PolyData[i].pt;
            pos++;
            PolyData[i].Type = wxNONE;
            continue;
        case wxClipWindow::wxENTER:
            //pNewPoints[pos] = PolyData[i].pt;
            continue;
        case wxClipWindow::wxEXIT:
            {
                for(size_t j = PolyData[i].nIndex + 1; j < win.GetSize(); j++)
                {
                    wxClipWindow::CLIPVERT data = win.GetItem(j);
                    pNewPoints[pos] = data.pt;
                    pos++;
                    if(data.Type == wxClipWindow::wxENTER)
                    {
                        i = data.nIndex;
                        break;
                    }
                }
            }
            continue;
        }
    }

    wxDELETEA(pPoints);
    wxDELETEA(pZValues);
    wxDELETEA(pNewPoints);
    wxDELETEA(pNewZValues);

    return NULL;
}

void wxGISAlgorithm::SetPointOnEnvelope(OGRRawPoint* a, OGRRawPoint* b, OGRRawPoint* c, OGREnvelope* r, int code)
{
    /* если c левее r, то передвигаем c на прямую x = r->x_min
    если c правее r, то передвигаем c на прямую x = r->x_max */
    if (code & LEFT) {
        c->y += (a->y - b->y) * (r->MinX - c->x) / (a->x - b->x);
        c->x = r->MinX;
    } else if (code & RIGHT) {
        c->y += (a->y - b->y) * (r->MaxX - c->x) / (a->x - b->x);
        c->x = r->MaxX;
    }
    /* если c ниже r, то передвигаем c на прямую y = r->y_min
    если c выше r, то передвигаем c на прямую y = r->y_max */
    if (code & BOT) {
        c->x += (a->x - b->x) * (r->MinY - c->y) / (a->y - b->y);
        c->y = r->MinY;
    } else if (code & TOP) {
        c->x += (a->x - b->x) * (r->MaxY - c->y) / (a->y - b->y);
        c->y = r->MaxY;
    }
}

OGRRawPoint* wxGISAlgorithm::Crossing(OGRRawPoint p11, OGRRawPoint p12, OGRRawPoint p21, OGRRawPoint p22 )
{
    // знаменатель
    double Z  = (p12.y-p11.y)*(p21.x-p22.x)-(p21.y-p22.y)*(p12.x-p11.x);
    // числитель 1
    double Ca = (p12.y-p11.y)*(p21.x-p11.x)-(p21.y-p11.y)*(p12.x-p11.x);
    // числитель 2
    double Cb = (p21.y-p11.y)*(p21.x-p22.x)-(p21.y-p22.y)*(p21.x-p11.x);

    // если знаменатель = 0, прямые параллельны
    if( Z == 0 )
        return NULL;
    // если числители и знаменатель = 0, прямые совпадают
    if( (Z == 0) && (Ca == 0) && (Cb == 0) )
        return NULL;

    double Ua = Ca/Z;
    double Ub = Cb/Z;

    if( (0 <= Ua) && (Ua <= 1) && (0 <= Ub) && (Ub <= 1) )
    {
        OGRRawPoint* pt = new OGRRawPoint;
        pt->x = p11.x + (p12.x - p11.x) * Ub;
        pt->y = p11.y + (p12.y - p11.y) * Ub;
        return pt;
        
    }    
    else // иначе точка пересечения за пределами отрезков
        return NULL;
}

   //GEOSGeom hIniGeosGeom = pGeom->exportToGEOS();
    //hIniGeosGeom
    //GEOSGeom hThisGeosGeom = NULL;
    //GEOSGeom hOtherGeosGeom = NULL;
    //GEOSGeom hGeosProduct = NULL;
    //OGRGeometry *poOGRProduct = NULL;

    //hThisGeosGeom = exportToGEOS();
    //hOtherGeosGeom = poOtherGeom->exportToGEOS();
    //if( hThisGeosGeom != NULL && hOtherGeosGeom != NULL )
    //{
    //    hGeosProduct = GEOSIntersection( hThisGeosGeom, hOtherGeosGeom );
    //    GEOSGeom_destroy( hThisGeosGeom );
    //    GEOSGeom_destroy( hOtherGeosGeom );

    //    if( hGeosProduct != NULL )
    //    {
    //        poOGRProduct = OGRGeometryFactory::createFromGEOS(hGeosProduct);
    //        GEOSGeom_destroy( hGeosProduct );
    //    }
    //}

    //return poOGRProduct;

//--------------------------------------
// ClipWindow
//--------------------------------------

wxClipWindow::wxClipWindow(OGREnvelope* pEnv)
{
    OGRRawPoint pt;
    pt.x = pEnv->MinX;
    pt.y = pEnv->MinY;
    CLIPVERT data1 = {pt, -1, wxVERTEX};
    m_Env.push_back(data1);
    pt.x = pEnv->MaxX;
    pt.y = pEnv->MinY;
    CLIPVERT data2 = {pt, -1, wxVERTEX};
    m_Env.push_back(data2);
    pt.x = pEnv->MaxX;
    pt.y = pEnv->MaxY;
    CLIPVERT data3 = {pt, -1, wxVERTEX};
    m_Env.push_back(data3);
    pt.x = pEnv->MinX;
    pt.y = pEnv->MaxY;
    CLIPVERT data4 = {pt, -1, wxVERTEX};
    m_Env.push_back(data4);
    pt.x = pEnv->MinX;
    pt.y = pEnv->MinY;
    CLIPVERT data5 = {pt, -1, wxVERTEX};
    m_Env.push_back(data5);
}

wxClipWindow::~wxClipWindow(void)
{
}

int wxClipWindow::AddPoint(OGRRawPoint* a, int nIndex, VERTEXTYPE Type)
{
    OGRRawPoint pt;
    pt.x = a->x;
    pt.y = a->y;
    CLIPVERT data = {pt, nIndex, Type};

    for(size_t i = 0; i < m_Env.size() - 1; i++)
    {
        //1 MinX - x - MaxX MinY
        if(m_Env[i].pt.y == a->y)
        {
            if(m_Env[i].pt.x < a->x && m_Env[i + 1].pt.x > a->x)
            {
                m_Env.insert(m_Env.begin() + i, data);
                return i;
            }
            if(m_Env[i].pt.x > a->x && m_Env[i + 1].pt.x < a->x)
            {
                m_Env.insert(m_Env.begin() + i, data);
                return i;
            }
        }
        if(m_Env[i].pt.x == a->x)
        {
            if(m_Env[i].pt.y < a->y && m_Env[i + 1].pt.y > a->y)
            {
                m_Env.insert(m_Env.begin() + i, data);
                return i;
            }
            if(m_Env[i].pt.y > a->y && m_Env[i + 1].pt.y < a->y)
            {
                m_Env.insert(m_Env.begin() + i, data);
                return i;
            }
        }
    }
    wxASSERT(1);
    return -1;
}

