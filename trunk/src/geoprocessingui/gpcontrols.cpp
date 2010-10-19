/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  controls classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wx/valgen.h"

#include "../../art/state_16.xpm"
#include "../../art/open_16.xpm"

#include "wxgis/framework/messagedlg.h"

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

void wxGISDTBase::SetMessage(wxGISEnumGPMessageType nType, wxString sMsg)
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
//    event.Skip();
    IGPParameter* pParam = m_pBaseCtrl->GetParameter();
    wxString sData = GetValue();
    bool bValid(false);
    long dVal(0);
    double dfVal(0);
    switch(pParam->GetDataType())
    {
    case enumGISGPParamDTInteger:
        bValid = sData.ToLong(&dVal);
        break;
    case enumGISGPParamDTDouble:
        bValid = sData.ToDouble(&dfVal);
        break;
    case enumGISGPParamDTString:
        bValid = true;
        break;
    case enumGISGPParamDTPath:
        {
            bValid = false;
            wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(pParam->GetDomain());
            IGxObjectFilter* poFilter = poDomain->GetFilter(poDomain->GetSelFilter());
            wxFileName oName(sData);
            if(poFilter)
            {
                if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                {
                    bValid = true;
                    break;
                }
            }
            for(size_t i = 0; i < poDomain->GetFilterCount(); i++)
            {
                poFilter = poDomain->GetFilter(i);
                if(poFilter)
                {
                    if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                    {
                        poDomain->SetSelFilter(i);
                        bValid = true;
                        break;
                    }
                }
            }
        }
        break;
    default:
        bValid = true;
        break;
    }

    pParam->SetIsValid(bValid);
    if ( bValid )
    {
        pParam->SetMessage(wxGISEnumGPMessageNone);
        switch(pParam->GetDataType())
        {
        case enumGISGPParamDTInteger:
            pParam->SetValue(wxVariant(dVal, wxT("integer")));
            pParam->SetAltered(true);
            return;
        case enumGISGPParamDTDouble:
            pParam->SetValue(wxVariant(dfVal, wxT("double")));
            pParam->SetAltered(true);
            return;
        case enumGISGPParamDTString:
            pParam->SetValue(wxVariant(sData, wxT("string")));
            pParam->SetAltered(true);
            return;
        case enumGISGPParamDTPath:
            pParam->SetValue(wxVariant(sData, wxT("path")));
            pParam->SetAltered(true);
            return;
           default:
            pParam->SetValue(wxVariant(sData, wxT("val")));
            pParam->SetAltered(true);
            return;
        }
    }
    else
    {
        pParam->SetValue(wxVariant(sData, wxT("val")));
        pParam->SetMessage(wxGISEnumGPMessageError, _("The input data are invalid"));
    }
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

    m_PathTextCtrl = new wxGISTextCtrl( this, wxID_ANY, pParam->GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_PathTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_16_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTPath::~wxGISDTPath()
{
}

void wxGISDTPath::OnOpen(wxCommandEvent& event)
{
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());

    if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
    {
        wxGxObjectDialog dlg(this, wxID_ANY, _("Select input object"));
        dlg.SetExternalCatalog(m_pCatalog);
        dlg.SetAllowMultiSelect(false);
        dlg.SetAllFilters(false);
        dlg.SetOwnsFilter(false);
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetFilterCount(); i++)
            {
                if(i == pDomain->GetSelFilter())
                    dlg.AddFilter(pDomain->GetFilter(i), true);
                else
                    dlg.AddFilter(pDomain->GetFilter(i), false);
            }
        }
        dlg.SetOverwritePrompt(false);
        if(dlg.ShowModalOpen() == wxID_OK)
        {
            wxString sPath = dlg.GetFullPath();
            //sPath.Replace(wxT("\\\\"), wxT("\\"));
            //m_PathTextCtrl->ChangeValue( sPath );
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
            m_pParam->SetAltered(true);
            pDomain->SetSelFilter(dlg.GetCurrentFilterId());
        }
    }
    else
    {
        wxGxObjectDialog dlg(this, wxID_ANY, _("Select output object"));
        dlg.SetExternalCatalog(m_pCatalog);
        //dlg.SetName(sName);???
        dlg.SetAllowMultiSelect(false);
        dlg.SetAllFilters(false);
        dlg.SetOwnsFilter(false);
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetFilterCount(); i++)
            {
                if(i == pDomain->GetSelFilter())
                    dlg.AddFilter(pDomain->GetFilter(i), true);
                else
                    dlg.AddFilter(pDomain->GetFilter(i), false);
            }
        }
        dlg.SetOverwritePrompt(false);
        if(dlg.ShowModalSave() == wxID_OK)
        {
            wxString sPath = dlg.GetFullPath();
            //sPath.Replace(wxT("\\\\"), wxT("\\"));
            //m_PathTextCtrl->ChangeValue( sPath );
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
            m_pParam->SetAltered(true);
            pDomain->SetSelFilter(dlg.GetCurrentFilterId());
        }
    }
}

