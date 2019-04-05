/* --COPYRIGHT--,BSD
 * Copyright (c) 2011, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*
 * TI_aes_128.h
 *
 *  Created on: Nov 3, 2011
 *      Author: Eric Peeters
 */

/*!
 * @author		RKH
 * @date		06.06.2017
 * @copyright (C)	Spiro Medical AS 2013-2017
 */

#include "stdafx.h"
#include "AES128.h"

CAES128::CAES128()
{
	memcpy((void *) theKey,(void *) spiroAESKey,16);
}

CAES128::~CAES128()
{
}

void CAES128::setKey(unsigned char *_key)
{
	memcpy((void *) theKey,(void *) _key,16);
}

// multiply by 2 in the galois field
unsigned char CAES128::galois_mul2(unsigned char value)
{
	if (value>>7)
	{
		return ((value << 1)^0x1b);
	} else
		return (value << 1);
}

void CAES128::encrypt(unsigned char state[])
{
	unsigned char buf1, buf2, buf3, buf4, round, i;
	unsigned char rcon;

	unsigned char key[16];
	memcpy((void *) key,(void *) theKey,16);

	// Rcon initial value. All subsequent values are computed.
	rcon = 0x01;

	// main AES data loop
	for (round = 0; round < 10; round++){
		//add key + sbox
		for (i = 0; i <16; i++){
			state[i]=sbox[state[i] ^ key[i]];
		}
		//shift rows
		buf1 = state[1];
		state[1] = state[5];
		state[5] = state[9];
		state[9] = state[13];
		state[13] = buf1;

		buf1 = state[2];
		buf2 = state[6];
		state[2] = state[10];
		state[6] = state[14];
		state[10] = buf1;
		state[14] = buf2;

		buf1 = state[15];
		state[15] = state[11];
		state[11] = state[7];
		state[7] = state[3];
		state[3] = buf1;

		//process mixcolumn for all rounds but the last one
		if (round < 9) {
			for (i=0; i <4; i++){
				// compute the current index
				buf4 = (i << 2);
				buf1 = state[buf4] ^ state[buf4+1] ^ state[buf4+2] ^ state[buf4+3];
				buf2 = state[buf4];
				buf3 = state[buf4]^state[buf4+1]; buf3=galois_mul2(buf3); state[buf4] = state[buf4] ^ buf3 ^ buf1;
				buf3 = state[buf4+1]^state[buf4+2]; buf3=galois_mul2(buf3); state[buf4+1] = state[buf4+1] ^ buf3 ^ buf1;
				buf3 = state[buf4+2]^state[buf4+3]; buf3=galois_mul2(buf3); state[buf4+2] = state[buf4+2] ^ buf3 ^ buf1;
				buf3 = state[buf4+3]^buf2;     buf3=galois_mul2(buf3); state[buf4+3] = state[buf4+3] ^ buf3 ^ buf1;
			}
		}

		//key schedule
		// compute the 16 next round key bytes
		key[0] = sbox[key[13]]^key[0]^rcon;
		key[1] = sbox[key[14]]^key[1];
		key[2] = sbox[key[15]]^key[2];
		key[3] = sbox[key[12]]^key[3];
		for (i=4; i<16; i++) {
			key[i] = key[i] ^ key[i-4];
		}
		// compute the next Rcon value
		rcon = galois_mul2(rcon);
	}

	// process last AddRoundKey
	for (i = 0; i <16; i++){
		state[i]=state[i] ^ key[i];
	}
}

/*
Decrypts a HEX string. Must be length 16. This will be converted to 16 binary values before conversion
_input: The string
_output: decrypted string
*/
CString CAES128::decryptHexString(CString _input)
{
	unsigned char res[17];
	decryptHexString(res,_input);
	res[16] = 0;
	CString des(res);
	return des;
}


