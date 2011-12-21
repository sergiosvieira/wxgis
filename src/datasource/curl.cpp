/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  cURL class. This is smart clas for cURL handler
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008,2010,2011  Bishop
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
#include "wxgis/datasource/curl.h"

#include "wx/tokenzr.h"
#include "wx/filename.h"
#include "wx/file.h"
#include "wx/ffile.h"

wxGISCurl::wxGISCurl(wxString proxy, wxString sHeaders, int dnscachetimeout, int timeout, int conntimeout) : 
m_bIsValid(false), m_bUseProxy(false), slist(NULL)
{
	curl = curl_easy_init();
	if(curl)
	{
		bodystruct.size = 0;
		bodystruct.memory = NULL;
		headstruct.size = 0;
		headstruct.memory = NULL;

		wxStringTokenizer tkz(sHeaders, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
		while ( tkz.HasMoreTokens() )
		{
			wxString token = tkz.GetNextToken();
			slist = curl_slist_append(slist, token.mb_str());
		}

		m_sHeaders = sHeaders;
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
		curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, dnscachetimeout);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER ,&headstruct);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA ,&bodystruct);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, conntimeout);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

		if(!proxy.IsEmpty())
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy.mb_str());
			m_bUseProxy = true;
		}
		m_bIsValid = true;
	}
}

wxGISCurl::~wxGISCurl(void)
{
	free(bodystruct.memory);
	free(headstruct.memory);
	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
}

void wxGISCurl::SetRestrictions(wxString sRestrict)
{
	//need convert from restriction (country) to ip:port
	//if(!m_bUseProxy)
	//	curl_easy_setopt(curl, CURLOPT_PROXY, wgWX2MB(sRestrict));
}

void wxGISCurl::AppendHeader(wxString sHeadStr)
{
	slist = curl_slist_append(slist, sHeadStr.mb_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
}

void wxGISCurl::SetDefaultHeader(void)
{
	curl_slist_free_all(slist);
	slist = NULL;

	wxStringTokenizer tkz(m_sHeaders, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString token = tkz.GetNextToken();
		slist = curl_slist_append(slist, token.mb_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
}

void wxGISCurl::FollowLocation(bool bSet, unsigned short iMaxRedirs)
{
	if(bSet)
	{
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, iMaxRedirs);
	}
	else
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
}

PERFORMRESULT wxGISCurl::Get(wxString sURL)
{
	PERFORMRESULT result;
	result.IsValid = false;
	result.iSize = 0;
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, sURL.mb_str());
	//curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	res = curl_easy_perform(curl);

	//second try
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(curl);
	//
	if(res == CURLE_OK)
	{
		result.sHead = wxString((const char*)headstruct.memory, wxConvLocal, headstruct.size);
        //charset
        int posb = result.sHead.Find(wxT("charset="));
        wxString soSet;//(wxT("default"));
        if( posb != wxNOT_FOUND)
        {
            soSet = result.sHead.Mid(posb + 8, 50);
            int pose = soSet.Find(wxT("\r\n"));
            soSet = soSet.Left(pose);
        }
        //wxCSConv
        wxCSConv conv(soSet);

        if(conv.IsOk())
            result.sBody = wxString((const char*)bodystruct.memory, conv, bodystruct.size);
        else
            result.sBody = wxString((const char*)bodystruct.memory, wxConvLocal, bodystruct.size);

		result.iSize = headstruct.size + bodystruct.size;
		headstruct.size = 0;
		bodystruct.size = 0;
		result.IsValid = true;
	}
	return result;
}

bool wxGISCurl::GetFile(wxString sURL, wxString sPath)
{
	if(wxFileName::FileExists(sPath))
		return true/*false*/;
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, sURL.mb_str());
	//curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	res = curl_easy_perform(curl);
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(curl);
	if(res == CURLE_OK)
	{
		headstruct.size = 0;
		wxFile file(sPath, wxFile::write);
		if(file.IsOpened())
		{
			file.Write(bodystruct.memory, bodystruct.size);
			file.Close();
			bodystruct.size = 0;
			return true;
		}
		bodystruct.size = 0;
	}
	return false;
}

PERFORMRESULT wxGISCurl::Post(wxString sURL, wxString sPostData)
{
	PERFORMRESULT result;
	result.IsValid = false;
	result.iSize = 0;
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	//const wxWX2MBbuf tmp_buf = wxConvCurrent->cWX2MB(sPostData);
	//const char *tmp_str = (const char*) tmp_buf;
	const char *tmp_str = sPostData.mb_str(wxConvLocal);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS , tmp_str);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);
	curl_easy_setopt(curl, CURLOPT_URL, sURL.mb_str(wxConvLocal));
	//curl_easy_setopt(curl, CURLOPT_CAINFO, "curl-ca-bundle.crt");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	res = curl_easy_perform(curl);
	if(res == CURLE_COULDNT_RESOLVE_HOST)
		res = curl_easy_perform(curl);
	if(res == CURLE_OK)
	{
		result.sHead = wxString((const char*)headstruct.memory, wxConvLocal, headstruct.size);
        //charset
        int posb = result.sHead.Find(wxT("charset="));
        wxString soSet;//(wxT("default"));
        if( posb != wxNOT_FOUND)
        {
            soSet = result.sHead.Mid(posb + 8, 50);
            int pose = soSet.Find(wxT("\r\n"));
            soSet = soSet.Left(pose);
        }
        //wxCSConv
        wxCSConv conv(soSet);

        if(conv.IsOk())
            result.sBody = wxString((const char*)bodystruct.memory, conv, bodystruct.size);
        else
            result.sBody = wxString((const char*)bodystruct.memory, wxConvLocal, bodystruct.size);
		result.iSize = headstruct.size + bodystruct.size;
		headstruct.size = 0;
		bodystruct.size = 0;
		result.IsValid = true;
	}
	return result;
}