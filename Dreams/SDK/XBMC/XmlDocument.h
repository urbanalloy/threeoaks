// XmlDocument.h: interface for the CXmlDocument class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define XML_ROOT_NODE 0
#define XML_MAX_TAGNAME_SIZE	32
#define XML_MAX_INNERTEXT_SIZE	1024

typedef int    XmlNode;
typedef void (*XmlNodeCallback) (char* szTag, XmlNode node);      



class CXmlDocument  
{
public:
	CXmlDocument();
	virtual ~CXmlDocument();

	void	Create(char* szString);
	int     Load(char* szFile);
	void    Close();

	int		GetNodeCount(char* tag);

	void    EnumerateNodes(char* szTag, XmlNodeCallback pFunc);

	XmlNode GetChildNode(XmlNode node, char* szTag);
	XmlNode GetNextNode(XmlNode node);
	char*   GetNodeText(XmlNode node);
	char*   GetNodeTag(XmlNode node);

private:

	char*	m_doc;
	int		m_size;
	int		m_nodes;
	char	m_szTag[XML_MAX_TAGNAME_SIZE];
	char	m_szText[XML_MAX_INNERTEXT_SIZE];
};
