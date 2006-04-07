/*****************************************************************
|
|      Neptune - Xml Support
|
|      (c) 2001-2005 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_XML_H_
#define _NPT_XML_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"
#include "NptList.h"
#include "NptStrings.h"
#include "NptStreams.h"

/*----------------------------------------------------------------------
|       constants
+---------------------------------------------------------------------*/
const int NPT_ERROR_XML_INVALID_NESTING = NPT_ERROR_BASE_XML - 0;
const int NPT_ERROR_XML_TAG_MISMATCH    = NPT_ERROR_BASE_XML - 1;

/*----------------------------------------------------------------------
|       forward declarations
+---------------------------------------------------------------------*/
class NPT_XmlProcessor;

/*----------------------------------------------------------------------
|       NPT_XmlAttribute
+---------------------------------------------------------------------*/
class NPT_XmlAttribute
{
 public:
    // methods
    NPT_XmlAttribute(const char* name, const char* value);
    NPT_XmlAttribute(const char* prefix, const char* name, const char* value) :
        m_Prefix(prefix), m_Name(name), m_Value(value) {}
    const NPT_String& GetPrefix() const { return m_Prefix; }
    const NPT_String& GetName()   const { return m_Name;   }
    const NPT_String& GetValue()  const { return m_Value;  }
    void              SetValue(const char* value) { m_Value = value; }

 private:
    // members
    NPT_String m_Prefix;
    NPT_String m_Name;
    NPT_String m_Value;

    NPT_XmlAttribute(const NPT_XmlAttribute& attribute) :
        m_Prefix(attribute.m_Prefix), 
        m_Name(attribute.m_Name), 
        m_Value(attribute.m_Value) {}
    NPT_XmlAttribute& operator=(const NPT_XmlAttribute& a);

    // friends
    friend class NPT_XmlAttributeFinder;
    friend class NPT_XmlAttributeFinderWithPrefix;
};

/*----------------------------------------------------------------------
|       NPT_XmlNamespaceMap
+---------------------------------------------------------------------*/
class NPT_XmlNamespaceMap
{
public:
    // destructor
    ~NPT_XmlNamespaceMap();

    // methods   
    NPT_Result        SetNamespaceUri(const char* prefix, const char* uri);
    const NPT_String* GetNamespaceUri(const char* prefix);
    const NPT_String* GetNamespacePrefix(const char* uri);

private:
    // types
    class Entry {
    public:
        // constructor
        Entry(const char* prefix, const char* uri) : 
            m_Prefix(prefix), m_Uri(uri) {}

        // members
        NPT_String m_Prefix;
        NPT_String m_Uri;
    };

    // members
    NPT_List<Entry*> m_Entries;

    // friends
    friend class NPT_XmlWriter;
    friend class NPT_XmlNodeWriter;
    friend class NPT_XmlNodeCanonicalWriter;
};

/*----------------------------------------------------------------------
|       NPT_XmlNode
+---------------------------------------------------------------------*/
class NPT_XmlElementNode;
class NPT_XmlTextNode;
class NPT_XmlNode
{
 public:
    // types
    typedef enum {
        DOCUMENT,
        ELEMENT,
        TEXT
    } Type;

    // methods
    NPT_XmlNode(Type type) : m_Type(type), m_Parent(NULL) {}
    virtual     ~NPT_XmlNode() {}
    Type         GetType() const { return m_Type; }
    NPT_XmlNode* GetParent() const { return m_Parent; }

    // type casting
    virtual NPT_XmlElementNode*       AsElementNode()       { return NULL; }
    virtual const NPT_XmlElementNode* AsElementNode() const { return NULL; }
    virtual NPT_XmlTextNode*          AsTextNode()          { return NULL; }
    virtual const NPT_XmlTextNode*    AsTextNode() const    { return NULL; }

 protected:
    // methods
    virtual void SetParent(NPT_XmlNode* parent) { m_Parent = parent; }

    // members  
    Type         m_Type;
    NPT_XmlNode* m_Parent;

    // friends
    friend class NPT_XmlNodeFinder;
    friend class NPT_XmlSerializer;
    friend class NPT_XmlWriter;
    friend class NPT_XmlElementNode; // to allow access to SetParent()
};

/*----------------------------------------------------------------------
|       NPT_XmlElementNode
+---------------------------------------------------------------------*/
class NPT_XmlElementNode : public NPT_XmlNode
{
 public:
    // methods
                            NPT_XmlElementNode(const char* tag);
                            NPT_XmlElementNode(const char* prefix, const char* tag);
    virtual                ~NPT_XmlElementNode();
    NPT_List<NPT_XmlNode*>& GetChildren() { return m_Children; }
    NPT_XmlElementNode* GetChild(const char* tag, 
                                 const char* namespc = NULL,
                                 NPT_Ordinal n=0) const;
    NPT_Result              AddChild(NPT_XmlNode* child);
    NPT_Result              SetAttribute(const char* prefix,
                                         const char* name, 
                                         const char* value);
    NPT_Result              SetAttribute(const char* name, 
                                         const char* value);
    NPT_Result              AddText(const char* text); 
    NPT_List<NPT_XmlAttribute*>& 
                            GetAttributes() { return m_Attributes; }
    const NPT_String*       GetAttribute(const char* name,
                                         const char* namespc = NULL) const;
    const NPT_String&       GetPrefix() const { return m_Prefix; }
    const NPT_String&       GetTag()    const { return m_Tag;    }
    const NPT_String*       GetText(NPT_Ordinal n=0) const;
    
    // namespace methods   
    const NPT_String* GetNamespace() const;
    NPT_Result        SetNamespaceUri(const char* prefix, const char* uri);
    const NPT_String* GetNamespaceUri(const char* prefix) const;
    const NPT_String* GetNamespacePrefix(const char* uri) const;

