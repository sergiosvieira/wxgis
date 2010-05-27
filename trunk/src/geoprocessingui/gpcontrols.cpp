/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  controls classes.
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
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/geoprocessingui/gpcontrols.h"
#include "wx/dnd.h"

#include "../../art/state_16.xpm"
#include "../../art/open_16.xpm"
#include "../../art/sql_16.xpm"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTBase
///////////////////////////////////////////////////////////////////////////////
wxGISDTBase::wxGISDTBase( IGPParameter* pParam, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    m_pParam = pParam;

	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_16_xpm));
}

wxGISDTBase::~wxGISDTBase()
{
}

IGPParameter* wxGISDTBase::GetParameter(void)
{
    return m_pParam;
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISTextCtrl
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISTextCtrl, wxTextCtrl)
    EVT_KILL_FOCUS(wxGISTextCtrl::OnKillFocus)
END_EVENT_TABLE()

wxGISTextCtrl::wxGISTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) : wxTextCtrl(parent, id, value, pos, size, style, validator, name)
{
    m_pBaseCtrl = dynamic_cast<wxGISDTBase*>(parent);
}

wxGISTextCtrl::~wxGISTextCtrl(void)
{
}

void wxGISTextCtrl::OnKillFocus(wxFocusEvent& event)
{
    event.Skip();
    IGPParameter* pParam = m_pBaseCtrl->GetParameter();
    pParam->SetValue(wxVariant(GetValue(), wxT("path")));
    pParam->SetAltered(true);
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTPath
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTPath, wxPanel)
	EVT_BUTTON(wxID_OPEN, wxGISDTPath::OnOpen)
END_EVENT_TABLE()

wxGISDTPath::wxGISDTPath( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
{
    m_pCatalog = pCatalog;
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
    m_StateBitmap = new wxStaticBitmap( this, wxID_ANY, m_pParam->GetParameterType() == enumGISGPParameterTypeRequired ? m_ImageList.GetIcon(4) : wxNullBitmap , wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_StateBitmap, 0, wxALL, 5 );
	
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + _(" (optional)") : m_pParam->GetDisplayName(), wxDefaultPosition, wxDefaultSize, 0 );
	m_sParamDisplayName->Wrap( -1 );
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );
	
	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );
	
	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_PathTextCtrl = new wxGISTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_PathTextCtrl, 1, wxALL|wxEXPAND, 5 );     
	
	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_16_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	//m_bpButton = new wxBitmapButton( this, wxID_ANY, wxBitmap(sql_16_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTPath::~wxGISDTPath()
{
}

void wxGISDTPath::SetMessage(wxGISEnumGPMessageType nType, wxString sMsg)
{
    switch(nType)
    {
    case wxGISEnumGPMessageInformation:
        m_StateBitmap->SetBitmap(m_ImageList.GetIcon(0));
        break;
    case wxGISEnumGPMessageError:
        m_StateBitmap->SetBitmap(m_ImageList.GetIcon(2));
        break;
    case wxGISEnumGPMessageWarning:
        m_StateBitmap->SetBitmap(m_ImageList.GetIcon(3));
        break;
    case wxGISEnumGPMessageRequired:
        m_StateBitmap->SetBitmap(m_ImageList.GetIcon(4));
        break;
    case wxGISEnumGPMessageOk:
        m_StateBitmap->SetBitmap(m_ImageList.GetIcon(1));
        break;
    case wxGISEnumGPMessageNone:
        m_StateBitmap->SetBitmap(wxNullBitmap);
        break;
    default:
    case wxGISEnumGPMessageUnknown:
        m_StateBitmap->SetBitmap(wxNullBitmap);
        break;
    }
    m_StateBitmap->SetToolTip(sMsg);
}

void wxGISDTPath::OnOpen(wxCommandEvent& event)
{
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());

    if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
    {        
        wxGxObjectDialog dlg(this, wxID_ANY, _("Select input object")); 
        dlg.SetAllowMultiSelect(false);
        dlg.SetAllFilters(false);
        dlg.SetOwnsFilter(false);
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetFilterCount(); i++)
                dlg.AddFilter(pDomain->GetFilter(i), false);
        }
        dlg.SetOverwritePrompt(false);
        if(dlg.ShowModalOpen() == wxID_OK)
        {
            wxString sPath = dlg.GetFullPath();
            sPath.Replace(wxT("\\\\"), wxT("\\"));
            //m_PathTextCtrl->ChangeValue( sPath );
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
            m_pParam->SetAltered(true);
        }
    }
    else
    {
        wxGxObjectDialog dlg(this, wxID_ANY, _("Select output object")); 
        //dlg.SetName(sName);???
        dlg.SetAllowMultiSelect(false);
        dlg.SetAllFilters(false);
        dlg.SetOwnsFilter(false);
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetFilterCount(); i++)
                dlg.AddFilter(pDomain->GetFilter(i), false);
        }
        dlg.SetOverwritePrompt(false);
        if(dlg.ShowModalSave() == wxID_OK)
        {
            wxString sPath = dlg.GetFullPath();
            sPath.Replace(wxT("\\\\"), wxT("\\"));
            //m_PathTextCtrl->ChangeValue( sPath );
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
            m_pParam->SetAltered(true);
        }
    }
}

//validate
bool wxGISDTPath::Validate(void)
{
    wxString sPath = m_pParam->GetValue();
    if(sPath.IsEmpty())
    {
        m_pParam->SetAltered(false);
        if(m_pParam->GetParameterType() != enumGISGPParameterTypeRequired)
        {
            m_pParam->SetIsValid(true);
            m_pParam->SetMessage(wxGISEnumGPMessageNone);
            return true;
        }
        else
        {
            m_pParam->SetIsValid(false);
            m_pParam->SetMessage(wxGISEnumGPMessageRequired, _("The value is required"));
            return false;
        }
    }
    if(m_pCatalog)
    {
        IGxObjectContainer* pGxContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
        IGxObject* pGxObj = pGxContainer->SearchChild(sPath);
        if(pGxObj)
        {
           if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
           {
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
           }
           else
           {
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageWarning, _("The output object is exist. It will be overwrited!"));
           }
           return true;
        }
        else
        {
           if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
           {
                m_pParam->SetIsValid(false);
                m_pParam->SetMessage(wxGISEnumGPMessageError, _("The input object is not exist"));
                return false;
           }
           else
           {
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
               return true;
           }
        }

        //int ret = VSIStatL((const char*) sFolderPath.mb_str(*m_pMBConv), &BufL);
        //if(ret == 0)
    }
    return true;
}

void wxGISDTPath::Update(void)
{
    m_PathTextCtrl->ChangeValue( m_pParam->GetValue() );
    //Validate();
}


///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTChoice
///////////////////////////////////////////////////////////////////////////////

wxGISDTChoice::wxGISDTChoice( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    m_bitmap1->SetToolTip( wxT("some tip") );
	fgSizer1->Add( m_bitmap1, 0, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 1, wxALL|wxEXPAND, 5 );
	
	m_bitmap2 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap2, 0, wxALL, 5 );
	
	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	fgSizer1->Add( m_choice1, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTChoice::~wxGISDTChoice()
{
}
