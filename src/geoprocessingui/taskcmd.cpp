/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  Task Commands class.
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

#include "wxgis/geoprocessingui/taskcmd.h"
#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/state.xpm"

//	0	Show task config dialog
//  1   Show task execution dialog
//  2   Start
//  3   Stop
//  4   Show exec process
//  5   ?

IMPLEMENT_DYNAMIC_CLASS(wxGISTaskCmd, wxObject)

wxGISTaskCmd::wxGISTaskCmd(void)
{
    m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_xpm));
}

wxGISTaskCmd::~wxGISTaskCmd(void)
{
}

wxIcon wxGISTaskCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case 2:
            return m_ImageList.GetIcon(4);
		case 3:
            return m_ImageList.GetIcon(7);
		case 1:
            return m_ImageList.GetIcon(0);
		case 0:
		default:
			return wxNullIcon;
	}
}

wxString wxGISTaskCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show tool dialog"));
		case 1:
			return wxString(_("Show execution dialog"));
		case 2:
			return wxString(_("Start"));
		case 3:
			return wxString(_("Stop"));
		default:
		return wxEmptyString;
	}
}

wxString wxGISTaskCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return wxString(_("Geoprocessing"));
		default:
			return wxString(_("[No category]"));
	}
}

bool wxGISTaskCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case 1:
		case 0:
		case 2:
		case 3:
		default:
	        return false;
	}
	return false;
}

bool wxGISTaskCmd::GetEnabled(void)
{
    switch(m_subtype)
	{
		case 0: //Show task config dialog
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pCatalog->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject.get());
                    if(pGxTask && pGxTask->GetState() != enumGISTaskWork)
                        return true;
                }
			}
			return false;
        }
		case 1: //Show task execution dialog
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pCatalog->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject.get());
                    if(pGxTask && (pGxTask->GetState() == enumGISTaskWork || pGxTask->GetState() == enumGISTaskDone || pGxTask->GetState() == enumGISTaskError))
                        return true;
                }
			}
			return false;
        }
		case 2: //Start task
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pCatalog->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {                                        
                    IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject.get());
                    if(pGxTask && (pGxTask->GetState() == enumGISTaskPaused))
                        return true;
                }
			}
			return false;
        }
		case 3: //Stop task
		{
			IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
			if(pGxApp)
			{
                wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
				IGxSelection* pSel = pCatalog->GetSelection();
                for(size_t i = 0; i < pSel->GetCount(); i++)
                {
                    IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
                    IGxTask* pGxTask = dynamic_cast<IGxTask*>(pGxObject.get());
                    if(pGxTask && (pGxTask->GetState() == enumGISTaskWork))
                        return true;
                }
			}
			return false;
        }
		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISTaskCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISTaskCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show tool config dialog"));
		case 1:
			return wxString(_("Show task execution dialog"));
		case 2:
			return wxString(_("Start task"));
		case 3:
			return wxString(_("Stop task"));
		default:
			return wxEmptyString;
	}
}

void wxGISTaskCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case 0:
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pCatalog->GetSelection();
					for(size_t i = 0; i < pSel->GetCount(); i++)
					{
                        IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
						wxGxTaskObject* pGxTask = dynamic_cast<wxGxTaskObject*>(pGxObject.get());
						if(pGxTask)
							pGxTask->ShowToolConfig(dynamic_cast<wxWindow*>(m_pApp));
					}
				}
			}
			break;
		case 1:
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pCatalog->GetSelection();
					for(size_t i = 0; i < pSel->GetCount(); i++)
					{
                        IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
						wxGxTaskObject* pGxTask = dynamic_cast<wxGxTaskObject*>(pGxObject.get());
						if(pGxTask)
							pGxTask->ShowProcess(dynamic_cast<wxWindow*>(m_pApp));
					}
				}
			}
			break;
		case 2:
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pCatalog->GetSelection();
					for(size_t i = 0; i < pSel->GetCount(); i++)
					{
                        IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
						wxGxTaskObject* pGxTask = dynamic_cast<wxGxTaskObject*>(pGxObject.get());
						if(pGxTask)
							pGxTask->StartTask();
					}
				}
			}
			break;
		case 3:
			{
				IGxApplication* pGxApp = dynamic_cast<IGxApplication*>(m_pApp);
				if(pGxApp)
				{
					wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(pGxApp->GetCatalog());
					IGxSelection* pSel = pCatalog->GetSelection();
					for(size_t i = 0; i < pSel->GetCount(); i++)
					{
                        IGxObjectSPtr pGxObject = pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
						wxGxTaskObject* pGxTask = dynamic_cast<wxGxTaskObject*>(pGxObject.get());
						if(pGxTask)
							pGxTask->StopTask();
					}
				}
			}
			break;
		default:
			return;
	}
}

bool wxGISTaskCmd::OnCreate(IApplication* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISTaskCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case 0:
			return wxString(_("Show tool config dialog"));
		case 1:
			return wxString(_("Show task execution dialog"));
		case 2:
			return wxString(_("Start task"));
		case 3:
			return wxString(_("Stop task"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISTaskCmd::GetCount(void)
{
	return 4;
}