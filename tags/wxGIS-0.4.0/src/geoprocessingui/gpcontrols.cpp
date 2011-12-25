/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  controls classes.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxcontdialog.h"
#include "wxgis/catalogui/gxfileui.h"
#include "wxgis/cartoui/sqlquerydialog.h"

#include "wx/dnd.h"
#include "wx/valgen.h"
#include "wx/tokenzr.h"
//#include "wx/propgrid/props.h"
#include "wx/propgrid/propgrid.h"

#include "../../art/state.xpm"
#include "../../art/open.xpm"
#include "../../art/add_to_list.xpm"
#include "../../art/querysql.xpm"

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
	if(m_nCurrentType == nType && m_sCurrentMsg == sMsg)
		return;
	m_nCurrentType = nType;
	m_sCurrentMsg = sMsg;
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
/// Class wxGISDTPath
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTPath, wxPanel)
	EVT_BUTTON(wxID_OPEN, wxGISDTPath::OnOpen)
	EVT_UPDATE_UI(ID_PATHCTRL, wxGISDTPath::OnUpdateUI)
	EVT_TEXT(ID_PATHCTRL, wxGISDTPath::OnPathChange)
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

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );

	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );

    m_PathTextCtrl = new wxTextCtrl( this, ID_PATHCTRL, pParam->GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_CHARWRAP );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_PathTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
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
        dlg.SetAllFilters(true);
        dlg.SetOwnsFilter(false);
        dlg.SetName( Name.GetFullName() );
        if(pDomain)
        {
            for(size_t i = 0; i < pDomain->GetCount(); ++i)
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
            for(size_t i = 0; i < pDomain->GetCount(); ++i)
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
			m_pParam->SetSelDomainValue(dlg.GetCurrentFilterId());
            m_pParam->SetValue(wxVariant(sPath, wxT("path")));
        }
    }
    m_pParam->SetAltered(true);
    m_pParam->SetHasBeenValidated(false);

	UpdateControls();
}

//validate
bool wxGISDTPath::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

	m_pParam->SetHasBeenValidated(true);
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
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
               return true;
           }
        }
    }
    return true;
}

void wxGISDTPath::UpdateControls(void)
{
	if(!m_pParam->GetAltered())
		return;
	if(m_pParam->GetValue() == m_PathTextCtrl->GetValue())
		return;

	m_PathTextCtrl->ChangeValue( m_pParam->GetValue() );
	//sent updateui event
	wxUpdateUIEvent event(ID_PATHCTRL);
	GetEventHandler()->ProcessEvent( event );
}

void wxGISDTPath::UpdateValues(void)
{
	wxString sData = m_PathTextCtrl->GetValue();
    switch(m_pParam->GetDataType())
    {
      case enumGISGPParamDTPath:
      case enumGISGPParamDTFolderPath:
        m_pParam->SetValue(wxVariant(sData, wxT("path")));
		break;
      default:
        m_pParam->SetValue(wxVariant(sData, wxT("val")));
    }

    wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());
	if(poDomain && poDomain->GetCount() > 0)
	{
		IGxObjectFilter* poFilter = poDomain->GetFilter(m_pParam->GetSelDomainValue());
		wxFileName oName(m_pParam->GetValue());
		if(!oName.IsOk())
			return;

		for(size_t i = 0; i < poDomain->GetCount(); ++i)
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
	}
}