    // type casting
    NPT_XmlElementNode*       AsElementNode()       { return this; }
    const NPT_XmlElementNode* AsElementNode() const { return this; }

protected:
    // methods
    void SetParent(NPT_XmlNode* parent);
    void SetNamespaceParent(NPT_XmlElementNode* parent);
    void RelinkNamespaceMaps();

    NPT_Result AddAttribute(const char* name, const char* value);

    // members  
    NPT_String                  m_Prefix;
    NPT_String                  m_Tag;
    NPT_List<NPT_XmlNode*>      m_Children;
    NPT_List<NPT_XmlAttribute*> m_Attributes;
    NPT_XmlNamespaceMap*        m_NamespaceMap;
    NPT_XmlElementNode*         m_NamespaceParent;

    // friends
    friend class NPT_XmlTagFinder;
    friend class NPT_XmlSerializer;
    friend class NPT_XmlWriter;
    friend class NPT_XmlNodeWriter;
    friend class NPT_XmlNodeCanonicalWriter;
    friend class NPT_XmlParser;
    friend class NPT_XmlProcessor;
};

/*----------------------------------------------------------------------
|       NPT_XmlTextNode
+---------------------------------------------------------------------*/
class NPT_XmlTextNode : public NPT_XmlNode
{
 public:
    // types
    typedef enum {
        CHARACTER_DATA,
        IGNORABLE_WHITESPACE,
        CDATA_SECTION,
        ENTITY_REFERENCE,
        COMMENT
    } TokenType;

    // constructor
    NPT_XmlTextNode(TokenType token_type, const char* text);

    // methods
    const NPT_String& GetString() const { return m_Text; }

    // type casting
    NPT_XmlTextNode*       AsTextNode()       { return this; }
    const NPT_XmlTextNode* AsTextNode() const { return this; }

 private:
    // members  
    TokenType  m_TokenType;
    NPT_String m_Text;
};

/*----------------------------------------------------------------------
|       NPT_XmlParser
+---------------------------------------------------------------------*/
class NPT_XmlParser
{
 public:
    // methods
             NPT_XmlParser(bool keep_whitespace = false);
    virtual ~NPT_XmlParser();
    virtual  NPT_Result Parse(const char*   xml, 
                              NPT_XmlNode*& tree,
                              bool          incremental=false);
    virtual  NPT_Result Parse(const char*   xml, 
                              NPT_Size      size,
                              NPT_XmlNode*& tree,
                              bool          incremental=false);
    virtual  NPT_Result Parse(NPT_InputStream& stream, 
                              NPT_XmlNode*&    tree,
                              bool             incremental=false);
    virtual  NPT_Result Parse(NPT_InputStream& stream, 
                              NPT_Size&        size,
                              NPT_XmlNode*&    tree,
                              bool             incremental=false);
    
 protected:
    // NPT_XmlHandler methods
    NPT_Result OnStartElement(const char* name);
    NPT_Result OnElementAttribute(const char* name, const char* value);
    NPT_Result OnEndElement(const char* name);
    NPT_Result OnCharacterData(const char* data, unsigned long size);
    void       RemoveIgnorableWhitespace();

    // members
    NPT_XmlProcessor*   m_Processor;
    NPT_XmlElementNode* m_Tree;
    NPT_XmlElementNode* m_CurrentElement;
    bool                m_KeepWhitespace;

    // friends
    friend class NPT_XmlProcessor;
};

/*----------------------------------------------------------------------
|       NPT_XmlSerializer
+---------------------------------------------------------------------*/
class NPT_XmlSerializer
{
public:
    // methods
                       NPT_XmlSerializer(NPT_OutputStream* output,
                                         NPT_Cardinal      indentation = 0,
                                         bool              shrink_empty_elements = true);
    virtual           ~NPT_XmlSerializer();
    virtual NPT_Result StartDocument();
    virtual NPT_Result EndDocument();
    virtual NPT_Result StartElement(const char* prefix, const char* name);
    virtual NPT_Result EndElement(const char* prefix, const char* name);
    virtual NPT_Result Attribute(const char* prefix, const char* name, const char* value);
    virtual NPT_Result Text(const char* text);
    virtual NPT_Result CdataSection(const char* data);
    virtual NPT_Result Comment(const char* comment);

protected:
    // methods
    void       EscapeChar(unsigned char c, char* text);
    NPT_Result ProcessPending();
    NPT_Result OutputEscapedString(const char* text, bool attribute);
    void       OutputIndentation(bool start);

    // members
    NPT_OutputStream* m_Output;
    bool              m_ElementPending;
    NPT_Cardinal      m_Depth;
    NPT_Cardinal      m_Indentation;
    NPT_String        m_IndentationPrefix;
    bool              m_ElementHasText;
    bool              m_ShrinkEmptyElements;
};

/*----------------------------------------------------------------------
|       NPT_XmlWriter
+---------------------------------------------------------------------*/
class NPT_XmlWriter
{
public:
    // constructor
    explicit NPT_XmlWriter(NPT_Cardinal indentation = 0) : m_Indentation(indentation) {}

    // methods
    NPT_Result Serialize(NPT_XmlNode& node, NPT_OutputStream& stream);

private:
    // members
    NPT_Cardinal m_Indentation;
};

/*----------------------------------------------------------------------
|       NPT_XmlCanonicalizer
+---------------------------------------------------------------------*/
class NPT_XmlCanonicalizer
{
public:
    // methods
    NPT_Result Serialize(NPT_XmlNode& node, NPT_OutputStream& stream);
};

#endif // _NPT_XML_H_
