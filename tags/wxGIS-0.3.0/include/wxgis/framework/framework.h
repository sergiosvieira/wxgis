/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  framework header.
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

#pragma once

#include "wxgis/core/core.h"

/** \enum wxGISPluginIDs
    \brief A plugin command or menu command ID enumerator.

    New command ID from user plugin will be set from wxID_HIGHEST + 1 to wxID_HIGHEST + 1049
    New menu item ID from user plugin will be set from wxID_HIGHEST + 1050 to wxID_HIGHEST + 2049
*/
enum wxGISPluginIDs
{
	ID_PLUGINCMD = wxID_HIGHEST + 1,
    ID_MENUCMD = ID_PLUGINCMD + 1050
};	

/** \enum wxGISEnumMouseState
    \brief A mouse state.
*/
enum wxGISEnumMouseState
{
	enumGISMouseNone = 0x0000, 
	enumGISMouseLeftDown = 0x0001,
	enumGISMouseRightDown = 0x0002,
	enumGISMouseWheel = 0x0004
};

/** \enum wxGISEnumStatusBarPanes
    \brief A statusbar panes.

    This is predefined statusbar panes list.
*/
enum wxGISEnumStatusBarPanes
{
	enumGISStatusMain           = 0x0001, 
	enumGISStatusAnimation      = 0x0002,
	enumGISStatusPosition       = 0x0004,
	enumGISStatusPagePosition   = 0x0008,
	enumGISStatusSize           = 0x0010,
	enumGISStatusCapsLock       = 0x0020,
	enumGISStatusNumLock        = 0x0040,
	enumGISStatusScrollLock     = 0x0080,
	enumGISStatusClock          = 0x0100,
	enumGISStatusProgress       = 0x0200
}; 

/** \enum wxGISEnumCommandBars
    \brief A command bar types.

    This is predefined command bar types.
*/
enum wxGISEnumCommandBars
{
	enumGISCBNone = 0x0000,         /**< The type is undefined */
	enumGISCBMenubar = 0x0001,      /**< The type is menubar */
	enumGISCBContextmenu = 0x0002,  /**< The type is context menu */
	enumGISCBSubMenu = 0x0004,      /**< The type is submenu */
	enumGISCBToolbar = 0x0008       /**< The type is toolbar */
};

/** \enum wxGISEnumCommandKind
    \brief A command types.

    This is predefined command types.
*/
enum wxGISEnumCommandKind
{
	enumGISCommandSeparator = wxITEM_SEPARATOR, /**< The type is separator */ 
	enumGISCommandNormal = wxITEM_NORMAL,       /**< The type is normal command */
	enumGISCommandCheck = wxITEM_CHECK,         /**< The type is check command */
	enumGISCommandRadio = wxITEM_RADIO,         /**< The type is radio */
	enumGISCommandMax = wxITEM_MAX,             /**< The type is max available command from wxWidgets (the less command have no analogs in wxWidgets)*/
	enumGISCommandMenu = wxITEM_MAX + 10,       /**< The type is menu */
	enumGISCommandControl,                      /**< The type is control */
    enumGISCommandDropDown                      /**< The type is drop down */
};


/** \class IStatusBar framework.h
    \brief A statusbar interface class.

    This is base class for application statusbar.
*/
class IStatusBar
{
public:
    /** \fn IStatusBar(long style)
     *  \brief A constructor.
     *  \param style Statusbar style (the combination of wxGISEnumStatusBarPanes)
     */	
    IStatusBar(long style) : m_Panes(style){};
    /** \fn virtual ~IStatusBar(void)
     *  \brief A destructor.
     */
    virtual ~IStatusBar(void){};
	//pure virtual
    /** \fn virtual void SetMessage(const wxString& text, int i = 0)
     *  \brief Put message to the status bar.
     *  \param text The message text
     *  \param i The panel index (default 0 - the first panel)
     */	
    virtual void SetMessage(const wxString& text, int i = 0) = 0;
    /** \fn virtual wxString GetMessage
     *  \brief Get message from the status bar.
     *  \param i The panel index (default 0 - the first panel)
     *  \return The message in i-th panel of status bar
     */	
    virtual wxString GetMessage(int i = 0) = 0;
    /** \fn virtual IProgressor* GetAnimation(void)
     *  \brief Get the animated progressor (rotating globe).
     *  \return The IProgressor pointer
     */	
	virtual IProgressor* GetAnimation(void) = 0;
    /** \fn virtual IProgressor* GetProgressor(void)
     *  \brief Get the progressor (progress bar).
     *  \return The IProgressor pointer
     */		
    virtual IProgressor* GetProgressor(void) = 0;
    /** \fn virtual int GetPanePos(wxGISEnumStatusBarPanes nPane)
     *  \brief Get the pane pos by it's type.
     *  \return The pane index
     */	    
    virtual int GetPanePos(wxGISEnumStatusBarPanes nPane) = 0;
	//
    /** \fn virtual WXDWORD GetPanes(void)
     *  \brief Get the status bar style (the combination of wxGISEnumStatusBarPanes)
     *  \return The status bar style 
     */		
    virtual WXDWORD GetPanes(void){return m_Panes;};
    /** \fn virtual void SetPanes(WXDWORD Panes)
     *  \brief Set the pane style (the combination of wxGISEnumStatusBarPanes)
     *  \param Panes The status bar style
     */		
    virtual void SetPanes(WXDWORD Panes){m_Panes = Panes;};
protected:
	WXDWORD m_Panes;    /*!< a status bar style (the combination of wxGISEnumStatusBarPanes)*/
};

