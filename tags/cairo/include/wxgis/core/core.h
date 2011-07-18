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

#include <wx/process.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>

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

/** \class IApplication core.h
    \brief Base class for wxGIS application.
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
	virtual IGISConfig* GetConfig(void) = 0;
	virtual wxString GetAppName(void) = 0;
	virtual wxString GetAppVersionString(void) = 0;
    virtual bool Create(IGISConfig* pConfig) = 0;
    virtual bool SetupLog(wxString sLogPath) = 0;
    virtual bool SetupLoc(wxString sLoc, wxString sLocPath) = 0;
    virtual bool SetupSys(wxString sSysPath) = 0;
    virtual void SetDebugMode(bool bDebugMode) = 0;
};


class wxGISConnectionPointContainer
{
public:
	virtual ~wxGISConnectionPointContainer(void){};
	virtual long Advise(wxEvtHandler* pEvtHandler)
	{
		wxCHECK(pEvtHandler, wxNOT_FOUND);

		std::vector<wxEvtHandler*>::iterator pos = std::find(m_pPointsArray.begin(), m_pPointsArray.end(), pEvtHandler);
		if(pos != m_pPointsArray.end())
			return pos - m_pPointsArray.begin();
		m_pPointsArray.push_back(pEvtHandler);
		return m_pPointsArray.size() - 1;
	}

	virtual void Unadvise(long nCookie)
	{
		wxCHECK_RET(nCookie >= 0 && nCookie < m_pPointsArray.size(), "wrong cookie index");
		m_pPointsArray[nCookie] = NULL;
	}
protected:
	std::vector<wxEvtHandler*> m_pPointsArray;
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
    /** \fn void SetYield(bool bYield = false)
     *  \brief SetYield Yields control to pending messages in the windowing system.

	    This can be useful, for example, when a time-consuming process writes to a text window. Without an occasional yield, the text window will not be updated properly, and other processes will not respond.
     */		
	virtual void SetYield(bool bYield = false) = 0;
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

static bool CreateAndRunThread(wxThread* pThread, wxString sClassName = wxEmptyString, wxString sThreadName = wxEmptyString, int nPriority = WXTHREAD_DEFAULT_PRIORITY)
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
	pThread->SetPriority(nPriority);
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
  //  size_t pos(0);
  //  for(size_t i = 0; i < sInput.Len(); ++i)
  //  {
  //      if(pos == sPass.Len())
  //          pos = 0;
  //      wxChar Symbol = sInput[i] ^ sPass[pos];
  //      char SymbolHi = hibyte(Symbol);
  //      char SymbolLo = lobyte(Symbol);//(Symbol >> 4) & 0xff;
		//sRes += wxDecToHex(SymbolHi);
		//sRes += wxDecToHex(SymbolLo);
  //      pos++;
  //  }
    return sRes;
}

static wxString Decode(wxString sInput, wxString sPass)
{
    wxString sRes;
  //  size_t pos(0);
  //  for(size_t i = 0; i < sInput.Len(); i += 4)
  //  {
  //      if(pos == sPass.Len())
  //          pos = 0;
		//wxString sHex = sInput[i];
		//sHex += sInput[i + 1]; 
  //      char SymbolHi = wxHexToDec(sHex);
  //      sHex = sInput[i + 2];
		//sHex += sInput[i + 3];
  //      char SymbolLo = wxHexToDec(sHex);
  //      wxChar Symbol = (SymbolHi << 8) + SymbolLo;
  //      Symbol = Symbol ^ sPass[pos];
  //      sRes += Symbol;
  //      pos++;
  //  }
    return sRes;
}

/** \enum wxGISEnumTaskStateType core.h
 *  \brief The process task state.
 */
enum wxGISEnumTaskStateType
{
    enumGISTaskUnk = 0,
    enumGISTaskWork,
    enumGISTaskDone,
    enumGISTaskQuered,
    enumGISTaskPaused,
    enumGISTaskError
};

///** \class IStreamReader core.h
// *  \brief The stream reader receives data from reader thread.
// */
//class IStreamReader
//{
//public:
//	virtual ~IStreamReader(void){};
//	virtual void ProcessInput(wxString sInputData) = 0;
//};

