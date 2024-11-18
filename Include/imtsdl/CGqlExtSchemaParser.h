#pragma once


// Qt includes
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QVariant>

// ACF includes
#include <istd/ILogger.h>
#include <iprm/CParamsSet.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtsdl/CGqlSchemaParser.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>


namespace imtsdl
{


/**
	Extend class for \class CGqlSchemaParser allows to process undeclarated directives
	\c documentSchema - is an IMT special derictive for generation base classes for GraphQL handlers
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
	virtual bool ProcessCollectionSchema();
	bool ExtractDocumentTypeFromCurrentEntry(CSdlDocumentType& documentType);

	// reimplemented (CGqlSchemaParser)
	virtual bool ProcessCustomSection(const QString& sectionName) override;
	virtual bool ValidateSchema() override;
	virtual bool ProcessCustomSchemaValue(const QString& key, const QString& value) override;


protected:
	SdlDocumentTypeList m_documentTypes;

	std::shared_ptr<iprm::CParamsSet> m_customSchemaParamsPtr;
};


} // namespace imtsdl