/*
Decrypts a HEX string. Must be length 16. This will be converted to 16 binary values before conversion
_input: The string
result[] : decrypted output 8 integers, binary
*/
void CAES128::decryptHexString(short int result[],CString _input)
{
	UINT8 resultChar[16]; // unsigned char resultChar[16];
	decryptHexString(resultChar,_input);
	
	UINT16 ui16;

	ui16 = resultChar[1];
	ui16 <<= 8;
	ui16 += resultChar[0];
	result[0] = (short int)ui16;
	
	ui16 = resultChar[3];
	ui16 <<= 8;
	ui16 += resultChar[2];
	result[1] = (short int)ui16;

	ui16 = resultChar[5];
	ui16 <<= 8;
	ui16 += resultChar[4];
	result[2] = (short int)ui16;

	ui16 = resultChar[7];
	ui16 <<= 8;
	ui16 += resultChar[6];
	result[3] = (short int)ui16;

	ui16 = resultChar[9];
	ui16 <<= 8;
	ui16 += resultChar[8];
	result[4] = (short int)ui16;

	ui16 = resultChar[11];
	ui16 <<= 8;
	ui16 += resultChar[10];
	result[5] = (short int)ui16;

	ui16 = resultChar[13];
	ui16 <<= 8;
	ui16 += resultChar[12];
	result[6] = (short int)ui16;

	ui16 = resultChar[15];
	ui16 <<= 8;
	ui16 += resultChar[14];
	result[7] = (short int)ui16;
}

/*
Decrypts a HEX string. Must be length 16. This will be converted to 16 binary values before conversion
_input: The string
result[] : decrypted output 16 characters, binary
*/
void CAES128::decryptHexString(unsigned char result[],CString _input)
{
	CString s = _input;
	s.MakeUpper();
	int length = s.GetLength();
	ASSERT(length == 32);

	LPTSTR  smpl = s.GetBuffer();
	unsigned short lsb,msb;
	for (int i = 0 ; i < 16 ; i++) {
		msb = (unsigned short) hexCharToInt(*smpl++);
		lsb = (unsigned short) hexCharToInt(*smpl++);
		result[i] = msb * 16 + lsb;
	}
	decrypt(result);
}

int CAES128::hexCharToInt(WCHAR _c)
{
	switch (_c) {
	case '0' : return 0;
	case '1' : return 1;
	case '2' : return 2;
	case '3' : return 3;
	case '4' : return 4;
	case '5' : return 5;
	case '6' : return 6;
	case '7' : return 7;
	case '8' : return 8;
	case '9' : return 9;
	case 'A' : return 10;
	case 'B' : return 11;
	case 'C' : return 12;
	case 'D' : return 13;
	case 'E' : return 14;
	case 'F' : return 15;
	}
	return 0;
}

bool CAES128::isEncrypted(CString _s)
{
	if (_s.Find(_T("/")) > 0) return false;
	return true;
}

