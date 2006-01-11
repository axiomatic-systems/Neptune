/*****************************************************************
|
|      Neptune - String Objects
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptConfig.h"
#include "NptTypes.h"
#include "NptConstants.h"
#include "NptStrings.h"
#include "NptResults.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       constants
+---------------------------------------------------------------------*/
#define NPT_STRINGS_WHITESPACE_CHARS "\r\n\t "

/*----------------------------------------------------------------------
|       helpers
+---------------------------------------------------------------------*/
inline char NPT_Uppercase(char x) {
    return (x >= 'a' && x <= 'z') ? x&0xdf : x;
}

inline char NPT_Lowercase(char x) {
    return (x >= 'A' && x <= 'Z') ? x^32 : x;
}
               
/*----------------------------------------------------------------------
|       NPT_String::EmptyString
+---------------------------------------------------------------------*/
char NPT_String::EmptyString = '\0';

/*----------------------------------------------------------------------
|       NPT_String::NPT_String
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
|       NPT_String::NPT_String
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
|       NPT_String::NPT_String
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
|       NPT_String::NPT_String
+---------------------------------------------------------------------*/
NPT_String::NPT_String(const char* str,
                       NPT_Ordinal first, 
                       NPT_Size    length)
{
    // shortcut
    if (str != NULL && length != 0) {
        // truncate length      
        NPT_Size str_length = StringLength(str);
        if (first < str_length) {
            if (first+length > str_length) {
                length = str_length-first;
            }
            if (length != 0) {
                m_Chars = Buffer::Create(str+first, length);
                return;
            }
        }
    } 
    m_Chars = NULL;
}

/*----------------------------------------------------------------------
|       NPT_String::NPT_String
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
|       NPT_String::SetLength
+---------------------------------------------------------------------*/
NPT_Result
NPT_String::SetLength(NPT_Size length)
{
    if (m_Chars == NULL) {
        return (length == 0 ? NPT_SUCCESS : NPT_ERROR_INVALID_PARAMETERS);
    }
    if (length <= GetBuffer()->GetAllocated()) {
        char* chars = UseChars();
        GetBuffer()->SetLength(length);
        chars[length] = '\0';
        return NPT_SUCCESS;
    } else {
        return NPT_ERROR_INVALID_PARAMETERS;
    }
}

/*----------------------------------------------------------------------
|       NPT_String::PrepareToWrite
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
|       NPT_String::PrepareToAppend
+---------------------------------------------------------------------*/
inline void
NPT_String::PrepareToAppend(NPT_Size length, NPT_Size allocate)
{
    NPT_ASSERT(allocate >= length);
    if (m_Chars == NULL || GetBuffer()->GetAllocated() < allocate) {
        // the buffer is too small, we need to allocate a new one.
        NPT_Size needed = allocate;
        if (m_Chars != NULL) {
            NPT_Size grow = GetBuffer()->GetAllocated()*2;
            if (grow > allocate) needed = grow;
        }
        char* copy = Buffer::Create(needed);
        if (m_Chars != NULL) {
            CopyString(copy, m_Chars);
            delete GetBuffer();
        } else {
            copy[0] = '\0';
        }
        m_Chars = copy;
    }
    GetBuffer()->SetLength(length);
}

/*----------------------------------------------------------------------
|       NPT_String::Reserve
+---------------------------------------------------------------------*/
void
NPT_String::Reserve(NPT_Size length)
{
    PrepareToAppend(GetLength(), length);
}

/*----------------------------------------------------------------------
|       NPT_String::Assign
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
|       NPT_String::operator=
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
            PrepareToWrite(length);
            CopyString(m_Chars, str);
        }
    }

    return *this;
}

/*----------------------------------------------------------------------
|       NPT_String::operator=
+---------------------------------------------------------------------*/
NPT_String&
NPT_String::operator=(const NPT_String& str)
{
    Assign(str.GetChars(), str.GetLength());
    return *this;
}

