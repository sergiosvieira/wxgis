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

#include <openssl/rand.h>

bool CreateRandomData(void)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return false;

	GByte key[EVP_KEY_SIZE];
	if(!RAND_bytes(key, sizeof(key)))
		return false;
	CPLString pszKey(CPLBinaryToHex(EVP_KEY_SIZE, key));
	if(!pConfig->Write(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(pszKey, wxConvUTF8)))
 		return false;
	GByte iv[EVP_IV_SIZE];
	if(!RAND_bytes(iv, sizeof(iv)))
		return false;
	CPLString pszIV(CPLBinaryToHex(EVP_IV_SIZE, iv));
	return pConfig->Write(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(pszIV, wxConvUTF8));
}

GByte *GetKey(void)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return NULL;
    //try get key data from config
	wxString sKey = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
	if(sKey.CmpNoCase(wxString(ERR)) == 0)
	{
		if(!CreateRandomData())//create random key data
            return NULL;
		//second try get key data from config
		sKey = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/key")), wxString(ERR));
		if(sKey.CmpNoCase(wxString(ERR)) == 0)
			return NULL;
	}

	int nKeyBytes;
	GByte *pabyKey = CPLHexToBinary( sKey.mb_str(wxConvUTF8), &nKeyBytes );
    return pabyKey;
}

GByte *GetIV(void)
{
	wxGISAppConfigSPtr pConfig = GetConfig();
	if(!pConfig)
		return NULL;

	//try get iv data from config
	wxString sIV = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
	if(sIV.CmpNoCase(wxString(ERR)) == 0)
	{
		if(!CreateRandomData())//create random key data
            return NULL;
		//second try get iv data from config
		sIV = pConfig->Read(enumGISHKCU, wxString(wxT("wxGISCommon/crypt/iv")), wxString(ERR));
		if(sIV.CmpNoCase(wxString(ERR)) == 0)
			return NULL;
	}

	int nIVBytes;
	GByte *pabyIV = CPLHexToBinary( sIV.mb_str(wxConvUTF8), &nIVBytes );
	return pabyIV;
}

EVP_CIPHER_CTX* CreateCTX(GByte* pabyKey, GByte* pabyIV, bool bDecrypt)
{
    EVP_CIPHER_CTX* pstCTX = new EVP_CIPHER_CTX;
	const EVP_CIPHER * cipher;

	EVP_CIPHER_CTX_init(pstCTX);

	cipher = EVP_des_cfb();//EVP_aes_256_cfb();

	bool bResult;
	if(bDecrypt)
	    bResult = EVP_EncryptInit(pstCTX, cipher, pabyKey, pabyIV);
    else
	    bResult = EVP_DecryptInit(pstCTX, cipher, pabyKey, pabyIV);
	if(!bResult)
		return NULL;
	return pstCTX;
}

bool Crypt(const wxString &sText, wxString &sCryptText)
{

	GByte *pabyKey = GetKey();
	GByte *pabyIV = GetIV();

	EVP_CIPHER_CTX* ctx = CreateCTX(pabyKey, pabyIV, false);
	if(!ctx)
	{
		wxLogError(_("Crypt: Failed EVP_EncryptInit!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return false;
	}

	CPLString pszText(sText.mb_str(wxConvUTF8));
	int outlen;
	unsigned char outbuf[BUFSIZE];

	bool bResult = EVP_EncryptUpdate(ctx, outbuf, &outlen, (const unsigned char*)pszText.data(), pszText.length() * sizeof(pszText[0]) + 1);

	if(!bResult)
	{
		wxLogError(_("Crypt: Failed EVP_EncryptUpdate!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return bResult;
	}

	int nLen = outlen;
	bResult = EVP_EncryptFinal(ctx, &outbuf[outlen], &outlen);
	nLen += outlen;

	CPLString pszOutput(CPLBinaryToHex(nLen, outbuf));
	sCryptText = wxString(pszOutput, wxConvUTF8);

	CPLFree( pabyKey );
	CPLFree( pabyIV );
	EVP_CIPHER_CTX_cleanup(ctx);
	EVP_CIPHER_CTX_free(ctx);

	return bResult;
}

bool Decrypt(const wxString &sText, wxString &sDecryptText)
{

	GByte *pabyKey = GetKey();
	GByte *pabyIV = GetIV();

	EVP_CIPHER_CTX* ctx = CreateCTX(pabyKey, pabyIV, true);
	if(!ctx)
	{
		wxLogError(_("Decrypt: Failed EVP_DecryptInit!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		return false;
	}

	int nTextBytes;
	GByte *pabyText = CPLHexToBinary( sText.mb_str(wxConvUTF8), &nTextBytes );

	int outlen;
	unsigned char outbuf[BUFSIZE];

	bool bResult = EVP_DecryptUpdate(ctx, outbuf, &outlen, pabyText, nTextBytes);
	if(!bResult)
	{
		wxLogError(_("Decrypt: Failed EVP_DecryptUpdate!"));
		CPLFree( pabyKey );
		CPLFree( pabyIV );
		CPLFree( pabyText );
		return bResult;
	}

	int nLen = outlen;
	bResult = EVP_DecryptFinal(ctx, &outbuf[outlen], &outlen);
	nLen += outlen;
	outbuf[nLen] = 0;

	CPLString szCryptText((const char*)outbuf);
	sDecryptText = wxString(szCryptText, wxConvUTF8);

	CPLFree( pabyKey );
	CPLFree( pabyIV );
	CPLFree( pabyText );

	EVP_CIPHER_CTX_cleanup(ctx);
	EVP_CIPHER_CTX_free(ctx);

	return bResult;
}
