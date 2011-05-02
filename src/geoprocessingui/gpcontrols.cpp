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
#include "wxgis/geoprocessingui/gpcontrols.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/gxfileui.h"
#include "wxgis/framework/messagedlg.h"

#include "wx/dnd.h"
#include "wx/valgen.h"

#include "../../art/state.xpm"
#include "../../art/open.xpm"


///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTBase
///////////////////////////////////////////////////////////////////////////////
wxGISDTBase::wxGISDTBase( IGPParameter* pParam, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    m_pParam = pParam;

	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_xpm));
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
    case enumGISGPParamDTSpatRef:
        bValid = !pParam->GetValue().IsNull();
        break;
    case enumGISGPParamDTPath:
        if(sData.IsEmpty())
            break;
        else
        {
            wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(pParam->GetDomain());
            IGxObjectFilter* poFilter = poDomain->GetFilter(poDomain->GetSel());
            wxFileName oName(sData);
            if(poFilter)
            {
                if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                {
                    bValid = true;
                    break;
                }
            }
            for(size_t i = 0; i < poDomain->GetCount(); i++)
            {
                poFilter = poDomain->GetFilter(i);
                if(poFilter)
                {
                    if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
                    {
                        poDomain->SetSel(i);
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
        case enumGISGPParamDTSpatRef:
            pParam->SetValue(wxVariant(sData, wxT("spat_ref")));
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

	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
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
    wxFileName Name(m_pParam->GetValue().MakeString());


    if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
    {
        wxGxObjectDialog dlg(this, m_pCatalog, wxID_ANY, _("Select input object"));
        dlg.SetAllowMultiSelect(false);
        dlg.SetAllFilters(false);
        dlg.SetOwnsFilter(false);
        dlg.SetName( Name.GetFullName() );
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetCount(); i++)
            {
                if(i == pDomain->GetSel())
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
            pDomain->SetSel(dlg.GetCurrentFilterId());
        }
    }
    else
    {
        wxGxObjectDialog dlg(this, m_pCatalog, wxID_ANY, _("Select output object"));
        dlg.SetAllowMultiSelect(false);
        dlg.SetAllFilters(false);
        dlg.SetOwnsFilter(false);
        dlg.SetName( Name.GetFullName() );
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetCount(); i++)
            {
                if(i == pDomain->GetSel())
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
            pDomain->SetSel(dlg.GetCurrentFilterId());
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
    }
    return true;
}

void wxGISDTPath::Update(void)
{
    m_PathTextCtrl->ChangeValue( m_pParam->GetValue() );
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());

    wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());
    IGxObjectFilter* poFilter = poDomain->GetFilter(poDomain->GetSel());
    wxFileName oName(m_pParam->GetValue());
    if(!oName.IsOk())
        return;

    for(size_t i = 0; i < poDomain->GetCount(); i++)
    {
        poFilter = poDomain->GetFilter(i);
        if(poFilter)
        {
            if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
            {
                poDomain->SetSel(i);
                break;
            }
        }
    }
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
    {
        wxString sData = poGPStringDomain->GetInternalString(m_pParam->GetValue());
        m_choice->SetStringSelection( sData );
    }

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
    int nPos = m_choice->GetCurrentSelection();
    wxString sData;
    wxGISGPStringDomain* poGPStringDomain = dynamic_cast<wxGISGPStringDomain*>(m_pParam->GetDomain());
    if(poGPStringDomain)
    {
        sData = poGPStringDomain->GetInternalString(nPos);
        poGPStringDomain->SetSel(nPos);
    }
    m_pParam->SetValue(sData);
    m_pParam->SetAltered(true);
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTBool
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISDTBool, wxPanel)
	EVT_CHECKBOX(ID_CHECKBOOL, wxGISDTBool::OnClick)
END_EVENT_TABLE()