/*----------------------------------------------------------------------
|       NPT_String::Append
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
    PrepareToAppend(new_length, new_length);
    
    // append the new string at the end of the current one
    CopyBuffer(m_Chars+old_length, str, length);
    m_Chars[new_length] = '\0';
}

/*----------------------------------------------------------------------
|       NPT_String::Compare
+---------------------------------------------------------------------*/
int 
NPT_String::Compare(const char *s, bool ignore_case) const
{
    const char *r1 = GetChars();
    const char *r2 = s;

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
|       NPT_String::SubString
+---------------------------------------------------------------------*/
NPT_String
NPT_String::SubString(NPT_Ordinal first, NPT_Size length) const
{
    return NPT_String(*this, first, length);
}

/*----------------------------------------------------------------------
|       NPT_StringStartsWith
|
|    returns:
|      1 if str starts with sub,
|      0 if str is large enough but does not start with sub
|     -1 if str is too short to start with sub
+---------------------------------------------------------------------*/
static inline int
NPT_StringStartsWith(const char* str, const char* sub)
{
    while (*str == *sub) {
        if (*str++ == '\0') {
            return 1;
        }
        sub++;
    }
    return (*sub == '\0') ? 1 : (*str == '\0' ? -1 : 0);
}

/*----------------------------------------------------------------------
|       NPT_String::StartsWith
+---------------------------------------------------------------------*/
bool 
NPT_String::StartsWith(const char *s) const
{
    return NPT_StringStartsWith(GetChars(), s) == 1;
}

/*----------------------------------------------------------------------
|       NPT_String::Find
+---------------------------------------------------------------------*/
int
NPT_String::Find(const char* str, NPT_Ordinal start) const
{
    // check args
    if (str == NULL || start >= GetLength()) return -1;

    // skip to start position
    const char* src = m_Chars + start;

    // look for a substring
    while (*src) {
        int cmp = NPT_StringStartsWith(src, str);
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
|       NPT_String::Find
+---------------------------------------------------------------------*/
int
NPT_String::Find(char c, NPT_Ordinal start) const
{
    // check args
    if (start >= GetLength()) return -1;

    // skip to start position
    const char* src = m_Chars + start;

    // look for the character
    while (*src) {
        if (*src == c) return (int)(src-m_Chars);
        src++;
    }

    return -1;
}

/*----------------------------------------------------------------------
|       NPT_String::MakeLowercase
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
|       NPT_String::MakeUppercase
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
|       NPT_String::ToLowercase
+---------------------------------------------------------------------*/
NPT_String
NPT_String::ToLowercase() const
{
    NPT_String result(*this);
    result.MakeLowercase();
    return result;
}

/*----------------------------------------------------------------------
|       NPT_String::ToUppercase
+---------------------------------------------------------------------*/
NPT_String
NPT_String::ToUppercase() const
{
    NPT_String result(*this);
    result.MakeUppercase();
    return result;
}

/*----------------------------------------------------------------------
|       NPT_String::Replace
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
|       NPT_String::Insert
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
|    NPT_String::ToInteger
+---------------------------------------------------------------------*/
NPT_Result 
NPT_String::ToInteger(long& value, bool relaxed) const
{
    bool negative   = false;
    bool empty      = true;
    const char* str = GetChars();
    char c;

    // defaults
    value = 0;

    // ignore leading whitespace
    if (relaxed) {
        while (*str == ' ' || *str == '\t') {
            str++;
        }
    }
    if (str == NULL || *str == '\0') {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // check for sign
    if (*str == '-') {
        // negative number
        negative = true; 
        str++;
    } else if (*str == '+') {
        // skip the + sign
        str++;
    }

    while ((c = *str++)) {
        if (c >= '0' && c <= '9') {
            value = 10*value + (c-'0');
            empty = false;
        } else {
            if (relaxed) {
                break;
            } else {
                return NPT_ERROR_INVALID_PARAMETERS;
            }
        } 
    }

    // check that the value was non empty
    if (empty) {
        return NPT_ERROR_INVALID_PARAMETERS;
    }

    // return the result
    value = negative ? -value : value;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_String::TrimLeft
+---------------------------------------------------------------------*/
void 
NPT_String::TrimLeft()
{
    TrimLeft(NPT_STRINGS_WHITESPACE_CHARS);
}

/*----------------------------------------------------------------------
|       NPT_String::TrimLeft
+---------------------------------------------------------------------*/
void 
NPT_String::TrimLeft(char c)
{
    char s[2] = {c, 0};
    TrimLeft((const char*)s);
}

/*----------------------------------------------------------------------
|       NPT_String::TrimLeft
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
|       NPT_String::TrimRight
+---------------------------------------------------------------------*/
void 
NPT_String::TrimRight()
{
    TrimRight(NPT_STRINGS_WHITESPACE_CHARS);
}

/*----------------------------------------------------------------------
|       NPT_String::TrimRight
+---------------------------------------------------------------------*/
void 
NPT_String::TrimRight(char c)
{
    char s[2] = {c, 0};
    TrimRight((const char*)s);
}

/*----------------------------------------------------------------------
|       NPT_String::TrimRight
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
|       NPT_String::Trim
+---------------------------------------------------------------------*/
void 
NPT_String::Trim()
{
    TrimLeft();
    TrimRight();
}

/*----------------------------------------------------------------------
|       NPT_String::Trim
+---------------------------------------------------------------------*/
void 
NPT_String::Trim(char c)
{
    char s[2] = {c, 0};
    TrimLeft((const char*)s);
    TrimRight((const char*)s);
}

/*----------------------------------------------------------------------
|       NPT_String::Trim
+---------------------------------------------------------------------*/
void 
NPT_String::Trim(const char* chars)
{
    TrimLeft(chars);
    TrimRight(chars);
}

/*----------------------------------------------------------------------
|       NPT_String::operator+(const NPT_String&, const char*)
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
|       NPT_String::operator+(const NPT_String& , const char*)
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