void CAES128::decrypt(unsigned char state[])
{
	unsigned char buf1, buf2, buf3, buf4, round, i;

	unsigned char key[16];
	memcpy((void *) key,(void *) theKey,16);

	// compute the last key of encryption before starting the decryption
	for (round = 0 ; round < 10; round++) {
		//key schedule
		key[0] = sbox[key[13]]^key[0]^Rcon[round];
		key[1] = sbox[key[14]]^key[1];
		key[2] = sbox[key[15]]^key[2];
		key[3] = sbox[key[12]]^key[3];
		for (i=4; i<16; i++) {
			key[i] = key[i] ^ key[i-4];
		}
	}

	//first Addroundkey
	for (i = 0; i <16; i++){
		state[i]=state[i] ^ key[i];
	}

	// main loop
	for (round = 0; round < 10; round++){

		//Inverse key schedule
		for (i=15; i>3; --i) {
			key[i] = key[i] ^ key[i-4];
		}  
		key[0] = sbox[key[13]]^key[0]^Rcon[9-round];
		key[1] = sbox[key[14]]^key[1];
		key[2] = sbox[key[15]]^key[2];
		key[3] = sbox[key[12]]^key[3]; 

		//mixcol - inv mix
		if (round > 0) {
			for (i=0; i <4; i++){
				buf4 = (i << 2);

				// precompute for decryption
				buf1 = galois_mul2(galois_mul2(state[buf4]^state[buf4+2]));
				buf2 = galois_mul2(galois_mul2(state[buf4+1]^state[buf4+3]));
				state[buf4] ^= buf1; state[buf4+1] ^= buf2; state[buf4+2] ^= buf1; state[buf4+3] ^= buf2; 

				// in all cases
				buf1 = state[buf4] ^ state[buf4+1] ^ state[buf4+2] ^ state[buf4+3];
				buf2 = state[buf4];
				buf3 = state[buf4]^state[buf4+1]; buf3=galois_mul2(buf3); state[buf4] = state[buf4] ^ buf3 ^ buf1;
				buf3 = state[buf4+1]^state[buf4+2]; buf3=galois_mul2(buf3); state[buf4+1] = state[buf4+1] ^ buf3 ^ buf1;
				buf3 = state[buf4+2]^state[buf4+3]; buf3=galois_mul2(buf3); state[buf4+2] = state[buf4+2] ^ buf3 ^ buf1;
				buf3 = state[buf4+3]^buf2;     buf3=galois_mul2(buf3); state[buf4+3] = state[buf4+3] ^ buf3 ^ buf1;
			}
		}

		//Inv shift rows
		// Row 1
		buf1 = state[13];
		state[13] = state[9];
		state[9] = state[5];
		state[5] = state[1];
		state[1] = buf1;
		//Row 2
		buf1 = state[10];
		buf2 = state[14];
		state[10] = state[2];
		state[14] = state[6];
		state[2] = buf1;
		state[6] = buf2;
		//Row 3
		buf1 = state[3];
		state[3] = state[7];
		state[7] = state[11];
		state[11] = state[15];
		state[15] = buf1;         

		for (i = 0; i <16; i++){
			// with shiftrow i+5 mod 16
			state[i]=rsbox[state[i]] ^ key[i];
		} 
	}
} // end function

//////////////////////////////////////////////////////////

CAES128_2::CAES128_2()
{
	memcpy((void *)theKey, (void *)spiroAESKey, 16);
}

CAES128_2::~CAES128_2()
{
}

void CAES128_2::setKey(UINT8 *_key)
{
	memcpy((void *)theKey, (void *)_key, 16);
}

// multiply by 2 in the galois field
unsigned char CAES128_2::galois_mul2(UINT8 value)
{
	if (value >> 7)
	{
		return ((value << 1) ^ 0x1b);
	}
	else
		return (value << 1);
}

