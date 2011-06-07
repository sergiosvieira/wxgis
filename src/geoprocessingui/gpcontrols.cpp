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
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/gxfileui.h"
#include "wxgis/framework/messagedlg.h"

#include "wx/dnd.h"
#include "wx/valgen.h"
#include "wx/tokenzr.h"
//#include "wx/propgrid/props.h"
#include "wx/propgrid/propgrid.h"

#include "../../art/state.xpm"
#include "../../art/open.xpm"
#include "../../art/add_to_list.xpm"


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
	wxArrayString saValues;
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
	case enumGISGPParamDTStringList:
	case enumGISGPParamDTIntegerList:
	case enumGISGPParamDTDoubleList:
		saValues =  wxStringTokenize(sData, wxString(wxT(",")), wxTOKEN_RET_EMPTY );
		for(size_t i = 0; i < saValues.GetCount(); ++i)
			saValues[i] = saValues[i].Trim(true).Trim(false);
		bValid = saValues.GetCount() > 0;
        break;
    case enumGISGPParamDTPath:
        if(sData.IsEmpty())
            break;
        else
        {
            wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(pParam->GetDomain());
			IGxObjectFilter* poFilter = poDomain->GetFilter(pParam->GetSelDomainValue());
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
						pParam->SetSelDomainValue(i);
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
 		case enumGISGPParamDTStringList:
		case enumGISGPParamDTIntegerList:
		case enumGISGPParamDTDoubleList:
            pParam->SetValue(wxVariant(saValues, wxT("list")));
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
				if(i == m_pParam->GetSelDomainValue())
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
            m_pParam->SetAltered(true);
			m_pParam->SetSelDomainValue(dlg.GetCurrentFilterId());
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
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
				if(i == m_pParam->GetSelDomainValue())
                    dlg.AddFilter(pDomain->GetFilter(i), true);
                else
                    dlg.AddFilter(pDomain->GetFilter(i), false);
            }
        }
        dlg.SetOverwritePrompt(false);
        if(dlg.ShowModalSave() == wxID_OK)
        {
            wxString sPath = dlg.GetFullPath();
            m_pParam->SetAltered(true);
			m_pParam->SetSelDomainValue(dlg.GetCurrentFilterId());
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
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
	if(poDomain && poDomain->GetCount() > 0)
	{
		IGxObjectFilter* poFilter = poDomain->GetFilter(m_pParam->GetSelDomainValue());
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
					m_pParam->SetSelDomainValue(i);
					break;
				}
			}
		}
    //Validate();
	}
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

    wxGISGPValueDomain* poGPValueDomain = dynamic_cast<wxGISGPValueDomain*>(m_pParam->GetDomain());
    if(poGPValueDomain)
	{
		for(size_t i = 0; i < poGPValueDomain->GetCount(); ++i)
			Choices.Add( poGPValueDomain->GetName(i));
	}
	m_choice = new wxChoice( this, ID_CHOICESTR, wxDefaultPosition, wxDefaultSize, Choices, 0 );
    if(m_pParam->GetValue().GetString().IsEmpty())
        m_choice->SetSelection( 0 );
    else
    {
		int nPos = wxNOT_FOUND;
		if(poGPValueDomain)
			nPos = poGPValueDomain->GetPosByValue(m_pParam->GetValue());
		if(nPos == wxNOT_FOUND)
			m_choice->Select(0);
		else
			m_choice->Select(nPos);
		//{
		//	wxString sData = poGPStringDomain->GetName(nPos);
		//	m_choice->SetStringSelection( sData );
		//}
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
    IGPDomain* pDomain = m_pParam->GetDomain();
    if(pDomain)
		m_pParam->SetSelDomainValue(nPos);
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

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTMultiParam
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTMultiParam, wxPanel)
EVT_GRID_CELL_CHANGE(wxGISDTMultiParam::OnCellChange)
END_EVENT_TABLE()

