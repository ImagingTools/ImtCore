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


public:
	/**
		\returns namespace from \c schemaParams, accorging to V2 Generation
	 */
	[[nodiscard]] static QString GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix = "sdl");

	[[nodiscard]] static QString GetTypeVerstion(const imtsdl::CSdlType& type);
	[[nodiscard]] static QString OptListConvertTypeWithNamespaceStruct(
		const imtsdl::CSdlField& sdlField,
		const QString& relatedNamespace,
		imtsdl::ISdlTypeListProvider& listProvider,
		bool listWrap = false,
		bool* isCustomPtr = nullptr,
		bool* isComplexPtr = nullptr,
		bool* isArrayPtr = nullptr);

	[[nodiscard]] static QString GetNullCheckString(const imtsdl::CSdlField& sdlField, bool checkNull = true);

	[[nodiscard]] static QString GetSettingValueString(
				const imtsdl::CSdlField& sdlField,
				const QString& relatedNamespace,
				imtsdl::ISdlTypeListProvider& listProvider,
				const QString& variableName,
				const QString& objectName = "object");

};


} // namespace imtsdlgenv2
