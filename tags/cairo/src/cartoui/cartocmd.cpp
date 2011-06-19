/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Carto Main Commands class.
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
#include "wxgis/cartoui/cartocmd.h"
#include "wxgis/display/rubberband.h"
//#include "wxgis/display/simplefillsymbol.h"

#include "../../art/geography16.xpm"
#include "../../art/cursors_16.xpm"

//	0	Full Extent
//	1	Prev Extent
//	2	Next Extent
//	3	?

//--------------------------------------------------
// wxGISCartoMainCmd
//--------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoMainCmd, wxObject)


wxGISCartoMainCmd::wxGISCartoMainCmd(void) : m_pMapView(NULL)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(geography16_xpm));
}

wxGISCartoMainCmd::~wxGISCartoMainCmd(void)
{
}

wxIcon wxGISCartoMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_ImageList.GetIcon(9);
		case 1:
			return m_ImageList.GetIcon(6);
		case 2:
			return m_ImageList.GetIcon(7);
		default:
			return wxNullIcon;
	}
}

wxString wxGISCartoMainCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Full Extent"));
		case 1:
			return wxString(_("&Previous Extent"));
		case 2:
			return wxString(_("&Next Extent"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCartoMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return wxString(_("Geography"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCartoMainCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return false;
		default:
			return false;
	}
}

bool wxGISCartoMainCmd::GetEnabled(void)
{
	if(!m_pMapView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); i++)
			{
				wxGISMapViewEx* pMapView = dynamic_cast<wxGISMapViewEx*>(pWinArr->at(i));
				if(pMapView)
				{
					m_pMapView = pMapView;
					break;
				}
			}
		}
	}
	if(!m_pMapView)
        return false;

	switch(m_subtype)
	{
		case 0:
			return m_pMapView->IsShown();
		case 1:
			return m_pMapView->IsShown() && m_pMapView->CanUndo();
		case 2:
			return m_pMapView->IsShown() && m_pMapView->CanRedo();
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCartoMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Full Extent
		case 1://Previous Extent
		case 2://Next Extent
			return enumGISCommandNormal;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCartoMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Full extent of the map"));
		case 1:
			return wxString(_("Previous extent of the map"));
		case 2:
			return wxString(_("Next extent of the map"));
		default:
			return wxEmptyString;
	}
}

void wxGISCartoMainCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_pMapView->SetFullExtent();
		case 1:
			return m_pMapView->Undo();
		case 2:
			return m_pMapView->Redo();
		default:
			break;
	}
}

bool wxGISCartoMainCmd::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoMainCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Full Extent"));
		case 1:
			return wxString(_("Previous Extent"));
		case 2:
			return wxString(_("Next Extent"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCartoMainCmd::GetCount(void)
{
	return 3;
}

//--------------------------------------------------
// wxGISCartoMainTool
//--------------------------------------------------

//	0	//Zoom In
//	1	//Zoom Out
//	2	//Pan
//	3	//Info
//	4	//?

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoMainTool, wxObject)


wxGISCartoMainTool::wxGISCartoMainTool(void) : m_pMapView(NULL), m_bCheck(false)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(geography16_xpm));
}

wxGISCartoMainTool::~wxGISCartoMainTool(void)
{
}

wxIcon wxGISCartoMainTool::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
			return m_ImageList.GetIcon(5);
		case 1:
			return m_ImageList.GetIcon(4);
		case 2:
			return m_ImageList.GetIcon(1);
		case 3:
			return m_ImageList.GetIcon(10);
		default:
			return wxNullIcon;
	}
}

wxString wxGISCartoMainTool::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Zoom &In"));
		case 1:
			return wxString(_("Zoom &Out"));
		case 2:
			return wxString(_("&Pan"));
		case 3:
			return wxString(_("&Information"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCartoMainTool::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return wxString(_("Geography"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCartoMainTool::GetChecked(void)
{
	return m_bCheck;
}

bool wxGISCartoMainTool::GetEnabled(void)
{
	if(!m_pMapView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); i++)
			{
				wxGISMapViewEx* pMapView = dynamic_cast<wxGISMapViewEx*>(pWinArr->at(i));
				if(pMapView)
				{
					m_pMapView = pMapView;
					break;
				}
			}
		}
	}
	if(!m_pMapView)
        return false;

	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return m_pMapView->IsShown();
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCartoMainTool::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://ZoomIn
		case 1://ZoomOut
		case 2://Pan
		case 3://Information
			return enumGISCommandCheck;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCartoMainTool::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Zoom map in"));
		case 1:
			return wxString(_("Zoom map out"));
		case 2:
			return wxString(_("Pan map"));
		case 3:
			return wxString(_("Feature information"));
		default:
			return wxEmptyString;
	}
}

