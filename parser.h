#ifndef DEPARTMENT_STORE_H
#define DEPARTMENT_STORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
	#define TRUE 1
#endif // TRUE
#ifndef FALSE
	#define FALSE 0
#endif // FALSE

enum _TagType
{
	TAG_START,
	TAG_INLINE
};
typedef enum _TagType TagType;

struct _XMLAttribute
{
	char* key;
	char* value;
};
typedef struct _XMLAttribute XMLAttribute;

struct _XMLAttributeList
{
	int heap_size;
	int size;
	XMLAttribute* data;
};
typedef struct _XMLAttributeList XMLAttributeList;

struct _XMLNodeList
{
	int heap_size;
	int size;
	struct _XMLNode** data;
};
typedef struct _XMLNodeList XMLNodeList;

struct _XMLNode
{
	char* tag;
	char* inner_text;
	struct _XMLNode* parent;
	XMLAttributeList attributes;
	XMLNodeList children;
};
typedef struct _XMLNode XMLNode;

struct _XMLDocument
{
	XMLNode* root;
	char* version;
	char* encoding;
};
typedef struct _XMLDocument XMLDocument;

int XMLDocument_Load(XMLDocument* doc, const char* path);
int XMLDocument_Write(XMLDocument* doc, const char* path, int indent);
void XMLDocument_Free(XMLDocument* doc);

XMLNode* XMLNode_New(XMLNode* parent);
void XMLNode_Free(XMLNode* node);
XMLNode* XMLNode_Child(XMLNode* parent, int index);
XMLNodeList* XMLNode_Children(XMLNode* parent, const char* tag);
char* XMLNode_Attr_Val(XMLNode* node, char* key);
XMLAttribute* XMLNode_Attr(XMLNode* node, char* key);

void XMLAttribute_Free(XMLAttribute* attr);

void XMLAttributeList_Init(XMLAttributeList* list);
void XMLAttributeList_Add(XMLAttributeList* list, XMLAttribute* attr);

void XMLNodeList_Init(XMLNodeList* list);
void XMLNodeList_Add(XMLNodeList* list, XMLNode* node);
XMLNode* XMLNodeList_At(XMLNodeList* list, int idnex);
void XMLNodeList_Free(XMLNodeList* list);

int Ends_With(const char* haystack, const char* needle);
static TagType Parse_Attrs(char* source, int* i, char* lexBuffer, int* lexIndex, XMLNode* currNode);
static void Node_Out(FILE* file, XMLNode* node, int indent, int times);

#endif // !DEPARTMENT_STORE_H
