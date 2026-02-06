#include "CSdlForwardDeclGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlUnion.h>


namespace imtsdlgencpp
{

// reimplemented (ICxxFileProcessor)

bool CSdlForwardDeclGeneratorComp::ProcessEntry(
			const imtsdl::CSdlEntryBase& sdlEntry,
			QIODevice* headerDevicePtr,
			QIODevice* /*sourceDevicePtr*/,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());

	if (headerDevicePtr == nullptr){
		return false;
	}

	// Get schema params from the entry
	const iprm::IParamsSet& schemaParams = sdlEntry.GetSchemaParams();
	
	QTextStream stream(headerDevicePtr);
	
	// Write pragma once
	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 3, false);
	
	// Write namespace
	const QString namespaceName = GetNamespaceFromSchemaParams(schemaParams);
	if (!namespaceName.isEmpty()){
		stream << QStringLiteral("namespace ") << namespaceName;
		FeedStream(stream, 1, false);
		stream << QStringLiteral("{");
		FeedStream(stream, 2, false);
	}
	
	// Generate forward declarations for all SDL elements
	GenerateForwardDeclarations(stream, schemaParams);
	
	// Close namespace
	if (!namespaceName.isEmpty()){
		FeedStream(stream, 1, false);
		stream << QStringLiteral("} // namespace ") << namespaceName;
		FeedStream(stream, 1, false);
	}

	return true;
}


// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CSdlForwardDeclGeneratorComp::GetIncludeDirectives() const
{
	// Forward declaration files don't need any includes
	static QList<imtsdl::IncludeDirective> retVal = {};
	return retVal;
}


// private methods

void CSdlForwardDeclGeneratorComp::GenerateForwardDeclarations(QTextStream& stream, const iprm::IParamsSet& schemaParams) const
{
	// Generate forward declarations for enums
	if (m_sdlEnumListCompPtr.IsValid()){
		const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
		if (!enumList.isEmpty()){
			stream << QStringLiteral("// Forward declarations for enums");
			FeedStream(stream, 1, false);
			
			for (const imtsdl::CSdlEnum& sdlEnum : enumList){
				// Only include enums from the same schema
				const QString enumNamespace = GetNamespaceFromSchemaParams(sdlEnum.GetSchemaParams());
				const QString currentNamespace = GetNamespaceFromSchemaParams(schemaParams);
				
				if (enumNamespace == currentNamespace){
					stream << QStringLiteral("enum class ") << sdlEnum.GetName() << QStringLiteral(";");
					FeedStream(stream, 1, false);
					stream << QStringLiteral("class Enum") << GetCapitalizedValue(sdlEnum.GetName()) << QStringLiteral(";");
					FeedStream(stream, 1, false);
				}
			}
			FeedStream(stream, 1, false);
		}
	}
	
	// Generate forward declarations for unions
	if (m_sdlUnionListCompPtr.IsValid()){
		const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);
		if (!unionList.isEmpty()){
			stream << QStringLiteral("// Forward declarations for unions");
			FeedStream(stream, 1, false);
			
			for (const imtsdl::CSdlUnion& sdlUnion : unionList){
				// Only include unions from the same schema
				const QString unionNamespace = GetNamespaceFromSchemaParams(sdlUnion.GetSchemaParams());
				const QString currentNamespace = GetNamespaceFromSchemaParams(schemaParams);
				
				if (unionNamespace == currentNamespace){
					stream << QStringLiteral("class ") << sdlUnion.GetName() << QStringLiteral(";");
					FeedStream(stream, 1, false);
					stream << QStringLiteral("class C") << sdlUnion.GetName() << QStringLiteral("Object;");
					FeedStream(stream, 1, false);
					stream << QStringLiteral("class C") << sdlUnion.GetName() << QStringLiteral("ObjectList;");
					FeedStream(stream, 1, false);
				}
			}
			FeedStream(stream, 1, false);
		}
	}
	
	// Generate forward declarations for types
	if (m_sdlTypeListCompPtr.IsValid()){
		const imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetTypes(true);
		if (!typeList.isEmpty()){
			stream << QStringLiteral("// Forward declarations for types");
			FeedStream(stream, 1, false);
			
			for (const imtsdl::CSdlType& sdlType : typeList){
				// Only include types from the same schema
				const QString typeNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
				const QString currentNamespace = GetNamespaceFromSchemaParams(schemaParams);
				
				if (typeNamespace == currentNamespace){
					stream << QStringLiteral("class C") << sdlType.GetName() << QStringLiteral(";");
					FeedStream(stream, 1, false);
					stream << QStringLiteral("class C") << sdlType.GetName() << QStringLiteral("List;");
					FeedStream(stream, 1, false);
				}
			}
		}
	}
}


} // namespace imtsdlgencpp