void wxGISCartoMainTool::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			break;
		default:
			break;
	}
}

bool wxGISCartoMainTool::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoMainTool::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Zoom In"));
		case 1:
			return wxString(_("Zoom Out"));
		case 2:
			return wxString(_("Pan"));
		case 3:
			return wxString(_("Information"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCartoMainTool::GetCount(void)
{
	return 4;
}

wxCursor wxGISCartoMainTool::GetCursor(void)
{
    wxImageList oCursorList(16, 16);
    oCursorList.Add(wxBitmap(cursors_16_xpm));
	switch(m_subtype)
	{
		case 0:	//z_in
		{
			wxImage CursorImage = oCursorList.GetBitmap(0).ConvertToImage();//m_ImageList.GetBitmap(13).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
			return wxCursor(CursorImage);
		}
		case 1:	//z_out
		{
			wxImage CursorImage = oCursorList.GetBitmap(1).ConvertToImage();//m_ImageList.GetBitmap(14).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
			return wxCursor(CursorImage);
		}
		case 2:	//pan
		{
			wxImage CursorImage = oCursorList.GetBitmap(3).ConvertToImage();//m_ImageList.GetBitmap(1).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
			return wxCursor(CursorImage);//wxCURSOR_HAND
		}
		case 3:	//inf
		{
			wxImage CursorImage = oCursorList.GetBitmap(2).ConvertToImage();//m_ImageList.GetBitmap(12).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
			return wxCursor(CursorImage);//*wxSTANDARD_CURSOR
		}
		default:
			return wxNullCursor;
	}
}

void wxGISCartoMainTool::SetChecked(bool bCheck)
{
	m_bCheck = bCheck;
	if(m_bCheck)
		m_pMapView->SetCursor(GetCursor());
}

void wxGISCartoMainTool::OnMouseDown(wxMouseEvent& event)
{
    event.Skip();
	switch(m_subtype)
	{
		case 0:	//z_in
		{
			wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(0, 0, 255), 2), m_pMapView, m_pMapView->GetDisplay());
			OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
			if(IsDoubleEquil(Env.MaxX, Env.MinX) || IsDoubleEquil(Env.MaxY, Env.MinY))
			{
				OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
				double widthdiv4 = (CurrentEnv.MaxX - CurrentEnv.MinX) / 4;
				double heightdiv4 = (CurrentEnv.MaxY - CurrentEnv.MinY) / 4;

				Env.MinX -= widthdiv4;
				Env.MinY -= heightdiv4;
				Env.MaxX += widthdiv4;
				Env.MaxY += heightdiv4;
			}

			if(m_pMapView->GetScaleRatio(Env, wxClientDC(m_pMapView)) > 1.0)
                m_pMapView->Do(Env);
		}
		break;
		case 1:	//z_out
		{
			wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(0, 0, 255), 2), m_pMapView, m_pMapView->GetDisplay());
			OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
			OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
			OGREnvelope NewEnv;
			NewEnv.MinX = CurrentEnv.MinX + CurrentEnv.MinX - Env.MinX;
			NewEnv.MinY = CurrentEnv.MinY + CurrentEnv.MinY - Env.MinY;
			NewEnv.MaxX = CurrentEnv.MaxX + CurrentEnv.MaxX - Env.MaxX;
			NewEnv.MaxY = CurrentEnv.MaxY + CurrentEnv.MaxY - Env.MaxY;
            m_pMapView->Do(NewEnv);
		}
		break;
		case 2:	//pan
		{
		    wxImageList oCursorList(16, 16);
		    oCursorList.Add(wxBitmap(cursors_16_xpm));
			wxImage CursorImage = oCursorList.GetBitmap(4).ConvertToImage();
			//m_ImageList.GetBitmap(11).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
			m_pMapView->SetCursor(wxCursor(CursorImage));
			m_pMapView->PanStart(event.GetPosition());
		}
		break;
		case 3:	//inf
		{
			wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(0, 0, 255), 2), m_pMapView, m_pMapView->GetDisplay());
			OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
			m_pMapView->Refresh(false);
		}
		break;
		default:
			break;
	}
}

void wxGISCartoMainTool::OnMouseUp(wxMouseEvent& event)
{
//    event.Skip();
	switch(m_subtype)
	{
		case 0:	//z_in
			break;
		case 1:	//z_out
			break;
		case 2:	//pan
			m_pMapView->PanStop(event.GetPosition());
			m_pMapView->SetCursor(GetCursor());
			break;
		case 3:	//inf
			break;
		default:
			break;
	}
}

