// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassist/CMarkdownKnowledgeBase.h>


namespace imtassist
{


/**
	Knowledge base from Markdown files, loaded at startup. \c DocumentPaths are files or
	directories, also in the resources (":/..."); the \c .md files of a directory are loaded
	with their subdirectories.

	\ingroup imtassist
*/
class CMarkdownKnowledgeBaseComp:
			public ilog::CLoggerComponentBase,
			public CMarkdownKnowledgeBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	enum
	{
		MAX_DOCUMENT_SIZE = 2 * 1024 * 1024
	};

	I_BEGIN_COMPONENT(CMarkdownKnowledgeBaseComp);
		I_REGISTER_INTERFACE(IAssistantKnowledgeBase);
		I_ASSIGN_MULTI_0(m_documentPathsAttrPtr, "DocumentPaths", "Markdown files or directories with Markdown files, e.g. :/AssistantKnowledge/Product", false);
	I_END_COMPONENT;

	/**
		Load the Markdown files of a file or directory path.
		\return Number of loaded documents.
	*/
	int LoadPath(const QString& path);

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool LoadFile(const QString& filePath, const QString& source);

private:
	I_MULTIATTR(QString, m_documentPathsAttrPtr);
};


} // namespace imtassist


