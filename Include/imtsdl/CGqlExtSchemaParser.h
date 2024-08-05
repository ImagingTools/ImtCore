#pragma once


// Qt includes
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QVariant>

// Acf includes
#include <istd/ILogger.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParser.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>


namespace imtsdl
{


/**
	Extend class for \class CGqlSchemaParser allows to process documentSchema
	documentSchema - is an IMT special derictive for generation base classes for GraphQL handlers
 */
class CGqlExtSchemaParser:
			public CGqlSchemaParser,
			virtual public ISdlDocumentTypeListProvider
{
	Q_DISABLE_COPY(CGqlExtSchemaParser)

protected:
	CGqlExtSchemaParser();

public:
	typedef CGqlSchemaParser BaseClass;

	CGqlExtSchemaParser(QIODevice& device);

	// reimplemented (ISdlDocumentTypeListProvider)
	virtual SdlDocumentTypeList GetDocumentTypes() const override;

protected:
	virtual bool ProcessDocumentSchema();

	// reimplemented (CGqlSchemaParser)
	virtual bool ProcessCustomSection(const QString& sectionName) override;

protected:
	SdlDocumentTypeList m_documentTypes;


};


} // namespace imtsdl