void wxGISCartoMainTool::OnMouseMove(wxMouseEvent& event)
{
    //event.Skip();
	switch(m_subtype)
	{
		case 0:	//z_in
			break;
		case 1:	//z_out
			break;
		case 2:	//pan
			if(event.Dragging())
				m_pMapView->PanMoveTo(event.GetPosition());
			break;
		case 3:	//inf
			break;
		default:
			break;
	}
}

void wxGISCartoMainTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

//--------------------------------------------------
// wxGISCartoFrameTool
//--------------------------------------------------

//	0	//Rotate
//	1	//Cancel Rotate
//	2	//Input Rotate Angle
//	3	//?

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoFrameTool, wxObject)


wxGISCartoFrameTool::wxGISCartoFrameTool(void) : m_pMapView(NULL), m_bCheck(false)
{
	//m_ImageList.Create(16, 16);
	//m_ImageList.Add(wxBitmap(geography16_xpm));
}

wxGISCartoFrameTool::~wxGISCartoFrameTool(void)
{
}

wxIcon wxGISCartoFrameTool::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		default:
			return wxNullIcon;
	}
}

wxString wxGISCartoFrameTool::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("&Rotate"));
		case 1:
			return wxString(_("Cancel rotate"));
		case 2:
			return wxString(_("Input rotate angle"));
		default:
			return wxEmptyString;
	}
}

wxString wxGISCartoFrameTool::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return wxString(_("View"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCartoFrameTool::GetChecked(void)
{
	return m_bCheck;
}

bool wxGISCartoFrameTool::GetEnabled(void)
{
	if(!m_pMapView)
	{
		const WINDOWARRAY* pWinArr = m_pApp->GetChildWindows();
		if(pWinArr)
		{
			for(size_t i = 0; i < pWinArr->size(); i++)
			{
				wxGISMapViewEx* pMapView = dynamic_cast<wxGISMapViewEx*>(pWinArr->at(i));
				if(pMapView)
				{
					m_pMapView = pMapView;
					break;
				}
			}
		}
	}
	if(!m_pMapView)
        return false;

	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
			return m_pMapView->IsShown();
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCartoFrameTool::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Rotate
			return enumGISCommandCheck;
		case 1://Cancel rotate
			return enumGISCommandNormal;
    	case 2://Input rotate
			return enumGISCommandControl;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCartoFrameTool::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Rotate map frame"));
		case 1:
			return wxString(_("Cancel rotate map frame"));
		case 2:
			return wxString(_("Input map frame rotate angle"));
		default:
			return wxEmptyString;
	}
}

void wxGISCartoFrameTool::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
	}
}

