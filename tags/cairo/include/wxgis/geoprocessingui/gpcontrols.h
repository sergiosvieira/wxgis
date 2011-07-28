/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  controls classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/cartoui/tableview.h"

#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/grid.h>

#include  "wx/imaglist.h"

/** \class wxGISDTBase gpcontrols.h
    \brief The base class for tool dialog controls.
*/
class wxGISDTBase : public wxPanel
{
public:
	wxGISDTBase( IGPParameter* pParam, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
    virtual ~wxGISDTBase();
    virtual void SetMessage(wxGISEnumGPMessageType nType = wxGISEnumGPMessageUnknown, wxString sMsg = wxEmptyString);
    virtual bool Validate(void) = 0;
    virtual void UpdateValues(void) = 0;
    virtual void UpdateControls(void) = 0;
    virtual IGPParameter* GetParameter(void);
protected:
	wxStaticBitmap* m_StateBitmap;
	wxStaticText* m_sParamDisplayName;
	wxString m_sFullDisplayName;
	wxStaticBitmap* m_bitmap;
    IGPParameter* m_pParam;
	wxImageList m_ImageList;
	wxGISEnumGPMessageType m_nCurrentType;
	wxString m_sCurrentMsg;
};

/** \class wxGISDTPath gpcontrols.h
    \brief The tool dialog control for catalog path value representation.
*/
class wxGISDTPath : public wxGISDTBase
{
    enum
	{
		ID_PATHCTRL = wxID_HIGHEST + 3603
	};
public:
	wxGISDTPath( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTPath();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
    virtual void OnOpen(wxCommandEvent& event);
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
	virtual void OnPathChange(wxCommandEvent& event);
protected:
    wxTextCtrl* m_PathTextCtrl;
	wxBitmapButton* m_bpButton;
    IGxCatalog* m_pCatalog;

    DECLARE_EVENT_TABLE()
};

/** \class wxGISDTDigit gpcontrols.h
    \brief The tool dialog control for digit value representation.
*/
class wxGISDTDigit : public wxGISDTBase
{
    enum
	{
		ID_DIGITCTRL = wxID_HIGHEST + 3604
	};
public:
	wxGISDTDigit( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTDigit();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
	virtual void OnDigitChange(wxCommandEvent& event);
protected:
    wxTextCtrl* m_DigitTextCtrl;
    IGxCatalog* m_pCatalog;

	DECLARE_EVENT_TABLE()
};

/** \class wxGISDTChoice gpcontrols.h
    \brief The tool dialog control for choice value representation.
*/
class wxGISDTChoice : public wxGISDTBase
{
    enum
	{
		ID_CHOICESTR = wxID_HIGHEST + 3605
	};
public:
	wxGISDTChoice( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	~wxGISDTChoice();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
    virtual void OnChoice(wxCommandEvent& event);
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
protected:
	wxChoice* m_choice;
DECLARE_EVENT_TABLE()
};

/** \class wxGISDTBool gpcontrols.h
    \brief The tool dialog control for bool value representation.
*/
class wxGISDTBool : public wxGISDTBase
{
    enum
	{
		ID_CHECKBOOL = wxID_HIGHEST + 3606
	};
public:
	wxGISDTBool( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTBool();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
    virtual void OnClick( wxCommandEvent& event );
    virtual void OnSize( wxSizeEvent& event );
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
protected:
    wxCheckBox* m_pCheckBox;
    IGxCatalog* m_pCatalog;
	wxString m_sFullText;

DECLARE_EVENT_TABLE()
};

/** \class wxGISDTSpatRef gpcontrols.h
    \brief The tool dialog control for spatial reference value representation.
*/
class wxGISDTSpatRef : public wxGISDTBase
{
    enum
	{
		ID_SPATREFSTR = wxID_HIGHEST + 3607
	};
public:
	wxGISDTSpatRef( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTSpatRef();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
    virtual void OnOpen(wxCommandEvent& event);
	virtual void OnTextChange(wxCommandEvent& event);
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
protected:
    wxTextCtrl* m_SpaRefTextCtrl;
	wxBitmapButton* m_bpButton;
    IGxCatalog* m_pCatalog;

    DECLARE_EVENT_TABLE()
};

/** \class wxGISDTMultiParam gpcontrols.h
    \brief The tool dialog control for multiple parameters.

	The multiple parameters stores in grid.
*/
class wxGISDTMultiParam : public wxGISDTBase
{
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 3607
	};
public:
	wxGISDTMultiParam( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTMultiParam();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
	virtual void OnCellChange(wxGridEvent &event);
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
protected:
	wxGrid* m_pg;
    IGxCatalog* m_pCatalog;

    DECLARE_EVENT_TABLE()
};

/** \class wxGISDTList gpcontrols.h
    \brief The tool dialog control the list of values.
*/
class wxGISDTList : public wxGISDTBase
{
	enum
	{
		ID_LISTCTRL = wxID_HIGHEST + 3608
	};
public:
	wxGISDTList( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISDTList();
	virtual wxString GetValue();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
    virtual void OnAdd(wxCommandEvent& event);
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
	virtual void OnTextChange(wxCommandEvent& event);
protected:
    wxTextCtrl* m_TextCtrl;
	wxBitmapButton* m_bpButton;
    IGxCatalog* m_pCatalog;

    DECLARE_EVENT_TABLE()
};

/** \class wxGISSQLQueryCtrl gpcontrols.h
    \brief The tool dialog control SQL query representation.
*/
class wxGISSQLQueryCtrl : public wxGISDTBase
{
	enum
	{
		ID_LISTCTRL = wxID_HIGHEST + 3608
	};
public:
	wxGISSQLQueryCtrl( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISSQLQueryCtrl();
	//wxGISDTBase
    virtual bool Validate(void);
    virtual void UpdateValues(void);
    virtual void UpdateControls(void);
    //events
    virtual void OnOpen(wxCommandEvent& event);
	virtual void OnUpdateUI(wxUpdateUIEvent &event);
protected:
    wxTextCtrl* m_QueryTextCtrl;
	wxBitmapButton* m_bpButton;
    IGxCatalog* m_pCatalog;

    DECLARE_EVENT_TABLE()
};
