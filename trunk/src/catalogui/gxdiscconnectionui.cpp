/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#include "wxgis/catalogui/gxdiscconnectionui.h"
#include "wxgis/framework/progressdlg.h"

wxGxDiscConnectionUI::wxGxDiscConnectionUI(CPLString Path, wxString Name, wxIcon SmallIco, wxIcon LargeIco, wxIcon SmallIcoDsbl, wxIcon LargeIcoDsbl) : wxGxDiscConnection(Path, Name)
{
	m_Conn16 = SmallIco;
	m_Conn48 = LargeIco;
	m_ConnDsbld16 = SmallIcoDsbl;
	m_ConnDsbld48 = LargeIcoDsbl;
}

wxGxDiscConnectionUI::~wxGxDiscConnectionUI(void)
{
}

wxIcon wxGxDiscConnectionUI::GetLargeImage(void)
{
    bool bIsOk = wxIsReadable(wxString(m_sPath, wxConvUTF8));
	if(bIsOk)
		return m_Conn48;
	else
		return m_ConnDsbld48;
}

wxIcon wxGxDiscConnectionUI::GetSmallImage(void)
{
    bool bIsOk = wxIsReadable(wxString(m_sPath, wxConvUTF8));
	if(bIsOk)
		return m_Conn16;
	else
		return m_ConnDsbld16;
}

void wxGxDiscConnectionUI::EditProperties(wxWindow *parent)
{
}

wxDragResult wxGxDiscConnectionUI::CanDrop(wxDragResult def)
{
    return def;
}

bool wxGxDiscConnectionUI::Drop(const wxArrayString& filenames, bool bMove)
{
    if(filenames.GetCount() == 0)
        return false;
    char **papszFileList = NULL;
    const char* szPath;
    for(size_t i = 0; i < filenames.GetCount(); ++i)
    {
        //Change to CPLString
        CPLString szFilePath = filenames[i].mb_str(wxConvUTF8);
        if(i == 0)
            szPath = CPLGetPath(szFilePath);
        papszFileList = CSLAddString( papszFileList, szFilePath );        
    }

	GxObjectArray Array;	
    if(!m_pCatalog->GetChildren(szPath, papszFileList, Array))
    {
        CSLDestroy( papszFileList );
        return false;
    }
    CSLDestroy( papszFileList );

    //create progress dialog
    ITrackCancel TrackCancel;
    wxGISProgressDlg ProgressDlg(&TrackCancel, NULL);
    wxWindowDisabler disableAll(&ProgressDlg);
    ProgressDlg.Show(true);
    IProgressor* pProgr1 = ProgressDlg.GetProgressor1();
    pProgr1->SetRange(Array.size());

    TrackCancel.SetProgressor(ProgressDlg.GetProgressor2());

    for(size_t i = 0; i < Array.size(); ++i)
    {
        pProgr1->SetValue(i);
        if(!TrackCancel.Continue())
            break;

        IGxObjectEdit* pGxObjectEdit = dynamic_cast<IGxObjectEdit*>(Array[i]);
        if(pGxObjectEdit)
        {
            if(bMove && pGxObjectEdit->CanMove(m_sPath))
            {
                if(pGxObjectEdit->Move(m_sPath, &TrackCancel))
                {
                    bool ret_code = AddChild(Array[i]);
                    if(!ret_code)
                    {
                        wxDELETE(Array[i]);
                    }
                    else
                    {
                        m_pCatalog->ObjectAdded(Array[i]->GetID());
                    }
                }
            }
            else if(!bMove && pGxObjectEdit->CanCopy(m_sPath))
            {
                if(pGxObjectEdit->Copy(m_sPath, &TrackCancel))
                {
                    bool ret_code = AddChild(Array[i]);
                    if(!ret_code)
                    {
                        wxDELETE(Array[i]);
                    }
                    else
                    {
                        m_pCatalog->ObjectAdded(Array[i]->GetID());
                    }
                }
            }
            else
                return false;
        }
    }
    return true;
}
