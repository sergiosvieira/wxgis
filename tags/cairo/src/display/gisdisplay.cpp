/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISDisplay class.
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
#include "wxgis/display/gisdisplay.h"
#include "wxgis/display/displaytransformation.h"
#include "wxgis/geometry/algorithm.h"

wxGISDisplayEx::wxGISDisplayEx(void)
{
	//default background color
	m_stBackGroudnColour.dRed = 0.9;
	m_stBackGroudnColour.dGreen = 1.0;
	m_stBackGroudnColour.dBlue = 1.0;
	m_stBackGroudnColour.dAlpha = 1.0;
	//m_stBackGroudnColour = {0.9,1.0,1.0,1.0}; //230, 255, 255, 255

	//default frame size
	m_oDeviceFrameRect = wxRect(0,0,800,600);
	m_dFrameCenterX = 400;
	m_dFrameCenterY = 300;

	//create first cached layer
	m_nMax_X = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	m_nMax_Y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	LAYERCACHEDATA layercachedata;
	layercachedata.bIsDerty = true;
	layercachedata.pCairoSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, m_nMax_X, m_nMax_Y);
	layercachedata.pCairoContext = cairo_create (layercachedata.pCairoSurface);
	m_saLayerCaches.push_back(layercachedata);

	m_dCacheCenterX = double(m_nMax_X) / 2;
	m_dCacheCenterY = double(m_nMax_Y) / 2;

	m_pMatrix = new cairo_matrix_t;
	m_pDisplayMatrix = new cairo_matrix_t;
	m_pDisplayMatrixNoRotate = new cairo_matrix_t;

	m_surface_tmp = cairo_image_surface_create (CAIRO_FORMAT_RGB24, m_oDeviceFrameRect.GetWidth(), m_oDeviceFrameRect.GetHeight());
	m_cr_tmp = cairo_create (m_surface_tmp);

	Clear();
}

wxGISDisplayEx::~wxGISDisplayEx(void)
{
	for(size_t i = 0; i < m_saLayerCaches.size(); i++)
	{
		cairo_destroy(m_saLayerCaches[i].pCairoContext);
		cairo_surface_destroy(m_saLayerCaches[i].pCairoSurface);		
	}
	wxDELETE(m_pMatrix);
	wxDELETE(m_pDisplayMatrix);
	wxDELETE(m_pDisplayMatrixNoRotate);
    cairo_destroy (m_cr_tmp);
    cairo_surface_destroy (m_surface_tmp);
}

void wxGISDisplayEx::Clear()
{
	for(size_t i = 1; i < m_saLayerCaches.size(); i++)
	{
		cairo_destroy(m_saLayerCaches[i].pCairoContext);
		cairo_surface_destroy(m_saLayerCaches[i].pCairoSurface);		
	}

	//default map bounds
	m_CurrentBounds.MinX = ENVMIN_X;
	m_CurrentBounds.MaxX = ENVMAX_X;
	m_CurrentBounds.MinY = ENVMIN_Y;
	m_CurrentBounds.MaxY = ENVMAX_Y;

	m_CurrentBoundsRotated = m_CurrentBounds;
	m_dRotatedBoundsCenterX = m_CurrentBoundsRotated.MinX + (m_CurrentBoundsRotated.MaxX - m_CurrentBoundsRotated.MinX) / 2;
	m_dRotatedBoundsCenterY = m_CurrentBoundsRotated.MinY + (m_CurrentBoundsRotated.MaxY - m_CurrentBoundsRotated.MinY) / 2;

	m_CurrentBoundsX8 = m_CurrentBounds;
	IncreaseEnvelope(&m_CurrentBoundsX8, 8);

	m_nLastCacheID = 0;
	m_nCurrentLayer = 0;
	m_dAngleRad = 0;
	cairo_matrix_init(m_pMatrix, 1, 0, 0, 1, 0, 0);
	cairo_matrix_init(m_pDisplayMatrix, 1, 0, 0, 1, 0, 0);
	cairo_matrix_init(m_pDisplayMatrixNoRotate, 1, 0, 0, 1, 0, 0);

	m_stFillColour.dRed = m_stFillColour.dGreen = m_stFillColour.dBlue = 1;
	m_stFillColour.dAlpha = 1;
	m_stLineColour.dRed = m_stLineColour.dGreen = m_stLineColour.dBlue = 0;
	m_stLineColour.dAlpha = 1;
	m_stPointColour = m_stLineColour;

	m_dLineWidth = m_dPointRadius = 0.5;

	m_bZeroCacheSet = false;

	OnEraseBackground();
}

