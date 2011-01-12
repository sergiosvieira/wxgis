/******************************************************************************
 * Project:  wxGIS (GIS)
 * Purpose:  core header.
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
#pragma once

#include "wxgis/base.h"

#include "wx/process.h"

#define wgDELETE(p,func) if(p != NULL) {p->func; delete p; p = NULL;}
#define wsDELETE(p) if(p != NULL) {p->Release(); p = NULL;}
#define wgWX2MB(x)  wxConvCurrent->cWX2MB(x)
#define wgMB2WX(x)  wxConvCurrent->cMB2WX(x)

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
    virtual wxXmlNode* GetConfigNode(wxGISEnumConfigKey Key, wxString sPath) = 0;
	virtual wxXmlNode* CreateConfigNode(wxGISEnumConfigKey Key, wxString sPath, bool bUniq) = 0;
    virtual wxXmlNode* GetConfigNode(wxString sPath, bool bCreateInCU, bool bUniq) = 0;

	//virtual wxXmlNode* GetRootNodeByName(wxString sApp, bool bUser, wxString sName) = 0;
	//virtual wxXmlNode* GetNodeByName(wxXmlNode* pRoot, wxString sName) = 0;
	//virtual wxXmlNode* GetXmlConfig(wxString sApp, bool bUser) = 0;
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
		//wxCriticalSectionLocker locker(m_PointsArrayCriticalSection);
		m_pPointsArray[nCookie] = NULL;
	}
protected:
	std::vector<wxObject*> m_pPointsArray;
	//wxCriticalSection m_PointsArrayCriticalSection;
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

enum wxGISEnumMessageType
{
	enumGISMessageUnk,
	enumGISMessageErr,
	enumGISMessageNorm,
	enumGISMessageQuestion,
    enumGISMessageInfo,
    enumGISMessageWarning,
    enumGISMessageTitle,
    enumGISMessageOK
};

/** \class IProgressor core.h
    \brief A progressor interface class.

    This is base class for progressors.
*/
class IProgressor 
{
public:
    /** \fn virtual ~IProgressor(void)
     *  \brief A destructor.
     */
	virtual ~IProgressor(void){};
	//pure virtual
    /** \fn bool Show(bool bShow)
     *  \brief Show/hide progressor.
     *  \param bShow The indicator to show (true) or hide (false) progressor
     *  \return The success of function execution
     */	
    virtual bool Show(bool bShow) = 0;
    //
    /** \fn virtual void SetRange(int range)
     *  \brief Set progressor range.
     *  \param range The progressor value range
     */	
    virtual void SetRange(int range) = 0;
    /** \fn int GetRange(void)
     *  \brief Set progressor range.
     *  \return The current progressor range
     */	
    virtual int GetRange(void) = 0;
    /** \fn void SetValue(int value)
     *  \brief Set progressor position.
     *  \param value The progressor current value
     */	    
    virtual void SetValue(int value) = 0;
    /** \fn int GetValue(void)
     *  \brief Get progressor position.
     *  \return The current progressor position
     */	   
    virtual int GetValue(void) = 0;
    //
    /** \fn void Play(void)
     *  \brief Start undefined progressor state.
     */	 	
    virtual void Play(void) = 0;
    /** \fn void Stop(void)
     *  \brief Stop undefined progressor state.
     */		
    virtual void Stop(void) = 0;
};

/** \class ITrackCancel core.h
    \brief A TrackCancel interface class.

    This is base class for TrackCancel classes. 
    The TrackCancel provide ability to stop by ESC, Cancel and etc. execution of some process or function
*/
class ITrackCancel
{
public:
    /** \fn ITrackCancel(void)
     *  \brief A constructor.
     */	
    ITrackCancel(void) : m_bIsCanceled(false), m_pProgressor(NULL){};
    /** \fn virtual ~ITrackCancel(void)
     *  \brief A destructor.
     */
    virtual ~ITrackCancel(void){};
	virtual void Cancel(void){m_bIsCanceled = true;};
	virtual bool Continue(void){return !m_bIsCanceled;};
	virtual void Reset(void){m_bIsCanceled = false;};
	virtual IProgressor* GetProgressor(void){return m_pProgressor;};
	virtual void SetProgressor(IProgressor* pProgressor){m_pProgressor = pProgressor; };
	virtual void PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType){};
