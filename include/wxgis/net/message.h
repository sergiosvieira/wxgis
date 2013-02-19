/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxNetMessage class. Network message class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010,2012  Bishop
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

#include "wxgis/net/net.h"

#include "wx/xml/xml.h"

#define WXNETVER 3

#define BUFF 250000
#define BUFFSIZE sizeof(unsigned char) * BUFF

/** \class wxNetMessage message.h
    \brief A network message class.

	Used in messaging between server and client
*/

class WXDLLIMPEXP_GIS_NET wxNetMessage : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxNetMessage)
public:
    /** \fn wxNetMessage(const char* pBuff, size_t len)
     *  \brief A constructor.
	 *	\param pBuff input buffer
	 *	\param len input buffer size
     */    
    wxNetMessage(const char* pBuff, size_t len);
    wxNetMessage(wxGISNetCommand nNetCmd = enumGISNetCmdUnk, wxGISNetCommandState nNetCmdState = enumGISNetCmdStUnk, short nPriority = enumGISPriorityNormal, int nId = wxNOT_FOUND);

    bool IsOk() const;

    bool operator == ( const wxNetMessage& obj ) const;
    bool operator != (const wxNetMessage& obj) const { return !(*this == obj); };
    bool operator < (const wxNetMessage& obj) const;

    short GetPriority(void) const;
    void SetPriority(short nPriority);
    wxGISNetCommand GetCommand(void) const;
    void SetCommand(wxGISNetCommand nCmd);
    wxGISNetCommandState GetState(void) const;
    void SetState(wxGISNetCommandState nState);

    int GetId(void) const;
    void SetId(int nId);
    
    wxString GetMessage(void) const;
    void SetMessage(const wxString& sMsg);    

    wxXmlNode* GetXMLRoot(void);
    wxXmlNode* GetXMLRoot(void) const;
    const wxScopedCharBuffer::CharType* GetData(void);
    size_t GetLength() const;
protected:
    virtual wxObjectRefData *CreateRefData() const;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const;
};

/** \class wxGISConfigRefData config.h
    \brief The reference data class for wxGISConfig
*/

class  wxNetMessageRefData : public wxObjectRefData
{
    friend class wxNetMessage;
public:
    wxNetMessageRefData( const wxNetMessageRefData& data );
    wxNetMessageRefData( const char* pBuff, size_t len );
    wxNetMessageRefData( wxGISNetCommand nNetCmd = enumGISNetCmdUnk, wxGISNetCommandState nNetCmdState = enumGISNetCmdStUnk, short nPriority = enumGISPriorityNormal, int nId = wxNOT_FOUND );
    virtual ~wxNetMessageRefData();
    
    bool operator == (const wxNetMessageRefData& data) const;
    bool IsOk(void);

    void SetPriority(short nPriority);
    void SetState(wxGISNetCommandState nState);
    void SetMessage(const wxString& sMsg);
    void SetId(int nId);
    void SetCommand(wxGISNetCommand nState);

	const wxScopedCharBuffer GetData(void);
protected:
	wxString FormatXmlString(const wxString &sInputString);
	bool LoadXMLFromStr(const wxString &sData);
    bool SaveXMLToStr(wxString *p_sData);    
    //virtual wxString StrFromBuff(unsigned char* pBuff, size_t nBuffByteSize, size_t nValSize);
protected:
	wxXmlDocument *m_pXmlDocument;
	short m_nPriority;
	wxGISNetCommand m_nCmd;
	wxGISNetCommandState m_nState;
    wxString m_sMessageData;
    int m_nId;
};
