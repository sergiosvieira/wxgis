/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog filters of GxObjects to show.
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

#include "wxgis/catalog/catalog.h"

//------------------------------------------------------------
// wxGxObjectFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxObjectFilter : public IGxObjectFilter
{
public:
	wxGxObjectFilter(void);
	virtual ~wxGxObjectFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxPrjFileFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxPrjFileFilter : public wxGxObjectFilter
{
public:
	wxGxPrjFileFilter(void);
	virtual ~wxGxPrjFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxGISEnumSaveObjectResults CanSaveObject( IGxObject* pLocation, wxString sName );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
};

//------------------------------------------------------------
// wxGxDatasetFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxDatasetFilter : public wxGxObjectFilter
{
public:
	wxGxDatasetFilter(wxGISEnumDatasetType nType);
	virtual ~wxGxDatasetFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
protected:
    wxGISEnumDatasetType m_nType;
};

//------------------------------------------------------------
// wxGxShapeFileFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxShapeFileFilter : public wxGxObjectFilter
{
public:
	wxGxShapeFileFilter(void);
	virtual ~wxGxShapeFileFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxMapInfoFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxMapInfoFilter : public wxGxShapeFileFilter
{
public:
	wxGxMapInfoFilter(bool bIsTab = true);
	virtual ~wxGxMapInfoFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
protected:
    bool m_bIsTab;
};

//------------------------------------------------------------
// wxGxKMLFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxKMLFilter : public wxGxShapeFileFilter
{
public:
	wxGxKMLFilter(void);
	virtual ~wxGxKMLFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};


//------------------------------------------------------------
// wxGxDXFFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxDXFFilter : public wxGxShapeFileFilter
{
public:
	wxGxDXFFilter(void);
	virtual ~wxGxDXFFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxFolderFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxFolderFilter : public wxGxObjectFilter
{
public:
	wxGxFolderFilter(void);
	virtual ~wxGxFolderFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
};

//------------------------------------------------------------
// wxGxRasterFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxRasterFilter : public wxGxObjectFilter
{
public:
	wxGxRasterFilter(void);
	virtual ~wxGxRasterFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void) = 0;
    virtual wxString GetExt(void) = 0;
    virtual wxString GetDriver(void) = 0;
    virtual int GetSubType(void) = 0;
};

//------------------------------------------------------------
// wxGxTiffFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxTiffFilter : public wxGxRasterFilter
{
public:
	wxGxTiffFilter(void);
	virtual ~wxGxTiffFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxImgFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxImgFilter : public wxGxRasterFilter
{
public:
	wxGxImgFilter(void);
	virtual ~wxGxImgFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxBmpFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxBmpFilter : public wxGxRasterFilter
{
public:
	wxGxBmpFilter(void);
	virtual ~wxGxBmpFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxJpegFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxJpegFilter : public wxGxRasterFilter
{
public:
	wxGxJpegFilter(void);
	virtual ~wxGxJpegFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxPngFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxPngFilter : public wxGxRasterFilter
{
public:
	wxGxPngFilter(void);
	virtual ~wxGxPngFilter(void);
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
    virtual wxString GetDriver(void);
    virtual int GetSubType(void);
};

//------------------------------------------------------------
// wxGxTextFilter
//------------------------------------------------------------

class WXDLLIMPEXP_GIS_CLT wxGxTextFilter : public wxGxObjectFilter
{
public:
	wxGxTextFilter(wxString soName, wxString soExt);
	virtual ~wxGxTextFilter(void);
	virtual bool CanChooseObject( IGxObject* pObject );
	virtual bool CanDisplayObject( IGxObject* pObject );
	virtual wxString GetName(void);
    virtual wxString GetExt(void);
protected:
    wxString m_soName;
    wxString m_soExt;
};