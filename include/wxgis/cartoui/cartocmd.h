/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Carto Main Commands & tools class.
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
#include "wxgis/cartoui/mapviewex.h"

/** \class wxGISCartoMainCmd cartocmd.h
    \brief The carto main commands.
*/
class wxGISCartoMainCmd :
	public ICommand
{
	DECLARE_DYNAMIC_CLASS(wxGISCartoMainCmd)
public:
	wxGISCartoMainCmd(void);
	virtual ~wxGISCartoMainCmd(void);
	//ICommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(IApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
private:
	IApplication* m_pApp;
	wxImageList m_ImageList;
	wxGISMapViewEx* m_pMapView;
};

/** \class wxGISCartoMainTool cartocmd.h
    \brief The carto main tool.
*/
class wxGISCartoMainTool :
	public ITool
{
	DECLARE_DYNAMIC_CLASS(wxGISCartoMainTool)
public:
	wxGISCartoMainTool(void);
	virtual ~wxGISCartoMainTool(void);
	//ICommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(IApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
	//ITool
	virtual wxCursor GetCursor(void);
	virtual void SetChecked(bool bCheck);
	virtual void OnMouseDown(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
	virtual void OnMouseDoubleClick(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
private:
	IApplication* m_pApp;
	wxImageList m_ImageList;
	wxGISMapViewEx* m_pMapView;
	bool m_bCheck;
};

/** \class wxGISCartoFrameTool cartocmd.h
    \brief The carto frame tools and commands.
*/
class wxGISCartoFrameTool :
	public ITool,
	public IToolControl
{
	DECLARE_DYNAMIC_CLASS(wxGISCartoFrameTool)
public:
	wxGISCartoFrameTool(void);
	virtual ~wxGISCartoFrameTool(void);
	//ICommand
	virtual wxIcon GetBitmap(void);
	virtual wxString GetCaption(void);
	virtual wxString GetCategory(void);
	virtual bool GetChecked(void);
	virtual bool GetEnabled(void);
	virtual wxString GetMessage(void);
	virtual wxGISEnumCommandKind GetKind(void);
	virtual void OnClick(void);
	virtual bool OnCreate(IApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
	//ITool
	virtual wxCursor GetCursor(void);
	virtual void SetChecked(bool bCheck);
	virtual void OnMouseDown(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
	virtual void OnMouseDoubleClick(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
	//IToolControl
	virtual IToolBarControl* GetControl(void);
	virtual wxString GetToolLabel(void);
	virtual bool HasToolLabel(void);
private:
	IApplication* m_pApp;
	//wxImageList m_ImageList;
	wxGISMapViewEx* m_pMapView;
	bool m_bCheck;
};