void wxGISDTPath::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTPath::OnPathChange(wxCommandEvent& event)
{
	m_pParam->SetHasBeenValidated(false);
    m_pParam->SetAltered(true);
	UpdateValues();
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTFolderPath
///////////////////////////////////////////////////////////////////////////////

wxGISDTFolderPath::wxGISDTFolderPath( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTPath( pParam, pCatalog, parent, id, pos, size, style )
{
}

wxGISDTFolderPath::~wxGISDTFolderPath()
{
}

void wxGISDTFolderPath::OnOpen(wxCommandEvent& event)
{
    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());
    wxFileName Name(m_pParam->GetValue().MakeString());

    wxGxContainerDialog dlg(this, m_pCatalog, wxID_ANY, m_pParam->GetDirection() == enumGISGPParameterDirectionInput ? _("Select input object") : _("Select output object"));
	dlg.SetButtonCaption(_("Select"));
	dlg.ShowCreateButton(true);
    dlg.SetOwnsFilter(false);
    dlg.SetOwnsShowFilter(false);
	dlg.ShowExportFormats();
    if(pDomain)
    {
        for(size_t i = 0; i < pDomain->GetCount(); ++i)
        {
			if(i == m_pParam->GetSelDomainValue())
                dlg.AddFilter(pDomain->GetFilter(i), true);
            else
                dlg.AddFilter(pDomain->GetFilter(i), false);
			dlg.AddShowFilter(pDomain->GetFilter(i));
        }
    }
    dlg.SetAllFilters(false);
    dlg.SetName( Name.GetFullName() );
    if(dlg.ShowModal() == wxID_OK)
    {
        wxString sPath = dlg.GetPath();
        m_pParam->SetValue(wxVariant(sPath, wxT("path")));
    }

    m_pParam->SetAltered(true);
    m_pParam->SetHasBeenValidated(false);

	UpdateControls();
}

//validate
bool wxGISDTFolderPath::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

	m_pParam->SetHasBeenValidated(true);
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
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
           }
        }
        else
        {
           if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
           {
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
           }
           else
           {
               m_pParam->SetIsValid(true);
               m_pParam->SetMessage(wxGISEnumGPMessageOk);
           }
        }
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTDigit
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTDigit, wxPanel)
	EVT_UPDATE_UI(ID_DIGITCTRL, wxGISDTDigit::OnUpdateUI)
	EVT_TEXT(ID_DIGITCTRL, wxGISDTDigit::OnDigitChange)
END_EVENT_TABLE()

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

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );

	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );

    m_DigitTextCtrl = new wxTextCtrl( this, ID_DIGITCTRL, pParam->GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT  );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_DigitTextCtrl, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
}

wxGISDTDigit::~wxGISDTDigit()
{
}

bool wxGISDTDigit::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

 	m_pParam->SetHasBeenValidated(true);
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

void wxGISDTDigit::UpdateControls(void)
{
	if(!m_pParam->GetAltered())
		return;
	if(m_pParam->GetValue() == m_DigitTextCtrl->GetValue())
		return;
    m_DigitTextCtrl->ChangeValue( m_pParam->GetValue() );
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTDigit::UpdateValues(void)
{
	wxString sData = m_DigitTextCtrl->GetValue();
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
        bValid = true;
        break;
    }

    m_pParam->SetIsValid(bValid);

    if ( bValid )
    {
        switch(m_pParam->GetDataType())
        {
        case enumGISGPParamDTInteger:
            m_pParam->SetValue(wxVariant(dVal, wxT("integer")));
            return;
        case enumGISGPParamDTDouble:
            m_pParam->SetValue(wxVariant(dfVal, wxT("double")));
            return;
        default:
            m_pParam->SetValue(wxVariant(sData, wxT("val")));
            return;
        }
    }
}

void wxGISDTDigit::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTDigit::OnDigitChange(wxCommandEvent& event)
{
	m_pParam->SetHasBeenValidated(false);
    m_pParam->SetAltered(true);
	UpdateValues();
}
///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTChoice
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISDTChoice, wxGISDTBase)
	EVT_CHOICE(ID_CHOICESTR, wxGISDTChoice::OnChoice)
	EVT_UPDATE_UI(ID_CHOICESTR, wxGISDTChoice::OnUpdateUI)
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

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );

	m_bPathSizer = new wxBoxSizer( wxHORIZONTAL );

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

	m_bPathSizer->Add( m_choice, 1, wxALL|wxEXPAND, 5 );

	fgSizer1->Add( m_bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
}