class ICommand;
class IApplication;

/** \class IGISCommandBar framework.h
    \brief A IGISCommandBar interface class.

    This is base class for applicaton command bars.    
*/
class IGISCommandBar :
	public IPointer
{
public:
    /** \fn virtual ~IGISCommandBar(void)
     *  \brief A destructor.
     */
    virtual ~IGISCommandBar(void) {};
	virtual void SetName(const wxString& sName) = 0;
	virtual wxString GetName(void) = 0;
	virtual void SetCaption(const wxString& sCaption) = 0;
	virtual wxString GetCaption(void) = 0;
	virtual void SetType(wxGISEnumCommandBars type) = 0;
	virtual wxGISEnumCommandBars GetType(void) = 0;
	virtual void AddCommand(ICommand* pCmd) = 0;
	virtual void RemoveCommand(size_t nIndex) = 0;
	virtual void MoveCommandLeft(size_t nIndex) = 0;
	virtual void MoveCommandRight(size_t nIndex) = 0;
	virtual size_t GetCommandCount(void) = 0;
	virtual ICommand* GetCommand(size_t nIndex) = 0;
	virtual void Serialize(IApplication* pApp, wxXmlNode* pNode, bool bStore = false) = 0;
};

/** \def vector<IGISCommandBar*> COMMANDBARARRAY
    \brief A CommandBar array.
*/
typedef std::vector<IGISCommandBar*> COMMANDBARARRAY;

/** \def vector<wxWindow*> WINDOWARRAY
    \brief A wxWindow array.
*/
typedef std::vector<wxWindow*> WINDOWARRAY;

/** \class IApplication framework.h
    \brief A IApplication interface class.

    This is base class for applicaton.    
*/
class IApplication
{
public:
    /** \fn virtual ~IApplication(void)
     *  \brief A destructor.
     */
    virtual ~IApplication(void) {};
	//pure virtual
	virtual void OnAppAbout(void) = 0;
    virtual void OnAppOptions(void) = 0;
	virtual IStatusBar* GetStatusBar(void) = 0;
	virtual IGISConfig* GetConfig(void) = 0;
	virtual IGISCommandBar* GetCommandBar(wxString sName) = 0;
	virtual void RemoveCommandBar(IGISCommandBar* pBar) = 0;
	virtual bool AddCommandBar(IGISCommandBar* pBar) = 0;
	virtual void Customize(void) = 0;
	virtual ICommand* GetCommand(long CmdID) = 0;
	virtual ICommand* GetCommand(wxString sCmdName, unsigned char nCmdSubType) = 0;
	virtual void ShowStatusBar(bool bShow) = 0;
	virtual bool IsStatusBarShown(void) = 0;
	virtual void ShowToolBarMenu(void) = 0;
	virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true) = 0;
	virtual bool IsApplicationWindowShown(wxWindow* pWnd) = 0;
	virtual const WINDOWARRAY* const GetChildWindows(void) = 0;
	virtual void RegisterChildWindow(wxWindow* pWnd) = 0;
	virtual void UnRegisterChildWindow(wxWindow* pWnd) = 0;
	virtual wxString GetAppName(void) = 0;
	virtual wxString GetAppVersionString(void) = 0;
    virtual bool Create(IGISConfig* pConfig) = 0;
    virtual bool SetupLog(wxString sLogPath) = 0;
    virtual bool SetupLoc(wxString sLoc, wxString sLocPath) = 0;
    virtual bool SetupSys(wxString sSysPath) = 0;
    virtual void SetDebugMode(bool bDebugMode) = 0;
    //events
	virtual void OnMouseDown(wxMouseEvent& event) = 0;
	virtual void OnMouseUp(wxMouseEvent& event) = 0;
	virtual void OnMouseDoubleClick(wxMouseEvent& event) = 0;
	virtual void OnMouseMove(wxMouseEvent& event) = 0;
};

