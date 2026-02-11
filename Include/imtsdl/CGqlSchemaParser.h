// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QVariant>

// ACF includes
#include <istd/ILogger.h>

// ImtCore includes
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdl/CSdlEnumConverter.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlUnion.h>


namespace iprm {

class CParamsSet;

} // namespace iprm::CParamsSet


namespace imtsdl
{

/**
	Class for parsing a GraphQL schema
	\details used schema definition, discribed at \link https://graphql.com/learn/schema/ or \link https://www.apollographql.com/docs/apollo-server/schema/schema/
 */
class CGqlSchemaParser:
			virtual public istd::IPolymorphic,
			virtual public ISdlTypeListProvider,
			virtual public ISdlRequestListProvider,
			virtual public ISdlEnumListProvider,
			virtual public ISdlUnionListProvider,
			virtual protected istd::ILogger,
			protected CSdlEnumConverter
{

	Q_DISABLE_COPY(CGqlSchemaParser)

protected:
	CGqlSchemaParser();

public:
	CGqlSchemaParser(QIODevice& device);

	virtual bool ParseGqlSchema();
	bool SetSchemaName(const QString& schemaName) const;

	// reimplemented (ISdlTypeListProvider)
	virtual QStringList GetTypeNames() const override;
	virtual SdlTypeList GetSdlTypes(bool onlyLocal) const override;
	virtual SdlFieldList GetFields(const QString typeName) const override;

	// reimplemented (ISdlRequestListProvider)
	virtual SdlRequestList GetRequests(bool onlyLocal) const override;

	// reimplemented (ISdlEnumListProvider)
	virtual SdlEnumList GetEnums(bool onlyLocal) const override;

	// reimplemented (ISdlUnionListProvider)
	virtual SdlUnionList GetUnions(bool onlyLocal) const override;

protected:
	virtual bool ProcessSchema();
	virtual bool ProcessSchemaImports();
	virtual bool ProcessType();
	virtual bool ProcessInterface();
	virtual bool ProcessUnion();
	virtual bool ProcessScalar();
	virtual bool ProcessEnum();
	virtual bool ProcessInput();
	virtual bool ProcessExtend();
	virtual bool ProcessDirective();
	virtual bool ProcessQuery();
	virtual bool ProcessMutation();
	virtual bool ProcessSubscription();
	virtual bool ProcessValue(SdlFieldList& output, bool* endOfReadPtr);
	virtual bool ProcessRequests(CSdlRequest::Type type);

	/**
		Processing custom schema values
		\param sectionName - name of a section
		\warning Base implementation always returns \c false and do nothing
	 */
	virtual bool ProcessCustomSchemaValue(const QString& key, const QString& value);

	/**
		Processing Custom Sections
		\param sectionName - name of a section
		\warning Base implementation always returns \c false and generates an error
	 */
	virtual bool ProcessCustomSection(const QString& sectionName);

	virtual bool ValidateSchema();

	void SetDevice(QIODevice& device);
	bool ReadToDelimeter(
				const QByteArray& delimeters,
				QByteArray& result,
				char* foundDelimeterPtr = nullptr,
				bool allowEmptyResult = true,
				bool skipDelimeter = true);
	bool ReadToDelimeterOrSpace(
			const QByteArray& delimeters,
			QByteArray& result,
			char* foundDelimeterPtr = nullptr,
			bool allowEmptyResult = true,
			bool skipDelimeter = true);

	bool MoveToCharType(QChar::Category category, char* foundDelimeterPtr = nullptr, bool skipDelimeter = false);
	bool MoveToCharType(const QList<QChar::Category>& categoryList, char* foundDelimeterPtr = nullptr, bool skipDelimeter = false);
	bool MoveToNextReadableSymbol(char* foundDelimeterPtr = nullptr, bool skipDelimeter = false);
	bool MoveInside();
	bool MoveAfterWord(const QString& word);

protected:
	QTextStream m_stream;
	char m_lastReadChar;
	qulonglong m_lastReadLine;
	bool m_useLastReadChar;
	QMap<KeyId, QByteArray> m_keywordMap;
	QString m_originalSchemaFile;

	SdlTypeList m_sdlTypes;
	SdlRequestList m_requests;
	SdlEnumList m_enums;
	SdlUnionList m_unions;

	/**
		\brief saved schema parameters, declarated in .sdl file
		\note this parameter is NEVER null
		\warning this parameter MUST NOT be setted to nullptr
		\code m_schemaParamsPtr != nullptr // = true \endcode
	 */
	std::shared_ptr<iprm::CParamsSet> m_schemaParamsPtr;
};


} // namespace imtsdl









