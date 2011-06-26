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
void WXDLLIMPEXP_GIS_GEOM IncreaseEnvelope(OGREnvelope *pEnv, double dSize);
//Sutherland-Hodgman Polygon Clipping
inline bool IsInsideEnvelope(const OGRRawPoint &pt, wxGISEnumPointPosition nPos, const OGREnvelope &Env)
{
	switch(nPos)
	{
	case enumGISPtPosLeft://XMin
		return (pt.x > Env.MinX);
	case enumGISPtPosRight://XMax
		return (pt.x < Env.MaxX);
	case enumGISPtPosBottom://YMin
		return (pt.y > Env.MinY);
	case enumGISPtPosTop://YMax
		return (pt.y < Env.MaxY);
	}
	return false;
}

inline OGRRawPoint SolveIntersection(const OGRRawPoint &pt1, const OGRRawPoint &pt2, wxGISEnumPointPosition nPos, const OGREnvelope &Env)
{
	OGRRawPoint out;
	double r_n = DBL_EPSILON, r_d = DBL_EPSILON;

	switch ( nPos )
	{
	case enumGISPtPosRight: // x = MAX_X boundary
		r_n = -( pt1.x - Env.MaxX ) * ( Env.MaxY - Env.MinY );
		r_d = ( pt2.x - pt1.x ) * ( Env.MaxY - Env.MinY );
		break;
	case enumGISPtPosLeft: // x = MIN_X boundary
		r_n = -( pt1.x - Env.MinX ) * ( Env.MaxY - Env.MinY );
		r_d = ( pt2.x - pt1.x ) * ( Env.MaxY - Env.MinY );
		break;
	case enumGISPtPosTop: // y = MAX_Y boundary
		r_n = ( pt1.y - Env.MaxY ) * ( Env.MaxX - Env.MinX );
		r_d = -( pt2.y - pt1.y ) * ( Env.MaxX - Env.MinX );
		break;
	case enumGISPtPosBottom: // y = MIN_Y boundary
		r_n = ( pt1.y - Env.MinY ) * ( Env.MaxX - Env.MinX );
		r_d = -( pt2.y - pt1.y ) * ( Env.MaxX - Env.MinX );
		break;
	}

	if ( fabs( r_d ) > DBL_EPSILON && fabs( r_n ) > DBL_EPSILON )
	{ // they cross
		double r = r_n / r_d;
		out.x = pt1.x + r * ( pt2.x - pt1.x );
		out.y = pt1.y + r * ( pt2.y - pt1.y );
	}
	else
	{
		// Should never get here, but if we do for some reason, cause a
		// clunk because something else is wrong if we do.
		wxASSERT( fabs( r_d ) > DBL_EPSILON && fabs( r_n ) > DBL_EPSILON );
	}

	return out;
}

inline void TrimFeatureToBoundary(OGRRawPoint* pOGRRawPointsIn, int nPointCountIn, OGRRawPoint** pOGRRawPointsOut, int *pnPointCountOut, wxGISEnumPointPosition nPos, const OGREnvelope &Env, bool shapeOpen )
{
	// The shapeOpen parameter selects whether this function treats the
	// shape as open or closed. False is appropriate for polygons and
	// true for polylines.

	unsigned int i1 = nPointCountIn - 1; // start with last point

	// and compare to the first point initially.
	for ( unsigned int i2 = 0; i2 < nPointCountIn; ++i2 )
	{ // look at each edge of the polygon in turn
		if ( IsInsideEnvelope( pOGRRawPointsIn[i2], nPos, Env ) ) // end point of edge is inside boundary
		{
			if ( IsInsideEnvelope( pOGRRawPointsIn[i1], nPos, Env) )
				(*pOGRRawPointsOut)[(*pnPointCountOut)++] = pOGRRawPointsIn[i2];
			else
			{
				// edge crosses into the boundary, so trim back to the boundary, and
				// store both ends of the new edge
				if ( !( i2 == 0 && shapeOpen ) )
					(*pOGRRawPointsOut)[(*pnPointCountOut)++] = SolveIntersection( pOGRRawPointsIn[i1], pOGRRawPointsIn[i2], nPos, Env );
				(*pOGRRawPointsOut)[(*pnPointCountOut)++] = pOGRRawPointsIn[i2];
			}
		}
		else // end point of edge is outside boundary
		{
			// start point is in boundary, so need to trim back
			if ( IsInsideEnvelope( pOGRRawPointsIn[i1], nPos, Env) )
			{
				if ( !( i2 == 0 && shapeOpen ) )
					(*pOGRRawPointsOut)[(*pnPointCountOut)++] = SolveIntersection( pOGRRawPointsIn[i1], pOGRRawPointsIn[i2], nPos, Env );
			}
		}
		i1 = i2;
	}
}

void WXDLLIMPEXP_GIS_GEOM ClipGeometryByEnvelope(OGRRawPoint* pOGRRawPoints, int *pnPointCount, const OGREnvelope &Env);

//
//OGRRawPoint WXDLLIMPEXP_GIS_GEOM GetPointOnEnvelope(OGREnvelope &Env, OGRRawPoint &Pt1, OGRRawPoint &Pt2, int code);
//inline int WXDLLIMPEXP_GIS_GEOM GetPositionCode(OGREnvelope &Env,  OGRRawPoint &Pt)
//{
//	return ( (Pt.x < Env.MinX) ? enumGISPtPosLeft : 0) | ( (Pt.x > Env.MaxX) ? enumGISPtPosRight : 0) | ( (Pt.y < Env.MinY) ? enumGISPtPosBottom : 0) | ( (Pt.y > Env.MaxY) ? enumGISPtPosTop : 0);
//}
