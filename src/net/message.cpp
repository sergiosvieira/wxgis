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

#include "wxgis/net/message.h"

#include <wx/stream.h>
#include <wx/sstream.h>

//-----------------------------------------------------------------------------
// wxNetMessage
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxNetMessage, wxObject)

wxNetMessage::wxNetMessage(const char* pBuff, size_t len)
{
     m_refData = new wxNetMessageRefData(pBuff, len);
}

wxNetMessage::wxNetMessage(wxGISNetCommand nNetCmd, wxGISNetCommandState nNetCmdState, short nPriority, int nId)
{
     m_refData = new wxNetMessageRefData(nNetCmd, nNetCmdState, nPriority, nId);
}

wxObjectRefData *wxNetMessage::CreateRefData() const
{
    return new wxNetMessageRefData();
}

wxObjectRefData *wxNetMessage::CloneRefData(const wxObjectRefData *data) const
{
    return new wxNetMessageRefData(*(wxNetMessageRefData *)data);
}

bool wxNetMessage::IsOk() const
{ 
    return m_refData != NULL && ((wxNetMessageRefData *)m_refData)->IsOk(); 
}

short wxNetMessage::GetPriority(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nPriority;
}

void wxNetMessage::SetPriority(short nPriority)
{
    ((wxNetMessageRefData *)m_refData)->SetPriority( nPriority );
}

wxGISNetCommand wxNetMessage::GetCommand(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nCmd;
}

void wxNetMessage::SetCommand(wxGISNetCommand nCmd)
{
    ((wxNetMessageRefData *)m_refData)->SetCommand( nCmd );
}

wxString wxNetMessage::GetMessage(void) const
{
    wxXmlNode* pRoot = GetXMLRoot();
    if(pRoot)
        return pRoot->GetAttribute(wxT("msg"), wxEmptyString);
    return wxEmptyString;
}

void wxNetMessage::SetMessage(const wxString& sMsg)
{
    ((wxNetMessageRefData *)m_refData)->SetMessage( sMsg );
}

wxGISNetCommandState wxNetMessage::GetState(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nState;
}

void wxNetMessage::SetState(wxGISNetCommandState nState)
{
    ((wxNetMessageRefData *)m_refData)->SetState( nState );
}

int wxNetMessage::GetId(void) const
{
    return ((wxNetMessageRefData *)m_refData)->m_nId;
}

void wxNetMessage::SetId(int nId)
{
    ((wxNetMessageRefData *)m_refData)->SetId( nId );
}

bool wxNetMessage::operator< (const wxNetMessage& msg) const
{
    return GetPriority() < msg.GetPriority();
}

bool wxNetMessage::operator == ( const wxNetMessage& obj ) const
{
    if (m_refData == obj.m_refData)
        return true;
    if (!m_refData || !obj.m_refData)
        return false;

    return ( *(wxNetMessageRefData*)m_refData == *(wxNetMessageRefData*)obj.m_refData );
}

wxXmlNode* wxNetMessage::GetXMLRoot(void)
{
    if(((wxNetMessageRefData *)m_refData)->m_pXmlDocument)
        return ((wxNetMessageRefData *)m_refData)->m_pXmlDocument->GetRoot();
    return NULL;
}

wxXmlNode* wxNetMessage::GetXMLRoot(void) const
{
    if(((wxNetMessageRefData *)m_refData)->m_pXmlDocument)
        return ((wxNetMessageRefData *)m_refData)->m_pXmlDocument->GetRoot();
    return NULL;
}

const wxScopedCharBuffer::CharType* wxNetMessage::GetData(void)
{
    return ((wxNetMessageRefData *)m_refData)->GetData().data();
}

size_t wxNetMessage::GetLength() const 
{
    return ((wxNetMessageRefData *)m_refData)->GetData().length() * sizeof( wxScopedCharBuffer::CharType );
}

//-----------------------------------------------------------------------------
// wxNetMessageRefData
//-----------------------------------------------------------------------------

wxNetMessageRefData::wxNetMessageRefData( const wxNetMessageRefData& data ) : wxObjectRefData()
{
    m_pXmlDocument = data.m_pXmlDocument;
    m_nPriority = data.m_nPriority;
    m_nState = data.m_nState;
}

