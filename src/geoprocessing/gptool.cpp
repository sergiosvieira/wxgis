/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  base geoprocessing tool.
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

#include "wxgis/geoprocessing/gptool.h"

#include "wx/tokenzr.h"

int CPL_STDCALL ExecToolProgress( double dfComplete, const char *pszMessage, void *pData)
{
    bool bCancel = false;
    ITrackCancel* pTrackCancel = (ITrackCancel*)pData;

    if(pTrackCancel)
    {
        if( pszMessage )
        {
            wxString soMsg(wgMB2WX(pszMessage));
            if(!soMsg.IsEmpty())
                pTrackCancel->PutMessage( wgMB2WX(pszMessage), -1, enumGISMessageNorm );
        }
        IProgressor* pRogress = pTrackCancel->GetProgressor();
        if( pRogress )
            pRogress->SetValue((int) (dfComplete * 100));
        bCancel = !pTrackCancel->Continue();
    }
    return !bCancel;
}


wxGISGPTool::wxGISGPTool(void) : m_pCatalog(NULL)
{
}

wxGISGPTool::~wxGISGPTool(void)
{
    for(size_t i = 0; i < m_pParamArr.size(); i++)
        wxDELETE(m_pParamArr[i]);
}

void wxGISGPTool::SetCatalog(IGxCatalog* pCatalog)
{
    m_pCatalog = pCatalog;
}

IGxCatalog* wxGISGPTool::GetCatalog(void)
{
    return m_pCatalog;
}

wxString wxGISGPTool::GetAsString(void)
{
    wxString sOutParam;
    for(size_t i = 0; i < m_pParamArr.size(); i++)
    {
        IGPParameter* pParam = m_pParamArr[i];
        if(pParam)
        {
            sOutParam += pParam->GetAsString();
            sOutParam += GPTOOLSEPARATOR;
        }
    }
    return sOutParam;
}

bool wxGISGPTool::SetFromString(wxString sParams)
{
    GetParameterInfo();
	wxStringTokenizer tkz(sParams, wxString(GPTOOLSEPARATOR), wxTOKEN_RET_EMPTY );
    size_t counter(0);
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		//token.Replace(GPTOOLSEPARATOR, wxT(""));
        if(counter >= m_pParamArr.size())
            return false;
        IGPParameter* pParam = m_pParamArr[counter];
        if(pParam)
        {
            if(!pParam->SetFromString(token))
                return false;
            else
                pParam->SetAltered(true);
        }
        counter++;
	}
    return true;
}
