#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>


class QTextStream;

namespace iprm
{
class IParamsSet;
}


namespace imtsdl
{
class ISdlProcessArgumentsParser;
}


namespace imtsdlgenv2
{


class CSdlGenTools
{
public:
	class CStructNamespaceConverter
	{
	public:
		CStructNamespaceConverter();
		CStructNamespaceConverter(const imtsdl::CSdlField& sdlField, const QString& relatedNamespace, const imtsdl::ISdlTypeListProvider& listProvider, bool listWrap);

		const imtsdl::ISdlTypeListProvider* typeListProviderPtr = nullptr;
		QString relatedNamespace;
		bool listWrap = false;
		bool addVersion = false;
		bool cleanNamespace = true;
		const imtsdl::CSdlField* sdlFieldPtr = nullptr;
		[[nodiscard]] QString GetString() const;
	};

	/**
		\returns namespace from \c schemaParams, accorging to V2 Generation
	 */
	[[nodiscard]] static QString GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix = "sdl");

	[[nodiscard]] static QString GetTypeVerstion(const imtsdl::CSdlType& type);
	[[nodiscard]] static QString OptListConvertTypeWithNamespaceStruct(
		const imtsdl::CSdlField& sdlField,
		const QString& relatedNamespace,
		const imtsdl::ISdlTypeListProvider& listProvider,
		bool listWrap = false,
		bool* isCustomPtr = nullptr,
		bool* isComplexPtr = nullptr,
		bool* isArrayPtr = nullptr);

	/**
		\brief Generates a string, checks, if a field of object, named \c objectName, defined or not
		\param sdlField - field to be checked
		\param checkNull - if true - null expect (== nullptr); if false - NON null expected (!= nullptr)
		\param objectName - name of oject, holds a property
	 */
	[[nodiscard]] static QString GetNullCheckString(const imtsdl::CSdlField& sdlField, bool checkNull = true, const QString& objectName = "object");

	[[nodiscard]] static QString GetSettingValueString(
				const imtsdl::CSdlField& sdlField,
				const QString& relatedNamespace,
				const imtsdl::ISdlTypeListProvider& listProvider,
				const QString& variableName,
				const QString& objectName = "object");

	static void AddArrayInternalChecksFail(QTextStream& stream, const imtsdl::CSdlField& field, bool checkEmpty, const QString& objectName = "object", uint hIndents = 1);

};


} // namespace imtsdlgenv2