wxGISDTChoice::~wxGISDTChoice()
{
}

bool wxGISDTChoice::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

	m_pParam->SetHasBeenValidated(true);

	bool bRetVal(false);

    switch(m_pParam->GetDataType())
    {
	case enumGISGPParamDTIntegerChoice:
		if(m_pParam->GetValue().GetType() == wxString(wxT("long")))
			bRetVal = true;
        break;
    case enumGISGPParamDTDoubleChoice:
		if(m_pParam->GetValue().GetType() == wxString(wxT("double")))
			bRetVal = true;
        break;
	case enumGISGPParamDTStringChoice:
	case enumGISGPParamDTStringChoiceEditable:
	default:
		if(m_pParam->GetValue().GetType() == wxString(wxT("string")))
			bRetVal = true;
    }

    m_pParam->SetIsValid(bRetVal);
    if ( bRetVal )
    {
        m_pParam->SetMessage(wxGISEnumGPMessageOk);
        //m_pParam->SetAltered(true);
    }
    else
        m_pParam->SetMessage(wxGISEnumGPMessageError, _("The input data are invalid"));

    return bRetVal;

}

void wxGISDTChoice::UpdateValues(void)
{
	wxString sData = m_choice->GetStringSelection();
    bool bValid(false);
    long dVal(0);
    double dfVal(0);
    switch(m_pParam->GetDataType())
    {
    case enumGISGPParamDTIntegerChoice:
        bValid = sData.ToLong(&dVal);
        break;
    case enumGISGPParamDTDoubleChoice:
        bValid = sData.ToDouble(&dfVal);
        break;
    case enumGISGPParamDTStringChoice:
	case enumGISGPParamDTStringChoiceEditable:
    default:
        bValid = true;
        break;
    }

    m_pParam->SetIsValid(bValid);

    if ( bValid )
    {
        switch(m_pParam->GetDataType())
        {
		case enumGISGPParamDTIntegerChoice:
            m_pParam->SetValue(wxVariant(dVal, wxT("integer")));
            return;
        case enumGISGPParamDTDoubleChoice:
            m_pParam->SetValue(wxVariant(dfVal, wxT("double")));
            return;
		case enumGISGPParamDTStringChoice:
		case enumGISGPParamDTStringChoiceEditable:
		default:
            m_pParam->SetValue(wxVariant(sData, wxT("string")));
            return;
        }
    }
}

void wxGISDTChoice::UpdateControls(void)
{
	if(!m_pParam->GetAltered())
		return;
    int nPos = m_choice->GetCurrentSelection();
	if(m_pParam->GetSelDomainValue() == nPos)
		return;

    wxGISGPValueDomain* poGPValueDomain = dynamic_cast<wxGISGPValueDomain*>(m_pParam->GetDomain());
    if(poGPValueDomain)
	{
		if(poGPValueDomain->GetAltered())
		{
			poGPValueDomain->SetAltered(false);
			m_choice->Clear();
			for(size_t i = 0; i < poGPValueDomain->GetCount(); ++i)
				m_choice->Append( poGPValueDomain->GetName(i));
			nPos = m_pParam->GetSelDomainValue();
		}
		else
		{
			int nParamPos = m_pParam->GetSelDomainValue();
			if(nPos == nParamPos)
				return;
		}
		m_choice->SetSelection( nPos );
	}
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTChoice::OnChoice(wxCommandEvent& event)
{
    int nPos = m_choice->GetCurrentSelection();
    IGPDomain* pDomain = m_pParam->GetDomain();
    if(pDomain)
		m_pParam->SetSelDomainValue(nPos);

	UpdateValues();

	m_pParam->SetHasBeenValidated(false);
    m_pParam->SetAltered(true);
}

void wxGISDTChoice::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTChoiceEditable
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISDTChoiceEditable, wxGISDTChoice)
	EVT_BUTTON(wxID_EDIT, wxGISDTChoiceEditable::OnEdit)