wxNetMessageRefData::wxNetMessageRefData( const char* pBuff, size_t len ) : wxObjectRefData()
{
    m_nPriority = enumGISPriorityNormal;
    m_nCmd = enumGISNetCmdUnk;
    m_nState = enumGISNetCmdStUnk;
    m_nId = wxNOT_FOUND;
    m_pXmlDocument = NULL;
    LoadXMLFromStr(wxString::FromUTF8(pBuff, len));
    /*
	wxUint8 sys_type = (wxUint8)pBuff[0];
	pBuff += sizeof(wxUint8);
    nSize -= sizeof(wxUint8);
	
	if(sys_type == CURROS)
	{
        //m_sData.Alloc(nSize);
        //wxChar* pStrBuff = m_sData.GetWriteBuf(nSize);
        //memcpy(pStrBuff, pBuff, nSize - sizeof(wxUint8));
        //m_sData.UngetWriteBuf(nSize);

        //for(size_t i = 0; i < nSize; ++i)
        //    m_sData += (wxChar)pBuff[i];
		m_sData = wxString((const wxChar*)pBuff, nSize / sizeof(wxChar));
	}
	else if(sys_type == WIN)
	{
		m_sData = StrFromBuff(pBuff, nSize, sizeof(wxUint32));
	}
	else if(sys_type == LIN)
	{
		m_sData = StrFromBuff(pBuff, nSize, sizeof(wxUint16));
	}
    */

 //   m_sData = wxString::From8BitData(pBuff, len);

	////load xml
	//if(!LoadXMLFromStr(m_sData))
	//	return;



	//((wxGISConfigRefData *)m_refData)->m_sAppExeDirPath = wxPathOnly(stp.GetExecutablePath());

 //   m_bIsOk = true;
}

wxNetMessageRefData::wxNetMessageRefData( wxGISNetCommand nNetCmd, wxGISNetCommandState nNetCmdState, short nPriority, int nId ) : wxObjectRefData()
{
    m_nPriority = nPriority;
    m_nState = nNetCmdState;
    m_nCmd = nNetCmd;
    m_nId = nId;
    
    m_pXmlDocument = new wxXmlDocument();
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("msg"));
    pRootNode->AddAttribute(wxT("ver"), wxString::Format(wxT("%d"), WXNETVER));
    pRootNode->AddAttribute(wxT("prio"), wxString::Format(wxT("%d"), m_nPriority));
    pRootNode->AddAttribute(wxT("st"), wxString::Format(wxT("%d"), m_nState));
    pRootNode->AddAttribute(wxT("cmd"), wxString::Format(wxT("%d"), m_nCmd));
    pRootNode->AddAttribute(wxT("id"), wxString::Format(wxT("%d"), m_nId));
    m_pXmlDocument->SetRoot(pRootNode);
}

wxNetMessageRefData::~wxNetMessageRefData()
{
    wxDELETE(m_pXmlDocument);
}  

bool wxNetMessageRefData::operator == (const wxNetMessageRefData& data) const
{
    return m_pXmlDocument == data.m_pXmlDocument && 
        m_nPriority == data.m_nPriority && 
        m_nState == data.m_nState &&
        m_nCmd == data.m_nCmd &&
        m_nId == data.m_nId;
}

bool wxNetMessageRefData::IsOk(void)
{
    return m_pXmlDocument != NULL && m_pXmlDocument->IsOk();
}

bool wxNetMessageRefData::LoadXMLFromStr(const wxString &sData)
{
    m_sMessageData = sData;
	//if(sData[1] != '?')
	//    sData = sData.Prepend(wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	wxStringInputStream Stream(sData);
    
	wxASSERT(m_pXmlDocument == NULL);

    m_pXmlDocument = new wxXmlDocument();
	if(!m_pXmlDocument->Load(Stream) || !m_pXmlDocument->IsOk())
    {
        wxDELETE(m_pXmlDocument);
		return false;
    }
	
	wxXmlNode *pRoot = m_pXmlDocument->GetRoot();
	if(pRoot && pRoot->GetName() != wxString(wxT("msg")))
    {
        wxDELETE(m_pXmlDocument);
		return false;
    }

    if(wxAtoi(pRoot->GetAttribute(wxT("ver"), wxT("2"))) > WXNETVER)
    {
        wxDELETE(m_pXmlDocument);
		return false;
    }

    //header
	m_nCmd = (wxGISNetCommand)wxAtoi(pRoot->GetAttribute(wxT("cmd"), wxT("0")));
	m_nState = (wxGISNetCommandState)wxAtoi(pRoot->GetAttribute(wxT("st"), wxT("0")));
	m_nPriority = wxAtoi(pRoot->GetAttribute(wxT("prio"), wxT("0")));	
 	m_nId = wxAtoi(pRoot->GetAttribute(wxT("id"), wxT("-1")));	   

	return true;
}

