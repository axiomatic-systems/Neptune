/*****************************************************************
|
|      Neptune - Xml Parser
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
#include "NptXmlParser.h"
#include "NptUtils.h"
#include "NptDebug.h"

/*----------------------------------------------------------------------
|       local compilation flags
+---------------------------------------------------------------------*/
//#define NPT_XML_PARSER_DEBUG
#ifdef NPT_XML_PARSER_DEBUG
#define NPT_XML_Debug_0(s) NPT_Debug(s)
#define NPT_XML_Debug_1(s,x0) NPT_Debug(s,x0)
#define NPT_XML_Debug_2(s,x0,x1) NPT_Debug(s,x0,x1)
#define NPT_XML_Debug_3(s,x0,x1,x2) NPT_Debug(s,x0,x1,x2)
#define NPT_XML_Debug_4(s,x0,x1,x2,x3) NPT_Debug(s,x0,x1,x2,x3)
#else
#define NPT_XML_Debug_0(s)
#define NPT_XML_Debug_1(s,x0)
#define NPT_XML_Debug_2(s,x0,x1)
#define NPT_XML_Debug_3(s,x0,x1,x2)
#define NPT_XML_Debug_4(s,x0,x1,x2,x3)
#endif

/*----------------------------------------------------------------------
|       NPT_XmlAttributeFinder
+---------------------------------------------------------------------*/
class NPT_XmlAttributeFinder : public NPT_ListItemFinder<NPT_XmlAttribute>
{
public:
    NPT_XmlAttributeFinder(const char* name) : m_Name(name) {}
    NPT_Result operator()(const NPT_XmlAttribute& attribute) const {
        if (attribute.m_Name == m_Name) {
            return NPT_SUCCESS;
        } else {
            return NPT_FAILURE;
        }
    }

private:
    const char* m_Name;
};

/*----------------------------------------------------------------------
|       NPT_XmlTagFinder
+---------------------------------------------------------------------*/
class NPT_XmlTagFinder : public NPT_ListItemFinder<NPT_XmlNode*>
{
public:
    NPT_XmlTagFinder(const char* tag) : m_Tag(tag) {}
    NPT_Result operator()(NPT_XmlNode* const & node) const {
        const NPT_XmlElementNode* element = node->AsElementNode();
        if (element && element->m_Tag == m_Tag) {
            return NPT_SUCCESS;
        } else {
            return NPT_FAILURE;
        }
    }

private:
    const char* m_Tag;
};

/*----------------------------------------------------------------------
|       NPT_XmlTextFinder
+---------------------------------------------------------------------*/
class NPT_XmlTextFinder : public NPT_ListItemFinder<NPT_XmlNode*>
{
public:
    NPT_Result operator()(NPT_XmlNode* const & node) const {
        if (node->AsTextNode() != NULL) {
            return NPT_SUCCESS;
        } else {
            return NPT_FAILURE;
        }
    }
};

/*----------------------------------------------------------------------
|       NPT_XmlNode::NPT_XmlNode
+---------------------------------------------------------------------*/
NPT_XmlNode::NPT_XmlNode(Type type) :
    m_Type(type),
    m_Parent(NULL)
{
}

/*----------------------------------------------------------------------
|       NPT_XmlNode::~NPT_XmlNode
+---------------------------------------------------------------------*/
NPT_XmlNode::~NPT_XmlNode()
{
}

/*----------------------------------------------------------------------
|       NPT_XmlNode::SetParent
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlNode::SetParent(NPT_XmlNode* parent)
{
    m_Parent = parent;
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_XmlNode::GetParent
+---------------------------------------------------------------------*/
NPT_XmlNode*
NPT_XmlNode::GetParent() const
{
    return m_Parent;
}
    