cairo_t* wxGISDisplayEx::CreateContext(wxDC* dc)
{
#ifdef __WXMSW__
//#if CAIRO_HAS_WIN32_SURFACE
     HDC hdc = (HDC)dc->GetHDC();
     cairo_t *cr = cairo_create(cairo_win32_surface_create( hdc ));
     return cr;
#endif

#ifdef __WXGTK__
//#ifdef GDK_WINDOWING_X11
     GdkDrawable *drawable = dc->m_window;
     cairo_t *cr = gdk_cairo_create( drawable ) ;
     return cr;
#endif
     return NULL;
}

void wxGISDisplayEx::OnEraseBackground(void)
{	
	wxCriticalSectionLocker locker(m_CritSect);
	cairo_set_source_rgb(m_saLayerCaches[0].pCairoContext, m_stBackGroudnColour.dRed, m_stBackGroudnColour.dGreen, m_stBackGroudnColour.dBlue);
	cairo_paint(m_saLayerCaches[0].pCairoContext);
}

void wxGISDisplayEx::Output(wxDC* pDC, double *x1, double *y1, double *x2, double *y2)
{
	wxCriticalSectionLocker locker(m_CritSect);
	cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	surface = cairo_get_target(cr);

	cairo_set_source_surface (cr, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
	//cairo_set_source_surface (cr, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, m_dOrigin_X, m_dOrigin_Y);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	if(x1 && y1 && x2 && y2)
		cairo_clip_extents(cr, x1, y1, x2, y2);
	cairo_paint (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

void wxGISDisplayEx::ZoomingDraw(wxRect &rc, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);
	cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	surface = cairo_get_target(cr);

	//compute scale
	double dScaleX = double(rc.GetWidth()) / m_oDeviceFrameRect.GetWidth();
	double dScaleY = double(rc.GetHeight()) / m_oDeviceFrameRect.GetHeight();
	cairo_scale(cr, dScaleX, dScaleY);
	cairo_set_source_surface (cr, m_saLayerCaches[m_saLayerCaches.size() - 1].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
	//cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
	//cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

void wxGISDisplayEx::WheelingDraw(double dZoom, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);
	cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	surface = cairo_get_target(cr);

	if(IsDoubleEquil(dZoom, 1)) // no zoom
	{
		cairo_set_source_surface (cr, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
		cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);	
	}
	else if(dZoom > 1) // zoom in
	{
		cairo_scale(cr, dZoom, dZoom);

		double dDCXDelta = m_dFrameCenterX / dZoom;
		double dDCYDelta = m_dFrameCenterY / dZoom;
		double dOrigin_X = m_dCacheCenterX - dDCXDelta;
		double dOrigin_Y = m_dCacheCenterY - dDCYDelta;

		cairo_set_source_surface (cr, m_saLayerCaches[m_saLayerCaches.size() - 1].pCairoSurface, -dOrigin_X, -dOrigin_Y);
	}
	else //zoom out
	{
		cairo_set_source_rgb(cr, m_stBackGroudnColour.dRed, m_stBackGroudnColour.dGreen, m_stBackGroudnColour.dBlue);
		cairo_paint(cr);

		double dDCXDelta = m_dFrameCenterX * dZoom;
		double dDCYDelta = m_dFrameCenterY * dZoom;
		double dOrigin_X = m_dFrameCenterX - dDCXDelta;
		double dOrigin_Y = m_dFrameCenterY - dDCYDelta;

		cairo_translate (cr, dOrigin_X, dOrigin_Y);
		cairo_scale(cr, dZoom, dZoom);

		cairo_set_source_surface (cr, m_saLayerCaches[m_saLayerCaches.size() - 1].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
	}
	//cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_BEST);

	//cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

void wxGISDisplayEx::PanningDraw(wxCoord x, wxCoord y, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);

	cairo_set_source_rgb(m_cr_tmp, m_stBackGroudnColour.dRed, m_stBackGroudnColour.dGreen, m_stBackGroudnColour.dBlue);
	cairo_paint(m_cr_tmp);

	cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_saLayerCaches.size() - 1].pCairoSurface, -m_dOrigin_X - x, -m_dOrigin_Y - y);

	cairo_paint (m_cr_tmp);

	cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	surface = cairo_get_target(cr);

	cairo_set_source_surface (cr, m_surface_tmp, 0, 0);
	cairo_paint (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

void wxGISDisplayEx::RotatingDraw(double dAngle, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);

	cairo_set_source_rgb(m_cr_tmp, m_stBackGroudnColour.dRed, m_stBackGroudnColour.dGreen, m_stBackGroudnColour.dBlue);
	cairo_paint(m_cr_tmp);

	int w = cairo_image_surface_get_width (cairo_get_target(m_cr_tmp));
	int h = cairo_image_surface_get_height (cairo_get_target(m_cr_tmp));
	//cairo_translate (m_cr_tmp, 0.5 * m_oDeviceFrameRect.GetWidth(), 0.5 * m_oDeviceFrameRect.GetHeight());

	//double dWorldCenterX = m_CurrentBounds.MinX + double(m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	//double dWorldCenterY = m_CurrentBounds.MinY + double(m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;
	//World2DC(&dWorldCenterX, &dWorldCenterY);

	//cairo_translate (m_cr_tmp, dWorldCenterX, dWorldCenterY);
	cairo_translate (m_cr_tmp, m_dFrameCenterX, m_dFrameCenterY);
	cairo_rotate (m_cr_tmp, dAngle);
	//cairo_translate (m_cr_tmp, -0.5 * m_oDeviceFrameRect.GetWidth(), -0.5 * m_oDeviceFrameRect.GetHeight());
	//cairo_translate (m_cr_tmp, -dWorldCenterX, -dWorldCenterY);
	cairo_translate (m_cr_tmp, -m_dFrameCenterX, -m_dFrameCenterY);
	cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_saLayerCaches.size() - 1].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);

	cairo_paint (m_cr_tmp);

	cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	surface = cairo_get_target(cr);

	cairo_set_source_surface (cr, m_surface_tmp, 0, 0);
	cairo_paint (cr);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);

	cairo_matrix_t mat = {1, 0, 0, 1, 0, 0};
	cairo_set_matrix (m_cr_tmp, &mat);
}