void CAES128_2::encrypt(UINT8 state[])
{
	UINT8 buf1, buf2, buf3, buf4, round, i;
	UINT8 rcon;

	UINT8 key[16];
	memcpy((void *)key, (void *)theKey, 16);

	// Rcon initial value. All subsequent values are computed.
	rcon = 0x01;

	// main AES data loop
	for (round = 0; round < 10; round++) {
		//add key + sbox
		for (i = 0; i <16; i++) {
			state[i] = sbox[state[i] ^ key[i]];
		}
		//shift rows
		buf1 = state[1];
		state[1] = state[5];
		state[5] = state[9];
		state[9] = state[13];
		state[13] = buf1;

		buf1 = state[2];
		buf2 = state[6];
		state[2] = state[10];
		state[6] = state[14];
		state[10] = buf1;
		state[14] = buf2;

		buf1 = state[15];
		state[15] = state[11];
		state[11] = state[7];
		state[7] = state[3];
		state[3] = buf1;

		//process mixcolumn for all rounds but the last one
		if (round < 9) {
			for (i = 0; i <4; i++) {
				// compute the current index
				buf4 = (i << 2);
				buf1 = state[buf4] ^ state[buf4 + 1] ^ state[buf4 + 2] ^ state[buf4 + 3];
				buf2 = state[buf4];
				buf3 = state[buf4] ^ state[buf4 + 1]; buf3 = galois_mul2(buf3); state[buf4] = state[buf4] ^ buf3 ^ buf1;
				buf3 = state[buf4 + 1] ^ state[buf4 + 2]; buf3 = galois_mul2(buf3); state[buf4 + 1] = state[buf4 + 1] ^ buf3 ^ buf1;
				buf3 = state[buf4 + 2] ^ state[buf4 + 3]; buf3 = galois_mul2(buf3); state[buf4 + 2] = state[buf4 + 2] ^ buf3 ^ buf1;
				buf3 = state[buf4 + 3] ^ buf2;     buf3 = galois_mul2(buf3); state[buf4 + 3] = state[buf4 + 3] ^ buf3 ^ buf1;
			}
		}

		//key schedule
		// compute the 16 next round key bytes
		key[0] = sbox[key[13]] ^ key[0] ^ rcon;
		key[1] = sbox[key[14]] ^ key[1];
		key[2] = sbox[key[15]] ^ key[2];
		key[3] = sbox[key[12]] ^ key[3];
		for (i = 4; i<16; i++) {
			key[i] = key[i] ^ key[i - 4];
		}
		// compute the next Rcon value
		rcon = galois_mul2(rcon);
	}

	// process last AddRoundKey
	for (i = 0; i <16; i++) {
		state[i] = state[i] ^ key[i];
	}
}

/*
Decrypts a HEX string. Must be length 16. This will be converted to 16 binary values before conversion
_input: The string
_output: decrypted string
*/
CString CAES128_2::decryptHexString(CString _input)
{
	UINT8 res[17];
	decryptHexString(res, _input);
	res[16] = 0;
	CString des(res);
	return des;
}


/*
Decrypts a HEX string. Must be length 16. This will be converted to 16 binary values before conversion
_input: The string
result[] : decrypted output 8 integers, binary
*/
void CAES128_2::decryptHexString(INT16 result[], CString _input)
{
	UINT8 resultChar[16]; // unsigned char resultChar[16];
	decryptHexString(resultChar, _input);

	UINT16 ui16;

	ui16 = resultChar[1];
	ui16 <<= 8;
	ui16 += resultChar[0];
	result[0] = (UINT16)ui16;

	ui16 = resultChar[3];
	ui16 <<= 8;
	ui16 += resultChar[2];
	result[1] = (UINT16)ui16;

	ui16 = resultChar[5];
	ui16 <<= 8;
	ui16 += resultChar[4];
	result[2] = (UINT16)ui16;

	ui16 = resultChar[7];
	ui16 <<= 8;
	ui16 += resultChar[6];
	result[3] = (UINT16)ui16;

	ui16 = resultChar[9];
	ui16 <<= 8;
	ui16 += resultChar[8];
	result[4] = (UINT16)ui16;

	ui16 = resultChar[11];
	ui16 <<= 8;
	ui16 += resultChar[10];
	result[5] = (UINT16)ui16;

	ui16 = resultChar[13];
	ui16 <<= 8;
	ui16 += resultChar[12];
	result[6] = (UINT16)ui16;

	ui16 = resultChar[15];
	ui16 <<= 8;
	ui16 += resultChar[14];
	result[7] = (UINT16)ui16;
}

/*
Decrypts a HEX string. Must be length 16. This will be converted to 16 binary values before conversion
_input: The string
result[] : decrypted output 16 characters, binary
*/
void CAES128_2::decryptHexString(UINT8 result[], CString _input)
{
	CString s = _input;
	s.MakeUpper();
	int length = s.GetLength();
	ASSERT(length == 32);

	LPTSTR  smpl = s.GetBuffer();
	UINT8 lsb, msb;
	for (int i = 0; i < 16; i++) {
		msb = hexCharToInt(*smpl++);
		lsb = hexCharToInt(*smpl++);
		result[i] = msb;
		result[i] <<= 4;
		result[i] += lsb;
	}
	decrypt(result);
}

