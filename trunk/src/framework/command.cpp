/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommonCmd class.
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
#include "wxgis/framework/command.h"
#include "../../art/mainframe.xpm"

//	0	Exit
//	1	About
//	2	Customize
//	3	Separator
//	4	StatusBar
//	5	?

IMPLEMENT_DYNAMIC_CLASS(wxGISCommonCmd, wxObject)


wxGISCommonCmd::wxGISCommonCmd(void) : ICommand()
{
}

wxGISCommonCmd::~wxGISCommonCmd(void)
{
}

wxIcon wxGISCommonCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 1:
			return wxIcon(mainframe_xpm);
		case 0:	
		case 2:	
		case 3:	
		case 4:	
		default:
			return wxIcon();
	}
}

wxString wxGISCommonCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("E&xit"));
		case 1:	
			return wxString(_("&About..."));
		case 2:	
			return wxString(_("Customize..."));
		case 3:	
			return wxString(_("Separator"));
		case 4:	
			return wxString(_("StatusBar"));
		default:
			return wxString();
	}
}

wxString wxGISCommonCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("File"));
		case 1:	
			return wxString(_("Help"));
		case 2:	
		case 4:
			return wxString(_("View"));
		case 3:	
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISCommonCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 0:	
		case 1:	
		case 2:	
		case 3:	
		default:
			return false;
		case 4:
			return m_pApp->IsStatusBarShown();
	}
	return false;
}

bool wxGISCommonCmd::GetEnabled(void)
{
	switch(m_subtype)
	{
		case 0://Exit
			return true;
		case 1://About
			return true;
		case 2://Customize
			return true;
		case 3://Separator
			return true;
		case 4://StatusBar
			return true;
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCommonCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0://Exit
			return enumGISCommandNormal;
		case 1://About
			return enumGISCommandNormal;
		case 2://Customize
			return enumGISCommandNormal;
		case 3://Separator
			return enumGISCommandSeparator;
		case 4://StatusBar
			return enumGISCommandCheck;
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCommonCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Exit application"));
		case 1:	
			return wxString(_("About application"));
		case 2:	
			return wxString(_("Customize application"));
		case 3:	
			return wxString(_("Separator"));
		case 4:	
			return wxString(_("Show/hide statusbar"));
		default:
			return wxString();
	}
}

void wxGISCommonCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:	
		{
			wxFrame* pFrame = dynamic_cast<wxFrame*>(m_pApp);
			if(pFrame)
				pFrame->Close();
			break;
		}
		case 1:
			m_pApp->OnAppAbout();
			break;
		case 2:
			m_pApp->Customize();
			break;
		case 4:
			m_pApp->ShowStatusBar(!m_pApp->IsStatusBarShown());
			break;
		case 3:
		default:
			return;
	}
}

bool wxGISCommonCmd::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCommonCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:	
			return wxString(_("Exit"));
		case 1:	
			return wxString(_("About"));
		case 2:	
			return wxString(_("Customize"));
		case 4:	
			return wxString(_("Show/hide statusbar"));
		case 3:	
		default:
			return wxString();
	}
}

unsigned char wxGISCommonCmd::GetCount(void)
{
	return 5;
}