size_t wxGISDisplayEx::AddCache(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(m_bZeroCacheSet)
	{
		LAYERCACHEDATA layercachedata;
		layercachedata.bIsDerty = true;
		layercachedata.pCairoSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, m_nMax_X, m_nMax_Y);
		layercachedata.pCairoContext = cairo_create (layercachedata.pCairoSurface);
		m_saLayerCaches.push_back(layercachedata);
		m_nLastCacheID++;
		//cairo_set_matrix (layercachedata.pCairoContext, m_pMatrix);
	}
	else
		m_bZeroCacheSet = true;
	return m_nLastCacheID;
}

size_t wxGISDisplayEx::GetLastCacheID(void)
{
	return m_nLastCacheID;
}

bool wxGISDisplayEx::IsCacheDerty(size_t nCacheID)
{
	return m_saLayerCaches[nCacheID].bIsDerty;
}

void wxGISDisplayEx::SetCacheDerty(size_t nCacheID, bool bIsDerty)
{
	m_saLayerCaches[nCacheID].bIsDerty = bIsDerty;
}

void wxGISDisplayEx::SetDrawCache(size_t nCacheID)
{
	//if(m_nCurrentLayer == nCacheID)
	//	return;
	//else 
	if(nCacheID == 0)
	{
		//OnEraseBackground();
		m_nCurrentLayer = 0;
	}
	else
	{
		wxCriticalSectionLocker locker(m_CritSect);
		//merge previous cache
		//TODO: clip by frame size cairo_clip()?
		cairo_set_source_surface (m_saLayerCaches[nCacheID].pCairoContext, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, 0, 0);
		cairo_set_operator (m_saLayerCaches[nCacheID].pCairoContext, CAIRO_OPERATOR_SOURCE);
		cairo_paint (m_saLayerCaches[nCacheID].pCairoContext);

		m_nCurrentLayer = nCacheID;
	}
	//cairo_set_matrix (m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_pMatrix);
	//cairo_transform (m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_pMatrix);
}

void wxGISDisplayEx::SetDerty(bool bIsDerty)
{
	for(size_t i = 0; i < m_saLayerCaches.size(); i++)
		m_saLayerCaches[i].bIsDerty = bIsDerty;
}

