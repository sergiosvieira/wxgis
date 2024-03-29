/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISDisplay class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Bishop
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

#include "wxgis/display/display.h"

#include "ogrsf_frmts.h"

#include <cairo.h>

#ifdef __WXMSW__
	#include <cairo-win32.h>
#endif

#ifdef __WXGTK__
    #include <gdk/gdk.h>
	#include <gtk/gtk.h>
#endif

inline WXDLLIMPEXP_GIS_DSP WXGISRGBA ToRGBA(unsigned char nR, unsigned char nG, unsigned char nB, unsigned char nA)
{
    WXGISRGBA ret = {double(nR) / 255, double(nG) / 255, double(nB) / 255, double(nA) / 255};
    return ret;
};

inline WXDLLIMPEXP_GIS_DSP WXGISRGBA ToRGBA(const wxColour &Color)
{
    WXGISRGBA ret = {double(Color.Red()) / 255, double(Color.Green()) / 255, double(Color.Blue()) / 255, double(Color.Alpha()) / 255};
    return ret;
};

/** \class wxGISDisplay gisdisplay.h
    \brief The class to draw map contents.

    This class draw to virtual or real display.
	It use some caches (memory rgba rasters) and output DC to draw.
*/
WX_DEFINE_ARRAY(wxRealPoint*, wxGISPointsArray);

class WXDLLIMPEXP_GIS_DSP wxGISDisplay
{
public:
	wxGISDisplay(void);
	virtual ~wxGISDisplay(void);
	//
	virtual size_t AddCache(void);
	virtual void Clear();
	virtual size_t GetLastCacheID(void) const;
	virtual size_t GetFlashCacheID(void) const;
	virtual bool IsCacheDerty(size_t nCacheID) const;
	virtual void SetCacheDerty(size_t nCacheID, bool bIsDerty);
	virtual void SetDrawCache(size_t nCacheID, bool bNoDerty = false);
	virtual size_t GetDrawCache(void) const;
	virtual void SetDerty(bool bIsDerty);
	virtual void SetDertyUpperCache(size_t nFromCacheNo, bool bIsDerty = true);
	virtual bool IsDerty(void) const;
    virtual size_t GetCacheCount(void) const {return m_saLayerCaches.size();};
	//frame
	virtual void SetDeviceFrame(wxRect &rc);
	virtual wxRect GetDeviceFrame(void) const;
	//current draw bounds
	virtual void SetBounds(const OGREnvelope &Env);
	virtual OGREnvelope GetBounds(bool bRotated = true) const;
    virtual wxRealPoint GetBoundsCenter(void) const {return wxRealPoint(m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);};
	//misc
	virtual void SetRotate(double dAngleRad);
	virtual double GetRotate(void) const {return m_dAngleRad;};
	virtual void DC2World(double* pdX, double* pdY);
	virtual void World2DC(double* pdX, double* pdY);
	virtual void DC2WorldDist(double* pdX, double* pdY, bool bRotated = true);
	virtual void World2DCDist(double* pdX, double* pdY, bool bRotated = true);
	//
	virtual void OnEraseBackground(void); //Fill #0 cache of background color
	virtual void Output(wxDC* pDC, wxGISPointsArray ClipGeometry);
	//Styles
    virtual void SetColor(wxGISEnumDrawStyle eStyle, const WXGISRGBA &Color);
	virtual void SetLineCap(cairo_line_cap_t line_cap = CAIRO_LINE_CAP_BUTT);
	virtual void SetLineWidth(double dWidth);
	virtual void SetPointRadius(double dRadius);
	virtual void SetFillRule(cairo_fill_rule_t fill_rule = CAIRO_FILL_RULE_WINDING);
	//Draw
	virtual void DrawGeometry(OGRGeometry* poGeometry);
	virtual void DrawRaster(cairo_surface_t *surface, OGREnvelope& Envelope, bool bDrawEnvelope = false);
	virtual void ZoomingDraw(const wxRect& rc, wxDC* pDC);
	virtual void WheelingDraw(double dZoom, wxDC* pDC);
	virtual void PanningDraw(wxCoord x, wxCoord y, wxDC* pDC);
	virtual void RotatingDraw(double dAngle, wxDC* pDC);
	virtual OGREnvelope TransformRect(wxRect &rect);
	//Testing
	virtual void TestDraw(void);

    typedef struct _layercachedata
    {
	    bool bIsDerty;
	    cairo_surface_t *pCairoSurface;
	    cairo_t *pCairoContext;
    } LAYERCACHEDATA;
protected:
	virtual void SetColor(double dRed, double dGreen, double dBlue, double dAlpha = 0);
	virtual void SetColor(WXGISRGBA Color);
	virtual bool DrawPoint(double dX, double dY);
	virtual bool DrawPointFast(double dX, double dY);
	virtual bool DrawPoint(const OGRPoint* pPoint);
	virtual bool DrawPointFast(const OGRPoint* pPoint);
	virtual bool DrawLine(const OGRLineString* pLine, bool bIsRing = false);
	virtual bool DrawRing(const OGRLinearRing* pRing);
	virtual bool DrawPolygon(const OGRPolygon* pPolygon);
	virtual bool CheckDrawAsPoint(const OGRGeometry* pGeometry, bool bCheckEnvelope = false);
	virtual cairo_t* CreateContext(wxDC* dc);
	virtual void InitTransformMatrix(void);
	virtual inline double GetScaledWidth(double nWidth)
	{
		double x_new, y_new;
		x_new = y_new = nWidth;
		DC2WorldDist(&x_new, &y_new);
		return (fabs(x_new) + fabs(y_new)) / 2;
	}
protected:
	wxVector<LAYERCACHEDATA> m_saLayerCaches;
	WXGISRGBA m_stBackGroudnColour;
	WXGISRGBA m_stFillColour, m_stLineColour, m_stPointColour;
	size_t m_nLastCacheID, m_nCurrentLayer;
	int m_nMax_X, m_nMax_Y;
	wxRect m_oDeviceFrameRect;
	OGREnvelope m_RealBounds, m_CurrentBounds, m_CurrentBoundsRotated, m_CurrentBoundsX8;
	double m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY;
	//wxSize m_ppi;
	double m_dAngleRad;
	cairo_matrix_t *m_pMatrix, *m_pDisplayMatrix, *m_pDisplayMatrixNoRotate;
	double m_dOrigin_X, m_dOrigin_Y;
	double m_dFrameCenterX, m_dFrameCenterY;
	double m_dCacheCenterX, m_dCacheCenterY;
	double m_dFrameXShift, m_dFrameYShift;
	wxCriticalSection m_CritSect;
	bool m_bZeroCacheSet;
	double m_dPointRadius;
	double m_dLineWidth;
	double m_dFrameRatio;
    double m_dScale;

	//temp cairo for output double buffering
	cairo_surface_t *m_surface_tmp;
	cairo_t *m_cr_tmp;
	//OGRGeometrySPtr m_pCutGeom;
};