/** \class ICommand framework.h
    \brief A ICommand interface class.

    This is base class for command.    
*/
class ICommand :
	public wxObject
{
public:
    /** \fn ICommand(void)
     *  \brief A constructor.
     */	
    ICommand(void) : m_subtype(0){};
    /** \fn virtual ~ICommand(void)
     *  \brief A destructor.
     */
    virtual ~ICommand(void) {};
	//pure virtual
	virtual wxIcon GetBitmap(void) = 0;
	virtual wxString GetCaption(void) = 0;
	virtual wxString GetCategory(void) = 0;
	virtual bool GetChecked(void) = 0;
	virtual bool GetEnabled(void) = 0;
	virtual wxString GetMessage(void) = 0;
	virtual wxGISEnumCommandKind GetKind(void) = 0;
	virtual void OnClick(void) = 0;
	virtual bool OnCreate(IApplication* pApp) = 0;
	virtual wxString GetTooltip(void) = 0;
	virtual unsigned char GetCount(void) = 0;
	virtual void SetID(long nID){m_CommandID = nID;};
	virtual long GetID(void){return m_CommandID;};
	//
	virtual void SetSubType(unsigned char SubType){m_subtype = SubType;};
	virtual unsigned char GetSubType(void){return m_subtype;};
protected:
	unsigned char m_subtype;
	long m_CommandID;
};

/** \def vector<ICommand*> COMMANDARRAY
    \brief A ICommand array.
*/
typedef std::vector<ICommand*> COMMANDARRAY;

class IToolBarControl
{
public:
    /** \fn virtual ~IToolBarControl(void)
     *  \brief A destructor.
     */
    virtual ~IToolBarControl(void) {};
	virtual void Activate(IApplication* pApp) = 0;
	virtual void Deactivate(void) = 0;
};

/** \class IToolControl framework.h
    \brief A IToolControl interface class.

    This is base class for tool control.    
*/
class IToolControl
{
public:
    /** \fn virtual ~IToolControl(void)
     *  \brief A destructor.
     */
    virtual ~IToolControl(void) {};
	virtual IToolBarControl* GetControl(void) = 0;
	virtual wxString GetToolLabel(void) = 0;
	virtual bool HasToolLabel(void) = 0;
};

/** \class ITool framework.h
    \brief A ITool interface class.

    This is base class for tool.    
*/
class ITool :
	public ICommand
{
public:
    /** \fn virtual ~ITool(void)
     *  \brief A destructor.
     */
    virtual ~ITool(void) {};
	virtual void SetChecked(bool bCheck) = 0;
	virtual wxCursor GetCursor(void) = 0;
	virtual void OnMouseDown(wxMouseEvent& event) = 0;
	virtual void OnMouseUp(wxMouseEvent& event) = 0;
	virtual void OnMouseMove(wxMouseEvent& event) = 0;
	virtual void OnMouseDoubleClick(wxMouseEvent& event) = 0;
};

/** \class IDropDownCommand framework.h
    \brief A IDropDownCommand interface class.

    This is base class for drop down command.    
*/
class IDropDownCommand
{
public:
    /** \fn virtual ~IDropDownCommand(void)
     *  \brief A destructor.
     */
    virtual ~IDropDownCommand(void) {};
    /** \fn virtual wxMenu* GetDropDownMenu(void)
     *  \brief Return DropDown Menu to show under button.
     *  \return The menu pointer
     */
	virtual wxMenu* GetDropDownMenu(void) = 0;
    /** \fn virtual void OnDropDownCommand(int nID)
     *  \brief Execute the DropDown menu command
     *  \param nID The command ID to execute. The command ID will be from ID_MENUCMD to 2049. See #wxGISPluginIDs
     */	
    virtual void OnDropDownCommand(int nID) = 0;
};

/** \class IPropertyPage framework.h
    \brief A IPropertyPage interface class for property pages in options dialogs.

    This is base class for property page in options dialog.    
*/
class IPropertyPage : public wxPanel
{
public:
    /** \fn virtual ~IPropertyPage(void)
     *  \brief A destructor.
     */
    virtual ~IPropertyPage(void) {};
    /** \fn virtual bool Create(IApplication* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
     *  \brief A Create function.
     *  \param application The main app pointer
     *  \param parent The parent window pointer
     *  \param id The window ID
     *  \param pos The window position
     *  \param size The window size
     *  \param style The window style
     *  \param name The window name
     *  \return true if creation is success, otherwize false
     *  
     *  The PropertyPage is two step creating.
     */
    virtual bool Create(IApplication* application, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("panel")) = 0;
    /** \fn virtual wxString GetName(void)
     *  \brief Get the property page name.
     *  \return The property page name
     */
    virtual wxString GetPageName(void) = 0;
    /** \fn  virtual void Apply(void)
     *  \brief Executed when OK is pressed
     */
    virtual void Apply(void) = 0;
};