//validate
bool wxGISDTPath::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;

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
               if(m_pParam->GetMessageType() == wxGISEnumGPMessageError)
                   return false;
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
           }
           else
           {
               if(m_pParam->GetMessageType() == wxGISEnumGPMessageError)
                   return false;
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageWarning, _("The output object exists and will be overwritten!"));
           }
           return true;
        }
        else
        {
           if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
           {
                m_pParam->SetIsValid(false);
                m_pParam->SetMessage(wxGISEnumGPMessageError, _("The input object doesn't exist"));
                return false;
           }
           else
           {
               if(m_pParam->GetMessageType() == wxGISEnumGPMessageError)
                   return false;

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
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
    //Validate();
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTDigit
///////////////////////////////////////////////////////////////////////////////

wxGISDTDigit::wxGISDTDigit( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
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

    m_PathTextCtrl = new wxGISTextCtrl( this, wxID_ANY, pParam->GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT  );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_PathTextCtrl, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTDigit::~wxGISDTDigit()
{
}

bool wxGISDTDigit::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;

    wxString sData = m_pParam->GetValue();
    bool bValid(false);
    long dVal(0);
    double dfVal(0);
    switch(m_pParam->GetDataType())
    {
    case enumGISGPParamDTInteger:
        bValid = sData.ToLong(&dVal);
        break;
    case enumGISGPParamDTDouble:
        bValid = sData.ToDouble(&dfVal);
        break;
    default:
        bValid = false;
        break;
    }

    m_pParam->SetIsValid(bValid);
    if ( bValid )
    {
        m_pParam->SetMessage(wxGISEnumGPMessageOk);
        switch(m_pParam->GetDataType())
        {
        case enumGISGPParamDTInteger:
        case enumGISGPParamDTDouble:
        default:
            m_pParam->SetAltered(true);
            return true;
        }
    }
    else
    {
        m_pParam->SetMessage(wxGISEnumGPMessageError, _("The input data are invalid"));
        return false;
    }
    return true;
}

void wxGISDTDigit::Update(void)
{
    m_PathTextCtrl->ChangeValue( m_pParam->GetValue() );
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
    //Validate();
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTChoice
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISDTChoice, wxGISDTBase)
	EVT_CHOICE(ID_CHOICESTR, wxGISDTChoice::OnChoice)
END_EVENT_TABLE()

wxGISDTChoice::wxGISDTChoice( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
{
    //m_pCatalog = pCatalog;
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

	wxArrayString Choices;

    wxGISGPStringDomain* poGPStringDomain = dynamic_cast<wxGISGPStringDomain*>(m_pParam->GetDomain());
    if(poGPStringDomain)
        Choices = poGPStringDomain->GetArrayString();

	m_choice = new wxChoice( this, ID_CHOICESTR, wxDefaultPosition, wxDefaultSize, Choices, 0 );
    if(m_pParam->GetValue().GetString().IsEmpty())
        m_choice->SetSelection( 0 );
    else
        m_choice->SetStringSelection( m_pParam->GetValue() );

	bPathSizer->Add( m_choice, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTChoice::~wxGISDTChoice()
{
}

bool wxGISDTChoice::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;
    m_pParam->SetIsValid(true);
    m_pParam->SetMessage(wxGISEnumGPMessageOk);

    return true;
}

void wxGISDTChoice::Update(void)
{
    //m_choice->SetStringSelection( m_pParam->GetValue() );
    //wxGISGPStringDomain* poGPStringDomain = dynamic_cast<wxGISGPStringDomain*>(m_pParam->GetDomain());
    //if(poGPStringDomain)
    //    poGPStringDomain->SetSelString(m_choice->GetCurrentSelection());
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
    //Validate();
}

void wxGISDTChoice::OnChoice(wxCommandEvent& event)
{
    m_pParam->SetValue(m_choice->GetStringSelection());
    wxGISGPStringDomain* poGPStringDomain = dynamic_cast<wxGISGPStringDomain*>(m_pParam->GetDomain());
    if(poGPStringDomain)
        poGPStringDomain->SetSelString(m_choice->GetCurrentSelection());
    m_pParam->SetAltered(true);
}
