/******************************************************************************
 * Project:  wxGIS (GIS)
 * Purpose:  core header.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010 Bishop
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

static bool CreateAndRunThread(wxThread* pThread, wxString sClassName = wxEmptyString, wxString sThreadName = wxEmptyString)
{
	if(!pThread)
		return false;
	if(sClassName.IsEmpty())
		sClassName = wxString(_("wxGISCore"));
    if ( pThread->Create() != wxTHREAD_NO_ERROR )
    {
		wxLogError(_("%s: Can't create %s Thread!"), sClassName, sThreadName);
		return false;
    }
	if(pThread->Run() != wxTHREAD_NO_ERROR )
    {
		wxLogError(_("%s: Can't run %s Thread!"), sClassName, sThreadName);
		return false;
    }
	return true;
}


//static unsigned int GetValue(wxChar hex)
//{ 
//    if( (hex > 47) & (hex < 57) ) 
//        return hex - 48; 
//    else 
//        return hex - 88; 
//} 
//
//static wxString HexToChar(wxString sHex) 
//{ 
//    wxString sRes;
//    for(size_t i = 0; i < sHex.Len() / 2; i++) 
//        sRes += GetValue(sHex[i * 2]) * 16 + GetValue(sHex[i * 2 + 1]); 
//    return sRes;
//} 

//void str_to_hex(char *str, char *buf) 
//{ 
//const wxChar hex[16] = "0123456789abcdef";
//char *p, *t; 
//t = buf; 
//for(p = str; *p; ++p) { 
//*(t++) = hex[(unsigned)*p >> 4]; 
//*(t++) = hex[(unsigned)*p & 15]; 
//*(t++) = ' '; 
//} 


static wxString Encode(wxString sInput, wxString sPass)
{
    wxString sRes;
    size_t pos(0);
    for(size_t i = 0; i < sInput.Len(); i++)
    {
        if(pos == sPass.Len())
            pos = 0;
        wxChar Symbol = sInput[i] ^ sPass[pos];
        sRes += wxString::Format(wxT("%02x"), Symbol);
        pos++;
    }
    return sRes;
}

static wxChar xtod(wxChar c)
{
    if (c >= '0' && c <= '9') 
        return c - '0';
    if (c >= 'A' && c <= 'F') 
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') 
        return c - 'a' + 10;
    return 0;        // not Hex digit
}

static wxString Decode(wxString sInput, wxString sPass)
{
    wxString sRes;
    size_t pos(0);
    for(size_t i = 0; i < sInput.Len(); i += 2)
    {
        if(pos == sPass.Len())
            pos = 0;
        wxChar Symbol = xtod(sInput[i]) * 16 + xtod(sInput[i + 1]); 
        Symbol = Symbol ^ sPass[pos];
        sRes += Symbol;
        pos++;
    }
    return sRes;
}