bool wxGISCartoFrameTool::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoFrameTool::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Rotate map frame"));
		case 1:
			return wxString(_("Cancel rotate map frame"));
		case 2:
			return wxString(_("Input map frame rotate angle"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCartoFrameTool::GetCount(void)
{
	return 3;
}

wxCursor wxGISCartoFrameTool::GetCursor(void)
{
 //   wxImageList oCursorList(16, 16);
 //   oCursorList.Add(wxBitmap(cursors_16_xpm));
	//switch(m_subtype)
	//{
	//	case 0:	//z_in
	//	{
	//		wxImage CursorImage = oCursorList.GetBitmap(0).ConvertToImage();//m_ImageList.GetBitmap(13).ConvertToImage();
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
	//		return wxCursor(CursorImage);
	//	}
	//	case 1:	//z_out
	//	{
	//		wxImage CursorImage = oCursorList.GetBitmap(1).ConvertToImage();//m_ImageList.GetBitmap(14).ConvertToImage();
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
	//		return wxCursor(CursorImage);
	//	}
	//	case 2:	//pan
	//	{
	//		wxImage CursorImage = oCursorList.GetBitmap(3).ConvertToImage();//m_ImageList.GetBitmap(1).ConvertToImage();
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
	//		return wxCursor(CursorImage);//wxCURSOR_HAND
	//	}
	//	default:
	//		return wxNullCursor;
	//}
}

void wxGISCartoFrameTool::SetChecked(bool bCheck)
{
	m_bCheck = bCheck;
	if(m_bCheck)
		m_pMapView->SetCursor(GetCursor());
}

void wxGISCartoFrameTool::OnMouseDown(wxMouseEvent& event)
{
    event.Skip();
	//switch(m_subtype)
	//{
	//	case 0:	//z_in
	//	{
	//		wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(0, 0, 255), 2), m_pMapView, m_pMapView->GetDisplay());
	//		OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
	//		if(IsDoubleEquil(Env.MaxX, Env.MinX) || IsDoubleEquil(Env.MaxY, Env.MinY))
	//		{
	//			OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
	//			double widthdiv4 = (CurrentEnv.MaxX - CurrentEnv.MinX) / 4;
	//			double heightdiv4 = (CurrentEnv.MaxY - CurrentEnv.MinY) / 4;

	//			Env.MinX -= widthdiv4;
	//			Env.MinY -= heightdiv4;
	//			Env.MaxX += widthdiv4;
	//			Env.MaxY += heightdiv4;
	//		}

	//		if(m_pMapView->GetScaleRatio(Env, wxClientDC(m_pMapView)) > 1.0)
 //               m_pMapView->Do(Env);
	//	}
	//	break;
	//	case 1:	//z_out
	//	{
	//		wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(0, 0, 255), 2), m_pMapView, m_pMapView->GetDisplay());
	//		OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
	//		OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
	//		OGREnvelope NewEnv;
	//		NewEnv.MinX = CurrentEnv.MinX + CurrentEnv.MinX - Env.MinX;
	//		NewEnv.MinY = CurrentEnv.MinY + CurrentEnv.MinY - Env.MinY;
	//		NewEnv.MaxX = CurrentEnv.MaxX + CurrentEnv.MaxX - Env.MaxX;
	//		NewEnv.MaxY = CurrentEnv.MaxY + CurrentEnv.MaxY - Env.MaxY;
 //           m_pMapView->Do(NewEnv);
	//	}
	//	break;
	//	case 2:	//pan
	//	{
	//	    wxImageList oCursorList(16, 16);
	//	    oCursorList.Add(wxBitmap(cursors_16_xpm));
	//		wxImage CursorImage = oCursorList.GetBitmap(4).ConvertToImage();
	//		//m_ImageList.GetBitmap(11).ConvertToImage();
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
	//		CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
	//		m_pMapView->SetCursor(wxCursor(CursorImage));
	//		m_pMapView->PanStart(event.GetPosition());
	//	}
	//	break;
	//	case 3:	//inf
	//	{
	//		wxGISRubberEnvelope RubberEnvelope(wxPen(wxColour(0, 0, 255), 2), m_pMapView, m_pMapView->GetDisplay());
	//		OGREnvelope Env = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
	//		m_pMapView->Refresh(false);
	//	}
	//	break;
	//	default:
	//		break;
	//}
}

void wxGISCartoFrameTool::OnMouseUp(wxMouseEvent& event)
{
////    event.Skip();
	//switch(m_subtype)
	//{
	//	case 0:	//z_in
	//		break;
	//	case 1:	//z_out
	//		break;
	//	case 2:	//pan
	//		m_pMapView->PanStop(event.GetPosition());
	//		m_pMapView->SetCursor(GetCursor());
	//		break;
	//	case 3:	//inf
	//		break;
	//	default:
	//		break;
	//}
}

void wxGISCartoFrameTool::OnMouseMove(wxMouseEvent& event)
{
 //   //event.Skip();
	//switch(m_subtype)
	//{
	//	case 0:	//z_in
	//		break;
	//	case 1:	//z_out
	//		break;
	//	case 2:	//pan
	//		if(event.Dragging())
	//			m_pMapView->PanMoveTo(event.GetPosition());
	//		break;
	//	case 3:	//inf
	//		break;
	//	default:
	//		break;
	//}
}

void wxGISCartoFrameTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

IToolBarControl* wxGISCartoFrameTool::GetControl(void)
{
	switch(m_subtype)
	{
		case 2:	
			////if(!m_pGxLocationComboBox)
			//{
			//	wxArrayString PathArray;
			//	wxGxLocationComboBox* pGxLocationComboBox = new wxGxLocationComboBox(dynamic_cast<wxWindow*>(m_pApp), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 400, 22 ), PathArray);
			//	return static_cast<IToolBarControl*>(pGxLocationComboBox);
			//}
		default:
			return NULL;
	}
}

wxString wxGISCartoFrameTool::GetToolLabel(void)
{
	switch(m_subtype)
	{
		case 2:	
			return wxEmptyString;
		default:
			return wxEmptyString;
	}
}

bool wxGISCartoFrameTool::HasToolLabel(void)
{
	switch(m_subtype)
	{
		case 2:	
			return false;
		default:
			return false;
	}
}