END_EVENT_TABLE()

wxGISDTChoiceEditable::wxGISDTChoiceEditable( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTChoice( pParam, pCatalog, parent, id, pos, size, style )
{
	m_bpButton = new wxBitmapButton( this, wxID_EDIT, wxBitmap(add_to_list_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	this->Layout();
}

wxGISDTChoiceEditable::~wxGISDTChoiceEditable()
{
}

void wxGISDTChoiceEditable::OnEdit(wxCommandEvent& event)
{
	wxPGArrayStringEditorDialog dlg;
    wxGISGPStringDomain* poGPStringDomain = dynamic_cast<wxGISGPStringDomain*>(m_pParam->GetDomain());
    if(!poGPStringDomain)
		return;
	wxArrayString Choices;
	for(size_t i = 0; i < poGPStringDomain->GetCount(); ++i)
		Choices.Add( poGPStringDomain->GetName(i));


    dlg.SetDialogValue( wxVariant(Choices) );
    dlg.Create(this, _("Change value list"), _("Value list"));
    int res = dlg.ShowModal();

    if ( res == wxID_OK && dlg.IsModified() )
    {
        wxVariant value = dlg.GetDialogValue();
        if ( !value.IsNull() )
        {
			Choices = value.GetArrayString();
			poGPStringDomain->Clear();
			for(size_t i = 0; i < Choices.GetCount(); ++i)
				poGPStringDomain->AddString(Choices[i], Choices[i]);
			poGPStringDomain->SetAltered(true);

			//m_pParam->SetValue( value );
            m_pParam->SetAltered(true);
			UpdateControls();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTBool
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISDTBool, wxPanel)
	EVT_CHECKBOX(ID_CHECKBOOL, wxGISDTBool::OnClick)
	EVT_SIZE(wxGISDTBool::OnSize)
	EVT_UPDATE_UI(ID_CHECKBOOL, wxGISDTBool::OnUpdateUI)
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

	m_sFullText = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_pCheckBox = new wxCheckBox( this, ID_CHECKBOOL, wxT("..."), wxDefaultPosition, wxDefaultSize );
    m_pCheckBox->SetValue(pParam->GetValue());
	m_pCheckBox->SetToolTip(m_sFullText);
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
		return m_pParam->GetIsValid();

    m_pParam->SetIsValid(true);
    m_pParam->SetMessage(wxGISEnumGPMessageOk);
	m_pParam->SetHasBeenValidated(true);
    return true;
}

void wxGISDTBool::UpdateControls(void)
{
	if(!m_pParam->GetAltered())
		return;
	if(m_pCheckBox->GetValue() == m_pParam->GetValue().GetBool())
		return;

    m_pCheckBox->SetValue(m_pParam->GetValue().GetBool());
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTBool::UpdateValues(void)
{
    m_pParam->SetValue(m_pCheckBox->GetValue());
}

void wxGISDTBool::OnClick(wxCommandEvent& event)
{ 
    event.Skip(); 
	m_pParam->SetHasBeenValidated(false);
    m_pParam->SetAltered(true);
	UpdateValues();
}

void wxGISDTBool::OnSize(wxSizeEvent& event)
{
    event.Skip(); 
	wxRect rc = GetParent()->GetClientRect();
	wxString sNewLabel = m_pCheckBox->Ellipsize(m_sFullText, wxClientDC(m_pCheckBox), wxELLIPSIZE_END, rc.GetWidth() - 80);
	m_pCheckBox->SetLabel(sNewLabel);
}

void wxGISDTBool::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}
///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTSpatRef
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTSpatRef, wxPanel)
	EVT_BUTTON(wxID_OPEN, wxGISDTSpatRef::OnOpen)
	EVT_UPDATE_UI(ID_SPATREFSTR, wxGISDTSpatRef::OnUpdateUI)
	EVT_TEXT(ID_SPATREFSTR, wxGISDTSpatRef::OnTextChange)
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

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
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
    m_SpaRefTextCtrl = new wxTextCtrl( this, ID_SPATREFSTR, sWKT, wxDefaultPosition, wxSize(100,100)/*wxDefaultSize*/, wxTE_READONLY | wxTE_MULTILINE );/// | wxTE_BESTWRAP | wxTE_NO_VSCROLL
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_SpaRefTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
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
            
			m_pParam->SetHasBeenValidated(false);
			m_pParam->SetAltered(true);
            m_pParam->SetMessage(wxGISEnumGPMessageNone);
			UpdateControls();
        }
    }
}

