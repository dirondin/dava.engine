/*==================================================================================
Copyright (c) 2008, binaryzebra
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the binaryzebra nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/

#ifndef __DAVAENGINE_STRING_UTILS__
#define __DAVAENGINE_STRING_UTILS__

#include "Base/BaseTypes.h"
#include <fribidi/fribidi-bidi-types.h>

namespace DAVA
{
/**
 * \namespace StringUtils
 *
 * \brief Namespace with string helper functions.
 */
namespace StringUtils
{
/**
* \enum eLineBreakType
*
* \brief Values that represent line break types.
*/
static enum eLineBreakType
{
    LB_MUSTBREAK = 0,   /**< Break is mandatory */
    LB_ALLOWBREAK,      /**< Break is allowed */
    LB_NOBREAK,         /**< No break is possible */
    LB_INSIDECHAR       /**< A UTF-8/16 sequence is unfinished */
};

/**
* \brief Gets information about line breaks using libunibreak library.
* \param string The input string.
* \param [out] breaks The output vector of breaks.
* \param locale (Optional) The locale code.
*/
void GetLineBreaks(const WideString& string, Vector<uint8>& breaks, const char8* locale = 0);

/**
* \brief Trims the given string.
* \param [in,out] string The string.
*/
void Trim(WideString& string);

/**
* \brief Trim left.
* \param [in,out] string The string.
*/
void TrimLeft(WideString& string);

/**
* \brief Trim right.
* \param [in,out] string The string.
*/
static void TrimRight(WideString& string);

/**
 * \brief Query if 't' is space.
 * \param t The char16 to process.
 * \return true if space, false if not.
 */
inline bool IsWhitespace(char16 t)
{
    return iswspace(static_cast<wint_t>(t)) != 0;
}

struct sBiDiParams
{
    sBiDiParams()
    {
        base_dir = FRIBIDI_PAR_ON;
        embedding_levels = NULL;
        bidi_types = NULL;
        processStr = NULL;
    }
    ~sBiDiParams()
    {
        SAFE_DELETE(embedding_levels);
        SAFE_DELETE(bidi_types);
        SAFE_DELETE(processStr);
    }
    FriBidiParType base_dir;
    FriBidiLevel *embedding_levels;
    FriBidiCharType *bidi_types;
    FriBidiChar *processStr;
};

bool BiDiPrepare(const WideString& logicalStr, WideString& shapeStr, WideString& visualStr, sBiDiParams& params, bool* isRTL);

bool BiDiReorder(WideString& string, sBiDiParams& params, uint32 lineOffset);

/**
 * \brief Bi-directional text transform.
 * \param [in,out] string The string.
 * \param [out] isRTL Sets true if given string is Right-to-Left.
 * \param [out] logical2virtual (Optional) If non-null, pointer to the logical to virtual indeces list.
 * \param [out] virtual2logical (Optional) If non-null, pointer to the virtual to logical indeces list.
 * \return true if it succeeds, false if it fails.
 */
bool BiDiTransform(WideString& string, bool& isRTL, Vector<int32>* logical2virtual = 0, Vector<int32>* virtual2logical = 0);

bool BiDiTransformEx(const WideString& logicalStr, WideString& shapedStr, WideString& visualStr, bool& isRTL, Vector<int32>* logical2virtual = 0, Vector<int32>* virtual2logical = 0);

}
}

#endif 
