/////////////////////////////////////////////////////////////////////////////////
//// Name:		RegExValidator.h
//// Purpose:     declaration of regular expression validator class.
//// Author:      Priyank Bolia <http://priyank.co.in>
//// URL:			http://www.priyank.co.in
//// Modified by:
//// Created:     29/12/2005
//// Copyright:   (c) 2005 Priyank Bolia <http://priyank.co.in>
//// Licence:     wxWindows license <http://www.wxwidgets.org/licence3.txt>
/////////////////////////////////////////////////////////////////////////////////

#include "wxgis/framework/wxregexvalidator.h"

wxRegExValidator::wxRegExValidator(wxString regExpString, wxString* valPtr) : m_regEx(regExpString)
{
    m_stringValue = valPtr;
    m_reString = regExpString;
}

wxRegExValidator::~wxRegExValidator(void) {}

wxObject* wxRegExValidator::Clone() const
{
    return new wxRegExValidator(m_reString, m_stringValue);
}

bool wxRegExValidator::TransferToWindow()
{
    if (m_stringValue == NULL)
        return true;

    if (m_regEx.Matches(*m_stringValue))
    {
        return wxTextValidator::TransferToWindow();
    }

    return false;
}

bool wxRegExValidator::TransferFromWindow(void)
{
    wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow;
    wxString tempString = control->GetValue();

    if (m_regEx.Matches(tempString))
    {
        return wxTextValidator::TransferFromWindow();
    }

    return false;
}
