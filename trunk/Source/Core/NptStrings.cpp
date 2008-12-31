/*****************************************************************
|
|   Neptune - String Objects
|
| Copyright (c) 2002-2008, Axiomatic Systems, LLC.
| All rights reserved.
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions are met:
|     * Redistributions of source code must retain the above copyright
|       notice, this list of conditions and the following disclaimer.
|     * Redistributions in binary form must reproduce the above copyright
|       notice, this list of conditions and the following disclaimer in the
|       documentation and/or other materials provided with the distribution.
|     * Neither the name of Axiomatic Systems nor the
|       names of its contributors may be used to endorse or promote products
|       derived from this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY AXIOMATIC SYSTEMS ''AS IS'' AND ANY
| EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
| WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
| DISCLAIMED. IN NO EVENT SHALL AXIOMATIC SYSTEMS BE LIABLE FOR ANY
| DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
| (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
| ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
| (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptTypes.h"
#include "NptConstants.h"
#include "NptStrings.h"
#include "NptResults.h"
#include "NptUtils.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
#define NPT_STRINGS_WHITESPACE_CHARS "\r\n\t "

const unsigned int NPT_STRING_FORMAT_BUFFER_DEFAULT_SIZE = 256;
const unsigned int NPT_STRING_FORMAT_BUFFER_MAX_SIZE     = 0x80000; // 512k

/*----------------------------------------------------------------------
|   helpers
+---------------------------------------------------------------------*/
inline char NPT_Uppercase(char x) {
    return (x >= 'a' && x <= 'z') ? x&0xdf : x;
}

inline char NPT_Lowercase(char x) {
    return (x >= 'A' && x <= 'Z') ? x^32 : x;
}
               
/*----------------------------------------------------------------------
|   NPT_String::EmptyString
+---------------------------------------------------------------------*/
char NPT_String::EmptyString = '\0';

/*----------------------------------------------------------------------
|   NPT_String::FromInteger
+---------------------------------------------------------------------*/
NPT_String
NPT_String::FromInteger(NPT_Int64 value)
{
    char str[32];
    char* c = &str[31];
    *c-- = '\0';

    // handle the sign
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }

    // process the digits
    do {
        int digit = (int)(value%10);
        *c-- = '0'+digit;
        value /= 10;
    } while(value);

    if (negative) {
        *c = '-';
    } else {
        ++c;
    }

    return NPT_String(c);
}

/*----------------------------------------------------------------------
|   NPT_String::FromIntegerU
+---------------------------------------------------------------------*/
NPT_String
NPT_String::FromIntegerU(NPT_UInt64 value)
{
    char str[32];
    char* c = &str[31];
    *c = '\0';

    // process the digits
    do {
        int digit = (int)(value%10);
        *--c = '0'+digit;
        value /= 10;
    } while(value);

    return NPT_String(c);
}

/*----------------------------------------------------------------------
|   NPT_String::Format
+---------------------------------------------------------------------*/
NPT_String
NPT_String::Format(const char* format, ...)
{
    NPT_String result;
    NPT_Size   buffer_size = NPT_STRING_FORMAT_BUFFER_DEFAULT_SIZE; // default value
    
    va_list  args;
    va_start(args, format);

    for(;;) {
        /* try to format (it might not fit) */
        result.Reserve(buffer_size);
        char* buffer = result.UseChars();
        int f_result = NPT_FormatStringVN(buffer, buffer_size, format, args);
        if (f_result >= (int)(buffer_size)) f_result = -1;
        if (f_result >= 0) {
            result.SetLength(f_result);
            break;
        }
        
        /* the buffer was too small, try something bigger         */
        /* (we don't trust the return value of NPT_FormatStringVN */
        /* for the actual size needed)                            */
        buffer_size *= 2;
        if (buffer_size > NPT_STRING_FORMAT_BUFFER_MAX_SIZE) break;
    }

    va_end(args);
    
    return result;
}

/*----------------------------------------------------------------------
|   NPT_String::NPT_String
+---------------------------------------------------------------------*/
NPT_String::NPT_String(const char* str)
{
    if (str == NULL) {
        m_Chars = NULL;
    } else {
        m_Chars = Buffer::Create(str);
    }
}

