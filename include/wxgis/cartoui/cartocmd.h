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
#include "wxgis/cartoui/mapview.h"

/** \class wxGISRotationComboBox cartocmd.h
    \brief The frame angle rotation input combobox.
*/
class wxGISRotationComboBox : 
	public wxComboBox,
	public IToolBarControl
{
public:
	wxGISRotationComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("LocationComboBox"));
	virtual ~wxGISRotationComboBox(void);
	virtual void UpdateAngle(void);
	virtual void SetMapView(wxGISMapView* pMapView){m_pMapView = pMapView;};
	//events
	void OnTextEnter(wxCommandEvent& event);
	//IToolBarControl
	virtual void Activate(IFrameApplication* pApp);
	virtual void Deactivate(void);
protected:
	wxGISMapView* m_pMapView;

    DECLARE_EVENT_TABLE()
};

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
	virtual bool OnCreate(IFrameApplication* pApp);
	virtual wxString GetTooltip(void);
	virtual unsigned char GetCount(void);
private:
	IFrameApplication* m_pApp;
	wxIcon m_IconFullExt, m_IconNextExt, m_IconPrevExt;
	wxGISMapView* m_pMapView;
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
	virtual bool OnCreate(IFrameApplication* pApp);
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
	IFrameApplication* m_pApp;
	wxIcon m_IconZoomIn, m_IconZoomOut, m_IconPan, m_IconInfo;
	wxCursor m_CurZoomIn, m_CurZoomOut, m_CurPan1, m_CurPan2, m_CurInfo;
	wxGISMapView* m_pMapView;
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
	virtual bool OnCreate(IFrameApplication* pApp);
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
	IFrameApplication* m_pApp;
	wxIcon m_IconRotate, m_IconCancelRotate;
	wxCursor m_CurRotate;
	wxGISMapView* m_pMapView;
	wxGISRotationComboBox* m_pRotationComboBox;
	bool m_bCheck;
};

