/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  framework header.
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

#pragma once

#include "wxgis/base.h"

#define wgDELETE(p,func) if(p != NULL) {p->func; delete p; p = NULL;}
#define wsDELETE(p) if(p != NULL) {p->Release(); p = NULL;}
#define wgWX2MB(x)  wxConvCurrent->cWX2MB(x)
#define wgMB2WX(x)  wxConvCurrent->cMB2WX(x)

enum wxGISEnumMouseState
{
	enumGISMouseNone = 0x0000, 
	enumGISMouseLeftDown = 0x0001,
	enumGISMouseRightDown = 0x0002,
	enumGISMouseWheel = 0x0004
};

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

enum wxGISEnumCommandBars
{
	enumGISCBNone = 0x0000, 
	enumGISCBMenubar = 0x0001,
	enumGISCBContextmenu = 0x0002,
	enumGISCBSubMenu = 0x0004,
	enumGISCBToolbar = 0x0008
};

enum wxGISEnumCommandKind
{
	enumGISCommandSeparator = wxITEM_SEPARATOR, 
	enumGISCommandNormal = wxITEM_NORMAL,
	enumGISCommandCheck = wxITEM_CHECK,
	enumGISCommandRadio = wxITEM_RADIO,
	enumGISCommandMax = wxITEM_MAX,
	enumGISCommandMenu = wxITEM_MAX + 10,
	enumGISCommandControl
};

enum wxGISEnumConfigKey
{
	enumGISHKLM = 0x0000, 
	enumGISHKCU = 0x0001,
	enumGISHKCC = 0x0002,
	enumGISHKCR = 0x0004
};

class IGISConfig
{
public:
	virtual ~IGISConfig(void){};
	//pure virtual
	//virtual wxXmlNode* GetRootNodeByName(wxString sApp, bool bUser, wxString sName) = 0;
	//virtual wxXmlNode* GetNodeByName(wxXmlNode* pRoot, wxString sName) = 0;
	//virtual wxXmlNode* GetXmlConfig(wxString sApp, bool bUser) = 0;
    virtual wxXmlNode* GetConfigNode(wxGISEnumConfigKey Key, wxString sPath) = 0;
	virtual wxXmlNode* CreateConfigNode(wxGISEnumConfigKey Key, wxString sPath, bool bUniq) = 0;
};

class IProgressor
{
public:
	virtual ~IProgressor(void){};
	//pure virtual
	virtual bool Show(bool bShow) = 0;
};

class IStatusBar
{
public:
	IStatusBar(long style) : m_Panes(style){};
	virtual ~IStatusBar(void){};
	//pure virtual
	virtual void SetMessage(const wxString& text, int i = 0) = 0;
	virtual wxString GetMessage(int i = 0) = 0;
	virtual IProgressor* GetAnimation(void) = 0;
	virtual IProgressor* GetProgressor(void) = 0;
    virtual int GetPanePos(wxGISEnumStatusBarPanes nPane) = 0;
	//
	virtual WXDWORD GetPanes(void){return m_Panes;};
	virtual void SetPanes(WXDWORD Panes){m_Panes = Panes;};
protected:
	WXDWORD m_Panes;
};

class IConnectionPointContainer
{
public:
	virtual ~IConnectionPointContainer(void){};
	virtual long Advise(wxObject* pObject)
	{
		if(pObject == NULL)
			return -1;
		m_pPointsArray.push_back(pObject);
		return m_pPointsArray.size() - 1;
	}
	virtual void Unadvise(long nCookie)
	{
		if(nCookie < 0 || m_pPointsArray.size() <= nCookie)
			return;
		wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
		m_pPointsArray[nCookie] = NULL;
	}
protected:
	std::vector<wxObject*> m_pPointsArray;
	wxCriticalSection m_PointsArrayCriticalSection;
};

class IPointer
{
public:
	IPointer(void) : m_RefCount(0){};
	virtual ~IPointer(void){};
	virtual wxInt32 Reference(void){return m_RefCount++;};
	virtual wxInt32 Dereference(void){return m_RefCount--;};
	virtual wxInt32 Release(void)
	{
		Dereference();
		if(m_RefCount <= 0)
		{
			delete this;
			return 0;
		}
		else
			return m_RefCount;
	}
protected:
	wxInt32 m_RefCount;
};

class ITrackCancel
{
public:
	ITrackCancel(void) : m_bIsCanceled(false){};
	virtual ~ITrackCancel(void){};
	virtual void Cancel(void){m_bIsCanceled = true;};
	virtual bool Continue(void){return !m_bIsCanceled;};
	virtual void Reset(void){m_bIsCanceled = false;};
	virtual IProgressor* GetProgressor(void){return m_pProgressor;};
	virtual void SetProgressor(IProgressor* pProgressor){m_pProgressor = pProgressor; };
protected:
	bool m_bIsCanceled;
	IProgressor* m_pProgressor;
};

class ICommand;
class IApplication;

class IGISCommandBar :
	public IPointer
{
public:
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

typedef std::vector<IGISCommandBar*> COMMANDBARARRAY;
typedef std::vector<wxWindow*> WINDOWARRAY;

class IApplication
{
public:
	virtual ~IApplication(void) {};
	//pure virtual
	virtual void OnAppAbout(void) = 0;
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
	virtual WINDOWARRAY* GetChildWindows(void) = 0;
	virtual void RegisterChildWindow(wxWindow* pWnd) = 0;
};

class ICommand :
	public wxObject
{
public:
	ICommand(void) : m_subtype(0){};
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

typedef std::vector<ICommand*> COMMANDARRAY;

class IToolBarControl
{
public:
	virtual ~IToolBarControl(void) {};
	virtual void Activate(IApplication* pApp) = 0;
	virtual void Deactivate(void) = 0;
};

class IToolControl :
	public ICommand
{
public:
	virtual ~IToolControl(void) {};
	virtual IToolBarControl* GetControl(void) = 0;
	virtual wxString GetToolLabel(void) = 0;
	virtual bool HasToolLabel(void) = 0;
};

class ITool :
	public ICommand
{
public:
	virtual ~ITool(void) {};
	virtual void SetChecked(bool bCheck) = 0;
	virtual wxCursor GetCursor(void) = 0;
	virtual void OnMouseDown(wxMouseEvent& event) = 0;
	virtual void OnMouseUp(wxMouseEvent& event) = 0;
	virtual void OnMouseMove(wxMouseEvent& event) = 0;
	virtual void OnMouseDoubleClick(wxMouseEvent& event) = 0;
};