//validate
bool wxGISDTSpatRef::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

	m_pParam->SetHasBeenValidated(true);

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

void wxGISDTSpatRef::UpdateControls(void)
{
	if(!m_pParam->GetAltered())
		return;
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
        sWKT = wxString(pszWKT, wxConvLocal);
        OGRFree( pszWKT );
    }
    if(m_SpaRefTextCtrl->GetValue() != sWKT)
    {
       m_SpaRefTextCtrl->ChangeValue( sWKT );
        SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
    }
}

void wxGISDTSpatRef::UpdateValues(void)
{
    //m_pParam->SetValue(m_SpaRefTextCtrl->GetValue());
    //m_pParam->SetAltered(true);
}

void wxGISDTSpatRef::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTSpatRef::OnTextChange(wxCommandEvent& event)
{
	m_pParam->SetHasBeenValidated(false);
    m_pParam->SetAltered(true);
	//UpdateValues();
}
///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTMultiParam
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTMultiParam, wxPanel)
	EVT_GRID_CELL_CHANGED(wxGISDTMultiParam::OnCellChange)
	EVT_UPDATE_UI(ID_PPCTRL, wxGISDTMultiParam::OnUpdateUI)
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

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
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

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
}

wxGISDTMultiParam::~wxGISDTMultiParam()
{
}

//validate
bool wxGISDTMultiParam::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

	m_pParam->SetHasBeenValidated(true);

    if(!m_pParam->GetAltered())//TODO: Validate multiparam
        return true;

    m_pParam->SetMessage(wxGISEnumGPMessageRequired, _("The value is required"));
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
	return true;
}

void wxGISDTMultiParam::UpdateControls(void)
{
	if(m_pParam->GetAltered())
		return;
	m_pParam->SetAltered(true);

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
					case enumGISGPParamDTFolderPath:
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

	m_pParam->SetAltered(true);
	m_pParam->SetHasBeenValidated( false );

	IGPParameter* pCellParam = pParam2->GetParameter(event.GetCol(), event.GetRow());
	if(pCellParam)
	{
        pCellParam->SetAltered(true);

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
			case enumGISGPParamDTFolderPath:
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

void wxGISDTMultiParam::UpdateValues(void)
{
	//TODO: Set values from control
}

void wxGISDTMultiParam::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTList
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTList, wxPanel)
	EVT_BUTTON(wxID_ADD, wxGISDTList::OnAdd)
	EVT_UPDATE_UI(ID_LISTCTRL, wxGISDTList::OnUpdateUI)
	EVT_TEXT(ID_LISTCTRL, wxGISDTList::OnTextChange)
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

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );

	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );

    m_TextCtrl = new wxTextCtrl( this, ID_LISTCTRL, GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
    //m_PathTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_TextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_ADD, wxBitmap(add_to_list_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
}

wxGISDTList::~wxGISDTList()
{
}

void wxGISDTList::OnAdd(wxCommandEvent& event)
{
	wxPGArrayStringEditorDialog dlg;
    dlg.SetDialogValue( m_pParam->GetValue() );
    dlg.Create(this, _("Change value list"), _("Value list"));
    int res = dlg.ShowModal();

    if ( res == wxID_OK && dlg.IsModified() )
    {
        wxVariant value = dlg.GetDialogValue();
        if ( !value.IsNull() )
        {
			m_pParam->SetValue( value );
            m_pParam->SetAltered(true);
			UpdateControls();
        }
    }
}

