#ifndef __OSS_TTXML_H__
#define __OSS_TTXML_H__

typedef struct XmlNode {
	char * name;
	char ** attrib;
	int nattrib;
	struct XmlNode * child;
	struct XmlNode * next;
} XmlNode;

// XmlNode* xml_load(const char * filename);
void xml_free(XmlNode *target);
// char* xml_attr(XmlNode *x, const char *name);
XmlNode * xml_find(XmlNode *xml, const char *name);
XmlNode *xml_load_buffer(const char *buffer, unsigned int buffer_len);

#endif