UINT8 CAES128_2::hexCharToInt(WCHAR _c)
{
	switch (_c) {
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'A': return 10;
	case 'B': return 11;
	case 'C': return 12;
	case 'D': return 13;
	case 'E': return 14;
	case 'F': return 15;
	}
	return 0;
}

bool CAES128_2::isEncrypted(CString _s)
{
	if (_s.Find(_T("/")) > 0) return false;
	return true;
}

void CAES128_2::decrypt(UINT8 state[])
{
	UINT8 buf1, buf2, buf3, buf4, round;

	UINT8 key[16];
	memcpy((void *)key, (void *)theKey, 16);

	// compute the last key of encryption before starting the decryption
	for (round = 0; round < 10; round++) {
		//key schedule
		key[0] = sbox[key[13]] ^ key[0] ^ Rcon[round];
		key[1] = sbox[key[14]] ^ key[1];
		key[2] = sbox[key[15]] ^ key[2];
		key[3] = sbox[key[12]] ^ key[3];
		for (int i = 4; i<16; i++) {
			key[i] = key[i] ^ key[i - 4];
		}
	}

	//first Addroundkey
	for (int i = 0; i <16; i++) {
		state[i] = state[i] ^ key[i];
	}

	// main loop
	for (round = 0; round < 10; round++) {

		//Inverse key schedule
		for (int i = 15; i>3; --i) {
			key[i] = key[i] ^ key[i - 4];
		}
		key[0] = sbox[key[13]] ^ key[0] ^ Rcon[9 - round];
		key[1] = sbox[key[14]] ^ key[1];
		key[2] = sbox[key[15]] ^ key[2];
		key[3] = sbox[key[12]] ^ key[3];

		//mixcol - inv mix
		if (round > 0) {
			for (int i = 0; i <4; i++) {
				buf4 = (i << 2);

				// precompute for decryption
				buf1 = galois_mul2(galois_mul2(state[buf4] ^ state[buf4 + 2]));
				buf2 = galois_mul2(galois_mul2(state[buf4 + 1] ^ state[buf4 + 3]));
				state[buf4] ^= buf1; state[buf4 + 1] ^= buf2; state[buf4 + 2] ^= buf1; state[buf4 + 3] ^= buf2;

				// in all cases
				buf1 = state[buf4] ^ state[buf4 + 1] ^ state[buf4 + 2] ^ state[buf4 + 3];
				buf2 = state[buf4];
				buf3 = state[buf4] ^ state[buf4 + 1]; buf3 = galois_mul2(buf3); state[buf4] = state[buf4] ^ buf3 ^ buf1;
				buf3 = state[buf4 + 1] ^ state[buf4 + 2]; buf3 = galois_mul2(buf3); state[buf4 + 1] = state[buf4 + 1] ^ buf3 ^ buf1;
				buf3 = state[buf4 + 2] ^ state[buf4 + 3]; buf3 = galois_mul2(buf3); state[buf4 + 2] = state[buf4 + 2] ^ buf3 ^ buf1;
				buf3 = state[buf4 + 3] ^ buf2;     buf3 = galois_mul2(buf3); state[buf4 + 3] = state[buf4 + 3] ^ buf3 ^ buf1;
			}
		}

		//Inv shift rows
		// Row 1
		buf1 = state[13];
		state[13] = state[9];
		state[9] = state[5];
		state[5] = state[1];
		state[1] = buf1;
		//Row 2
		buf1 = state[10];
		buf2 = state[14];
		state[10] = state[2];
		state[14] = state[6];
		state[2] = buf1;
		state[6] = buf2;
		//Row 3
		buf1 = state[3];
		state[3] = state[7];
		state[7] = state[11];
		state[11] = state[15];
		state[15] = buf1;

		for (int i = 0; i <16; i++) {
			// with shiftrow i+5 mod 16
			state[i] = rsbox[state[i]] ^ key[i];
		}
	}
} // end function

