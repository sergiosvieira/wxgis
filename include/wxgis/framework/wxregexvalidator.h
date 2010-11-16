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
#pragma once

#include "wxgis/framework/framework.h"

#include <wx/valtext.h>
#include <wx/regex.h>
#include <wx/valgen.h>

class WXDLLIMPEXP_GIS_FRW wxRegExValidator :
	public wxTextValidator
{
protected:
   wxRegEx m_regEx;
   wxString m_reString;
public:
	wxRegExValidator(wxString regExpString, wxString* valPtr = NULL);
	~wxRegExValidator(void);
    wxObject* Clone() const;
    virtual bool TransferToWindow();
    virtual bool TransferFromWindow(void);
};