bool wxNetMessageRefData::SaveXMLToStr(wxString *p_sData)
{
    wxCHECK_MSG(p_sData, false, wxT("input string pointer is empty"));
	//if(p_sData->operator [](1) != '?')
	//	*p_sData = p_sData->Prepend(wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	wxStringOutputStream Stream(p_sData);    
	if(m_pXmlDocument && Stream.IsOk())
		return m_pXmlDocument->Save(Stream, wxXML_NO_INDENTATION);
	return false;
}

wxString wxNetMessageRefData::FormatXmlString(const wxString &sInputString)
{
    wxString sOutput = sInputString;
	sOutput.Replace(wxT("&"), wxT("&amp;"));
	sOutput.Replace(wxT("\""), wxT("&quot;"));
	sOutput.Replace(wxT("'"), wxT("&apos;"));
	sOutput.Replace(wxT(">"), wxT("&gt;"));
	sOutput.Replace(wxT("<"), wxT("&lt;"));
	//sOutput.Replace(wxT("/"), wxT("&frasl;"));
	return sOutput;
}

void wxNetMessageRefData::SetPriority(short nPriority)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasAttribute(wxT("prio")))
                pRootNode->DeleteAttribute(wxT("prio"));
            pRootNode->AddAttribute(wxT("prio"), wxString::Format(wxT("%d"), nPriority));
        }
    }
    m_nPriority = nPriority;
    m_sMessageData.Clear();
}

void wxNetMessageRefData::SetState(wxGISNetCommandState nState)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasAttribute(wxT("st")))
                pRootNode->DeleteAttribute(wxT("st"));
            pRootNode->AddAttribute(wxT("st"), wxString::Format(wxT("%d"), nState));
        }
    }
    m_nState = nState;
    m_sMessageData.Clear();
}

void wxNetMessageRefData::SetCommand(wxGISNetCommand nCmd)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasAttribute(wxT("cmd")))
                pRootNode->DeleteAttribute(wxT("cmd"));
            pRootNode->AddAttribute(wxT("cmd"), wxString::Format(wxT("%d"), nCmd));
        }
    }
    m_nCmd = nCmd;
    m_sMessageData.Clear();
}

void wxNetMessageRefData::SetMessage(const wxString& sMsg)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasAttribute(wxT("msg")))
                pRootNode->DeleteAttribute(wxT("msg"));
            pRootNode->AddAttribute(wxT("msg"), sMsg);//FormatXmlString(sMsg));
        }
    }
    m_sMessageData.Clear();
}

void wxNetMessageRefData::SetId(int nId)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasAttribute(wxT("id")))
                pRootNode->DeleteAttribute(wxT("id"));
            pRootNode->AddAttribute(wxT("id"), wxString::Format(wxT("%d"), nId));
        }
    }
    m_nId = nId;
    m_sMessageData.Clear();
}

const wxScopedCharBuffer wxNetMessageRefData::GetData(void) 
{
    if(!m_sMessageData.IsEmpty())
        return m_sMessageData.ToUTF8();
    if(SaveXMLToStr(&m_sMessageData))
    {
        return m_sMessageData.ToUTF8();
    }
    return wxScopedCharBuffer();
}

//wxString wxNetMessage::StrFromBuff(unsigned char* pBuff, size_t nBuffByteSize, size_t nValSize)
//{
//    wxString sData;
//	size_t nRealCount = nBuffByteSize / nValSize + 1;
//    sData.Alloc(nRealCount);
//
//	//wxStringCharType* pchar = sData.GetWriteBuf(nRealCount);//new wxChar[nRealCount];
//
//	for(int i = 0; i < nRealCount; ++i)
//	{
//		//pchar[i] = pBuff[0];
//		sData.Append( pBuff[0] );
//		pBuff += nValSize;
//	}
//	//pchar[nRealCount] = 0;
//    //sData.UngetWriteBuf();
//	return sData;//wxString(pchar, nRealCount);
//}

