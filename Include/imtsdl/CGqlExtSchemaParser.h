// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual SdlDocumentTypeList GetDocumentTypes(bool onlyLocal) const override;

protected:
	virtual bool ProcessCollectionSchema();
	bool ExtractDocumentTypeFromCurrentEntry(CSdlDocumentType& documentType);

	// reimplemented (CGqlSchemaParser)
	virtual bool ProcessCustomSection(const QString& sectionName) override;
	virtual bool ValidateSchema() override;
	virtual bool ProcessCustomSchemaValue(const QString& key, const QString& value) override;


protected:
	SdlDocumentTypeList m_documentTypes;
};


} // namespace imtsdl