bool wxGISDTList::Validate(void)
{
    if(m_pParam->GetHasBeenValidated())
		return m_pParam->GetIsValid();

	m_pParam->SetHasBeenValidated(true);
    wxArrayString saValues = m_pParam->GetValue().GetArrayString();
    if(saValues.GetCount() <= 0)
	{
		if( m_pParam->GetParameterType() == enumGISGPParameterTypeRequired )
			m_pParam->SetMessage(wxGISEnumGPMessageRequired, _("The value is required"));
		else
			m_pParam->SetMessage(wxGISEnumGPMessageError, _("The input data are invalid"));

		m_pParam->SetIsValid(false);
		return false;
	}
	else
	{
		m_pParam->SetIsValid(true);
	}
    return true;
}

void wxGISDTList::UpdateControls(void)
{
	if(!m_pParam->GetAltered())
		return;
	wxString sVal  = GetValue();
	if(m_TextCtrl->GetValue().CmpNoCase(sVal) == 0)
		return;
    m_TextCtrl->ChangeValue( GetValue() );
    m_TextCtrl->SetInsertionPointEnd( );
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
		sOutput.Append(Arr[i].Trim(true).Trim(false));
		if(i != Arr.GetCount() - 1)
			sOutput.Append(wxT(", "));
	}
	return sOutput;
}

void wxGISDTList::UpdateValues(void)
{
	wxArrayString saValues =  wxStringTokenize(m_TextCtrl->GetValue(), wxString(wxT(",")), /*wxTOKEN_RET_EMPTY*/wxTOKEN_RET_EMPTY_ALL );
	//for(size_t i = 0; i < saValues.GetCount(); ++i)
	//	saValues[i] = saValues[i].Trim(true).Trim(false);	
	
	m_pParam->SetValue( wxVariant(saValues, wxT("list")) );
}

void wxGISDTList::OnUpdateUI(wxUpdateUIEvent &event)
{
	Validate();
    SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());
}

void wxGISDTList::OnTextChange(wxCommandEvent& event)
{
    m_pParam->SetAltered(true);
	UpdateValues();
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISSQLQueryCtrl
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISSQLQueryCtrl, wxPanel)
	EVT_BUTTON(wxID_OPEN, wxGISSQLQueryCtrl::OnOpen)
	EVT_UPDATE_UI(ID_LISTCTRL, wxGISSQLQueryCtrl::OnUpdateUI)
END_EVENT_TABLE()