protected:
	bool m_bIsCanceled;
	IProgressor* m_pProgressor;
};

static bool CreateAndRunThread(wxThread* pThread, wxString sClassName = wxEmptyString, wxString sThreadName = wxEmptyString)
{
	if(!pThread)
		return false;
	if(sClassName.IsEmpty())
		sClassName = wxString(_("wxGISCore"));
    if ( pThread->Create() != wxTHREAD_NO_ERROR )
    {
		wxLogError(_("%s: Can't create %s Thread!"), sClassName.c_str(), sThreadName.c_str());
		return false;
    }
	if(pThread->Run() != wxTHREAD_NO_ERROR )
    {
		wxLogError(_("%s: Can't run %s Thread!"), sClassName.c_str(), sThreadName.c_str());
		return false;
    }
	return true;
}

#define hibyte(a) ((a>>8) & 0xFF)
#define lobyte(a) ((a) & 0xFF)

static wxString Encode(wxString sInput, wxString sPass)
{
    wxString sRes;
    size_t pos(0);
    for(size_t i = 0; i < sInput.Len(); i++)
    {
        if(pos == sPass.Len())
            pos = 0;
        wxChar Symbol = sInput[i] ^ sPass[pos];
        char SymbolHi = hibyte(Symbol);
        char SymbolLo = lobyte(Symbol);//(Symbol >> 4) & 0xff;
		sRes += wxDecToHex(SymbolHi);
		sRes += wxDecToHex(SymbolLo);
        pos++;
    }
    return sRes;
}

static wxString Decode(wxString sInput, wxString sPass)
{
    wxString sRes;
    size_t pos(0);
    for(size_t i = 0; i < sInput.Len(); i += 4)
    {
        if(pos == sPass.Len())
            pos = 0;
		wxString sHex = sInput[i];
		sHex += sInput[i + 1]; 
        char SymbolHi = wxHexToDec(sHex);
        sHex = sInput[i + 2];
		sHex += sInput[i + 3];
        char SymbolLo = wxHexToDec(sHex);
        wxChar Symbol = (SymbolHi << 8) + SymbolLo;
        Symbol = Symbol ^ sPass[pos];
        sRes += Symbol;
        pos++;
    }
    return sRes;
}

/** \enum wxGISEnumTaskStateType core.h
 *  \brief The process task state.
 */
enum wxGISEnumTaskStateType
{
    enumGISTaskWork = 1,
    enumGISTaskDone,
    enumGISTaskQuered,
    enumGISTaskPaused,
    enumGISTaskError
};

/** \class IStreamReader core.h
 *  \brief The stream reader receives data from reader thread.
 */
class IStreamReader
{
public:
	virtual ~IStreamReader(void){};
	virtual void ProcessInput(wxString sInputData) = 0;
};

/** \class IProcess core.h
 *  \brief The process interface class.
 */
class IProcess : public wxProcess
{
public:
	IProcess(wxString sCommand) : wxProcess(wxPROCESS_REDIRECT)
	{
		m_sCommand = sCommand;
		m_nState = enumGISTaskPaused;
	}
	virtual ~IProcess(void){};
    virtual void OnStart(long nPID) = 0;
    virtual void OnCancel(void) = 0;
	virtual void SetState(wxGISEnumTaskStateType nState){m_nState = nState;};
	virtual wxGISEnumTaskStateType GetState(void){return m_nState;};
	virtual wxString GetCommand(void){return m_sCommand;};
    virtual wxDateTime GetStart(void){return m_dtBeg;};
protected:
	wxDateTime m_dtBeg;
    wxGISEnumTaskStateType m_nState;
    wxString m_sCommand;
};

/** \class IProcessParent core.h
 *  \brief The wxGISProcess parent interface class.
 */
class IProcessParent
{
public:
	virtual ~IProcessParent(void){};
    virtual void OnFinish(IProcess* pProcess, bool bHasErrors) = 0;
};