//////////////////////////////////////////////////////////

CEncryptedString::CEncryptedString()
{
	buffer = NULL;
	bufferLength = 0;
	memcpy((void *) theUserKey,(void *) spiroAESKey,16);
}


CEncryptedString::CEncryptedString(CString _encryptedString)
{
	buffer = NULL;
	bufferLength = 0;

	bufferLength = sizeof(wchar_t) * _encryptedString.GetLength() + 1;
	bufferLength >>= BITSHIFTS_IN_16;
	bufferLength += 1;
	bufferLength <<= BITSHIFTS_IN_16;
	buffer = new unsigned char[bufferLength];
	memset((void *) buffer,0,bufferLength);
	wchar_t *tP = _encryptedString.GetBuffer();
	int i = 0;
	while (*tP) {
		memcpy((void *) &buffer[i],tP,sizeof(wchar_t));
		i += sizeof(wchar_t);
		tP++;
	}
}

CEncryptedString::~CEncryptedString()
{
	if (buffer) delete [] buffer;
}

void CEncryptedString::setKey(unsigned char *_key, int _length)
{
	unsigned char thisKey[AES128_MAX_KEY_LENGTH];
	for (int i = 0; i < AES128_MAX_KEY_LENGTH; i++) {
		thisKey[i] = i < _length ? *(_key + i) : ASCII_SPACE;
	}
	memcpy((void *)theUserKey, (void *)thisKey, AES128_MAX_KEY_LENGTH);
	theUserKey[AES128_MAX_KEY_LENGTH] = 0;
}

/*
Decrypts a CString, allocates n*16 byte memory for result and returns pointer to this memory
*/
void CEncryptedString::encryptString(CString _s)
{
	if (buffer) delete [] buffer;
	buffer = NULL;

	bufferLength = sizeof(wchar_t) * _s.GetLength() + 1;
	bufferLength >>= BITSHIFTS_IN_16;
	bufferLength += 1;
	bufferLength <<= BITSHIFTS_IN_16;
	buffer = new unsigned char[bufferLength];
	memset((void *) buffer,0,bufferLength);
	wchar_t *tP = _s.GetBuffer();
	int i = 0;
	while (*tP) {
		memcpy((void *) &buffer[i],tP,sizeof(wchar_t));
		i += sizeof(wchar_t);
		tP++;
	}
	
	i = 0;
	CAES128 machine;
	machine.setKey(theUserKey);
	while (i < bufferLength) {
		machine.encrypt(&buffer[i]);
		i += AES128_MAX_KEY_LENGTH;
	}
}

void CEncryptedString::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		ar << bufferLength;
		ar.Write((void *) buffer,bufferLength);
	}
	else {
		ar >> bufferLength;
		if (buffer) delete [] buffer;
		buffer = new unsigned char[bufferLength];
		ar.Read((void *) buffer,bufferLength);
	}
}

CString CEncryptedString::getEncryptedString(void)
{
	CString s(buffer);
	return s;
}

CString CEncryptedString::getDecryptedString(void)
{
	if (NULL == buffer) return _T("");
	if (0 == bufferLength) return _T("");

	unsigned char *copy = new unsigned char [bufferLength];
	memcpy((void *) copy,(void *) buffer,bufferLength);

	int i = 0;
	CAES128 machine;
	machine.setKey(theUserKey);
	while (i < bufferLength) {
		machine.decrypt(&copy[i]);
		i += AES128_MAX_KEY_LENGTH;
	}
	CString s((wchar_t *) copy);
	delete [] copy;

	return s;
}
