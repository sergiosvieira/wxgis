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
#pragma once

#include "wxgis/geometry/geometry.h"

//class WXDLLIMPEXP_GIS_GEOM wxGISAlgorithm
//{
//public:
//    wxGISAlgorithm(void);
//    virtual ~wxGISAlgorithm(void);
//    virtual OGRGeometry* FastLineIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2);
//    virtual OGRGeometry* FastPolyIntersection(OGRGeometry* pGeom1, OGRGeometry* pGeom2);
//protected:
//    virtual OGRRawPoint* Crossing(OGRRawPoint p11, OGRRawPoint p12, OGRRawPoint p21, OGRRawPoint p22);
//    virtual void SetPointOnEnvelope(OGRRawPoint* a, OGRRawPoint* b, OGRRawPoint* c, OGREnvelope* r, int code);
//    virtual OGRGeometry* PolyIntersection(OGREnvelope* pEnv, OGRLineString* pOGRLineString);
//};
//
//typedef enum _vertex_type{wxVERTEX, wxENTER, wxEXIT} VERTEXTYPE;
//
////--------------------------------------
//// wxWAListItem
////--------------------------------------
//
//class wxWAListItem
//{
//public:
//    wxWAListItem(void) : m_pNext(NULL), m_pData(NULL){};
//    virtual wxWAListItem* GetNext(void){return (wxWAListItem*)m_pNext;};
//public:
//    OGRRawPoint m_Point;
//    VERTEXTYPE m_Type;
//    long m_pNext;
//    long m_pData;
//};
//
////--------------------------------------
//// wxWAList
////--------------------------------------
//
//class wxWAList
//{
//public:
//    wxWAList(void);
//    virtual ~wxWAList(void);
//    virtual wxWAListItem* AddPoint(OGRRawPoint Point, VERTEXTYPE Type);
//    virtual wxWAListItem* InsertPoint(size_t nIndex, OGRRawPoint Point, VERTEXTYPE Type);
//    virtual wxWAListItem* RemovePoint(wxWAListItem* pItem);
//    virtual size_t GetSize(void){return m_ItemsArr.size();};
//    virtual wxWAListItem* GetHead(void);
//    virtual wxWAListItem* operator[](size_t nIndex);
//protected:
//    std::vector<wxWAListItem*> m_ItemsArr;
//};
//
////--------------------------------------
//// ClipWindow
////--------------------------------------
//class wxClipWindow : public wxWAList
//{
//public:
//    wxClipWindow(OGREnvelope* pEnv);
//    virtual ~wxClipWindow(void);
//    virtual wxWAListItem* AddPoint(OGRRawPoint Point, VERTEXTYPE Type);
//};
//OGRRawPoint WXDLLIMPEXP_GIS_GEOM GetPointOnEnvelope(OGREnvelope &Env, OGRRawPoint &Pt1, OGRRawPoint &Pt2);

/** \fn OGRGeometrySPtr WXDLLIMPEXP_GIS_GEOM EnvelopeToGeometry(OGREnvelopeSPtr pEnv, OGSSpatialReferensSPtr pSpaRef)
 *  \brief Create OGRGeometry from OGREnvelope.
 *  \param pEnv Input envelope
 *  \param pSpaRef Spatial Refernce of output geometry
 *  \return Geometry
 */	

OGRGeometrySPtr WXDLLIMPEXP_GIS_GEOM EnvelopeToGeometry(const OGREnvelope &Env, OGRSpatialReferenceSPtr pSpaRef = OGRSpatialReferenceSPtr());
OGRRawPoint WXDLLIMPEXP_GIS_GEOM GetPointOnEnvelope(OGREnvelope &Env, OGRRawPoint &Pt1, OGRRawPoint &Pt2, int code);
inline int WXDLLIMPEXP_GIS_GEOM GetPositionCode(OGREnvelope &Env,  OGRRawPoint &Pt)
{
	return ( (Pt.x < Env.MinX) ? enumGISPtPosLeft : 0) | ( (Pt.x > Env.MaxX) ? enumGISPtPosRight : 0) | ( (Pt.y < Env.MinY) ? enumGISPtPosBottom : 0) | ( (Pt.y > Env.MaxY) ? enumGISPtPosTop : 0);
}
