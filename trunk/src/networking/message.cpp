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

#include "wxgis/networking/message.h"

#include <wx/stream.h>
#include <wx/sstream.h>

wxNetMessage::wxNetMessage(unsigned char* pBuff, size_t nSize) : m_bIsOk(false), m_pXmlDocument(NULL)
{
	wxUint8 sys_type = (wxUint8)pBuff[0];
	pBuff += sizeof(wxUint8);
	
	if(sys_type == CURROS)
	{
		m_sData = wxString((const wxChar*)pBuff, nSize);
	}
	else if(sys_type == WIN)
	{
		m_sData = StrFromBuff(pBuff, nSize, sizeof(wxUint32));
	}
	else if(sys_type == LIN)
	{
		m_sData = StrFromBuff(pBuff, nSize, sizeof(wxUint16));
	}


	//load xml
	if(!LoadXMLFromStr(m_sData))
		return;

    m_bIsOk = !m_sData.IsEmpty();
}

wxString wxNetMessage::StrFromBuff(unsigned char* pBuff, size_t nBuffByteSize, size_t nValSize)
{
	size_t nRealCount = nBuffByteSize / nValSize + 1;

	wxChar* pchar = new wxChar[nRealCount];

	for(int i = 0; i < nRealCount; i++)
	{
		pchar[i] = pBuff[0];
		pBuff += nValSize;
	}
	pchar[nRealCount] = 0;
	return wxString(pchar, nRealCount);
}


wxNetMessage::wxNetMessage(void) : m_bIsOk(false), m_pXmlDocument(NULL)
{
    m_nPriority = enumGISPriorityNormal;
    m_nState = enumGISMsgStUnk;

    m_pXmlDocument = new wxXmlDocument();
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("msg"));
    pRootNode->AddProperty(wxT("ver"), wxString::Format(wxT("%d"), WXNETVER));
    pRootNode->AddProperty(wxT("priority"), wxString::Format(wxT("%d"), m_nPriority));
    pRootNode->AddProperty(wxT("state"), wxString::Format(wxT("%d"), m_nState));
    m_pXmlDocument->SetRoot(pRootNode);
    m_bIsOk = true;
}
 
//wxNetMessage::wxNetMessage(long nID, wxGISMessageState nState, short nPriority, wxString sMessage, wxString sModuleSrc, wxString sModuleDst) : m_bIsOk(false), m_pXmlDocument(NULL)
//{
//    m_nPriority = nPriority;
//    m_nID = nID;
//    m_nState = nState;
//
//    m_pXmlDocument = new wxXmlDocument();
//    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("msg"));
//    pRootNode->AddProperty(wxT("ver"), wxString::Format(wxT("%d"), WXNETVER));
//    pRootNode->AddProperty(wxT("priority"), wxString::Format(wxT("%d"), m_nPriority));
//    if(!sModuleSrc.IsEmpty())
//        pRootNode->AddProperty(wxT("src"), sModuleSrc);
//    if(!sModuleDst.IsEmpty())
//        pRootNode->AddProperty(wxT("dst"), sModuleDst);
//    pRootNode->AddProperty(wxT("state"), wxString::Format(wxT("%d"), m_nState));
//    if(!sMessage.IsEmpty())
//        pRootNode->AddProperty(wxT("message"), wxString::Format(wxT("%s"), sMessage.c_str()));
//    m_pXmlDocument->SetRoot(pRootNode);
//    m_bIsOk = true;
//}
//
wxNetMessage::wxNetMessage(wxString sMsgData) : m_bIsOk(false), m_pXmlDocument(NULL)
{
	m_sData = sMsgData;

	//load xml
	if(!LoadXMLFromStr(sMsgData))
		return;

	m_bIsOk = true;
}

wxNetMessage::~wxNetMessage(void)
{
    wxDELETE(m_pXmlDocument);
}

const short wxNetMessage::GetPriority(void)
{
    return m_nPriority;
}