wxGISDTBool::wxGISDTBool( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
{
    m_pCatalog = pCatalog;
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_StateBitmap = new wxStaticBitmap( this, wxID_ANY, m_pParam->GetParameterType() == enumGISGPParameterTypeRequired ? m_ImageList.GetIcon(4) : wxNullBitmap , wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_StateBitmap, 0, wxALL, 5 );

    m_pCheckBox = new wxCheckBox( this, ID_CHECKBOOL, m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + _(" (optional)") : m_pParam->GetDisplayName(), wxDefaultPosition, wxDefaultSize );
    m_pCheckBox->SetValue(pParam->GetValue());
	fgSizer1->Add( m_pCheckBox, 1, wxALL|wxEXPAND, 5 );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTBool::~wxGISDTBool()
{
}

bool wxGISDTBool::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;

    m_pParam->SetIsValid(true);
    m_pParam->SetMessage(wxGISEnumGPMessageOk);
    m_pParam->SetAltered(true);
    return true;
}

void wxGISDTBool::Update(void)
{
    m_pCheckBox->SetValue(m_pParam->GetValue());
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
    //Validate();
}

void wxGISDTBool::OnClick(wxCommandEvent& event)
{ 
    event.Skip(); 
    m_pParam->SetValue(m_pCheckBox->GetValue());
    m_pParam->SetAltered(true);
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTSpatRef
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTSpatRef, wxPanel)
	EVT_BUTTON(wxID_OPEN, wxGISDTSpatRef::OnOpen)
END_EVENT_TABLE()

wxGISDTSpatRef::wxGISDTSpatRef( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
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

    CPLString szWKT(pParam->GetValue().MakeString().mb_str());
    OGRSpatialReference SpaRef;
    wxString sWKT;
    const char* szpWKT = szWKT.c_str();
    if(SpaRef.importFromWkt((char**)&szpWKT) == OGRERR_NONE)
    {
        char *pszWKT;
        SpaRef.exportToPrettyWkt( &pszWKT );
        sWKT = wxString(pszWKT, *wxConvCurrent);
        OGRFree( pszWKT );
    }
    m_PathTextCtrl = new wxTextCtrl( this, wxID_ANY, sWKT, wxDefaultPosition, wxSize(100,100)/*wxDefaultSize*/, wxTE_READONLY | wxTE_MULTILINE | wxTE_AUTO_SCROLL );/// | wxTE_BESTWRAP | wxTE_NO_VSCROLL
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_PathTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTSpatRef::~wxGISDTSpatRef()
{
}

void wxGISDTSpatRef::OnOpen(wxCommandEvent& event)
{
    wxGxObjectDialog dlg(this, m_pCatalog, wxID_ANY, _("Select Spatial Reference"));
    dlg.SetAllowMultiSelect(false);
    dlg.SetAllFilters(false);
    dlg.SetOwnsFilter(false);
    dlg.SetStartingLocation(_("Coordinate Systems"));
    if(dlg.ShowModalOpen() == wxID_OK)
    {
        GxObjectArray* pSelObj = dlg.GetSelectedObjects();
        if(pSelObj->size() < 1)
            return;
        wxGxPrjFileUI* pGxPrjFileUI = dynamic_cast<wxGxPrjFileUI*>(pSelObj->operator[](0));
        if(pGxPrjFileUI)
        {
            OGRSpatialReferenceSPtr pOGRSpatialReference = pGxPrjFileUI->GetSpatialReference();                
            char *pszWKT;
            pOGRSpatialReference->exportToWkt( &pszWKT );
            m_pParam->SetValue(wxVariant(wxString(pszWKT, *wxConvCurrent), wxT("SRS")));
            OGRFree( pszWKT );
            pOGRSpatialReference->exportToPrettyWkt( &pszWKT );
            m_PathTextCtrl->ChangeValue(wxString(pszWKT, wxConvLocal));
            OGRFree( pszWKT );
            
            m_pParam->SetIsValid(true);
            m_pParam->SetAltered(true);
            m_pParam->SetMessage(wxGISEnumGPMessageNone);
        }
    }
}

//validate
bool wxGISDTSpatRef::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;

    if(!m_pParam->GetAltered())
        return true;

    CPLString szWKT(m_pParam->GetValue().MakeString().mb_str());
    OGRSpatialReference SpaRef;
    wxString sWKT;
    const char* szpWKT = szWKT.c_str();
    if( SpaRef.importFromWkt((char**)&szpWKT) == OGRERR_NONE )
    {
        m_pParam->SetIsValid(true);
        m_pParam->SetMessage(wxGISEnumGPMessageOk);
        return true;
    }
    else
    {
        m_pParam->SetIsValid(false);
        m_pParam->SetMessage(wxGISEnumGPMessageError, _("Unsupported Spatial reference"));
        return false;
    }
    return true;
}

void wxGISDTSpatRef::Update(void)
{
    wxString sWKT = m_pParam->GetValue().MakeString();
    if(sWKT.IsEmpty())
        return;
    CPLString szWKT(sWKT.mb_str());
    OGRSpatialReference SpaRef;
    
    sWKT.Empty();
    const char* szpWKT = szWKT.c_str();
    if(SpaRef.importFromWkt((char**)&szpWKT) == OGRERR_NONE)
    {
        char *pszWKT;
        SpaRef.exportToPrettyWkt( &pszWKT );
        sWKT = wxString(pszWKT, *wxConvCurrent);
        OGRFree( pszWKT );
    }
    m_PathTextCtrl->ChangeValue( sWKT );
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

