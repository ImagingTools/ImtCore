#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>


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
		CStructNamespaceConverter(const imtsdl::CSdlEntryBase& sdlEntry, const QString& relatedNamespace, const imtsdl::ISdlTypeListProvider& listProvider, const imtsdl::ISdlEnumListProvider& enumListProvider, bool listWrap);
		CStructNamespaceConverter(const imtsdl::CSdlField& sdlField, const QString& relatedNamespace, const imtsdl::ISdlTypeListProvider& listProvider,	const imtsdl::ISdlEnumListProvider& enumListProvider, bool listWrap);

		const imtsdl::ISdlTypeListProvider* typeListProviderPtr = nullptr;
		const imtsdl::ISdlEnumListProvider* enumListProviderPtr = nullptr;
		QString relatedNamespace;
		bool listWrap = false;
		bool addVersion = false;
		bool cleanNamespace = true;
		const imtsdl::CSdlField* sdlFieldPtr = nullptr;
		const imtsdl::CSdlEntryBase* sdlEntryPtr = nullptr;
		[[nodiscard]] QString GetString() const;
	};

	/**
		\returns namespace from \c schemaParams, accorging to V2 Generation
		\todo remove \param prefix or define it elsewhere
	 */
	[[nodiscard]] static QString GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix = "sdl");

	/**
		\overload for \c GetNamespaceFromSchemaParams, uses params from \c type
	 */
	[[nodiscard]] static QString GetNamespaceFromTypeSchemaParams(const imtsdl::CSdlType& type, const QString& prefix = "sdl");

	[[nodiscard]] static QString GetSchemaVerstionString(const iprm::IParamsSet& schemaParams, bool addVPrefix = true);
	[[nodiscard]] static QString OptListConvertTypeWithNamespaceStruct(
		const imtsdl::CSdlField& sdlField,
		const QString& relatedNamespace,
		const imtsdl::ISdlTypeListProvider& listProvider,
		const imtsdl::ISdlEnumListProvider& enumlistProvider,
		bool listWrap = false,
		bool* isCustomPtr = nullptr,
		bool* isComplexPtr = nullptr,
		bool* isArrayPtr = nullptr,
		bool* isEnumPtr = nullptr);

	/**
		\brief Generates a string, checks, if a field of object, named \c objectName, defined or not
		\param sdlField - field to be checked
		\param checkNull - if true - null (NON VALID) expected, (== nullptr); if false - NON null (VALID) expected (!= nullptr)
		\param objectName - name of oject, holds a property
	 */
	[[nodiscard]] static QString GetNullCheckString(const imtsdl::CSdlField& sdlField, bool checkNull = true, const QString& objectName = QString());

	[[nodiscard]] static QString GetSettingValueString(
				const imtsdl::CSdlField& sdlField,
				const QString& relatedNamespace,
				const imtsdl::ISdlTypeListProvider& listProvider,
				const QString& variableName,
				const QString& objectName = QString());

	static void AddArrayInternalChecksFail(QTextStream& stream, const imtsdl::CSdlField& field, bool checkEmpty, const QString& objectName = QString(), uint hIndents = 1);

	template <class SdlEntryClass>
	[[nodiscard]] static QString GetSdlEntryVersion(const SdlEntryClass& entry, bool addVPrefix = true);
};


template<class SdlEntryClass>
inline QString CSdlGenTools::GetSdlEntryVersion(const SdlEntryClass& entry, bool addVPrefix)
{
	return GetSchemaVerstionString(entry.GetSchemaParams(), addVPrefix);
}


} // namespace imtsdlgenv2