bool wxGISDisplayEx::IsDerty(void)
{
	for(size_t i = 0; i < m_saLayerCaches.size(); i++)
		if(m_saLayerCaches[i].bIsDerty != false)
			return true;
	return false;
}

void wxGISDisplayEx::SetDeviceFrame(wxRect &rc)
{
	m_oDeviceFrameRect = rc;
	
    cairo_destroy (m_cr_tmp);
    cairo_surface_destroy (m_surface_tmp);

	m_surface_tmp = cairo_image_surface_create (CAIRO_FORMAT_RGB24, m_oDeviceFrameRect.GetWidth(), m_oDeviceFrameRect.GetHeight());
	m_cr_tmp = cairo_create (m_surface_tmp);


	//compute current transform matrix
	InitTransformMatrix();
}

wxRect wxGISDisplayEx::GetDeviceFrame(void)
{
	return m_oDeviceFrameRect;
}

void wxGISDisplayEx::SetBounds(OGREnvelope& Bounds)
{
	//update bounds to frame ratio
	m_CurrentBounds = Bounds;
	m_dRotatedBoundsCenterX = Bounds.MinX + (Bounds.MaxX - Bounds.MinX) / 2;
	m_dRotatedBoundsCenterY = Bounds.MinY + (Bounds.MaxY - Bounds.MinY) / 2;
	m_CurrentBoundsRotated = m_CurrentBounds;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
		RotateEnvelope(&m_CurrentBoundsRotated, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
	//SetEnvelopeRatio(&m_CurrentBoundsRotated, double(m_oDeviceFrameRect.GetWidth()) / m_oDeviceFrameRect.GetHeight());
	m_CurrentBoundsX8 = m_CurrentBoundsRotated;
	IncreaseEnvelope(&m_CurrentBoundsX8, 8);

	SetDerty(true);
	//compute current transform matrix
	InitTransformMatrix();
}

OGREnvelope wxGISDisplayEx::GetBounds(void)
{
	return m_CurrentBoundsRotated;//m_CurrentBounds;
}

void wxGISDisplayEx::InitTransformMatrix(void)
{
	m_dFrameCenterX = double(m_oDeviceFrameRect.GetWidth()) / 2;
	m_dFrameCenterY = double(m_oDeviceFrameRect.GetHeight()) / 2;
	double dWorldCenterX = double(m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	double dWorldCenterY = double(m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;

	//origin (UL corner)
	m_dOrigin_X = m_dCacheCenterX - m_dFrameCenterX;
	m_dOrigin_Y = m_dCacheCenterY - m_dFrameCenterY;
	//get scale
	double dScaleX = fabs(m_dFrameCenterX / dWorldCenterX);
	double dScaleY = fabs(m_dFrameCenterY / dWorldCenterY);
	double dScale = std::min(dScaleX, dScaleY);

	//if(!IsDoubleEquil(dScale, dScaleX))
	//{
	//	double dDeltaX = fabs(dWorldCenterX - m_dFrameCenterX / dScale);
	//	m_CurrentBounds.MinX -= dDeltaX;
	//	m_CurrentBounds.MaxX += dDeltaX;
	//	dWorldCenterX = double(m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	//}
	//if(!IsDoubleEquil(dScale, dScaleY))
	//{
	//	double dDeltaY = fabs(dWorldCenterY - m_dFrameCenterY / dScale);
	//	m_CurrentBounds.MinY -= dDeltaY;
	//	m_CurrentBounds.MaxY += dDeltaY;
	//	dWorldCenterY = double(m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;
	//}

	double dWorldDeltaX = dWorldCenterX + m_CurrentBounds.MinX;
	double dWorldDeltaY = dWorldCenterY + m_CurrentBounds.MinY;

	double dWorldDeltaXSt = dScale * dWorldDeltaX;// + m_dAngleRad * dWorldDeltaY;
	double dWorldDeltaYSt = dScale * dWorldDeltaY;//m_dAngleRad * dWorldDeltaX + 

	double dCenterX = m_dCacheCenterX - dWorldDeltaXSt;//(dWorldCenterX + m_CurrentBounds.MinX) * dScale;//
	double dCenterY = m_dCacheCenterY + dWorldDeltaYSt;//(dWorldCenterY + m_CurrentBounds.MinY) * dScale;//
	m_dFrameXShift = m_dFrameCenterX - dWorldDeltaXSt;//(dWorldCenterX + m_CurrentBounds.MinX) * dScale;//
	m_dFrameYShift = m_dFrameCenterY + dWorldDeltaYSt;//(dWorldCenterY + m_CurrentBounds.MinY) * dScale;//

//	cairo_matrix_init (m_pMatrix, 1, 0, 0, 1, m_dCacheCenterX, m_dCacheCenterY); 
	//cairo_matrix_init (m_pMatrix, dScale, 0.0, 0.0, -dScale, dCenterX, dCenterY); 
	cairo_matrix_init_translate (m_pMatrix, m_dCacheCenterX, m_dCacheCenterY); 
	//rotate
	//cairo_matrix_rotate(m_pMatrix, 45.0 * M_PI / 180.0);
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	//{
		//cairo_matrix_translate(m_pMatrix, dWorldDeltaXSt, dWorldDeltaYSt);
		cairo_matrix_rotate(m_pMatrix, m_dAngleRad);
		//cairo_matrix_translate(m_pMatrix, -dWorldDeltaXSt, dWorldDeltaYSt);
	//}
	//else
	cairo_matrix_translate(m_pMatrix, -dWorldDeltaXSt, dWorldDeltaYSt);

	cairo_matrix_scale(m_pMatrix, dScale, -dScale);

	cairo_matrix_init_translate (m_pDisplayMatrix, m_dFrameCenterX, m_dFrameCenterY); 
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
		cairo_matrix_rotate(m_pDisplayMatrix, m_dAngleRad);
	cairo_matrix_translate(m_pDisplayMatrix, -dWorldDeltaXSt, dWorldDeltaYSt);
	cairo_matrix_scale(m_pDisplayMatrix, dScale, -dScale);

	cairo_matrix_init_translate (m_pDisplayMatrixNoRotate, m_dFrameCenterX, m_dFrameCenterY); 
	cairo_matrix_translate(m_pDisplayMatrixNoRotate, -dWorldDeltaXSt, dWorldDeltaYSt);
	cairo_matrix_scale(m_pDisplayMatrixNoRotate, dScale, -dScale);

	//set matrix to all caches
	for(size_t i = 0; i < m_saLayerCaches.size(); ++i)
		cairo_set_matrix (m_saLayerCaches[i].pCairoContext, m_pMatrix);
}

void wxGISDisplayEx::DC2World(double* pdX, double* pdY)
{
	//cairo_matrix_t InvertMatrix = {m_pMatrix->xx, m_pMatrix->yx, m_pMatrix->xy, m_pMatrix->yy, m_dFrameXShift, m_dFrameYShift};//set center of real window not cache
	cairo_matrix_t InvertMatrix = {m_pDisplayMatrix->xx, m_pDisplayMatrix->yx, m_pDisplayMatrix->xy, m_pDisplayMatrix->yy, m_pDisplayMatrix->x0, m_pDisplayMatrix->y0};//set center of real window not cache
	cairo_matrix_invert(&InvertMatrix);
	cairo_matrix_transform_point(&InvertMatrix, pdX, pdY);
	//cairo_device_to_user (m_saLayerCaches[m_nCurrentLayer].pCairoContext, pdX, pdY);
}

void wxGISDisplayEx::World2DC(double* pdX, double* pdY)
{
	//cairo_matrix_t Matrix = {m_pMatrix->xx, m_pMatrix->yx, m_pMatrix->xy, m_pMatrix->yy, m_dFrameXShift, m_dFrameYShift};//set center of real window not cache
	cairo_matrix_t Matrix = {m_pDisplayMatrix->xx, m_pDisplayMatrix->yx, m_pDisplayMatrix->xy, m_pDisplayMatrix->yy, m_pDisplayMatrix->x0, m_pDisplayMatrix->y0};//set center of real window not cache
	cairo_matrix_transform_point(&Matrix, pdX, pdY);
	//cairo_user_to_device (m_saLayerCaches[m_nCurrentLayer].pCairoContext, pdX, pdY);
}

void wxGISDisplayEx::DC2WorldDist(double* pdX, double* pdY)
{
	//cairo_matrix_transform_distance
	cairo_device_to_user_distance (m_saLayerCaches[m_nCurrentLayer].pCairoContext, pdX, pdY);
}

void wxGISDisplayEx::World2DCDist(double* pdX, double* pdY)
{
	cairo_user_to_device_distance (m_saLayerCaches[m_nCurrentLayer].pCairoContext, pdX, pdY);
}
void wxGISDisplayEx::TestDraw(void)
{
	srand ( time(NULL) );
	SetDrawCache(0);

	m_stLineColour.dRed = 0.0;
	m_stLineColour.dGreen = 0.0;
	m_stLineColour.dBlue = 0.0;
	m_stLineColour.dAlpha = 1.0;
	m_stPointColour = m_stLineColour;

	int random_number1 = (rand() % 50); 
	int random_number2 = (rand() % 50); 
	int random_number3 = (rand() % 50); 
	m_stFillColour.dRed = double(205 + random_number1) / 255;
	m_stFillColour.dGreen = double(205 + random_number2) / 255;
	m_stFillColour.dBlue = double(205 + random_number3) / 255;
	m_stFillColour.dAlpha = 1.0;

	SetColor(m_stLineColour);
	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -178.0, 79.0);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 178.0, -79.0);
	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 178.0, 79.0);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -178.0, -79.0);
	cairo_set_line_width (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 0.2);
	cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);

	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, 6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, 6);