/*----------------------------------------------------------------------
|   NPT_String::NPT_String
+---------------------------------------------------------------------*/
NPT_String::NPT_String(const char* str, NPT_Size length)
{
    if (str == NULL || length == 0) {
        m_Chars = NULL;
    } else {
        m_Chars = Buffer::Create(str, length);
    }
}

/*----------------------------------------------------------------------
|   NPT_String::NPT_String
+---------------------------------------------------------------------*/
NPT_String::NPT_String(const NPT_String& str)
{
    if (str.GetLength() == 0) {
        m_Chars = NULL;
    } else {
        m_Chars = Buffer::Create(str.GetChars(), str.GetLength());
    }
}

/*----------------------------------------------------------------------
|   NPT_String::NPT_String
+---------------------------------------------------------------------*/
NPT_String::NPT_String(char c, NPT_Cardinal repeat)
{
    if (repeat != 0) {
        m_Chars = Buffer::Create(c, repeat);
    } else {
        m_Chars = NULL;
    }
}

/*----------------------------------------------------------------------
|   NPT_String::SetLength
+---------------------------------------------------------------------*/
NPT_Result
NPT_String::SetLength(NPT_Size length, bool pad)
{
    // special case for 0
    if (length == 0) {
        Reset();
        return NPT_SUCCESS;
    }
    
    // reserve the space
    Reserve(length);

    // pad with spaces if necessary
    char* chars = UseChars();
    if (pad) {
        unsigned int current_length = GetLength();
        if (length > current_length) {
            unsigned int pad_length = length-current_length;
            NPT_SetMemory(chars+current_length, ' ', pad_length);
        }
    }
    
    // update the length and terminate the buffer
    GetBuffer()->SetLength(length);
    chars[length] = '\0';
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   NPT_String::PrepareToWrite
+---------------------------------------------------------------------*/
inline char*
NPT_String::PrepareToWrite(NPT_Size length)
{
    NPT_ASSERT(length != 0);
    if (m_Chars == NULL || GetBuffer()->GetAllocated() < length) {
        // the buffer is too small, we need to allocate a new one.
        NPT_Size needed = length;
        if (m_Chars != NULL) {
            NPT_Size grow = GetBuffer()->GetAllocated()*2;
            if (grow > length) needed = grow;
            delete GetBuffer();
        }
        m_Chars = Buffer::Create(needed);
    }    
    GetBuffer()->SetLength(length);
    return m_Chars;
}

/*----------------------------------------------------------------------
|   NPT_String::Reserve
+---------------------------------------------------------------------*/
void
NPT_String::Reserve(NPT_Size allocate)
{
    if (m_Chars == NULL || GetBuffer()->GetAllocated() < allocate) {
        // the buffer is too small, we need to allocate a new one.
        NPT_Size needed = allocate;
        if (m_Chars != NULL) {
            NPT_Size grow = GetBuffer()->GetAllocated()*2;
            if (grow > allocate) needed = grow;
        }
        NPT_Size length = GetLength();
        char* copy = Buffer::Create(needed, length);
        if (m_Chars != NULL) {
            CopyString(copy, m_Chars);
            delete GetBuffer();
        } else {
            copy[0] = '\0';
        }
        m_Chars = copy;
    }
}

/*----------------------------------------------------------------------
|   NPT_String::Assign
+---------------------------------------------------------------------*/
void
NPT_String::Assign(const char* str, NPT_Size length)
{
    if (str == NULL || length == 0) {
        Reset();
    } else {
        PrepareToWrite(length);
        CopyBuffer(m_Chars, str, length);
        m_Chars[length] = '\0';
    }
}

/*----------------------------------------------------------------------
|   NPT_String::operator=
+---------------------------------------------------------------------*/
NPT_String&
NPT_String::operator=(const char* str)
{
    if (str == NULL) {
        Reset();
    } else {
        NPT_Size length = StringLength(str);
        if (length == 0) {
            Reset();
        } else {
            CopyString(PrepareToWrite(length), str);
        }
    }

    return *this;
}

/*----------------------------------------------------------------------
|   NPT_String::operator=
+---------------------------------------------------------------------*/
NPT_String&
NPT_String::operator=(const NPT_String& str)
{
    // do nothing if we're assigning to ourselves
    if (this != &str) {
        Assign(str.GetChars(), str.GetLength());
    }
    return *this;
}

/*----------------------------------------------------------------------
|   NPT_String::Append
+---------------------------------------------------------------------*/
void
NPT_String::Append(const char* str, NPT_Size length)
{
    // shortcut
    if (str == NULL || length == 0) return;

    // compute the new length
    NPT_Size old_length = GetLength();
    NPT_Size new_length = old_length + length;

    // allocate enough space
    Reserve(new_length);
    
    // append the new string at the end of the current one
    CopyBuffer(m_Chars+old_length, str, length);
    m_Chars[new_length] = '\0';

    // update the length
    GetBuffer()->SetLength(new_length);
}

/*----------------------------------------------------------------------
|   NPT_String::Compare
+---------------------------------------------------------------------*/
int 
NPT_String::Compare(const char *s, bool ignore_case) const
{
    return NPT_String::Compare(GetChars(), s, ignore_case);
}

/*----------------------------------------------------------------------
|   NPT_String::Compare
+---------------------------------------------------------------------*/
int 
NPT_String::Compare(const char *s1, const char *s2, bool ignore_case)
{
    const char *r1 = s1;
    const char *r2 = s2;

    if (ignore_case) {
        while (NPT_Uppercase(*r1) == NPT_Uppercase(*r2)) {
            if (*r1++ == '\0') {
                return 0;
            } 
            r2++;
        }
        return NPT_Uppercase(*r1) - NPT_Uppercase(*r2);
    } else {
        while (*r1 == *r2) {
            if (*r1++ == '\0') {
                return 0;
            } 
            r2++;
        }
        return (*r1 - *r2);
    }
}

/*----------------------------------------------------------------------
|   NPT_String::CompareN
+---------------------------------------------------------------------*/
int 
NPT_String::CompareN(const char *s, NPT_Size count, bool ignore_case) const
{
    return NPT_String::CompareN(GetChars(), s, count, ignore_case);
}

/*----------------------------------------------------------------------
|   NPT_String::CompareN
+---------------------------------------------------------------------*/
int 
NPT_String::CompareN(const char* s1, const char *s2, NPT_Size count, bool ignore_case)
{
    const char* me = s1;

    if (ignore_case) {
        for (unsigned int i=0; i<count; i++) {
            if (NPT_Uppercase(me[i]) != NPT_Uppercase(s2[i])) {
                return NPT_Uppercase(me[i]) - NPT_Uppercase(s2[i]);
            }
        }
        return 0;
    } else {
        for (unsigned int i=0; i<count; i++) {
            if (me[i] != s2[i]) {
                return (me[i] - s2[i]);
            }
        }
        return 0;
    }
}

/*----------------------------------------------------------------------
|   NPT_String::Split
+---------------------------------------------------------------------*/
NPT_List<NPT_String> 
NPT_String::Split(const char* separator) const
{
    NPT_List<NPT_String> result;
    NPT_Size             separator_length = NPT_StringLength(separator);
    
    // sepcial case for empty separators
    if (separator_length == 0) {
        result.Add(*this);
        return result;
    }
    
    int current = 0;  
    int next;  
    do {
        next = Find(separator, current);
        unsigned int end = (next>=0?next:GetLength());
        result.Add(SubString(current, end-current));
        current = next+separator_length;
    } while (next >= 0);
    
    return result;
}

/*----------------------------------------------------------------------
|   NPT_String::SubString
+---------------------------------------------------------------------*/
NPT_String
NPT_String::SubString(NPT_Ordinal first, NPT_Size length) const
{
    if (first >= GetLength()) {
        first = GetLength();
        length = 0;
    } else if (first+length >= GetLength()) {
        length = GetLength()-first;
    }
    return NPT_String(GetChars()+first, length);
}

/*----------------------------------------------------------------------
|   NPT_StringStartsWith
|
|    returns:
|   1 if str starts with sub,
|   0 if str is large enough but does not start with sub
|     -1 if str is too short to start with sub
+---------------------------------------------------------------------*/
static inline int
NPT_StringStartsWith(const char* str, const char* sub, bool ignore_case)
{
    if (ignore_case) {
        while (NPT_Uppercase(*str) == NPT_Uppercase(*sub)) {
            if (*str++ == '\0') {
                return 1;
            }
            sub++;
        }
    } else {
        while (*str == *sub) {
            if (*str++ == '\0') {
                return 1;
            }
            sub++;
        }
    }
    return (*sub == '\0') ? 1 : (*str == '\0' ? -1 : 0);
}

/*----------------------------------------------------------------------
|   NPT_String::StartsWith
+---------------------------------------------------------------------*/
bool 
NPT_String::StartsWith(const char *s, bool ignore_case) const
{
    if (s == NULL) return false;
    return NPT_StringStartsWith(GetChars(), s, ignore_case) == 1;
}

/*----------------------------------------------------------------------
|   NPT_String::EndsWith
+---------------------------------------------------------------------*/
bool 
NPT_String::EndsWith(const char *s, bool ignore_case) const
{
    if (s == NULL) return false;
    NPT_Size str_length = NPT_StringLength(s);
    if (str_length > GetLength()) return false;
    return NPT_StringStartsWith(GetChars()+GetLength()-str_length, s, ignore_case) == 1;
}

/*----------------------------------------------------------------------
|   NPT_String::Find
+---------------------------------------------------------------------*/
int
NPT_String::Find(const char* str, NPT_Ordinal start, bool ignore_case) const
{
    // check args
    if (str == NULL || start >= GetLength()) return -1;

    // skip to start position
    const char* src = m_Chars + start;

    // look for a substring
    while (*src) {
        int cmp = NPT_StringStartsWith(src, str, ignore_case);
        switch (cmp) {
            case -1:
                // ref is too short, abort
                return -1;
            case 1:
                // match
                return (int)(src-m_Chars);
        }
        src++;
    }

    return -1;
}

/*----------------------------------------------------------------------
|   NPT_String::Find
+---------------------------------------------------------------------*/
int
NPT_String::Find(char c, NPT_Ordinal start, bool ignore_case) const
{
    // check args
    if (start >= GetLength()) return -1;

    // skip to start position
    const char* src = m_Chars + start;

    // look for the character
    if (ignore_case) {
        while (*src) {
            if (NPT_Uppercase(*src) == NPT_Uppercase(c)) {
                return (int)(src-m_Chars);
            }
            src++;
        }
    } else {
        while (*src) {
            if (*src == c) return (int)(src-m_Chars);
            src++;
        }
    }

    return -1;
}

/*----------------------------------------------------------------------
|   NPT_String::ReverseFind
+---------------------------------------------------------------------*/
int
NPT_String::ReverseFind(const char* str, NPT_Ordinal start, bool ignore_case) const
{
    // check args
    if (str == NULL || *str == '\0') return -1;

    // look for a substring
    NPT_Size my_length = GetLength();
    NPT_Size str_length = NPT_StringLength(str);
    int i=my_length-start-str_length;
    const char* src = GetChars();
    if (i<0) return -1;
    for (;i>=0; i--) {
        int cmp = NPT_StringStartsWith(src+i, str, ignore_case);
        if (cmp == 1) {
            // match
            return i;
        }
    }

    return -1;
}

/*----------------------------------------------------------------------
|   NPT_String::ReverseFind
+---------------------------------------------------------------------*/
int
NPT_String::ReverseFind(char c, NPT_Ordinal start, bool ignore_case) const
{
    // check args
    NPT_Size length = GetLength();
    int i = length-start-1;
    if (i < 0) return -1;

    // look for the character
    const char* src = GetChars();
    if (ignore_case) {
        for (;i>=0;i--) {
            if (NPT_Uppercase(src[i]) == NPT_Uppercase(c)) {
                return i;
            }
        }
    } else {
        for (;i>=0;i--) {
            if (src[i] == c) return i;
        }
    }

    return -1;
}

/*----------------------------------------------------------------------
|   NPT_String::MakeLowercase
+---------------------------------------------------------------------*/
void
NPT_String::MakeLowercase()
{
    // the source is the current buffer
    const char* src = GetChars();

    // convert all the characters of the existing buffer
    char* dst = const_cast<char*>(src);
    while (*dst != '\0') {
        *dst = NPT_Lowercase(*dst);
        dst++;
    }
}

/*----------------------------------------------------------------------
|   NPT_String::MakeUppercase
+---------------------------------------------------------------------*/
void
NPT_String::MakeUppercase() 
{
    // the source is the current buffer
    const char* src = GetChars();

    // convert all the characters of the existing buffer
    char* dst = const_cast<char*>(src);
    while (*dst != '\0') {
        *dst = NPT_Uppercase(*dst);
        dst++;
    }
}

/*----------------------------------------------------------------------
|   NPT_String::ToLowercase
+---------------------------------------------------------------------*/
NPT_String
NPT_String::ToLowercase() const
{
    NPT_String result(*this);
    result.MakeLowercase();
    return result;
}

/*----------------------------------------------------------------------
|   NPT_String::ToUppercase
+---------------------------------------------------------------------*/
NPT_String
NPT_String::ToUppercase() const
{
    NPT_String result(*this);
    result.MakeUppercase();
    return result;
}

/*----------------------------------------------------------------------
|   NPT_String::Replace
+---------------------------------------------------------------------*/
void
NPT_String::Replace(char a, char b) 
{
    // check args
    if (m_Chars == NULL || a == '\0' || b == '\0') return;

    // we are going to modify the characters
    char* src = m_Chars;

    // process the buffer in place
    while (*src) {
        if (*src == a) *src = b;
        src++;
    }
}

/*----------------------------------------------------------------------
|   NPT_String::Insert
+---------------------------------------------------------------------*/
void
NPT_String::Insert(const char* str, NPT_Ordinal where)
{
    // check args
    if (str == NULL || where > GetLength()) return;

    // measure the string to insert
    NPT_Size str_length = StringLength(str);
    if (str_length == 0) return;

    // compute the size of the new string
    NPT_Size old_length = GetLength();
    NPT_Size new_length = str_length + GetLength();

    // prepare to write the new string
    char* src = m_Chars;
    char* nst = Buffer::Create(new_length, new_length);
    char* dst = nst;

    // copy the beginning of the old string
    if (where > 0) {
        CopyBuffer(dst, src, where);
        src += where;
        dst += where;
    }

    // copy the inserted string
    CopyString(dst, str);
    dst += str_length;

    // copy the end of the old string
    if (old_length > where) {
        CopyString(dst, src);
    }

    // use the new string
    if (m_Chars) delete GetBuffer();
    m_Chars = nst;
}

/*----------------------------------------------------------------------
|   NPT_String::Erase
+---------------------------------------------------------------------*/
void
NPT_String::Erase(NPT_Ordinal start, NPT_Cardinal count /* = 1 */)
{
    // check bounds
    NPT_Size length = GetLength();
    if (start+count > length) {
        if (start >= length) return;
        count = length-start;
    }
    if (count == 0) return;

    CopyString(m_Chars+start, m_Chars+start+count);
    GetBuffer()->SetLength(length-count);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger(int& value, bool relaxed) const
{
    return NPT_ParseInteger(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger(unsigned int& value, bool relaxed) const
{
    return NPT_ParseInteger(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger(long& value, bool relaxed) const
{
    return NPT_ParseInteger(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger(unsigned long& value, bool relaxed) const
{
    return NPT_ParseInteger(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger32
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger32(NPT_Int32& value, bool relaxed) const
{
    return NPT_ParseInteger32(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger32
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger32(NPT_UInt32& value, bool relaxed) const
{
    return NPT_ParseInteger32(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger64
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger64(NPT_Int64& value, bool relaxed) const
{
    return NPT_ParseInteger64(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToInteger64
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger64(NPT_UInt64& value, bool relaxed) const
{
    return NPT_ParseInteger64(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|    NPT_String::ToFloat
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToFloat(float& value, bool relaxed) const
{
    return NPT_ParseFloat(GetChars(), value, relaxed);
}

/*----------------------------------------------------------------------
|   NPT_String::TrimLeft
+---------------------------------------------------------------------*/
void 
NPT_String::TrimLeft()
{
    TrimLeft(NPT_STRINGS_WHITESPACE_CHARS);
}

/*----------------------------------------------------------------------
|   NPT_String::TrimLeft
+---------------------------------------------------------------------*/
void 
NPT_String::TrimLeft(char c)
{
    char s[2] = {c, 0};
    TrimLeft((const char*)s);
}

/*----------------------------------------------------------------------
|   NPT_String::TrimLeft
+---------------------------------------------------------------------*/
void 
NPT_String::TrimLeft(const char* chars)
{
    if (m_Chars == NULL) return;
    const char* s = m_Chars;
    while (char c = *s) {
        const char* x = chars;
        while (*x) {
            if (*x == c) break;
            x++;
        }
        if (*x == 0) break; // not found
        s++;
    }
    if (s == m_Chars) {
        // nothing was trimmed
        return;
    }

    // shift chars to the left
    char* d = m_Chars;
    GetBuffer()->SetLength(GetLength()-(s-d));
    while ((*d++ = *s++)) {};
}

/*----------------------------------------------------------------------
|   NPT_String::TrimRight
+---------------------------------------------------------------------*/
void 
NPT_String::TrimRight()
{
    TrimRight(NPT_STRINGS_WHITESPACE_CHARS);
}

/*----------------------------------------------------------------------
|   NPT_String::TrimRight
+---------------------------------------------------------------------*/
void 
NPT_String::TrimRight(char c)
{
    char s[2] = {c, 0};
    TrimRight((const char*)s);
}

/*----------------------------------------------------------------------
|   NPT_String::TrimRight
+---------------------------------------------------------------------*/
void 
NPT_String::TrimRight(const char* chars)
{
    if (m_Chars == NULL || m_Chars[0] == '\0') return;
    char* tail = m_Chars+GetLength()-1;
    char* s = tail;
    while (s != m_Chars-1) {
        const char* x = chars;
        while (*x) {
            if (*x == *s) {
                *s = '\0';
                break;
            }
            x++;
        }
        if (*x == 0) break; // not found
        s--;
    }
    if (s == tail) {
        // nothing was trimmed
        return;
    }
    GetBuffer()->SetLength(1+(int)(s-m_Chars));
}

/*----------------------------------------------------------------------
|   NPT_String::Trim
+---------------------------------------------------------------------*/
void 
NPT_String::Trim()
{
    TrimLeft();
    TrimRight();
}

/*----------------------------------------------------------------------
|   NPT_String::Trim
+---------------------------------------------------------------------*/
void 
NPT_String::Trim(char c)
{
    char s[2] = {c, 0};
    TrimLeft((const char*)s);
    TrimRight((const char*)s);
}

/*----------------------------------------------------------------------
|   NPT_String::Trim
+---------------------------------------------------------------------*/
void 
NPT_String::Trim(const char* chars)
{
    TrimLeft(chars);
    TrimRight(chars);
}

/*----------------------------------------------------------------------
|   NPT_String::operator+(const NPT_String&, const char*)
+---------------------------------------------------------------------*/
NPT_String 
operator+(const NPT_String& s1, const char* s2)
{
    // shortcut
    if (s2 == NULL) return NPT_String(s1);

    // measure strings
    NPT_Size s1_length = s1.GetLength();
    NPT_Size s2_length = NPT_String::StringLength(s2);

    // allocate space for the new string
    NPT_String result;
    char* start = result.PrepareToWrite(s1_length+s2_length);

    // concatenate the two strings into the result
    NPT_String::CopyBuffer(start, s1, s1_length);
    NPT_String::CopyString(start+s1_length, s2);
    
    return result;
}

/*----------------------------------------------------------------------
|   NPT_String::operator+(const NPT_String& , const char*)
+---------------------------------------------------------------------*/
NPT_String 
operator+(const char* s1, const NPT_String& s2)
{
    // shortcut
    if (s1 == NULL) return NPT_String(s2);

    // measure strings
    NPT_Size s1_length = NPT_String::StringLength(s1);
    NPT_Size s2_length = s2.GetLength();

    // allocate space for the new string
    NPT_String result;
    char* start = result.PrepareToWrite(s1_length+s2_length);

    // concatenate the two strings into the result
    NPT_String::CopyBuffer(start, s1, s1_length);
    NPT_String::CopyString(start+s1_length, s2.GetChars());
    
    return result;
}

/*----------------------------------------------------------------------
|   NPT_String::operator+(const NPT_String& , char)
+---------------------------------------------------------------------*/
NPT_String 
operator+(const NPT_String& s1, char c)
{
    // allocate space for the new string
    NPT_String result;
    result.Reserve(s1.GetLength()+1);
    
    // append
    result = s1;
    result += c;

    return result;
}

