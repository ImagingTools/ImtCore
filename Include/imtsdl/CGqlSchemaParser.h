#pragma once


// Qt includes

#include <QtCore/QTextStream>
#include <QtCore/QIODevice>
#include <QtCore/QMap>
#include <QtCore/QVariant>

// imtsdl includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlMethod.h>


namespace imtsdl
{


class CGqlSchemaParser
{


public:
	/// \todo add constructors
	CGqlSchemaParser(QIODevice& device);

	SdlTypeList GetTypes() const;
	QStringList GetTypeNames() const;
	SdlFieldList GetFields(const QString typeName) const;

private:
	bool ParseGqlSchema();
	bool ProcessSchema();
	bool ProcessType();
	bool ProcessInterface();
	bool ProcessUnion();
	bool ProcessScalar();
	bool ProcessEnum();
	bool ProcessInput();
	bool ProcessExtend();
	bool ProcessDirective();
	bool ProcessQuery();
	bool ProcessMutation();
	bool ProcessSubscription();
	bool ProcessValue(SdlFieldList& output, bool* endOfReadPtr);

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
	bool MoveToNextReaddableSymbol(char* foundDelimeterPtr = nullptr, bool skipDelimeter = false);


private:
	QTextStream m_stream;
	char m_lastReadChar;
	qulonglong m_lastReadLine;
	bool m_useLastReadChar;
	QMap<KeyId, QByteArray> m_keywordMap;

	SdlTypeList m_sdlTypes;
	SdlMethodList m_sdlQueries;
	SdlMethodList m_sdlMutations;
};


} // namespace imtsdl









