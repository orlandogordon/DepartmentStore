#include "parser.h"

#include "parser.h"

int XMLDocument_Load(XMLDocument* doc, const char* path)
{
	FILE* file = fopen(path, "r");
	if (!file) {
		fprintf(stderr, "Could not load file from '%s'\n", path);
		return FALSE;
	}

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* source = (char*)malloc(sizeof(char) * size + 1);
	fread(source, 1, size, file);
	fclose(file);
	source[size] = '\0';

	doc->root = XMLNode_New(NULL);

	char lexBuffer[256];
	int lexIndex = 0;
	int i = 0;

	XMLNode* currNode = doc->root;

	while (source[i] != '\0')
	{
		if (source[i] == '<') {
			lexBuffer[lexIndex] = '\0';

			// Inner text
			if (lexIndex > 0) {
				if (!currNode) {
					fprintf(stderr, "Text outside of document\n");
					return FALSE;
				}
				currNode->inner_text = strdup(lexBuffer);
				lexIndex = 0;
			}

			// End of node
			if (source[i + 1] == '/') {
				i += 2;
				while (source[i] != '>')
					lexBuffer[lexIndex++] = source[i++];
				lexBuffer[lexIndex] = '\0';

				if (!currNode) {
					fprintf(stderr, "Already at the root\n");
					return FALSE;
				}

				if (strcmp(currNode->tag, lexBuffer)) {
					fprintf(stderr, "Mistmatched tags (%s != %s)\n", currNode->tag, lexBuffer);
					return FALSE;
				}

				currNode = currNode->parent;
				i++;
				continue;
			}

			// Special nodes
			if (source[i + 1] == '!') {
				while (source[i] != ' ' && source[i] != '>')
					lexBuffer[lexIndex++] = source[i++];
				lexBuffer[lexIndex] = '\0';

				// Comments
				if (!strcmp(lexBuffer, "<!--")) {
					lexBuffer[lexIndex] = '\0';
					while (!Ends_With(lexBuffer, "-->")) {
						lexBuffer[lexIndex++] = source[i++];
						lexBuffer[lexIndex] = '\0';
					}
					continue;
				}
			}

			// Declaration tags
			if (source[i + 1] == '?') {
				while (source[i] != ' ' && source[i] != '>')
					lexBuffer[lexIndex++] = source[i++];
				lexBuffer[lexIndex] = '\0';

				if (!strcmp(lexBuffer, "<?xml")) {
					lexIndex = 0;
					XMLNode* desc = XMLNode_New(NULL);
					Parse_Attrs(source, &i, lexBuffer, &lexIndex, desc);

					doc->version = XMLNode_Attr_Val(desc, "version");
					doc->encoding = XMLNode_Attr_Val(desc, "encoding");

					continue;

				}
			}

			// Set current node
			currNode = XMLNode_New(currNode);

			// Start tag
			i++;
			if (Parse_Attrs(source, &i, lexBuffer, &lexIndex, currNode) == TAG_INLINE) {
				currNode = currNode->parent;
				i++;
				continue;
			}

			// Set tag name if none
			lexBuffer[lexIndex] = '\0';
			if (!currNode->tag)
				currNode->tag = strdup(lexBuffer);

			// Reset lexer
			lexIndex = 0;
			i++;
			continue;
		}
		else {
			lexBuffer[lexIndex++] = source[i++];
		}
	}

	return TRUE;
}

XMLNode* XMLNode_New(XMLNode* parent)
{
	XMLNode* node = (XMLNode*)malloc(sizeof(XMLNode));
	node->parent = parent;
	node->tag = NULL;
	node->inner_text = NULL;
	XMLAttributeList_Init(&node->attributes);
	XMLNodeList_Init(&node->children);
	if (parent)
		XMLNodeList_Add(&parent->children, node);
	return node;
}

void XMLNode_Free(XMLNode* node)
{
	if (node->tag)
		free(node->tag);
	if (node->inner_text)
		free(node->inner_text); 
	for (int i = 0; i < node->attributes.size; i++)
		XMLAttribute_Free(&node->attributes.data[i]);
	free(node);
}

XMLNode* XMLNode_Child(XMLNode* parent, int index)
{
	return parent->children.data[index];
}

void XMLAttribute_Free(XMLAttribute* attr)
{
	free(attr->key);
	free(attr->value);
}

void XMLAttributeList_Init(XMLAttributeList* list)
{
	list->heap_size = 1;
	list->size = 0;
	list->data = (XMLAttribute*) malloc(sizeof(XMLAttribute) * list->heap_size);
}

void XMLAttributeList_Add(XMLAttributeList* list, XMLAttribute* attr)
{
	while (list->size >= list->heap_size)
	{
		list->heap_size *= 2;
		list->data = (XMLAttribute*) realloc(list->data, sizeof(XMLAttribute) * list->heap_size);
	}

	list->data[list->size++] = *attr;
}

void XMLNodeList_Init(XMLNodeList* list)
{
	list->heap_size = 1;
	list->size = 0;
	list->data = (XMLNode**)malloc(sizeof(XMLNode*) * list->heap_size);
}

void XMLNodeList_Add(XMLNodeList* list, XMLNode* node)
{
	while (list->size >= list->heap_size)
	{
		list->heap_size *= 2;
		list->data = (XMLNode**)realloc(list->data, sizeof(XMLNode*) * list->heap_size);
	}

	list->data[list->size++] = node;
}

