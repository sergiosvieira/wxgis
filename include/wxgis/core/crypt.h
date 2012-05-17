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
#pragma once

#include "wxgis/core/core.h"

#include "cpl_string.h"
#include <openssl/evp.h>

#define EVP_KEY_SIZE	32
#define EVP_IV_SIZE		8
#define BUFSIZE			1024

/** \fn bool CreateRandomData(void)
 *  \brief Create key and iv data and store it in config.
 *  \return true if succeeded, false otherwise
 */
bool CreateRandomData(void);
GByte *GetKey(void);
GByte *GetIV(void);
EVP_CIPHER_CTX* CreateCTX(GByte* pabyKey, GByte* pabyIV, bool bDecrypt);

/** \fn bool Crypt(const wxString &sText, wxString &sCryptText)
 *  \brief Crypt input string and convert it to HEX.
 *  \param sText input text
 *  \param sCryptText output crypted text
 *  \return true if succeeded, false otherwise
 */
WXDLLIMPEXP_GIS_CORE bool Crypt(const wxString &sText, wxString &sCryptText);
/** \fn bool Decrypt(const wxString &sText, wxString &sDecryptText)
 *  \brief Decrypt crypted text.
 *  \param sText input crypted text
 *  \param sDecryptText output decrypted text
 *  \return true if succeeded, false otherwise
 */
WXDLLIMPEXP_GIS_CORE bool Decrypt(const wxString &sText, wxString &sDecryptText);


