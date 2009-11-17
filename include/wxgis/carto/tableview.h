/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
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

#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#include "wxgis/carto/carto.h"
#include "wxgis/carto/featuredataset.h"

//------------------------------------------------------------------
// wxGISTable
//------------------------------------------------------------------

class WXDLLIMPEXP_GIS_CRT wxGISTable : 
	public wxGridTableBase
{
public:
    wxGISTable(wxGISDataset* pwxGISDataset);
    ~wxGISTable();

	//overrides
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell(int row, int col) { return false; }
    virtual wxString GetValue(int row, int col);
    virtual void SetValue(int row, int col, const wxString &value);
    virtual wxString GetColLabelValue(int col);
	virtual wxString GetRowLabelValue(int row);

    //int GetNumberStoredRows();
    //wxGridCellAttr* GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

    //bool CanGetValueAs(int row, int col, const wxString& typeName);
    //bool CanSetValueAs(int row, int col, const wxString& typeName);
    //
    //bool GetValueAsBool(int row, int col);
    //void SetValueAsBool(int row, int col, bool b);

    //bool needsResizing(int col) { return columns[col].needResize; }
    //bool AppendRows(size_t rows);
    //bool DeleteRows(size_t pos, size_t rows);
    //int  LastRow() { return lastRow; }

    //bool CheckInCache(int row);

    //void StoreLine();
    //void UndoLine(int row);

private:
	wxGISFeatureDataset* m_pwxGISDataset;
	OGRFeatureDefn* m_pOGRFeatureDefn;
	wxString m_sFIDKeyName;
	int nCols;          // columns from dataSet
    int nRows;          // rows initially returned by dataSet


    //pgQueryThread *thread;
    //pgConn *connection;
    //bool hasOids;
    //char relkind;
    //wxString tableName;
    //Oid relid;
    //wxString primaryKeyColNumbers;

    //cacheLine *GetLine(int row);
    //wxString MakeKey(cacheLine *line);
    //void SetNumberEditor(int col, int len);

    //cacheLinePool *dataPool, *addPool;
    //cacheLine savedLine;
    //int lastRow;

    //int *lineIndex;     // reindex of lines in dataSet to handle deleted rows

    //int rowsCached;     // rows read from dataset; if nRows=rowsCached, dataSet can be deleted
    //int rowsAdded;      // rows added (never been in dataSet)
    //int rowsStored;     // rows added and stored to db
    //int rowsDeleted;    // rows deleted from initial dataSet
    //sqlCellAttr *columns;

    //friend class ctlSQLGrid;
};

//------------------------------------------------------------------
// wxGridCtrl
//------------------------------------------------------------------

class wxGridCtrl: 
	public wxGrid 
{
	DECLARE_DYNAMIC_CLASS(wxGridCtrl)
protected:
	virtual void DrawRowLabel(wxDC& dc, int row);
    virtual void OnLabelLeftClick(wxGridEvent& event);
    virtual void OnSelectCell(wxGridEvent& event);
public:
	wxGridCtrl();
	virtual ~wxGridCtrl(void);
	wxGridCtrl(wxWindow* parent, const long& id);
	wxImageList* m_pImageList;

    DECLARE_EVENT_TABLE();
};



class WXDLLIMPEXP_GIS_CRT wxGISTableView : 
	public wxPanel
{
enum
{
    ID_FIRST = wxID_HIGHEST + 40,
    ID_PREV,
    ID_NEXT,
    ID_LAST,
	ID_POS
};
public:
	wxGISTableView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSTATIC_BORDER|wxTAB_TRAVERSAL);
	virtual ~wxGISTableView(void);
	virtual void SetTable(wxGridTableBase* table, bool takeOwnership = false, wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells)
	{		
		if(m_grid)
		{
			//m_grid->ClearSelection();
			//m_grid->ClearGrid();
			m_grid->SetTable(table, takeOwnership, selmode);
			m_grid->SetGridCursor(0,0);
			m_grid->MakeCellVisible(0,0);
			m_position->Clear();
			(*m_position) << 1;

			m_staticText2->SetLabel(wxString::Format(_("of %u"), m_grid->GetNumberRows()));

			//m_grid->Scroll(0,0);
			//m_grid->SelectRow(0, false);
			//m_grid->Refresh();
		}
	}
	virtual void SetReadOnly(bool bIsReadOnly){if(m_grid) m_grid->EnableEditing( bIsReadOnly );};
	void OnLabelLeftClick(wxGridEvent& event);
	void OnSelectCell(wxGridEvent& event);
	void OnBtnFirst(wxCommandEvent& event);
	void OnBtnNext(wxCommandEvent& event);
	void OnBtnPrev(wxCommandEvent& event);
	void OnBtnLast(wxCommandEvent& event);
	void OnSetPos(wxCommandEvent& event);
private:
	wxGridCtrl* m_grid;
	wxStaticText* m_staticText1, *m_staticText2;
	wxBitmapButton* m_bpFirst;
	wxBitmapButton* m_bpPrev;
	wxTextCtrl* m_position;
	wxBitmapButton* m_bpNext;
	wxBitmapButton* m_bpLast;

DECLARE_EVENT_TABLE()
};
