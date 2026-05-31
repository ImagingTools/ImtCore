// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlDocumentType.h>


class QTextStream;

namespace iprm
{
class IParamsSet;
}


namespace imtsdl
{
class ISdlProcessArgumentsParser;
}


namespace imtsdlgencpp
{


class CSdlGenTools
{
public:
	class CStructNamespaceConverter
	{
	public:
		CStructNamespaceConverter();
		CStructNamespaceConverter(const imtsdl::CSdlEntryBase& sdlEntry, const QString& relatedNamespace, const imtsdl::ISdlTypeListProvider& listProvider, const imtsdl::ISdlEnumListProvider& enumListProvider, const imtsdl::ISdlUnionListProvider& unionListProvider, bool listWrap);
		CStructNamespaceConverter(const imtsdl::CSdlField& sdlField, const QString& relatedNamespace, const imtsdl::ISdlTypeListProvider& listProvider,	const imtsdl::ISdlEnumListProvider& enumListProvider, const imtsdl::ISdlUnionListProvider& unionListProvider, bool listWrap);

		const imtsdl::ISdlTypeListProvider* typeListProviderPtr = nullptr;
		const imtsdl::ISdlEnumListProvider* enumListProviderPtr = nullptr;
		const imtsdl::ISdlUnionListProvider* unionListProviderPtr = nullptr;
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
		const imtsdl::ISdlUnionListProvider& unionlistProvider,
		bool listWrap = false,
		bool* isCustomPtr = nullptr,
		bool* isComplexPtr = nullptr,
		bool* isArrayPtr = nullptr,
		bool* isEnumPtr = nullptr,
		bool* isUnionPtr = nullptr,
		const bool& addVersion = true);

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

	/**
		\param errorMessage - error message, can contain placeholders \warning placeholders MUST start from 3. 0-2 - reserved for additional info
		\param argset - placeholders for \c errorMessage
		\param onlyDebug - if enabled, the message will print only in debug mode
	 */
	static void AddErrorReport(QTextStream& stream, const QString& errorMessage, uint hIndents = 0,  const QStringList& argset = QStringList(), bool onlyDebug = true);

	template <class SdlEntryClass>
	[[nodiscard]] static QString GetSdlEntryVersion(const SdlEntryClass& entry, bool addVPrefix = true);

	[[nodiscard]] static QString GetTempVariableWrappedValue(const imtsdl::CSdlField& sdlField);

	static void GenerateIsRequestSupportedMethodImpl(
				QTextStream& stream,
				const imtsdl::SdlRequestList& requestList,
				const QString& className);

	[[nodiscard]] static QString GetQObjectTypeName(
				const imtsdl::CSdlField& sdlField,
				const imtsdl::SdlTypeList& typeList,
				const imtsdl::SdlEnumList& enumList,
				const imtsdl::SdlUnionList& unionList,
				bool withPointer = true,
				bool treatArrayAsElement = false);

	[[nodiscard]] static QString GetTempVariableWrappedValue(const QString& variableName);

	[[nodiscard]] static std::shared_ptr<imtsdl::CSdlEntryBase> GetCollectionReferenceForDocument(
				const imtsdl::CSdlDocumentType& documentType,
				const imtsdl::SdlTypeList& typeList,
				const imtsdl::SdlUnionList& unionList,
				imtsdl::CSdlDocumentType::OperationType operationType);

	[[nodiscard]] static QString GetCollectionReferenceName(
				const imtsdl::CSdlDocumentType& documentType,
				const imtsdl::SdlTypeList& typeList,
				const imtsdl::SdlUnionList& unionList,
				imtsdl::CSdlDocumentType::OperationType operationType);
};


// inline methods

template<class SdlEntryClass>
inline QString CSdlGenTools::GetSdlEntryVersion(const SdlEntryClass& entry, bool addVPrefix)
{
	return GetSchemaVerstionString(entry.GetSchemaParams(), addVPrefix);
}

inline QString CSdlGenTools::GetTempVariableWrappedValue(const imtsdl::CSdlField& sdlField)
{
	return GetTempVariableWrappedValue(sdlField.GetId());
}

inline QString CSdlGenTools::GetCollectionReferenceName(
			const imtsdl::CSdlDocumentType& documentType,
			const imtsdl::SdlTypeList& typeList,
			const imtsdl::SdlUnionList& unionList,
			imtsdl::CSdlDocumentType::OperationType operationType)
{
	auto refEntry = GetCollectionReferenceForDocument(documentType, typeList, unionList, operationType);
	if (refEntry)
	{
		return refEntry->GetName();
	}

	return {};
}



} // namespace imtsdlgencpp