wxGISDTMultiParam::wxGISDTMultiParam( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
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

	wxGISGPMultiParameter* pMParam = dynamic_cast<wxGISGPMultiParameter*>(pParam);
	m_pg = new wxGrid(this, wxGISDTMultiParam::ID_PPCTRL, wxDefaultPosition, wxSize(200,200), wxBORDER_THEME | wxWANTS_CHARS);
	// Grid
	m_pg->CreateGrid(pMParam->GetRowCount(), pMParam->GetColumnCount());
	m_pg->EnableEditing( true );
	m_pg->EnableGridLines( true );
	m_pg->EnableDragGridSize( false );
	m_pg->SetMargins( 0, 0 );

	// Columns
	m_pg->EnableDragColMove( false );
	m_pg->EnableDragColSize( true );
	m_pg->SetColLabelSize( GRID_COL_SIZE );
	m_pg->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

	// Rows
	m_pg->EnableDragRowSize( true );
	m_pg->SetRowLabelSize( 4 );//GRID_ROW_SIZE );
	m_pg->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

	// Label Appearance

	// Cell Defaults
	m_pg->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	//m_grid->SetDefaultCellBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));


	for(size_t i = 0; i < pMParam->GetColumnCount(); ++i)
	{
		m_pg->SetColLabelValue(i, pMParam->GetColumnName(i));
		m_pg->SetColSize(i, 175);
	}
	//m_pg->AutoSizeColumns();
	m_pg->AutoSizeRows();
	bPathSizer->Add( m_pg, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTMultiParam::~wxGISDTMultiParam()
{
}

//validate
bool wxGISDTMultiParam::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;
    m_pParam->SetMessage(wxGISEnumGPMessageRequired, _("The value is required"));
    if(!m_pParam->GetAltered())
        return true;
	if(m_pParam->GetIsValid())
	{
		m_pParam->SetMessage(wxGISEnumGPMessageOk);
		return true;
	}
	else
	{
		//TODO: Get error message and set it
		m_pParam->SetMessage(wxGISEnumGPMessageError);
		return false;
	}
}

void wxGISDTMultiParam::Update(void)
{
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
	if(m_pParam->GetAltered())
		return;
	if(m_pg->GetNumberRows())
		m_pg->DeleteRows(0, m_pg->GetNumberRows());
	wxGISGPMultiParameter* pParam2 = dynamic_cast<wxGISGPMultiParameter*>(m_pParam);
	m_pg->AppendRows(pParam2->GetRowCount());
	if(pParam2)
	{
		for(size_t i = 0; i < pParam2->GetRowCount(); ++i)
		{
			for(size_t j = 0; j < pParam2->GetColumnCount(); ++j)
			{
				IGPParameter* pCellParam = pParam2->GetParameter(j, i);
				if(!pCellParam)
					continue;
				IGPDomain* pDomain = pCellParam->GetDomain();
				if(pDomain)
				{
					wxArrayString sDomainNames;
					for(size_t k = 0; k < pDomain->GetCount(); ++k)
						sDomainNames.Add(pDomain->GetName(k));
					wxGridCellChoiceEditor* pCEditor = new wxGridCellChoiceEditor(sDomainNames);
					m_pg->SetCellEditor(i, j, pCEditor);
					if(pDomain->GetCount()) 
						m_pg->SetCellValue(i, j, sDomainNames[pCellParam->GetSelDomainValue()]);
				}
				else
				{
					switch(pCellParam->GetDataType())
					{
					case enumGISGPParamDTBool:
						m_pg->SetColFormatBool(j);
						m_pg->SetCellValue(i, j, wxString::Format(wxT("%d"), pCellParam->GetValue().GetBool()));
						break;
					case enumGISGPParamDTInteger:
						m_pg->SetColFormatNumber(j);
						m_pg->SetCellValue(i, j, wxString::Format(wxT("%d"), pCellParam->GetValue().GetInteger()));
						break;
					case enumGISGPParamDTDouble:
						m_pg->SetColFormatFloat(j, 8, 6);
						m_pg->SetCellValue(i, j, wxString::Format(wxT("%f"), pCellParam->GetValue().GetDouble()));
						break;
					case enumGISGPParamDTString:
						m_pg->SetCellValue(i, j, pCellParam->GetValue().GetString());
						break;
						//TODO:
					case enumGISGPParamDTSpatRef:
					case enumGISGPParamDTQuery:
					case enumGISGPParamDTPath:
					case enumGISGPParamDTStringList:
					case enumGISGPParamDTPathArray:
						m_pg->SetCellValue(i, j, pCellParam->GetValue().GetString());
						break;
						break;
						//end TODO
					case enumGISGPParamDTParamArray:
					case enumGISGPParamDTUnknown:
					case enumGISGPParamMax:
					default:
						break;
					}
				}
			}
		}
	}
}


void wxGISDTMultiParam::OnCellChange(wxGridEvent &event)
{
	wxGISGPMultiParameter* pParam2 = dynamic_cast<wxGISGPMultiParameter*>(m_pParam);
	wxASSERT(pParam2);
	IGPParameter* pCellParam = pParam2->GetParameter(event.GetCol(), event.GetRow());
	if(pCellParam)
	{
		m_pParam->SetAltered(true);
        pCellParam->SetAltered(true);

		m_pParam->SetHasBeenValidated( false );
		pCellParam->SetHasBeenValidated( false );
		wxString sCellValue = m_pg->GetCellValue(event.GetRow(), event.GetCol());
		IGPDomain* pDomain = pCellParam->GetDomain();
		if(pDomain)
		{
			int nPos = pDomain->GetPosByName(sCellValue);
			if(nPos != wxNOT_FOUND)
				pCellParam->SetSelDomainValue(nPos);
		}
		else
		{
			switch(pCellParam->GetDataType())
			{
			case enumGISGPParamDTBool:
				pCellParam->SetValue( wxVariant((bool)wxAtoi(sCellValue)) );
				break;        
			case enumGISGPParamDTInteger:
				pCellParam->SetValue( wxVariant(wxAtoi(sCellValue)) );
				break;        
			case enumGISGPParamDTDouble:
				pCellParam->SetValue( wxVariant(wxAtof(sCellValue)) );
				break;        
			case enumGISGPParamDTString:
			case enumGISGPParamDTSpatRef:
			case enumGISGPParamDTPath:
			case enumGISGPParamDTPathArray:
			case enumGISGPParamDTStringList:
				pCellParam->SetValue( wxVariant(sCellValue) );
				break;        
			case enumGISGPParamDTUnknown:
			default:
				pCellParam->SetValue( wxVariant(sCellValue) );
				break;        
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTList
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTList, wxPanel)
	EVT_BUTTON(wxID_ADD, wxGISDTList::OnAdd)
END_EVENT_TABLE()

wxGISDTList::wxGISDTList( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
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

    m_TextCtrl = new wxGISTextCtrl( this, wxID_ANY, GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_TextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_ADD, wxBitmap(add_to_list_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTList::~wxGISDTList()
{
}

void wxGISDTList::OnAdd(wxCommandEvent& event)
{
	wxPGArrayStringEditorDialog dlg;
    dlg.SetDialogValue( m_pParam->GetValue() );
    dlg.Create(this, _("Add value to list"), _("Value list"));
    int res = dlg.ShowModal();

    if ( res == wxID_OK && dlg.IsModified() )
    {
        wxVariant value = dlg.GetDialogValue();
        if ( !value.IsNull() )
        {
			m_pParam->SetValue( value );
            m_pParam->SetAltered(true);
        }
    }
	//show add value dialog
   // wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());
   // wxFileName Name(m_pParam->GetValue().MakeString());


   // if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
   // {
   //     wxGxObjectDialog dlg(this, m_pCatalog, wxID_ANY, _("Select input object"));
   //     dlg.SetAllowMultiSelect(false);
   //     dlg.SetAllFilters(false);
   //     dlg.SetOwnsFilter(false);
   //     dlg.SetName( Name.GetFullName() );
   //     if(pDomain)
   //     {
   //         for(size_t i = 0; i < pDomain->GetCount(); i++)
   //         {
			//	if(i == m_pParam->GetSelDomainValue())
   //                 dlg.AddFilter(pDomain->GetFilter(i), true);
   //             else
   //                 dlg.AddFilter(pDomain->GetFilter(i), false);
   //         }
   //     }
   //     dlg.SetOverwritePrompt(false);
   //     if(dlg.ShowModalOpen() == wxID_OK)
   //     {
   //         wxString sPath = dlg.GetFullPath();
   //         //sPath.Replace(wxT("\\\\"), wxT("\\"));
   //         //m_PathTextCtrl->ChangeValue( sPath );
   //         m_pParam->SetAltered(true);
			//m_pParam->SetSelDomainValue(dlg.GetCurrentFilterId());
   //         m_pParam->SetValue(wxVariant(sPath, wxT("path")));
   //     }
   // }
   // else
   // {
   //     wxGxObjectDialog dlg(this, m_pCatalog, wxID_ANY, _("Select output object"));
   //     dlg.SetAllowMultiSelect(false);
   //     dlg.SetAllFilters(false);
   //     dlg.SetOwnsFilter(false);
   //     dlg.SetName( Name.GetFullName() );
   //     if(pDomain)
   //     {
   //         for(size_t i = 0; i < pDomain->GetCount(); i++)
   //         {
			//	if(i == m_pParam->GetSelDomainValue())
   //                 dlg.AddFilter(pDomain->GetFilter(i), true);
   //             else
   //                 dlg.AddFilter(pDomain->GetFilter(i), false);
   //         }
   //     }
   //     dlg.SetOverwritePrompt(false);
   //     if(dlg.ShowModalSave() == wxID_OK)
   //     {
   //         wxString sPath = dlg.GetFullPath();
   //         m_pParam->SetAltered(true);
			//m_pParam->SetSelDomainValue(dlg.GetCurrentFilterId());
   //         m_pParam->SetValue(wxVariant(sPath, wxT("path")));
   //     }
   // }
}

//validate
bool wxGISDTList::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
        return true;

    wxArrayString saValues = m_pParam->GetValue().GetArrayString();
    if(saValues.GetCount() <= 0 && m_pParam->GetParameterType() == enumGISGPParameterTypeRequired)
    {
        m_pParam->SetIsValid(false);
        m_pParam->SetMessage(wxGISEnumGPMessageRequired, _("The value is required"));
        return false;
    }

    return true;
}

void wxGISDTList::Update(void)
{
    m_TextCtrl->ChangeValue( GetValue() );
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

wxString wxGISDTList::GetValue()
{
	if(!m_pParam)
		return wxEmptyString;
	wxArrayString Arr = m_pParam->GetValue().GetArrayString();
	wxString sOutput;
	for(size_t i = 0; i < Arr.GetCount(); ++i)
	{
		sOutput.Append(Arr[i]);
		if(i != Arr.GetCount() - 1)
			sOutput.Append(wxT(", "));
	}
	return sOutput;
}