/*----------------------------------------------------------------------
|       NPT_XmlElementNode::NPT_XmlElementNode
+---------------------------------------------------------------------*/
NPT_XmlElementNode::NPT_XmlElementNode(const char* tag) :
    NPT_XmlNode(ELEMENT),
    m_Tag(tag)
{
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::~NPT_XmlElementNode
+---------------------------------------------------------------------*/
NPT_XmlElementNode::~NPT_XmlElementNode()
{
    m_Children.Apply(NPT_ObjectDeleter<NPT_XmlNode>());
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::AddChild
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlElementNode::AddChild(NPT_XmlNode* child)
{
    child->SetParent(this);
    return m_Children.Add(child);
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::GetChild
+---------------------------------------------------------------------*/
NPT_XmlElementNode*
NPT_XmlElementNode::GetChild(const char* tag, NPT_Ordinal n)
{
    NPT_XmlNode* node;
    if (NPT_SUCCEEDED(m_Children.Find(NPT_XmlTagFinder(tag), node, n))) {
        return node->AsElementNode();
    } else {
        return NULL;
    }
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::AddAttribute
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlElementNode::AddAttribute(const char* name, const char* value)
{
    if (name == NULL || value == NULL) return NPT_ERROR_INVALID_PARAMETERS;
    return m_Attributes.Add(NPT_XmlAttribute(name, value));
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::GetAttribute
+---------------------------------------------------------------------*/
const NPT_StringObject*
NPT_XmlElementNode::GetAttribute(const char* name) const
{
    NPT_XmlAttribute* attribute;
    if (NPT_SUCCEEDED(m_Attributes.Find(NPT_XmlAttributeFinder(name), 
                                        attribute))) { 
        return &attribute->GetValue();
    } else {
        return NULL;
    }
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::AddText
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlElementNode::AddText(const char* text)
{
    return AddChild(new NPT_XmlTextNode(text));
}

/*----------------------------------------------------------------------
|       NPT_XmlElementNode::GetText
+---------------------------------------------------------------------*/
const NPT_StringObject*
NPT_XmlElementNode::GetText(NPT_Ordinal n) const
{
    NPT_XmlNode* node;
    if (NPT_SUCCEEDED(m_Children.Find(NPT_XmlTextFinder(), node, n))) {
        return &node->AsTextNode()->GetString();
    } else {
        return NULL;
    }
}

/*----------------------------------------------------------------------
|       NPT_XmlTextNode::NPT_XmlTextNode
+---------------------------------------------------------------------*/
NPT_XmlTextNode::NPT_XmlTextNode(const char* text) :
    NPT_XmlNode(TEXT),
    m_Text(text)
{
}

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator
+---------------------------------------------------------------------*/
class NPT_XmlAccumulator {
public:
             NPT_XmlAccumulator();
            ~NPT_XmlAccumulator();
    void     Append(char c);
    void     Append(const char* s);
    void     Reset() { m_Valid = 0; }
    char*    GetString();
    NPT_Size GetSize()   { return m_Valid;  }
    char*    GetBuffer() { return m_Buffer; }

private:
    char*    m_Buffer;
    NPT_Size m_Allocated;
    NPT_Size m_Valid;

    void Allocate(NPT_Size size);
};

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator::NPT_XmlAccumulator
+---------------------------------------------------------------------*/
NPT_XmlAccumulator::NPT_XmlAccumulator() :
    m_Buffer(NULL),
    m_Allocated(0),
    m_Valid(0)
{
}

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator::~NPT_XmlAccumulator
+---------------------------------------------------------------------*/
NPT_XmlAccumulator::~NPT_XmlAccumulator()
{
    delete[] m_Buffer;
}

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator::Allocate
+---------------------------------------------------------------------*/
void
NPT_XmlAccumulator::Allocate(NPT_Size size)
{
    // check if we have enough
    if (m_Allocated >= size) return;

    // compute new size
    do {
        m_Allocated = m_Allocated ? m_Allocated * 2 : 32;
    } while (m_Allocated < size);

    // reallocate
    char* new_buffer = new char[m_Allocated];
    NPT_CopyMemory(new_buffer, m_Buffer, m_Valid);
    delete[] m_Buffer;
    m_Buffer = new_buffer;
}

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator::Append
+---------------------------------------------------------------------*/
inline void
NPT_XmlAccumulator::Append(char c)
{
    Allocate(m_Valid+1);
    m_Buffer[m_Valid++] = c;
}

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator::Append
+---------------------------------------------------------------------*/
void
NPT_XmlAccumulator::Append(const char* s)
{
    char c;
    while ((c = *s++)) Append(c);
}

/*----------------------------------------------------------------------
|       NPT_XmlAccumulator::GetString
+---------------------------------------------------------------------*/
inline char*
NPT_XmlAccumulator::GetString()
{
    // ensure that the buffer is NULL terminated 
    Allocate(m_Valid+1);
    m_Buffer[m_Valid] = '\0';
    return (char*)m_Buffer;
}

/*----------------------------------------------------------------------
|       macros
+---------------------------------------------------------------------*/
#define NPT_XML_CHAR_IS_WHITESPACE(c) \
((c) == ' ' || (c) == '\t' || (c) == 0x0D || (c) == 0x0A)

#define NPT_XML_CHAR_IS_ANY_CHAR(c) \
(NPT_XML_CHAR_IS_WHITESPACE((c)) || ((c) >= 0x20))

#define NPT_XML_CHAR_IS_DIGIT(c) \
((c) >= '0' && (c) <= '9')

#define NPT_XML_CHAR_IS_LETTER(c) \
(((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z') || ((c) >= 0xC0 && (c) <= 0xD6) || ((c) >= 0xD8 && (c) <= 0xF6) || ((c) >= 0xF8))

#define NPT_XML_CHAR_IS_NAME_CHAR(c) \
(NPT_XML_CHAR_IS_DIGIT((c)) || NPT_XML_CHAR_IS_LETTER((c)) || (c) == '.' || (c) == '-' || (c) == '_' || (c) == ':')

#define NPT_XML_CHAR_IS_ENTITY_CHAR(c) \
(NPT_XML_CHAR_IS_NAME_CHAR((c)) || ((c) == '#'))

#define NPT_XML_CHAR_IS_CONTENT_CHAR(c) \
(NPT_XML_CHAR_IS_ANY_CHAR((c)) && ((c) != '&') && ((c) != '<'))

#define NPT_XML_CHAR_IS_VALUE_CHAR(c) \
(NPT_XML_CHAR_IS_ANY_CHAR((c)) && ((c) != '&') && ((c) != '<'))

/*----------------------------------------------------------------------
|       NPT_XmlProcessor class
+---------------------------------------------------------------------*/
class NPT_XmlProcessor {
public:
               NPT_XmlProcessor(NPT_XmlHandler* handler);
              ~NPT_XmlProcessor();
    NPT_Result ProcessBuffer(NPT_String buffer, NPT_Size size);
    
private:
    // types
    typedef enum {
        CONTEXT_NONE,
        CONTEXT_OPEN_TAG,
        CONTEXT_CLOSE_TAG,
        CONTEXT_ATTRIBUTE,
        CONTEXT_VALUE_SINGLE_QUOTE,
        CONTEXT_VALUE_DOUBLE_QUOTE
    } Context;

    typedef enum {
        STATE_IN_WHITESPACE,
        STATE_IN_NAME,
        STATE_IN_NAME_SPECIAL,
        STATE_IN_VALUE_START,
        STATE_IN_VALUE,
        STATE_IN_TAG_START,
        STATE_IN_EMPTY_TAG_END,
        STATE_IN_TEXT,
        STATE_IN_PROCESSING_INSTRUCTION_START,
        STATE_IN_PROCESSING_INSTRUCTION,
        STATE_IN_PROCESSING_INSTRUCTION_END,
        STATE_IN_COMMENT,
        STATE_IN_COMMENT_END_1,
        STATE_IN_COMMENT_END_2,
        STATE_IN_DTD,
        STATE_IN_DTD_MARKUP_DECL,
        STATE_IN_DTD_MARKUP_DECL_END,
        STATE_IN_CDATA,
        STATE_IN_CDATA_END_1,
        STATE_IN_CDATA_END_2,
        STATE_IN_SPECIAL,
        STATE_IN_ENTITY
    } State;

    // members
    NPT_XmlHandler*    m_Handler;
    State              m_State;
    Context            m_Context;
    NPT_XmlAccumulator m_Name;
    NPT_XmlAccumulator m_Value;
    NPT_XmlAccumulator m_Text;
    NPT_XmlAccumulator m_Entity;

    // methods
#ifdef NPT_XML_PARSER_DEBUG
    const char* StateName(State state) {
        switch (state) {
          case STATE_IN_WHITESPACE: return "IN_WHITESPACE";
          case STATE_IN_NAME: return "IN_NAME";
          case STATE_IN_NAME_SPECIAL: return "IN_NAME_SPECIAL";
          case STATE_IN_VALUE_START: return "IN_VALUE_START";
          case STATE_IN_VALUE: return "IN_VALUE";
          case STATE_IN_TAG_START: return "IN_TAG_START";
          case STATE_IN_EMPTY_TAG_END: return "IN_EMPTY_TAG_END";
          case STATE_IN_TEXT: return "IN_TEXT";
          case STATE_IN_PROCESSING_INSTRUCTION_START: return "IN_PROCESSING_INSTRUCTION_START";
          case STATE_IN_PROCESSING_INSTRUCTION: return "IN_PROCESSING_INSTRUCTION";
          case STATE_IN_PROCESSING_INSTRUCTION_END: return "IN_PROCESSING_INSTRUCTION_END";
          case STATE_IN_COMMENT: return "IN_COMMENT";
          case STATE_IN_COMMENT_END_1: return "IN_COMMENT_END_1";
          case STATE_IN_COMMENT_END_2: return "IN_COMMENT_END_2";
          case STATE_IN_DTD: return "IN_DTD";
          case STATE_IN_DTD_MARKUP_DECL: return "IN_DTD_MARKUP_DECL";
          case STATE_IN_DTD_MARKUP_DECL_END: return "IN_DTD_MARKUP_DECL_END";
          case STATE_IN_CDATA: return "IN_CDATA";
          case STATE_IN_CDATA_END_1: return "IN_CDATA_END_1";
          case STATE_IN_CDATA_END_2: return "IN_CDATA_END_2";
          case STATE_IN_SPECIAL: return "IN_SPECIAL";
          case STATE_IN_ENTITY: return "IN_ENTITY";
        }
        return "UNKNOWN";
    }

    const char* ContextName(Context context) {
        switch (context) {
          case CONTEXT_NONE: return "NONE";
          case CONTEXT_OPEN_TAG: return "OPEN_TAG";
          case CONTEXT_CLOSE_TAG: return "CLOSE_TAG";
          case CONTEXT_ATTRIBUTE: return "ATTRIBUTE";
          case CONTEXT_VALUE_SINGLE_QUOTE: return "VALUE_SINGLE_QUOTE";
          case CONTEXT_VALUE_DOUBLE_QUOTE: return "VALUE_DOUBLE_QUOTE";
        }
        return "UNKNOWN";
    }
#endif /* NPT_XML_PARSER_DEBUG */

    inline void SetState(State state) {
        NPT_XML_Debug_3("\nstate transition: %s to %s [ctx=%s]\n", 
                        StateName(m_State), 
                        StateName(state),
                        ContextName(m_Context));
        m_State = state;
    }

    inline void SetState(State state, Context context) {
        NPT_XML_Debug_4("\nstate transition: %s [ctx=%s] to %s [ctx=%s]\n", 
                        StateName(m_State), 
                        ContextName(m_Context),
                        StateName(state), 
                        ContextName(context));
        m_State = state;
        m_Context = context;
    }

    NPT_Result ResolveEntity(NPT_XmlAccumulator& source,
                             NPT_XmlAccumulator& destination);
};

/*----------------------------------------------------------------------
|       NPT_XmlProcessor::NPT_XmlProcessor
+---------------------------------------------------------------------*/
NPT_XmlProcessor::NPT_XmlProcessor(NPT_XmlHandler* handler) :
    m_Handler(handler),
    m_State(STATE_IN_WHITESPACE),
    m_Context(CONTEXT_NONE)
{
}

/*----------------------------------------------------------------------
|       NPT_XmlProcessor::~NPT_XmlProcessor
+---------------------------------------------------------------------*/
NPT_XmlProcessor::~NPT_XmlProcessor()
{
}

/*----------------------------------------------------------------------
|       NPT_XmlProcessor::ResolveEntity
+---------------------------------------------------------------------*/
NPT_Result 
NPT_XmlProcessor::ResolveEntity(NPT_XmlAccumulator& source,
                                NPT_XmlAccumulator& destination)
{
    const char* entity = (const char*)source.GetString();
    
    if (NPT_StringsEqual(entity, "lt")) {
        destination.Append('<');
    } else if (NPT_StringsEqual(entity, "gt")) {
        destination.Append('>');
    } else if (NPT_StringsEqual(entity, "amp")) {
        destination.Append('&');
    } else if (NPT_StringsEqual(entity, "quot")) {
        destination.Append('"');
    } else if (NPT_StringsEqual(entity, "apos")) {
        destination.Append('\'');
    } else if (entity[0] == '#' && 
               NPT_XML_CHAR_IS_DIGIT(entity[1]) &&
               NPT_XML_CHAR_IS_DIGIT(entity[2]) &&
               entity[3] == '\0') {
        destination.Append((entity[1]-'0') * 16 + entity[2]-'0');
    } else {
        // unknown entity, leave as-is
        destination.Append(source.GetString());
    }
    
    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_XmlProcessor::ProcessBuffer
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlProcessor::ProcessBuffer(NPT_String buffer, NPT_Size size)
{
    NPT_Result    result;
    unsigned char c;

    while (size-- && (c = *buffer++)) {
        NPT_XML_Debug_1("[%c]", (c == '\n' || c == '\r') ? '#' : c);
        switch (m_State) {
          case STATE_IN_WHITESPACE:
            if (NPT_XML_CHAR_IS_WHITESPACE(c)) break;
            switch (m_Context) {
              case CONTEXT_NONE:
                if (c == '<') {
                    SetState(STATE_IN_TAG_START);
                } else if (NPT_XML_CHAR_IS_CONTENT_CHAR(c)) {
                    m_Text.Reset();
                    m_Text.Append(c);
                    SetState(STATE_IN_TEXT);
                } else if (c == '&') {
                    m_Text.Reset();
                    m_Entity.Reset();
                    SetState(STATE_IN_ENTITY);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;
                
              case CONTEXT_ATTRIBUTE:
                if (c == '/') {
                    SetState(STATE_IN_EMPTY_TAG_END);
                } else if (c == '>') {
                    m_Text.Reset();
                    SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
                } else if (NPT_XML_CHAR_IS_NAME_CHAR(c)) {
                    m_Name.Reset();
                    m_Name.Append(c);
                    SetState(STATE_IN_NAME);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;

              case CONTEXT_CLOSE_TAG:
                if (c == '>') {
                    result = m_Handler->OnEndElement(m_Name.GetString());
                    if (NPT_FAILED(result)) return result;
                    m_Text.Reset();
                    SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;

              default:
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_NAME:
            if (NPT_XML_CHAR_IS_NAME_CHAR(c)) {
                m_Name.Append(c);
                break;
            }
            switch (m_Context) {
              case CONTEXT_ATTRIBUTE:
                if (c == '=') {
                    m_Value.Reset();
                    SetState(STATE_IN_VALUE_START);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;

              case CONTEXT_OPEN_TAG:
                if (c == '>' || NPT_XML_CHAR_IS_WHITESPACE(c)) {
                    result = m_Handler->OnStartElement(m_Name.GetString());
                    if (NPT_FAILED(result)) return result;
                    m_Name.Reset();
                    if (c == '>') {
                        SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
                    } else {
                        SetState(STATE_IN_WHITESPACE, CONTEXT_ATTRIBUTE);
                    }
                } else if (c == '/') {
                    SetState(STATE_IN_EMPTY_TAG_END);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;

              case CONTEXT_CLOSE_TAG:
                if (c == '>') {
                    result = m_Handler->OnEndElement(m_Name.GetString());
                    if (NPT_FAILED(result)) return result;
                    SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
                } else if (NPT_XML_CHAR_IS_WHITESPACE(c)) {
                    SetState(STATE_IN_WHITESPACE);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;

              default:
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_NAME_SPECIAL:
            if (NPT_XML_CHAR_IS_NAME_CHAR(c) || (c == '[')) {
                m_Name.Append(c);

                if (m_Name.GetSize() == 2) {
                    if (m_Name.GetBuffer()[0] == '-' &&
                        m_Name.GetBuffer()[1] == '-') {
                        m_Text.Reset();
                        m_Name.Reset();
                        SetState(STATE_IN_COMMENT, CONTEXT_NONE);
                        break;
                    }
                } else if (m_Name.GetSize() == 7) {
                    if (m_Name.GetBuffer()[0] == '[' &&
                        m_Name.GetBuffer()[1] == 'C' &&
                        m_Name.GetBuffer()[2] == 'D' &&
                        m_Name.GetBuffer()[3] == 'A' &&
                        m_Name.GetBuffer()[4] == 'T' &&
                        m_Name.GetBuffer()[5] == 'A' &&
                        m_Name.GetBuffer()[6] == '[') {
                        m_Text.Reset();
                        m_Name.Reset();
                        SetState(STATE_IN_CDATA, CONTEXT_NONE);
                        break;
                    }
                }
                break;
            }
            if (NPT_XML_CHAR_IS_WHITESPACE(c)) {
                const char* special = m_Name.GetString();
                if (special && NPT_StringsEqual(special, "DOCTYPE")) {
                    SetState(STATE_IN_DTD, CONTEXT_NONE);
                } else {
                    SetState(STATE_IN_SPECIAL, CONTEXT_NONE);
                }
                m_Name.Reset();
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_VALUE_START:
            if (c == '"') {
                m_Value.Reset();
                SetState(STATE_IN_VALUE, CONTEXT_VALUE_DOUBLE_QUOTE);
            } else if (c == '\'') {
                m_Value.Reset();
                SetState(STATE_IN_VALUE, CONTEXT_VALUE_SINGLE_QUOTE);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_VALUE:
            if ((c == '"'  && m_Context == CONTEXT_VALUE_DOUBLE_QUOTE) || 
                (c == '\'' && m_Context == CONTEXT_VALUE_SINGLE_QUOTE)) {
                result =  m_Handler->OnElementAttribute(m_Name.GetString(),
                                                        m_Value.GetString());
                if (NPT_FAILED(result)) return result;
                SetState(STATE_IN_WHITESPACE, CONTEXT_ATTRIBUTE);
            } else if (c == '&') {
                m_Entity.Reset();
                SetState(STATE_IN_ENTITY);
            } else if (NPT_XML_CHAR_IS_VALUE_CHAR(c)) {
                m_Value.Append(c);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_TAG_START:
            m_Name.Reset();
            if (c == '!') {
                SetState(STATE_IN_NAME_SPECIAL, CONTEXT_NONE);
            } else if (c == '?') {
                SetState(STATE_IN_PROCESSING_INSTRUCTION, CONTEXT_NONE);
            } else if (c == '/') {
                SetState(STATE_IN_NAME, CONTEXT_CLOSE_TAG);
            } else if (NPT_XML_CHAR_IS_NAME_CHAR(c)) {
                m_Name.Append(c);
                SetState(STATE_IN_NAME, CONTEXT_OPEN_TAG);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_EMPTY_TAG_END:
            if (c == '>') {
                result = m_Handler->OnEndElement(NULL);
                if (NPT_FAILED(result)) return result;
                m_Text.Reset();
                SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_ENTITY:
            switch (m_Context) {
              case CONTEXT_VALUE_SINGLE_QUOTE:
              case CONTEXT_VALUE_DOUBLE_QUOTE:
                if (c == ';') {
                    NPT_Result result = ResolveEntity(m_Entity, m_Value);
                    if (NPT_FAILED(result)) return result;
                    SetState(STATE_IN_VALUE);
                } else if (NPT_XML_CHAR_IS_ENTITY_CHAR(c)) {
                    m_Entity.Append(c);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;

              case CONTEXT_NONE:
                if (c == ';') {
                    NPT_Result result = ResolveEntity(m_Entity, m_Text);
                    if (NPT_FAILED(result)) return result;
                    SetState(STATE_IN_TEXT);
                } else if (NPT_XML_CHAR_IS_ENTITY_CHAR(c)) {
                    m_Entity.Append(c);
                } else {
                    return NPT_ERROR_INVALID_SYNTAX;
                }
                break;
                
              default:
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_COMMENT:
            if (c == '-') {
                SetState(STATE_IN_COMMENT_END_1);
            } else if (!NPT_XML_CHAR_IS_ANY_CHAR(c)) {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_COMMENT_END_1:
            if (c == '-') {
                SetState(STATE_IN_COMMENT_END_2);
            } else if (NPT_XML_CHAR_IS_ANY_CHAR(c)) {
                SetState(STATE_IN_COMMENT);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_COMMENT_END_2:
            if (c == '>') {
                SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_TEXT:
            if (c == '<') {
                result = m_Handler->OnCharacterData(m_Text.GetString(),
                                                    m_Text.GetSize());
                if (NPT_FAILED(result)) return result;
                m_Text.Reset();
                SetState(STATE_IN_TAG_START, CONTEXT_NONE);
            } else if (c == '&') {
                m_Entity.Reset();
                SetState(STATE_IN_ENTITY);
            } else {
                m_Text.Append(c);
            }
            break;

          case STATE_IN_PROCESSING_INSTRUCTION_START:
            break;

          case STATE_IN_PROCESSING_INSTRUCTION_END:
            if (c == '>') {
                SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
            } else {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_PROCESSING_INSTRUCTION:
            if (c == '?') {
                SetState(STATE_IN_PROCESSING_INSTRUCTION_END);
            }
            break;

          case STATE_IN_DTD:
            if (NPT_XML_CHAR_IS_WHITESPACE(c)) break;
            if (c == '[') {
                SetState(STATE_IN_DTD_MARKUP_DECL);
            } else if (c == '>') {
                SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
            }
            break;

          case STATE_IN_DTD_MARKUP_DECL:
            if (c == ']') {
                SetState(STATE_IN_DTD_MARKUP_DECL_END);
            }
            break;

          case STATE_IN_DTD_MARKUP_DECL_END:
            if (c == '>') {
                SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
            } else if (!NPT_XML_CHAR_IS_WHITESPACE(c)) {
                return NPT_ERROR_INVALID_SYNTAX;
            }
            break;

          case STATE_IN_CDATA:
            if (c == ']') {
                SetState(STATE_IN_CDATA_END_1);
            } else  {
                m_Text.Append(c);
            }
            break;

          case STATE_IN_CDATA_END_1:
            if (c == ']') {
                SetState(STATE_IN_CDATA_END_2);
            } else {
                m_Text.Append(']');
                m_Text.Append(c);
                SetState(STATE_IN_CDATA);
            }
            break;

          case STATE_IN_CDATA_END_2:
            if (c == '>') {
                result = m_Handler->OnCharacterData(m_Text.GetString(),
                                                    m_Text.GetSize());
                if (NPT_FAILED(result)) return result;
                m_Text.Reset();
                SetState(STATE_IN_TEXT, CONTEXT_NONE);
            } else {
                m_Text.Append("]]");
                m_Text.Append(c);
                SetState(STATE_IN_CDATA);
            }
            break;

          case STATE_IN_SPECIAL:
            if (c == '>') {
                SetState(STATE_IN_WHITESPACE, CONTEXT_NONE);
            }
            break;
        }
    }

    return NPT_SUCCESS;
}       

/*----------------------------------------------------------------------
|       NPT_XmlParser::NPT_XmlParser
+---------------------------------------------------------------------*/
NPT_XmlParser::NPT_XmlParser() :
    m_Tree(NULL),
    m_CurrentElement(NULL)
{
    m_Processor = new NPT_XmlProcessor(this);
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::~NPT_XmlParser
+---------------------------------------------------------------------*/
NPT_XmlParser::~NPT_XmlParser()
{
    delete m_Processor;
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::ParseString
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlParser::ParseString(const char* xml, NPT_XmlNode*& node)
{       
    NPT_Size size = NPT_StringLength(xml);

    return ParseBuffer(xml, size, node);
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::ParseBuffer
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlParser::ParseBuffer(const char* xml, NPT_Size size, NPT_XmlNode*& node)
{ 
    NPT_Result result;

    // if there is a current tree, reset it
    m_Tree = NULL;

    // parse the buffer
    result = m_Processor->ProcessBuffer(xml, size);

    // return a tree if we have one 
    node = m_Tree;

    return result;
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::OnStartElement
+---------------------------------------------------------------------*/
NPT_Result 
NPT_XmlParser::OnStartElement(const char* name)
{
    NPT_XML_Debug_1("\nNPT_XmlParser::OnStartElement: %s\n", name);

    // create new node
    NPT_XmlElementNode* node = new NPT_XmlElementNode(name);

    // add node to tree
    if (m_CurrentElement) {
        m_CurrentElement->AddChild(node);
    }
    m_CurrentElement = node;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::OnElementAttribute
+---------------------------------------------------------------------*/
NPT_Result 
NPT_XmlParser::OnElementAttribute(const char* name, const char* value)
{
    NPT_XML_Debug_2("\nNPT_XmlParser::OnElementAttribute: name=%s, value='%s'\n", 
                    name, value);

    if (m_CurrentElement == NULL) {
        return NPT_ERROR_INVALID_SYNTAX;
    }
                                 
    m_CurrentElement->AddAttribute(name, value);

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::OnEndElement
+---------------------------------------------------------------------*/
NPT_Result 
NPT_XmlParser::OnEndElement(const char* name)
{
    NPT_XML_Debug_1("\nNPT_XmlParser::OnEndElement: %s\n", name ? name : "NULL");

    // check that the name matches (if there is a name)
    if (m_CurrentElement == NULL ||
        (name != NULL && !NPT_StringsEqual(name, m_CurrentElement->GetTag()))) {
        return NPT_ERROR_XML_TAG_MISMATCH;
    }
    NPT_XmlNode* parent = m_CurrentElement->GetParent();
    if (parent == NULL) {
        m_Tree = m_CurrentElement;
    }
    m_CurrentElement = parent ? parent->AsElementNode() : NULL;

    return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|       NPT_XmlParser::OnCharacterData
+---------------------------------------------------------------------*/
NPT_Result
NPT_XmlParser::OnCharacterData(const char* data, unsigned long size)
{ 
    NPT_COMPILER_UNUSED(size);
    NPT_XML_Debug_1("\nNPT_XmlParser::OnCharacterData: %s\n", data);
    
    // check that we have a current element
    if (m_CurrentElement == NULL) {
        return NPT_ERROR_XML_INVALID_NESTING;
    }

    // add the text to the current element
    m_CurrentElement->AddText(data);

    return NPT_SUCCESS;
}






