///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __sqlquerydialog__
#define __sqlquerydialog__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISSQLQueryDialog
///////////////////////////////////////////////////////////////////////////////
class wxGISSQLQueryDialog : public wxDialog 
{
	private:
	
	protected:
		enum
		{
			ID_WXGISSQLQUERYDIALOG = 1000,
			ID_M_SELLAYERCOMBOBOX,
			ID_M_METHODSELCOMBOBOX,
			ID_M_BEQBTTON,
			ID_M_GREATERANDEQBUTTON,
			ID_M_ANDBUTTON,
			ID_M_SMALLERANDEQBUTTON,
			ID_M_ORBUTTON,
			ID_M_UNDERLINEBUTTON,
			ID_M_NOTBUTTON,
			ID_M_PERCENTBUTTON,
			ID_M_ISBUTTON,
			ID_M_SELECTSTATICTEXT,
			ID_M_CLEARBUTTON,
			ID_M_CHECKBUTTON,
			ID_M_HELPBUTTON,
			ID_M_LOADBUTTON,
			ID_M_SAVEBUTTON,
		};
		
		wxBoxSizer* bMainSizer;
		wxFlexGridSizer* fgSizer1;
		wxStaticText* m_staticText1;
		wxComboBox* m_SelLayerComboBox;
		wxStaticText* m_staticText2;
		wxComboBox* m_MethodSelComboBox;
		wxTextCtrl* m_textCtrl8;
		wxButton* m_button15;
		wxButton* m_button16;
		wxButton* m_button17;
		wxButton* m_bEqBtton;
		wxButton* m_NEqButton;
		wxButton* m_LikeButton;
		wxButton* m_GreaterButton;
		wxButton* m_GreaterAndEqButton;
		wxButton* m_AndButton;
		wxButton* m_SmallerButton;
		wxButton* m_SmallerAndEqButton;
		wxButton* m_OrButton;
		wxButton* m_UnderLineButton;
		wxButton* m_button28;
		wxButton* m_NotButton;
		wxButton* m_PercentButton;
		wxButton* m_IsButton;
		wxButton* m_button32;
		wxTextCtrl* m_textCtrl9;
		wxButton* m_button33;
		wxStaticText* m_SelectStaticText;
		wxTextCtrl* m_textCtrl10;
		wxBoxSizer* bButtonsSizer;
		wxButton* m_ClearButton;
		wxButton* m_CheckButton;
		wxButton* m_HelpButton;
		wxButton* m_LoadButton;
		wxButton* m_SaveButton;
		wxStaticLine* m_staticline1;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerApply;
		wxButton* m_sdbSizerCancel;
	
	public:
		
		wxGISSQLQueryDialog( wxWindow* parent, wxWindowID id = ID_WXGISSQLQUERYDIALOG, const wxString& title = _("Select by attributes"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 477,590 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~wxGISSQLQueryDialog();
	
};

#endif //__sqlquerydialog__