/** \class IProcess core.h
 *  \brief The process interface class.
 */
class IProcess
{
public:
	IProcess(wxString sCommand, wxArrayString saParams)
	{
		m_sCommand = sCommand;
        m_saParams = saParams;
		m_nState = enumGISTaskPaused;
	}
	virtual ~IProcess(void){};
    virtual void OnStart(void) = 0;
    virtual void OnCancel(void) = 0;
	virtual void SetState(wxGISEnumTaskStateType nState){m_nState = nState;};
	virtual wxGISEnumTaskStateType GetState(void){return m_nState;};
	virtual wxString GetCommand(void){return m_sCommand;};
	virtual wxArrayString GetParameters(void){return m_saParams;};
    virtual wxDateTime GetBeginTime(void){return m_dtBeg;};
    virtual wxDateTime GetEndTime(void){return m_dtEstEnd;};
protected:
	wxDateTime m_dtBeg;
	wxDateTime m_dtEstEnd;
    wxGISEnumTaskStateType m_nState;
    wxString m_sCommand;
    wxArrayString m_saParams;
};

/** \class IProcessParent core.h
 *  \brief The wxGISProcess parent interface class.
 */
class IProcessParent
{
public:
	virtual ~IProcessParent(void){};
    virtual void OnFinish(IProcess* pProcess, bool bHasErrors) = 0;
//    virtual void ProcessInput(wxString sInputData){};
};

#define DEFINE_SHARED_PTR(x) typedef boost::shared_ptr<x> x##SPtr
#define DEFINE_WEAK_PTR(x) typedef boost::weak_ptr<x> x##WPtr
static void wxNotDeleter(void*);

static wxString DoubleToString(double Val, bool IsLon)
{
	wxString znak;
	if(Val < 0)
	{
		if(IsLon) znak = _(" W");
		else znak = _(" S");
	}
	else
	{
		if(IsLon) znak = _(" E");
		else znak = _(" N");
	}
	Val = fabs(Val);
	int grad = floor(Val);
	int min = floor((Val - grad) * 60);
	int sec = floor((Val - grad - (double) min / 60) * 3600);
	wxString str;
	if(IsLon)
		str.Printf(wxT("%.3d-%.2d-%.2d%s"), grad, min, sec, znak.c_str());
	else
		str.Printf(wxT("%.2d-%.2d-%.2d%s"), grad, min, sec, znak.c_str());
	return str;
};

static double StringToDouble(wxString Val, wxString asterisk)
{
	wxString buff;
	unsigned char counter = 0;
	int grad, min, sec;
	for(size_t i = 0; i < Val.Len(); ++i)
	{
		wxChar ch = Val[i];
		if(ch == '-' || ch == ' ')
		{
			switch(counter)
			{
				case 0:
				grad = wxAtoi(buff.c_str());
				break;
				case 1:
				min = wxAtoi(buff.c_str());
				break;
				case 2:
				sec = wxAtoi(buff.c_str());
				break;
			}
		}
	}
	int mul = -1;
	if(buff == _(" E") || buff == _(" N"))
		mul = 1;
	return ((double) grad + (double)min / 60 + (double)sec / 3600) * mul;
};

static wxString NumberScale(double fScaleRatio)
{
	wxString str = wxString::Format(wxT("%.2f"), fScaleRatio);
	int pos = str.Find(wxT("."));
	if(pos == wxNOT_FOUND)
		pos = str.Len();
	wxString res = str.Right(str.Len() - pos);
	for(size_t i = 1; i < pos + 1; ++i)
	{
		res.Prepend(str[pos - i]);
		if((i % 3) == 0)
			res.Prepend(wxT(" "));
	}
	return res;
};

//std::numeric_limits<double>::epsilon() * 20
//FLT_EPSILON
inline bool IsDoubleEquil( double a, double b, double epsilon = 16 * DBL_EPSILON )
{
  const double diff = a - b;
  return diff > -epsilon && diff <= epsilon;
}