int Ends_With(const char* haystack, const char* needle)
{
	int h_len = strlen(haystack);
	int n_len = strlen(needle);

	if (h_len < n_len)
		return FALSE;

	for (int i = 0; i < n_len; i++)
	{
		if (haystack[h_len - n_len + i] != needle[i])
			return FALSE;
	}

	return TRUE;
}

static TagType Parse_Attrs(char* source, int* i, char* lexBuffer, int* lexIndex, XMLNode* currNode)
{
	XMLAttribute currAttr = { 0, 0 };
	while (source[*i] != '>')
	{
		lexBuffer[(*lexIndex)++] = source[(*i)++];

		// Tag name
		if (source[*i] == ' ' && !currNode->tag) {
			lexBuffer[*lexIndex] = '\0';
			currNode->tag = strdup(lexBuffer);
			*lexIndex = 0;
			(*i)++;
			continue;
		}

		// Usually ignore spaces
		if (lexBuffer[*lexIndex - 1] == ' ') {
			(*lexIndex)--;
		}

		// Attribute key
		if (source[*i] == '=') {
			lexBuffer[*lexIndex] = '\0';
			currAttr.key = strdup(lexBuffer);
			*lexIndex = 0;
			continue;
		}

		// Attribute Value
		if (source[*i] == '"') {
			if (!currAttr.key) {
				fprintf(stderr, "Value has no key\n");
				return TAG_START;
			}

			*lexIndex = 0;
			(*i)++;

			while (source[*i] != '"')
				lexBuffer[(*lexIndex)++] = source[(*i)++];
			lexBuffer[*lexIndex] = '\0';
			currAttr.value = strdup(lexBuffer);
			XMLAttributeList_Add(&currNode->attributes, &currAttr);
			currAttr.key = NULL;
			currAttr.value = NULL;
			*lexIndex = 0;
			(*i)++;
			continue;
		}

		// Inline node
		if (source[*i - 1] == '/' && source[*i] == '>') {
			lexBuffer[*lexIndex] = '\0';
			if (!currNode->tag)
				currNode->tag = strdup(lexBuffer);
			(*i)++;
			return TAG_INLINE;
		}

	}

	return TAG_START;
}

char* XMLNode_Attr_Val(XMLNode* node, char* key)
{
	for (int i = 0; i < node->attributes.size; i++)
	{
		XMLAttribute attr = node->attributes.data[i];
		if (!strcmp(attr.key, key))
			return attr.value;
	}
	return NULL;
}

XMLNode* XMLNodeList_At(XMLNodeList* list, int index)
{
	return list->data[index];
}

void XMLNodeList_Free(XMLNodeList* list)
{
	free(list);
}

XMLNodeList* XMLNode_Children(XMLNode* parent, const char* tag)
{
	XMLNodeList* list = (XMLNodeList*)malloc(sizeof(XMLNodeList));
	XMLNodeList_Init(list);

	for (int i = 0; i < parent->children.size; i++) {
		XMLNode* child = parent->children.data[i];
		if (!strcmp(child->tag, tag))
			XMLNodeList_Add(list, child);
	}

	return list;
}

XMLAttribute* XMLNode_Attr(XMLNode* node, char* key)
{
	for (int i = 0; i < node->attributes.size; i++)
	{
		XMLAttribute* attr = &node->attributes.data[i];
		if (!strcmp(attr->key, key))
			return attr;
	}
	return NULL;
}

static void Node_Out(FILE* file, XMLNode* node, int indent, int times)
{
	for (int i = 0; i < node->children.size; i++) {
		XMLNode* child = node->children.data[i];

		if (times > 0)
			fprintf(file, "%*s", indent * times, " ");

		fprintf(file, "<%s", child->tag);
		for (int i = 0; i < child->attributes.size; i++) {
			XMLAttribute attr = child->attributes.data[i];
			if (!attr.value || !strcmp(attr.value, ""))
				continue;
			fprintf(file, " %s=\"%s\"", attr.key, attr.value);
		}

		if (child->children.size == 0 && !child->inner_text)
			fprintf(file, " />\n");
		else {
			fprintf(file, ">");
			if (child->children.size == 0)
				fprintf(file, "%s</%s>\n", child->inner_text, child->tag);
			else {
				fprintf(file, "\n");
				Node_Out(file, child, indent, times + 1);
				if (times > 0)
					fprintf(file, "%*s", indent * times, " ");
				fprintf(file, "</%s>\n", child->tag);
			}
		}
	}
}

int XMLDocument_Write(XMLDocument* doc, const char* path, int indent)
{
	FILE* file = fopen(path, "w");
	if (!file) {
		fprintf(stderr, "Could not open file '%s'\n", path);
		return FALSE;
	}

	fprintf(
		file, "<?xml version=\"%s\" encoding=\"%s\" ?>\n",
		(doc->version) ? doc->version : "1.0",
		(doc->encoding) ? doc->encoding : "UTF-8"
	);
	Node_Out(file, doc->root, indent, 0);
	fclose(file);
	return TRUE;
}

void XMLDocument_Free(XMLDocument* doc)
{
	XMLNode_Free(doc->root);
}