//	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, 6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, -6);
//	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, -6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, -6);
//	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, -6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, 6);

	//cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 64.0, 25.6);
	//cairo_rel_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 51.2, 51.2);
	//cairo_rel_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -51.2, 51.2);
	//cairo_rel_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -51.2, -51.2);
	//cairo_close_path (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	//cairo_rectangle (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, 6, 12, -12);
	SetColor(m_stFillColour);
	cairo_fill_preserve (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	SetColor(m_stLineColour);
	cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
}

size_t wxGISDisplayEx::GetDrawCache(void)
{
	return m_nCurrentLayer;
}

void wxGISDisplayEx::SetColor(double dRed, double dGreen, double dBlue, double dAlpha)
{
	cairo_set_source_rgba(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dRed, dGreen, dBlue, dAlpha);
}

void wxGISDisplayEx::SetColor(RGBA Color)
{
	SetColor(Color.dRed, Color.dGreen, Color.dBlue, Color.dAlpha);
}

bool wxGISDisplayEx::DrawPoint(double dX, double dY)
{
	cairo_arc (m_saLayerCaches[m_nCurrentLayer].pCairoContext, dX, dY, m_dPointRadius, 0, 2*M_PI);
	return true;
}

bool wxGISDisplayEx::DrawPointFast(double dX, double dY)
{
	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, dX, dY);
	cairo_close_path (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	//cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);//may be on group calls but nothing to show during pending

	//cairo_move_to (cr, x, y);
	//cairo_line_to (cr, x, y);
	//cairo_stroke (cr);}
	return true;
}

