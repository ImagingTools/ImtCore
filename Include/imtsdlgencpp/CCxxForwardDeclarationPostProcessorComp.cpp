// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCxxForwardDeclarationPostProcessorComp.h"


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlUnion.h>
#include <imtsdl/CSdlDocumentType.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdlgencpp
{


// reimplemented (ICxxFilePostProcessor)

bool CCxxForwardDeclarationPostProcessorComp::ProcessFiles(const CxxEntryFileInfo& files, const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_schemaParamsCompPtr.IsValid());

	imtsdl::ISdlProcessArgumentsParser::CppGenerationMode mode = m_argumentParserCompPtr->GetCppGenerationMode();
	if (mode == imtsdl::ISdlProcessArgumentsParser::CGM_IMPLEMENTATION_ONLY){
		return true;
	}

	const CxxEntryFileInfo::FilePtr& fwdFilePtr = files.fwdHeaderFile;
	if (!fwdFilePtr){
		SendErrorMessage(0, QStringLiteral("The forward declaration file is not created"));

		return false;
	}

	QTextStream stream(fwdFilePtr.get());

	// write pragma once
	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 3, false);

	// check if we have enums
	bool hasEnums = false;
	if (m_sdlEnumListCompPtr.IsValid()){
		const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
		hasEnums = !enumList.isEmpty();
	}

	// check if we have requests (need forward declaration for GqlHandlerCompBase)
	bool hasRequests = false;
	if (m_requestsProviderListCompPtr.IsValid()){
		const imtsdl::SdlRequestList requestsList = m_requestsProviderListCompPtr->GetRequests(true);
		hasRequests = !requestsList.isEmpty();
	}

	// check if we have document types
	bool hasDocumentTypes = false;
	if (m_sdlDocumentTypeListCompPtr.IsValid()){
		const imtsdl::SdlDocumentTypeList documentTypesList = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(true);
		hasDocumentTypes = !documentTypesList.isEmpty();
	}

	// add include for CObjectCollectionControllerCompBase if we have document types
	if (hasDocumentTypes){
		stream << QStringLiteral("#include <imtservergql/CObjectCollectionControllerCompBase.h>");
		FeedStream(stream, 1, false);
	}

	// add include for GqlHandlerCompBase base class if we have requests
	if (hasRequests){
		stream << QStringLiteral("#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>");
		FeedStream(stream, 1, false);
	}

	if (hasEnums || hasDocumentTypes || hasRequests){
		FeedStream(stream, 2, false);
	}

	// add custom types includes (from external/dependent schemas)
	QList<imtsdl::IncludeDirective> customDirectives;
	if (m_sdlTypeListCompPtr.IsValid()){
		imtsdl::SdlTypeList list = m_sdlTypeListCompPtr->GetSdlTypes(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_sdlUnionListCompPtr.IsValid()){
		imtsdl::SdlUnionList list = m_sdlUnionListCompPtr->GetUnions(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_sdlDocumentTypeListCompPtr.IsValid()){
		imtsdl::SdlDocumentTypeList list = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_requestsProviderListCompPtr.IsValid()){
		imtsdl::SdlRequestList list = m_requestsProviderListCompPtr->GetRequests(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}
	if (m_sdlEnumListCompPtr.IsValid()){
		imtsdl::SdlEnumList list = m_sdlEnumListCompPtr->GetEnums(false);
		CreateDirectivesFromEntryList(list, customDirectives);
	}

	if (!customDirectives.isEmpty()){
		stream << QStringLiteral("// custom types includes");
		FeedStream(stream, 1, false);
		for (const imtsdl::IncludeDirective& directive: customDirectives){
			// Convert .h path to _fwd.h for forward declaration files
			QString fwdPath = directive.path;
			if (fwdPath.endsWith(QStringLiteral(".h>"))){
				fwdPath.replace(fwdPath.length() - 3, 3, QStringLiteral("_fwd.h>"));
			}
			stream << QStringLiteral("#include ");
			stream << fwdPath;
			FeedStream(stream, 1, false);
		}
		FeedStream(stream, 2, false);
	}

	// begin namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(*m_schemaParamsCompPtr);
	stream << QStringLiteral("namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 1, false);
	stream << '{';
	FeedStream(stream, 2, false);

	// generate enum classes
	const QString schemaBaseName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).baseName();

	if (hasEnums){
		// The enums together with their Q_NAMESPACE/Q_ENUM_NS registrations are
		// wrapped in a schema-unique inline namespace. Multiple schemas can
		// contribute to the same C++ namespace (e.g. sdl::V1_0::imtbase), and
		// emitting Q_NAMESPACE in each of them would generate colliding
		// definitions of the namespace staticMetaObject. The inline namespace
		// keeps the enums and the EnumXxx wrapper classes accessible from the
		// parent namespace while giving each schema its own meta object.

		stream << QStringLiteral("inline namespace ") << schemaBaseName << QStringLiteral("SdlEnums");
		FeedStream(stream, 1, false);
		stream << '{';
		FeedStream(stream, 2, false);

		// flush the stream before passing the device to enum processors
		stream.flush();

		const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
		FeedStream(stream, 1, false);
		for (const imtsdl::CSdlEnum& sdlEnum: enumList){
			stream <<  QStringLiteral("enum class ") << sdlEnum.GetName() << ';';
			FeedStream(stream, 1, false);
		}

		FeedStream(stream, 2, false);
		stream << QStringLiteral("} // inline namespace ") << schemaBaseName << QStringLiteral("SdlEnums");
		FeedStream(stream, 2, false);
	}

	stream.flush();

	// re-create stream after enum processors may have written to the file
	QTextStream fwdStream(fwdFilePtr.get());

	// forward declare all types
	if (m_sdlTypeListCompPtr.IsValid()){
		const imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
		if (!typeList.isEmpty()){
			fwdStream << QStringLiteral("// type forward declarations");
			FeedStream(fwdStream, 1, false);
			for (const imtsdl::CSdlType& sdlType: typeList){
				fwdStream << QStringLiteral("class C") << sdlType.GetName() << ';';
				FeedStream(fwdStream, 1, false);
			}
			FeedStream(fwdStream, 1, false);
		}
	}

	// forward declare all unions
	if (m_sdlUnionListCompPtr.IsValid()){
		const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);
		if (!unionList.isEmpty()){
			fwdStream << QStringLiteral("// union forward declarations");
			FeedStream(fwdStream, 1, false);
			for (const imtsdl::CSdlUnion& sdlUnion: unionList){
				fwdStream << QStringLiteral("class ") << sdlUnion.GetName() << ';';
				FeedStream(fwdStream, 1, false);
				if (m_argumentParserCompPtr->IsObjectsGenerationEnabled()){
					fwdStream << QStringLiteral("class C") << sdlUnion.GetName() << QStringLiteral("Object;");
					FeedStream(fwdStream, 1, false);
					fwdStream << QStringLiteral("class C") << sdlUnion.GetName() << QStringLiteral("ObjectList;");
					FeedStream(fwdStream, 1, false);
				}
			}
			FeedStream(fwdStream, 1, false);
		}
	}

	// forward declare requests
	if (hasRequests){
		const imtsdl::SdlRequestList requestsList = m_requestsProviderListCompPtr->GetRequests(true);
		fwdStream << QStringLiteral("// request forward declarations");
		FeedStream(fwdStream, 1, false);
		for (const imtsdl::CSdlRequest& request: requestsList){
			fwdStream << QStringLiteral("struct ") << request.GetName() << QStringLiteral("RequestInfo;");
			FeedStream(fwdStream, 1, false);

			fwdStream << QStringLiteral("class C") << request.GetName() << QStringLiteral("GqlRequest;");
			FeedStream(fwdStream, 1, false);
		}
		FeedStream(fwdStream, 1, false);
	}

	// generate CCollectionControllerCompBase class definitions (moved from .h)
	if (hasDocumentTypes && m_documentTypeProcessorCompListPtr.IsValid()){
		// flush the stream before passing the device to document type processors
		fwdStream.flush();

		const imtsdl::SdlDocumentTypeList documentTypesList = m_sdlDocumentTypeListCompPtr->GetDocumentTypes(true);
		const int documentTypeProcessorsCount = m_documentTypeProcessorCompListPtr.GetCount();
		for (int i = 0; i < documentTypeProcessorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_documentTypeProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);
			for (const imtsdl::CSdlDocumentType& documentType: documentTypesList){
				const bool ok = processorPtr->ProcessEntry(documentType, fwdFilePtr.get(), nullptr, paramsPtr);
				if (!ok){
					return false;
				}
			}
		}
	}

	// generate GqlHandlerCompBase class definition (moved from .h)
	if (hasRequests && m_autoProcessorCompListPtr.IsValid()){
		// flush the stream before passing the device to auto processors
		fwdStream.flush();

		const int processorsCount = m_autoProcessorCompListPtr.GetCount();
		imtsdl::CSdlType dummyType;
		for (int i = 0; i < processorsCount; ++i){
			ICxxFileProcessor* processorPtr = m_autoProcessorCompListPtr[i];
			Q_ASSERT(processorPtr != nullptr);
			const bool ok = processorPtr->ProcessEntry(dummyType, fwdFilePtr.get(), nullptr, paramsPtr);
			if (!ok){
				return false;
			}
		}
	}

	// end namespace
	fwdStream << '}';
	fwdStream << QStringLiteral(" // namespace ");
	fwdStream << sdlNamespace;
	FeedStream(fwdStream, 1, false);
	fwdStream.flush();

	return true;
}


} // namespace imtsdlgencpp
