#pragma once


// Qt includes
#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QVariant>

// Acf includes
#include <istd/ILogger.h>

// imtsdl includes
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/CSdlRequest.h>


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
			virtual protected istd::ILogger
{

	Q_DISABLE_COPY(CGqlSchemaParser)

protected:
	CGqlSchemaParser();

public:
	CGqlSchemaParser(QIODevice& device);

	virtual bool ParseGqlSchema();

	// reimplemented (ISdlTypeListProvider)
	virtual QStringList GetTypeNames() const override;
	virtual SdlTypeList GetSdlTypes() const override;
	virtual SdlFieldList GetFields(const QString typeName) const override;

	// reimplemented (ISdlRequestListProvider)
	virtual SdlRequestList GetRequests() const override;

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
	bool MoveAfterWord(const QString& word);

protected:
	QTextStream m_stream;
	char m_lastReadChar;
	qulonglong m_lastReadLine;
	bool m_useLastReadChar;
	QMap<KeyId, QByteArray> m_keywordMap;

	SdlTypeList m_sdlTypes;
	SdlRequestList m_requests;
};


} // namespace imtsdl