bool wxGISDisplayEx::DrawPointFast(const OGRPoint* pPoint)
{
	if(NULL == pPoint)
		return false;
	return DrawPointFast(pPoint->getX(), pPoint->getY());
}


void wxGISDisplayEx::SetLineCap(cairo_line_cap_t line_cap)
{
	cairo_set_line_cap(m_saLayerCaches[m_nCurrentLayer].pCairoContext, line_cap);
}

void wxGISDisplayEx::SetLineWidth(double dWidth)
{
	double x_new = (m_pMatrix->xx + m_pMatrix->xy) * dWidth;
	double y_new = (m_pMatrix->yx + m_pMatrix->yy) * dWidth;
	double dScale = (fabs(x_new) + fabs(y_new)) /*/ 4*/;
	m_dLineWidth = dWidth / dScale;
}

void wxGISDisplayEx::SetPointRadius(double dRadius)
{
	double x_new = (m_pMatrix->xx + m_pMatrix->xy) * dRadius;
	double y_new = (m_pMatrix->yx + m_pMatrix->yy) * dRadius;
	double dScale = (fabs(x_new) + fabs(y_new)) / 2;
	m_dPointRadius = dRadius / dScale;
}

void wxGISDisplayEx::SetFillRule(cairo_fill_rule_t fill_rule)
{
	cairo_set_fill_rule(m_saLayerCaches[m_nCurrentLayer].pCairoContext, fill_rule);
}

bool wxGISDisplayEx::DrawPoint(const OGRPoint* pPoint)
{
	if(NULL == pPoint)
		return false;
	return DrawPoint(pPoint->getX(), pPoint->getY());
}