void wxNetMessage::SetPriority(short nPriority)
{
    if(m_pXmlDocument)
    {
        wxXmlNode* pRootNode = m_pXmlDocument->GetRoot();
        if(pRootNode)
        {
            if(pRootNode->HasProp(wxT("prio")))
                pRootNode->DeleteProperty(wxT("prio"));
            pRootNode->AddProperty(wxT("prio"), wxString::Format(wxT("%d"), nPriority));
        }
    }
    m_nPriority = nPriority;
}

//
//bool wxNetMessage::operator< (const INetMessage& msg) const
//{
//    return m_nPriority < msg.m_nPriority;
//}
//
//INetMessage& wxNetMessage::operator= (const INetMessage& oSource)
//{
//    m_pXmlDocument = oSource.m_pXmlDocument;
//    m_nID = oSource.m_nID;
//	m_bIsOk = oSource.m_bIsOk;
//    m_nPriority = oSource.m_nPriority;
//
//    return *this;
//}
//
bool wxNetMessage::IsOk(void)
{
    if(!m_pXmlDocument)
        return false;
    return m_pXmlDocument->IsOk() && m_bIsOk;
}

const wxGISMessageDirection wxNetMessage::GetDirection(void)
{
    return m_nDirection;
}

void wxNetMessage::SetDirection(wxGISMessageDirection nDirection)
{
    m_nDirection = nDirection;
}

const wxXmlNode* wxNetMessage::GetRoot(void)
{
	if(!m_pXmlDocument)
		if(!LoadXMLFromStr(m_sData))
			return NULL;
    if(m_pXmlDocument)
        return m_pXmlDocument->GetRoot();
    return NULL;
}

const unsigned char* wxNetMessage::GetData(void)
{
	if(m_sData.IsEmpty())
		if(SavedXMLToStr(m_sData))
			return 0;

    if(GetDataLen() >= BUFF)
        return 0;

    wxUint8 sys_type = CURROS; 

	memset(m_cData, 0, sizeof(BUFF));
    memcpy(m_cData, &sys_type, sizeof(wxUint8));
    memcpy(m_cData + sizeof(wxUint8), m_sData.GetData(), GetDataLen());
    
    return (const unsigned char*)m_cData;
}

const size_t wxNetMessage::GetDataLen(void)
{
	if(m_sData.IsEmpty())
		if(SavedXMLToStr(m_sData))
			return 0;
	return (m_sData.Len() + 1) * sizeof(wxChar) + sizeof(wxUint8);
}

const wxGISMessageState wxNetMessage::GetState(void)
{
    return m_nState;
}

void wxNetMessage::SetState(wxGISMessageState nState)
{
    m_nState = nState;
}

bool wxNetMessage::LoadXMLFromStr(wxString sData)
{
	wxStringInputStream Stream(sData);
    
	wxASSERT(m_pXmlDocument == NULL);
    m_pXmlDocument = new wxXmlDocument();
	if(!m_pXmlDocument->Load(Stream) || !m_pXmlDocument->IsOk())
		return false;
	
	wxXmlNode *pRoot = m_pXmlDocument->GetRoot();
	if(pRoot && pRoot->GetName() != wxT("msg"))
		return false;

    if(wxAtoi(pRoot->GetPropVal(wxT("ver"), wxT("1"))) > WXNETVER)
		return false;

    //header
	m_nState = (wxGISMessageState)wxAtoi(pRoot->GetPropVal(wxT("st"), wxT("0")));
	//m_sMessage = pRoot->GetPropVal(wxT("message"), wxT(""));
	m_nPriority = wxAtoi(pRoot->GetPropVal(wxT("prio"), wxT("0")));
}

bool wxNetMessage::SavedXMLToStr(wxString sData)
{
	wxStringOutputStream Stream(&sData);
	if(m_pXmlDocument)
		return m_pXmlDocument->Save(Stream, wxXML_NO_INDENTATION);
	return false;
}

