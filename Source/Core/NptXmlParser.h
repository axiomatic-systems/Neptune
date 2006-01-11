/*****************************************************************
|
|      Neptune - Xml Parser
|
|      (c) 2001-2003 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

#ifndef _NPT_XML_PARSER_H_
#define _NPT_XML_PARSER_H_

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "NptTypes.h"
#include "NptList.h"
#include "NptStrings.h"

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
    NPT_XmlAttribute(const char* name, const char* value) :
        m_Name(name), m_Value(value) {}
    NPT_XmlAttribute(const NPT_XmlAttribute& attribute) :
        m_Name(attribute.m_Name), m_Value(attribute.m_Value) {}
    const NPT_StringObject& GetName() const { return m_Name; }
    const NPT_StringObject& GetValue() const { return m_Value; }

 private:
    // members
    NPT_StringObject m_Name;
    NPT_StringObject m_Value;

    // friends
    friend class NPT_XmlAttributeFinder;
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
                 NPT_XmlNode(Type type);
    virtual     ~NPT_XmlNode();
    NPT_Result   SetParent(NPT_XmlNode* parent);
    NPT_XmlNode* GetParent() const;

    // type casting
    virtual NPT_XmlElementNode* AsElementNode() { return NULL; }
    virtual const NPT_XmlElementNode* AsElementNode() const { return NULL; }
    virtual NPT_XmlTextNode* AsTextNode() { return NULL; }
    virtual const NPT_XmlTextNode* AsTextNode() const { return NULL; }

 protected:
    // members  
    Type         m_Type;
    NPT_XmlNode* m_Parent;

    // friends
    friend class NPT_XmlNodeFinder;
};

/*----------------------------------------------------------------------
|       NPT_XmlElementNode
+---------------------------------------------------------------------*/
class NPT_XmlElementNode : public NPT_XmlNode
{
 public:
    // methods
                            NPT_XmlElementNode(const char* tag);
    virtual                ~NPT_XmlElementNode();
    NPT_List<NPT_XmlNode*>& GetChildren() { return m_Children; }
    NPT_XmlElementNode*     GetChild(const char* tag, NPT_Ordinal n=0);
    NPT_Result              AddChild(NPT_XmlNode* child);
    NPT_Result              AddAttribute(const char* name, const char* value);
    NPT_Result              AddText(const char* text); 
    NPT_List<NPT_XmlAttribute>& 
                            GetAttributes() { return m_Attributes; }
    const NPT_StringObject* GetAttribute(const char* name) const;
    const NPT_StringObject& GetTag() const { return m_Tag; }
    const NPT_StringObject* GetText(NPT_Ordinal n=0) const;

    // type casting
    NPT_XmlElementNode* AsElementNode() { return this; }
    const NPT_XmlElementNode* AsElementNode() const { return this; }

protected:
    // members  
    NPT_StringObject           m_Tag;
    NPT_List<NPT_XmlNode*>     m_Children;
    NPT_List<NPT_XmlAttribute> m_Attributes;

    // friends
    friend class NPT_XmlTagFinder;
};

/*----------------------------------------------------------------------
|       NPT_XmlTextNode
+---------------------------------------------------------------------*/
class NPT_XmlTextNode : public NPT_XmlNode
{
 public:
    // methods
                            NPT_XmlTextNode(const char* text);
    const NPT_StringObject& GetString() const { return m_Text; }

    // type casting
    NPT_XmlTextNode* AsTextNode() { return this; }
    const NPT_XmlTextNode* AsTextNode() const { return this; }

 private:
    // members  
    NPT_StringObject m_Text;
};

/*----------------------------------------------------------------------
|       NPT_XmlHandler class
+---------------------------------------------------------------------*/
class NPT_XmlHandler {
 public:
    virtual NPT_Result OnStartElement(const char* name) = 0;
    virtual NPT_Result OnElementAttribute(const char* name, 
                                          const char* value) = 0;
    virtual NPT_Result OnEndElement(const char* name) = 0;
    virtual NPT_Result OnCharacterData(const char* data, unsigned long size) = 0;
};

/*----------------------------------------------------------------------
|       NPT_XmlParser
+---------------------------------------------------------------------*/
class NPT_XmlParser : NPT_XmlHandler
{
 public:
    // methods
             NPT_XmlParser();
    virtual ~NPT_XmlParser();
    virtual  NPT_Result ParseString(const char* xml, NPT_XmlNode*& tree);
    virtual  NPT_Result ParseBuffer(const char* xml, NPT_Size size,
                                    NPT_XmlNode*& tree);

 protected:
    // NPT_XmlHandler methods
    NPT_Result OnStartElement(const char* name);
    NPT_Result OnElementAttribute(const char* name, const char* value);
    NPT_Result OnEndElement(const char* name);
    NPT_Result OnCharacterData(const char* data, unsigned long size);

    // members
    NPT_XmlProcessor*   m_Processor;
    NPT_XmlElementNode* m_Tree;
    NPT_XmlElementNode* m_CurrentElement;
};

#endif // _NPT_XML_PARSER_H_