bool wxGISDisplayEx::DrawLine(const OGRLineString* pLine)
{
	if(NULL == pLine)
		return false;
	int nPointCount = pLine->getNumPoints();
	if(nPointCount < 2)
		return false;
    OGRRawPoint* pOGRRawPoints = new OGRRawPoint[nPointCount * 3];
	pLine->getPoints(pOGRRawPoints);

	ClipGeometryByEnvelope(pOGRRawPoints, &nPointCount, m_CurrentBoundsX8);

	cairo_move_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, pOGRRawPoints[0].x, pOGRRawPoints[0].y);

	for(int i = 1; i < nPointCount; ++i)
	{
        cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, pOGRRawPoints[i].x, pOGRRawPoints[i].y);	
	}
	wxDELETEA(pOGRRawPoints);
	return true;
}

bool wxGISDisplayEx::DrawRing(const OGRLinearRing* pRing)
{
	if(NULL == pRing)
		return false;
	DrawLine((OGRLineString*)pRing);
//	cairo_close_path(m_saLayerCaches[m_nCurrentLayer].pCairoContext);

	return true;
}

bool wxGISDisplayEx::DrawPolygon(const OGRPolygon* pPolygon)
{
	if(NULL == pPolygon)
		return false;
	//check ext ring in extent
	const OGRLinearRing *pRing = pPolygon->getExteriorRing();
	OGREnvelope Env;
	pRing->getEnvelope(&Env);

	if(!IsDoubleEquil(m_dAngleRad, 0.0))
		RotateEnvelope(&Env, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);

	bool bDraw = Env.Contains(m_CurrentBoundsRotated) || m_CurrentBoundsRotated.Contains(Env) || Env.Intersects(m_CurrentBoundsRotated);
	if( !bDraw )
		return false;

	DrawRing( pRing );
	int nNumInteriorRings = pPolygon->getNumInteriorRings();
	for(int nPart = 0; nPart < nNumInteriorRings; ++nPart)
	{
		const OGRLinearRing* pRing = pPolygon->getInteriorRing(nPart);
		//cairo_new_sub_path (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		if(!CheckDrawAsPoint((OGRGeometry*)pRing, true))
		//{
			DrawRing(pRing);
		//}
	}
	SetFillRule( CAIRO_FILL_RULE_EVEN_ODD );
	SetColor(m_stFillColour);
	cairo_fill_preserve (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	//cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);

	return true;
}

bool wxGISDisplayEx::CheckDrawAsPoint(const OGRGeometry* pGeometry, bool bCheckEnvelope)
{
	//double m_World2DC = GetRatio();
	OGREnvelope Envelope, TestEnv;
	pGeometry->getEnvelope(&Envelope);
	TestEnv = Envelope;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
		RotateEnvelope(&TestEnv, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
		//RotateEnvelope(&Envelope, m_dAngleRad);
	if(bCheckEnvelope && !m_CurrentBoundsRotated.Intersects(TestEnv))
		return true;

	double EnvWidth = Envelope.MaxX - Envelope.MinX;
	double EnvHeight = Envelope.MaxY - Envelope.MinY;

	World2DCDist(&EnvWidth, &EnvHeight);
	//cairo_user_to_device_distance(m_saLayerCaches[m_nCurrentLayer].pCairoContext, );
	EnvWidth = fabs(EnvWidth);
	EnvHeight = fabs(EnvHeight);
	if(	EnvWidth <= MINPOLYDRAWAREA && EnvHeight <= MINPOLYDRAWAREA )
	{
		if(	EnvWidth >= MINPOLYAREA && EnvHeight >= MINPOLYAREA )
		{
			cairo_move_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MinX, Envelope.MinY);
			cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MaxX, Envelope.MaxY);
		}
		return true;
	}
	return false;
}

