/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxNetMessage class. Network message class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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

#include "wxgis/networking/networking.h"

#define WXNETVER 2

#define BUFF 1000

#define WXNETMESSAGE1 wxT("<msg ver=\"%d\" st=\"%d\" prio=\"%u\" dst=\"%s\">%s</msg>")
#define WXNETMESSAGE2 wxT("<msg ver=\"%d\" st=\"%d\" prio=\"%u\" dst=\"%s\"/>")

/** \class wxNetMessage message.h
    \brief A network message class.

	Used in messaging between server and client
*/

class WXDLLIMPEXP_GIS_NET wxNetMessage : public INetMessage
{
public:
    /** \fn wxNetMessage(unsigned char* pBuff, size_t nSize)
     *  \brief A constructor.
	 *	\param pBuff input buffer
	 *	\param nSize input buffer size
     */    
    wxNetMessage(unsigned char* pBuff, size_t nSize);
    wxNetMessage(wxString sMsgData);
    /** \fn wxNetMessage(void)
     *  \brief A default constructor.
     */    
    wxNetMessage(wxGISMessageState nState = enumGISMsgStUnk, short nPriority = enumGISPriorityNormal, wxString sDst = wxEmptyString);
    /** \fn virtual ~wxNetMessage(void)
     *  \brief A destructor.
     */
	virtual ~wxNetMessage(void);
    virtual const short GetPriority(void);
    virtual void SetPriority(short nPriority);
////    //virtual bool operator< (const INetMessage& msg) const;
////    //virtual INetMessage& operator= (const INetMessage& oSource);
    virtual bool IsOk(void);
    virtual const wxGISMessageDirection GetDirection(void);
    virtual void SetDirection(wxGISMessageDirection nDirection);
    virtual const wxGISMessageState GetState(void);
    virtual void SetState(wxGISMessageState nState);
	virtual void SetDestination(wxString sDst);
	virtual const wxString GetDestination(void);
    virtual wxXmlNode* GetRoot(void);
    virtual const unsigned char* GetData(void);
    virtual const size_t GetDataLen(void);
	static wxString FormatXmlString(wxString sInputString)
	{
		sInputString.Replace(wxT("&"), wxT("&amp;"));
		sInputString.Replace(wxT("\""), wxT("&quot;"));
		sInputString.Replace(wxT("'"), wxT("&apos;"));
		sInputString.Replace(wxT(">"), wxT("&gt;"));
		sInputString.Replace(wxT("<"), wxT("&lt;"));
		sInputString.Replace(wxT("/"), wxT("&frasl;"));
		return sInputString;
	};
protected:
	virtual wxString StrFromBuff(unsigned char* pBuff, size_t nBuffByteSize, size_t nValSize);
	virtual bool LoadXMLFromStr(wxString sData);
	virtual bool SavedXMLToStr(wxString *p_sData);
protected:
	wxXmlDocument *m_pXmlDocument;
	bool m_bIsOk;
	short m_nPriority;
	wxGISMessageDirection m_nDirection;
	wxGISMessageState m_nState;
	wxString m_sDst;
	//wxString m_sMessage;
	wxString m_sData;
    unsigned char m_cData[BUFF];
};
//
////std::priority_queue<Message, std::deque<Message> > pq;


