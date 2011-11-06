/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _HTMLOUT_HXX
#define _HTMLOUT_HXX

#include "svtools/svtdllapi.h"
#include <tools/solar.h>
#include <rtl/textenc.h>
#include <svl/macitem.hxx>

class Color;
class ImageMap;
class String;
class SvStream;
class SvxMacroTableDtor;
class SvNumberFormatter;
class SvDataObject;

struct HTMLOutEvent
{
	const sal_Char *pBasicName;
	const sal_Char *pJavaName;
	sal_uInt16 nEvent;
};

struct SVT_DLLPUBLIC HTMLOutContext
{
	rtl_TextEncoding m_eDestEnc;
	rtl_TextToUnicodeConverter m_hConv;
	rtl_TextToUnicodeContext   m_hContext;

	HTMLOutContext( rtl_TextEncoding eDestEnc );
	~HTMLOutContext();
};

struct HTMLOutFuncs
{
#if defined(UNX)
	static const sal_Char sNewLine;		// nur \012 oder \015
#else
	static const sal_Char __FAR_DATA sNewLine[];	// \015\012
#endif

	SVT_DLLPUBLIC static void ConvertStringToHTML( const String& sSrc, ByteString& rDest,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
						String *pNonConvertableChars = 0);

	SVT_DLLPUBLIC static SvStream& Out_AsciiTag( SvStream&, const sal_Char* pStr,
								   sal_Bool bOn = sal_True,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252);
	SVT_DLLPUBLIC static SvStream& Out_Char( SvStream&, sal_Unicode cChar,
						HTMLOutContext& rContext, 
						String *pNonConvertableChars = 0 );
	SVT_DLLPUBLIC static SvStream& Out_String( SvStream&, const String&,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
						String *pNonConvertableChars = 0 );
	SVT_DLLPUBLIC static SvStream& Out_Hex( SvStream&, sal_uLong nHex, sal_uInt8 nLen,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252 );
	SVT_DLLPUBLIC static SvStream& Out_Color( SvStream&, const Color&,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252 );
    SVT_DLLPUBLIC static SvStream& Out_ImageMap( SvStream&, const String&, const ImageMap&, const String&,
								   const HTMLOutEvent *pEventTable,
								   sal_Bool bOutStarBasic,
								   const sal_Char *pDelim = 0,
								   const sal_Char *pIndentArea = 0,
								   const sal_Char *pIndentMap = 0,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
						String *pNonConvertableChars = 0 );
	SVT_DLLPUBLIC static SvStream& FlushToAscii( SvStream&, HTMLOutContext& rContext );

	SVT_DLLPUBLIC static SvStream& OutScript( SvStream& rStrm,
                                const String& rBaseURL,
								const String& rSource,
								const String& rLanguage,
								ScriptType eScriptType,
								const String& rSrc,
								const String *pSBLibrary = 0,
								const String *pSBModule = 0,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
						String *pNonConvertableChars = 0 );

	// der 3. Parameter ist ein Array von HTMLOutEvents, das mit einem
	// nur aus 0 bestehen Eintrag terminiert ist.
	SVT_DLLPUBLIC static SvStream& Out_Events( SvStream&, const SvxMacroTableDtor&,
								 const HTMLOutEvent*, sal_Bool bOutStarBasic,
						rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
						String *pNonConvertableChars = 0 );

	// <TD SDVAL="..." SDNUM="...">
	SVT_DLLPUBLIC static ByteString&	CreateTableDataOptionsValNum( ByteString& aStrTD,
				sal_Bool bValue, double fVal, sal_uLong nFormat,
				SvNumberFormatter& rFormatter,
                rtl_TextEncoding eDestEnc = RTL_TEXTENCODING_MS_1252,
                String *pNonConvertableChars = 0 );

	SVT_DLLPUBLIC static sal_Bool PrivateURLToInternalImg( String& rURL );
};

#endif

