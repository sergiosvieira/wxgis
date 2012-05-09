/******************************************************************************
 * Project:  wxGIS
 * Purpose:  base crypt functions.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/core/crypt.h"
#include "wxgis/core/config.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include "cpl_string.h"

bool CreateRandomData(void)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return false;

	unsigned char key[EVP_KEY_SIZE];
	if(!RAND_bytes(key, sizeof(key)))
		return false;
	CPLString pszKey(CPLBinaryToHex(EVP_KEY_SIZE, key));
	if(!pConfig->Write(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(pszKey, wxConvUTF8)))
		return false;
	unsigned char iv[EVP_IV_SIZE];
	if(!RAND_bytes(iv, sizeof(iv)))
		return false;
	CPLString pszIV(CPLBinaryToHex(EVP_IV_SIZE, iv));
	return pConfig->Write(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(pszIV, wxConvUTF8));
}

bool Crypt(const wxString &sText, wxString &sCryptText)
{
    sCryptText = sText;
    return true;

	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return false;

	//try get key data from config
	wxString sKey = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
	if(sKey.CmpNoCase(wxString(ERR)) == 0)
	{
		CreateRandomData();//create random key data
		//second try get key data from config
		sKey = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
		if(sKey.CmpNoCase(wxString(ERR)) == 0)
			return false;
	}

	int nKeyBytes;
	GByte *pabyKey = CPLHexToBinary( sKey.mb_str(wxConvUTF8), &nKeyBytes );

	//try get iv data from config
	wxString sIV = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
	if(sIV.CmpNoCase(wxString(ERR)) == 0)
	{
		CreateRandomData();//create random key data
		//second try get iv data from config
		sIV = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
		if(sIV.CmpNoCase(wxString(ERR)) == 0)
			return false;
	}

	int nIVBytes;
	GByte *pabyIV = CPLHexToBinary( sIV.mb_str(wxConvUTF8), &nIVBytes );

	EVP_CIPHER_CTX ctx;
	const EVP_CIPHER * cipher;

	EVP_CIPHER_CTX_init(&ctx);

	cipher = EVP_aes_256_cfb();

	bool bResult = EVP_EncryptInit(&ctx, cipher, pabyKey, pabyIV);
	if(!bResult)
	{
		wxLogError(_("Crypt: Failed EVP_EncryptInit!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return bResult;
	}

	CPLString pszText(sText.mb_str(wxConvUTF8));
	int outlen;
	unsigned char outbuf[BUFSIZE];

	bResult = EVP_EncryptUpdate(&ctx, outbuf, &outlen, (const unsigned char*)pszText.c_str(), pszText.length());

	if(!bResult)
	{
		wxLogError(_("Crypt: Failed EVP_EncryptUpdate!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return bResult;
	}

	int nLen = outlen;
	bResult = EVP_EncryptFinal(&ctx, &outbuf[outlen], &outlen);
	nLen += outlen;

	CPLString pszOutput(CPLBinaryToHex(nLen, outbuf));
	sCryptText = wxString(pszOutput, wxConvUTF8);

	CPLFree( pabyKey );
	CPLFree( pabyIV );
	EVP_CIPHER_CTX_cleanup(&ctx);

	return bResult;
}

bool Decrypt(const wxString &sText, wxString &sDecryptText)
{
    sDecryptText = sText;
    return true;

	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return false;

	//try get key data from config
	wxString sKey = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
	if(sKey.CmpNoCase(wxString(ERR)) == 0)
	{
		CreateRandomData();//create random key data
		//second try get key data from config
		sKey = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
		if(sKey.CmpNoCase(wxString(ERR)) == 0)
			return false;
	}

	int nKeyBytes;
	GByte *pabyKey = CPLHexToBinary( sKey.mb_str(wxConvUTF8), &nKeyBytes );

	//try get iv data from config
	wxString sIV = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
	if(sIV.CmpNoCase(wxString(ERR)) == 0)
	{
		CreateRandomData();//create random key data
		//second try get iv data from config
		sIV = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
		if(sIV.CmpNoCase(wxString(ERR)) == 0)
			return false;
	}

	int nIVBytes;
	GByte *pabyIV = CPLHexToBinary( sIV.mb_str(wxConvUTF8), &nIVBytes );

	EVP_CIPHER_CTX ctx;
	const EVP_CIPHER * cipher;

	EVP_CIPHER_CTX_init(&ctx);

	cipher = EVP_aes_256_cfb();

 	bool bResult = EVP_DecryptInit(&ctx, cipher, pabyKey, pabyIV);
	if(!bResult)
	{
		wxLogError(_("Decrypt: Failed EVP_DecryptInit!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return bResult;
	}

	int nTextBytes;
	GByte *pabyText = CPLHexToBinary( sText.mb_str(wxConvUTF8), &nTextBytes );

	int outlen;
	unsigned char outbuf[BUFSIZE];

	bResult = EVP_DecryptUpdate(&ctx, outbuf, &outlen, pabyText, nTextBytes);
	if(!bResult)
	{
		wxLogError(_("Decrypt: Failed EVP_DecryptUpdate!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		CPLFree( pabyText );
		return bResult;
	}

	int nLen = outlen;
	bResult = EVP_DecryptFinal(&ctx, &outbuf[outlen], &outlen);
	nLen += outlen;

	sDecryptText = wxString(outbuf, wxConvUTF8, nLen);

	CPLFree( pabyKey );
	CPLFree( pabyIV );
	CPLFree( pabyText );

	EVP_CIPHER_CTX_cleanup(&ctx);

	return bResult;
}
