/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  display transformation. Transform from world to screen coordinates and vice versa
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

#include "wxgis/display/display.h"

void WXDLLIMPEXP_GIS_DSP IncreaseEnvelope(OGREnvelope *pEnv, double dSize);
void WXDLLIMPEXP_GIS_DSP RotateEnvelope(OGREnvelope *pEnv, double dAngle, double dX, double dY);
void WXDLLIMPEXP_GIS_DSP SetEnvelopeRatio(OGREnvelope *pEnv, double dRatio);


//DisplayTransformation – This object defines how real-world coordinates are mapped to a output device. Three rectangles define the transformation. 
//The Bounds specifies the full extent in real-world coordinates. The VisibleBounds specifies what extent is currently visible. 
//And the DeviceFrame specifies where the VisibleBounds appears on the output device. Since the aspect ratio of the DeviceFrame may not always match the aspect ratio of the specified VisibleBounds, the transformation calculates the actual visible bounds that fits the DeviceFrame. This is called the FittedBounds and is in real-world coordinates. All coordinates can be rotated about the center of the visible bounds by simply setting the transformation’s Rotation property. 

//class wxGISDisplayTransformation :
//	public IDisplayTransformation
//{
//public:
//	wxGISDisplayTransformation(void);
//	virtual ~wxGISDisplayTransformation(void);
//	virtual void Reset(void);
//	virtual void SetDeviceFrame(wxRect rc);
//	virtual wxRect GetDeviceFrame(void);
//	virtual bool IsBoundsSet(void);
//	virtual void SetBounds(OGREnvelope bounds);
//	virtual OGREnvelope GetBounds(void);
//	virtual OGREnvelope GetVisibleBounds(void);
//	virtual double GetRatio(void);
//	virtual double GetScaleRatio(void);
//	virtual void SetSpatialReference(OGRSpatialReference* pSpatialReference);
//	virtual OGRSpatialReference* GetSpatialReference(void);
//	virtual wxPoint* TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount);
//	virtual void TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount, wxPoint* pResult);
//	virtual OGRRawPoint* TransformCoordDC2World(wxPoint* pPoints, size_t nPointCount);
//	virtual void SetPPI(wxSize new_res);
//	virtual OGREnvelope TransformRect(wxRect rect);
//protected:
//	// World2DC
//	double xWorld2DC(double x);
//	double yWorld2DC(double y);
//	// DC2World
//	double xDC2World(int x);
//	double yDC2World(int y);
//protected:
//	double	m_World2DC, m_DC2World, m_DCXDelta, m_DCYDelta, m_WorldXDelta, m_WorldYDelta;
//	wxRect m_DeviceFrameRect;
//	OGREnvelope m_Bounds;
//	OGRSpatialReference* m_pSpatialReference;
//	bool m_bIsBoundsSet;
//	wxSize m_ppi;
//};