wxGISSQLQueryCtrl::wxGISSQLQueryCtrl( IGPParameter* pParam, IGxCatalog* pCatalog, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
{
    m_pCatalog = pCatalog;
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_StateBitmap = new wxStaticBitmap( this, wxID_ANY, m_pParam->GetParameterType() == enumGISGPParameterTypeRequired ? m_ImageList.GetIcon(4) : wxNullBitmap , wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_StateBitmap, 0, wxALL, 5 );

	m_sFullDisplayName = m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + wxT(" ") + _("(optional)") : m_pParam->GetDisplayName();
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END );
	m_sParamDisplayName->SetToolTip(m_sFullDisplayName);
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );

	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );

    m_QueryTextCtrl = new wxTextCtrl( this, ID_LISTCTRL, pParam->GetValue(), wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
    //m_QueryTextCtrl->SetDropTarget(new wxFileDropTarget());
	bPathSizer->Add( m_QueryTextCtrl, 1, wxALL|wxEXPAND, 5 );

	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(querysql_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	m_sParamDisplayName->SetLabel(m_sFullDisplayName);
}

wxGISSQLQueryCtrl::~wxGISSQLQueryCtrl()
{
}

void wxGISSQLQueryCtrl::OnOpen(wxCommandEvent& event)
{
	wxGISSQLQueryDialog dlg(this);
	dlg.ShowModal();
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
   //         for(size_t i = 0; i < pDomain->GetCount(); ++i)
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
   //         for(size_t i = 0; i < pDomain->GetCount(); ++i)
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
bool wxGISSQLQueryCtrl::Validate(void)
{
    //if(m_pParam->GetHasBeenValidated())
    //    return true;

    //wxString sPath = m_pParam->GetValue();
    //if(sPath.IsEmpty())
    //{
    //    m_pParam->SetAltered(false);
    //    if(m_pParam->GetParameterType() != enumGISGPParameterTypeRequired)
    //    {
    //        m_pParam->SetIsValid(true);
    //        m_pParam->SetMessage(wxGISEnumGPMessageNone);
    //        return true;
    //    }
    //    else
    //    {
    //        m_pParam->SetIsValid(false);
    //        m_pParam->SetMessage(wxGISEnumGPMessageRequired, _("The value is required"));
    //        return false;
    //    }
    //}
    //if(m_pCatalog)
    //{
    //    IGxObjectContainer* pGxContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    //    IGxObject* pGxObj = pGxContainer->SearchChild(sPath);
    //    if(pGxObj)
    //    {
    //       if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
    //       {
    //           if(m_pParam->GetMessageType() == wxGISEnumGPMessageError)
    //               return false;
    //           m_pParam->SetIsValid(true);
    //           m_pParam->SetMessage(wxGISEnumGPMessageOk);
    //       }
    //       else
    //       {
    //           if(m_pParam->GetMessageType() == wxGISEnumGPMessageError)
    //               return false;
    //           m_pParam->SetIsValid(true);
    //           m_pParam->SetMessage(wxGISEnumGPMessageWarning, _("The output object exists and will be overwritten!"));
    //       }
    //       return true;
    //    }
    //    else
    //    {
    //       if(m_pParam->GetDirection() == enumGISGPParameterDirectionInput)
    //       {
    //            m_pParam->SetIsValid(false);
    //            m_pParam->SetMessage(wxGISEnumGPMessageError, _("The input object doesn't exist"));
    //            return false;
    //       }
    //       else
    //       {
    //           if(m_pParam->GetMessageType() == wxGISEnumGPMessageError)
    //               return false;

    //           m_pParam->SetIsValid(true);
    //           m_pParam->SetMessage(wxGISEnumGPMessageOk);
    //           return true;
    //       }
    //    }
    //}
    return true;
}

void wxGISSQLQueryCtrl::UpdateControls(void)
{
 //   m_PathTextCtrl->ChangeValue( m_pParam->GetValue() );
 //   SetMessage(m_pParam->GetMessageType(), m_pParam->GetMessage());

 //   wxGISGPGxObjectDomain* poDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_pParam->GetDomain());
	//if(poDomain && poDomain->GetCount() > 0)
	//{
	//	IGxObjectFilter* poFilter = poDomain->GetFilter(m_pParam->GetSelDomainValue());
	//	wxFileName oName(m_pParam->GetValue());
	//	if(!oName.IsOk())
	//		return;

	//	for(size_t i = 0; i < poDomain->GetCount(); ++i)
	//	{
	//		poFilter = poDomain->GetFilter(i);
	//		if(poFilter)
	//		{
	//			if(oName.GetExt().CmpNoCase(poFilter->GetExt()) == 0 || poFilter->GetExt() == wxEmptyString)
	//			{
	//				m_pParam->SetSelDomainValue(i);
	//				break;
	//			}
	//		}
	//	}
 //   //Validate();
	//}
}

void wxGISSQLQueryCtrl::UpdateValues(void)
{
	//TODO: Set values from control
}

void wxGISSQLQueryCtrl::OnUpdateUI(wxUpdateUIEvent &event)
{
	UpdateValues();
	Validate();
}