void wxGISDisplayEx::DrawGeometry(OGRGeometry* poGeometry)
{
    if(!poGeometry)
        return;

	//OGRGeometry* pDrawGeom = poGeometry->Intersection(m_pCutGeom.get());
	//if(!pDrawGeom)
 //       return;

	OGRwkbGeometryType type = wkbFlatten(poGeometry->getGeometryType());
	switch(type)
	{
	case wkbPoint:
		SetColor(m_stPointColour);
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, 
m_dPointRadius + m_dPointRadius + m_dLineWidth);
		if(!DrawPointFast((OGRPoint*)poGeometry))
			break;
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_dPointRadius + m_dPointRadius);
		SetColor(m_stFillColour);
		if(!DrawPointFast((OGRPoint*)poGeometry))
			return;
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbLineString:
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, 
m_dLineWidth);
		//check if line is too small
		if(!CheckDrawAsPoint(poGeometry))
			if(!DrawLine((OGRLineString*)poGeometry))
				break;
		SetColor(m_stLineColour);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbLinearRing:
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, 
m_dLineWidth);
		//check if line is too small
		if(!CheckDrawAsPoint(poGeometry))
			if(!DrawRing((OGRLinearRing*)poGeometry))
				break;
		SetColor(m_stLineColour);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbPolygon:
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, 
m_dLineWidth);
		//check if line is too small
		if(!CheckDrawAsPoint(poGeometry))
			if(!DrawPolygon((OGRPolygon*)poGeometry))
				break;
		SetColor(m_stLineColour);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbMultiPolygon:
		//break;
	case wkbMultiPoint:
		//break;
	case wkbMultiLineString:
		//break;
	case wkbGeometryCollection:
		{
		OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)poGeometry;
		for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
			DrawGeometry(pOGRGeometryCollection->getGeometryRef(i));
		}
		break;
	default:
	case wkbUnknown:
	case wkbNone:
		break;
	}
	//wxDELETE(pDrawGeom)
}

double wxGISDisplayEx::GetRatio(void)
{
	double x_new = m_pMatrix->xx + m_pMatrix->xy;
	double y_new = m_pMatrix->yx + m_pMatrix->yy;
	double dScale = (fabs(x_new) + fabs(y_new)) / 2;
	return dScale;
}

OGREnvelope wxGISDisplayEx::TransformRect(wxRect &rect)
{
	OGREnvelope out;
	double dX1, dX2, dY2, dY1; 
	if(IsDoubleEquil(m_dAngleRad, 0.0))//1)//
	{
		dX1 = rect.GetLeft();
		dX2 = rect.GetRight();
		dY2 = rect.GetTop();
		dY1 = rect.GetBottom(); 

		DC2World(&dX1, &dY1);
		DC2World(&dX2, &dY2);
	}
	else
	{
		double dWHalf = double(rect.width) / 2;
		double dHHalf = double(rect.height) / 2;
		double dXCenter = rect.x + dWHalf, dYCenter = rect.y + dHHalf; 
		DC2World(&dXCenter, &dYCenter);

		cairo_matrix_t InvertMatrix = {m_pDisplayMatrixNoRotate->xx, m_pDisplayMatrixNoRotate->yx, m_pDisplayMatrixNoRotate->xy, m_pDisplayMatrixNoRotate->yy, m_pDisplayMatrixNoRotate->x0, m_pDisplayMatrixNoRotate->y0};
		cairo_matrix_invert(&InvertMatrix);

		cairo_matrix_transform_distance(&InvertMatrix, &dWHalf, &dHHalf);

		dX1 = dXCenter - dWHalf;
		dX2 = dXCenter + dWHalf;
		dY1 = dYCenter - dHHalf;
		dY2 = dYCenter + dHHalf;
	}
	out.MinX = std::min(dX1, dX2);
	out.MinY = std::min(dY1, dY2);
	out.MaxX = std::max(dX1, dX2);
	out.MaxY = std::max(dY1, dY2);
	return out;
}

void wxGISDisplayEx::SetRotate(double dAngleRad)
{
	m_dAngleRad = dAngleRad;
//for rotate panning & zooming
	m_CurrentBoundsRotated = m_CurrentBounds;
	m_dRotatedBoundsCenterX = m_CurrentBoundsRotated.MinX + (m_CurrentBoundsRotated.MaxX - m_CurrentBoundsRotated.MinX) / 2;
	m_dRotatedBoundsCenterY = m_CurrentBoundsRotated.MinY + (m_CurrentBoundsRotated.MaxY - m_CurrentBoundsRotated.MinY) / 2;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
		RotateEnvelope(&m_CurrentBoundsRotated, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
	m_CurrentBoundsX8 = m_CurrentBoundsRotated;
	IncreaseEnvelope(&m_CurrentBoundsX8, 8);

	//compute current transform matrix
	InitTransformMatrix();

	SetDerty(true);
}
