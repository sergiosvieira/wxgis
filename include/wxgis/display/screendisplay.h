/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISScreenDisplay class.
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
#pragma once

#include "wxgis/framework/framework.h"
#include "wxgis/display/display.h"
#include "wxgis/display/displaytransformation.h"
#include "wx/dcgraph.h"

//-----------------------------------------------------
//wxRasterDrawThread
//-----------------------------------------------------

class WXDLLIMPEXP_GIS_DSP wxRasterDrawThread : public wxThread
{
public:
	wxRasterDrawThread(const unsigned char* pOrigData, unsigned char* pDestData, int nOrigX, int nOrigY, double rOrigX, double rOrigY, int nDestX, int nDestY, double rDeltaX, double rDeltaY, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel, int nYbeg, int nYend);
    virtual void *Entry();
    virtual void OnExit();
    /*virtual*/ static void OnNearestNeighbourInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
    /*virtual*/ static void OnBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
     /*virtual*/ static void OnHalfBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
     /*virtual*/ static void OnHalfQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
     /*virtual*/ static void OnFourQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
    /*virtual*/ static void OnBicubicInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
    static double BiCubicKernel(double x);
private:
    ITrackCancel* m_pTrackCancel;
    const unsigned char* m_pOrigData;
    unsigned char* m_pDestData;
    int m_nOrigX;
    int m_nOrigY;
    double m_rOrigX;
    double m_rOrigY;
    int m_nDestX;
    int m_nDestY;
    double m_rDeltaX;
    double m_rDeltaY;
    wxGISEnumDrawQuality m_Quality;
    int m_nYbeg, m_nYend;
};


//-----------------------------------------------------
// wxGISDisplay
//-----------------------------------------------------

class WXDLLIMPEXP_GIS_DSP wxGISDisplay :
	public IDisplay
{
public:
	wxGISDisplay(void);
	virtual ~wxGISDisplay(void);
	virtual bool IsDerty(void);
	virtual void SetDerty(bool bIsDerty);
	virtual IDisplayTransformation* GetDisplayTransformation(void);
protected:
	bool m_bIsDerty;
	wxGISDisplayTransformation* m_pDisplayTransformation;
};

//-----------------------------------------------------
// wxGISScreenDisplay
//-----------------------------------------------------


class WXDLLIMPEXP_GIS_DSP wxGISScreenDisplay :
	public ICachedDisplay
{
public:
	//wxGISScreenDisplay
	wxGISScreenDisplay(void);
	virtual ~wxGISScreenDisplay(void);
    wxImage Scale(const unsigned char* pData, int nOrigX, int nOrigY, double rOrigX, double rOrigY, int nDestX, int nDestY, double rDeltaX, double rDeltaY, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel = NULL);
    wxImage Scale(wxImage SourceImage, int nDestWidth, int nDestHeight, wxGISEnumDrawQuality Quality, ITrackCancel* pTrackCancel = NULL);
	//Stretch buffer to nDestWidth & nDestHeight and draw in x, y coordinates
	//Drawing...
	virtual void OnStretchDraw(wxDC &dc, wxCoord nDestWidth, wxCoord nDestHeight, wxCoord x = 0, wxCoord y = 0, bool bClearBackground = false, wxGISEnumDrawQuality quality = enumGISQualityNearest );
	//cut buffer by Rect and stretch result in GetDeviceFrame()
	virtual void OnStretchDraw2(wxDC &dc, wxRect Rect, bool bClearBackground = false, wxGISEnumDrawQuality quality = enumGISQualityNearest );
	//
	virtual void OnPanDraw(wxDC &dc, wxCoord x, wxCoord y);
    virtual void OnPanStop(wxDC &dc);
	//IDisplay
	virtual void OnDraw(wxDC &dc, wxCoord x = 0, wxCoord y = 0, bool bClearBackground = false);
	virtual void OnUpdate(void);
    virtual void SetDC(wxDC *pdc){m_pDrawDC = pdc;};
	virtual void SetBrush(wxBrush& Brush);
	virtual void SetPen(wxPen& Pen);
	virtual void SetFont(wxFont& Font);
	virtual void DrawPolygon(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE);
	virtual void DrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0, int fill_style = wxODDEVEN_RULE);
	virtual void DrawPoint(wxCoord x, wxCoord y);
	virtual void DrawLines(int n, wxPoint points[], wxCoord xoffset = 0, wxCoord yoffset = 0);
	virtual void DrawCircle(wxCoord x, wxCoord y, wxCoord radius);
	virtual void DrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
	virtual void DrawBitmap(const wxBitmap& bitmap, wxCoord x, wxCoord y, bool transparent = false);
	virtual void SetDerty(bool bIsDerty);
	virtual bool IsDerty(void);
	virtual IDisplayTransformation* GetDisplayTransformation(void);
    virtual RECTARARRAY* GetInvalidRect(void);
    virtual void AddInvalidRect(wxRect Rect);

	//ICachedDisplay
	virtual bool IsCacheDerty(size_t cache_id);
	virtual void SetCacheDerty(size_t cache_id, bool bIsDerty);
	virtual size_t AddCache(void);
	virtual void MergeCaches(size_t SrcCacheID, size_t DstCacheID);
	virtual void StartDrawing(size_t CacheID);
	virtual void FinishDrawing(void);
	virtual void ClearCaches(void);
	virtual size_t GetLastCacheID(void);
	//
	typedef struct _cachedata{
		bool IsDerty;
		wxBitmap bmp;
	} CACHEDATA;
protected:
	std::vector<CACHEDATA> m_caches;
	wxMemoryDC m_dc;
    wxDC *m_pDrawDC;
	//, m_GeoSelectionBuffer, m_AnnotationBuffer;
	size_t m_nLastCacheID;
    int m_nDrawCacheID;
	bool m_bIsDerty;
	wxGISDisplayTransformation* m_pDisplayTransformation;
    wxCriticalSection m_CritSect;
    RECTARARRAY m_InvalidRectArray;
};
