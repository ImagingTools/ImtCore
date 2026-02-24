// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "imtsdl/CSdlTools.h"
#include "imtsdl/CSdlType.h"
#include "imtsdl/CSdlUnion.h"
#include "imtsdlgencpp/CSdlGenTools.h"
#include <QtCore/qassert.h>
#include <imtsdlgencpp/CGqlCollectionControllerBaseClassGeneratorComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdlgencpp/CSdlUnionConverter.h>


namespace imtsdlgencpp
{


// static variables
const QMap<imtsdl::CSdlDocumentType::OperationType, QString> CGqlCollectionControllerBaseClassGeneratorComp::s_nonTrivialOperationMethodsMap = {
	std::make_pair(imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION,		"UpdateCollection"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_RENAME,					"RenameObject"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION,		"SetObjectDescription"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_HEADERS,				"GetHeaders"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_INFO,					"GetInfo"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_METAINFO,				"GetMetaInfo"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_DATAMETAINFO,			"GetDataMetaInfo"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY,		"GetObjectHistory"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_IMPORT,					"ImportObject"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_EXPORT,					"ExportObject"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_GET_VIEW,				"")
};

static QMap<imtsdl::CSdlDocumentType::OperationType, QString> s_operationsAliasList({
	std::make_pair(imtsdl::CSdlDocumentType::OT_GET,					QStringLiteral("OT_GET")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_INSERT,					QStringLiteral("OT_NEW")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_UPDATE,					QStringLiteral("OT_UPDATE")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_LIST,					QStringLiteral("OT_LIST")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION,		QStringLiteral("OT_UPDATE_COLLECTION")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_RENAME,					QStringLiteral("OT_RENAME")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION,		QStringLiteral("OT_SET_DESCRIPTION")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_HEADERS,				QStringLiteral("OT_HEADERS")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_INFO,					QStringLiteral("OT_INFO")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_METAINFO,				QStringLiteral("OT_METAINFO")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_DATAMETAINFO,			QStringLiteral("OT_DATAMETAINFO")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT,			QStringLiteral("OT_ELEMENTS_COUNT")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENT_IDS,			QStringLiteral("OT_ELEMENT_IDS")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY,		QStringLiteral("OT_ELEMENT_HISTORY")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_IMPORT,					QStringLiteral("OT_IMPORT")),
	std::make_pair(imtsdl::CSdlDocumentType::OT_EXPORT,					QStringLiteral("OT_EXPORT"))
});


// public methods

// reimplemented (ICxxFileProcessor)

bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessEntry (
			const imtsdl::CSdlEntryBase& sdlEntry,
			QIODevice* headerDevicePtr,
			QIODevice* sourceDevicePtr,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_baseClassExtenderCompPtr.IsValid());
	Q_ASSERT(m_sdlDocumentListCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());


	const auto* sdlDocumentType = dynamic_cast<const imtsdl::CSdlDocumentType*>(&sdlEntry);
	if (sdlDocumentType == nullptr || (headerDevicePtr == nullptr && sourceDevicePtr == nullptr)){
		return false;
	}

	bool retVal = true;
	if (headerDevicePtr != nullptr){
		retVal = retVal && ProcessHeaderClassFile(*sdlDocumentType, headerDevicePtr, paramsPtr);
	}
	if (sourceDevicePtr != nullptr){
		retVal = retVal && ProcessSourceClassFile(*sdlDocumentType, sourceDevicePtr, paramsPtr);
	}

	return retVal;
}


// reimplemented (ICxxFileProcessor)

QList<imtsdl::IncludeDirective> CGqlCollectionControllerBaseClassGeneratorComp::GetIncludeDirectives() const
{
	if (!m_sdlRequestListCompPtr.IsValid()){
		return {};
	}

	imtsdl::SdlDocumentTypeList list = m_sdlDocumentListCompPtr->GetDocumentTypes(true);
	if (list.isEmpty()){
		return {};
	}

	static QList retVal = {
		CreateImtDirective("<imtservergql/CObjectCollectionControllerCompBase.h>")
	};

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessHeaderClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, QIODevice* headerDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream ifStream(headerDevicePtr);

	const imtsdl::SdlDocumentTypeList subtypesList = sdlDocumentType.GetSubtypes();
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);

	// class begin
	ifStream << QStringLiteral("class C") << sdlDocumentType.GetName() << QStringLiteral("CollectionControllerCompBase: public ::imtservergql::CObjectCollectionControllerCompBase");
	FeedStream(ifStream, 1, false);

	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 2, false);

	// public section
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("typedef ::imtservergql::CObjectCollectionControllerCompBase BaseClass;");
	FeedStream(ifStream, 2, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("I_BEGIN_BASE_COMPONENT(C");
	ifStream << sdlDocumentType.GetName();
	ifStream << QStringLiteral("CollectionControllerCompBase)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("I_END_COMPONENT");
	FeedStream(ifStream, 2, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("virtual QMap<int, QByteArray> GetSupportedCommandIds() const override;");
	FeedStream(ifStream, 2, false);

	// protected section (base)
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	QList<imtsdl::CSdlDocumentType::OperationType> operationsList = sdlDocumentType.GetOperationsList().keys();
	for (const imtsdl::CSdlDocumentType& subtype : sdlDocumentType.GetSubtypes()){
		operationsList << subtype.GetOperationsList().keys();
	}

	// base class methods override definition
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;");
	FeedStream(ifStream, 1, false);

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_LIST)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual bool SetupGqlItem(const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_LIST);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_GET)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_GET);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_UPDATE)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual bool UpdateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_UPDATE);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_INSERT)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_INSERT);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_GET_VIEW)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_GET_VIEW);
	}

	// remove all collection trivial types
	operationsList.removeAll(imtsdl::CSdlDocumentType::OT_DELETE);
	operationsList.removeAll(imtsdl::CSdlDocumentType::OT_ELEMENT_IDS);
	operationsList.removeAll(imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT);

	// add methods for special cases (non-trivial collection methods)
	for (const imtsdl::CSdlDocumentType::OperationType& operationType: operationsList){
		if (s_nonTrivialOperationMethodsMap[operationType].isEmpty()){
			I_CRITICAL();

			continue;
		}
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual ");
		AddBaseMethodDeclarationForOperationType(ifStream, operationType, QString());
		ifStream << QStringLiteral(" override;");
		FeedStream(ifStream, 1, false);
	}


	// protected section
	// definition of pure virtual methods (to be reimplemented)
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	AddMethodsForDocument(ifStream, sdlDocumentType, 1);
	FeedStream(ifStream, 1, false);

	for (const imtsdl::CSdlDocumentType& subtype: subtypesList){
		AddMethodsForDocument(ifStream, subtype, 1);
		FeedStream(ifStream, 1, false);
	}

	// class end
	ifStream << QStringLiteral("};");
	FeedStream(ifStream, 3, false);

	return true;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessSourceClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, QIODevice* sourceDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream ifStream(sourceDevicePtr);

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);

	// GetSupportedCommandIds method
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");

	ifStream << QStringLiteral("QMap<int, QByteArray> ");
	ifStream << className;
	ifStream << QStringLiteral("::GetSupportedCommandIds() const");
	FeedStream(ifStream, 1, false);

	ifStream << '{';
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("static QMap<int, QByteArray> retVal = {");
	FeedStream(ifStream, 1, false);

	AddOperationMapPairs(ifStream, sdlDocumentType);
	for (const imtsdl::CSdlDocumentType& subtype: sdlDocumentType.GetSubtypes()){
		AddOperationMapPairs(ifStream, subtype);
	}

	FeedStreamHorizontally(ifStream);
	ifStream << '}' << ';';
	FeedStream(ifStream, 2, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("return retVal;");
	FeedStream(ifStream, 1, false);

	ifStream << '}';
	FeedStream(ifStream, 3, false);

	// operations
	AddOperationRequestMethodImplForDocument(ifStream, sdlDocumentType);

	bool isCorrect = AddCollectionMethodsImplForDocument(ifStream, sdlDocumentType);
	if (!isCorrect){
		return false;
	}

	AddSpecialMethodImplCodeForDocument(ifStream, sdlDocumentType);

	return true;
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodDeclarationForOperationType(
			QTextStream& stream,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const imtsdl::CSdlRequest& sdlRequest) const
{
	if (!s_nonTrivialOperationMethodsMap.contains(operationType)){
		SendCriticalMessage(0, "Unexpected type");
		I_CRITICAL();

		return;
	}

	QString requestCalssName = sdlRequest.GetName();
	requestCalssName[0] = requestCalssName[0].toUpper();
	QString functionName = s_nonTrivialOperationMethodsMap[operationType];
	if (functionName.isEmpty()){
		functionName = GetCapitalizedValue(sdlRequest.GetName());
	}
	functionName.prepend(QStringLiteral(" On"));

	imtsdl::CSdlField type = sdlRequest.GetOutputArgument();
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		&sdlRequest.GetSchemaParams(),
		m_argumentParserCompPtr.GetPtr(),
		false);
	CStructNamespaceConverter structNamespaceConverter(
		type,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		false);

	stream << structNamespaceConverter.GetString();
	stream << ' ';
	stream << functionName;

	stream << QStringLiteral("(const C");
	stream << requestCalssName;
	stream << QStringLiteral("GqlRequest& ");
	stream << GetDecapitalizedValue(requestCalssName);
	stream << QStringLiteral("GqlRequest, QString& errorMessage) const");
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddBaseMethodDeclarationForOperationType(
			QTextStream& stream,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const QString& className) const
{
	if (!s_nonTrivialOperationMethodsMap.contains(operationType)){
		SendCriticalMessage(0, "Unexpected type");
		I_CRITICAL();

		return;
	}

	stream << QStringLiteral("::imtbase::CTreeItemModel* ");
	if (!className.isEmpty()){
		stream << className << ':' << ':';
	}
	if (operationType == imtsdl::CSdlDocumentType::OT_GET_VIEW){
		stream << QStringLiteral("CreateInternalResponse");
	}
	else{
		stream << s_nonTrivialOperationMethodsMap[operationType];
	}
	stream << QStringLiteral("(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const");
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddRequiredIncludesForDocument(
			QTextStream& stream,
			const imtsdl::CSdlDocumentType& sdlDocumentType,
			uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// ");
	stream << sdlDocumentType.GetName();
	stream << QStringLiteral(" includes");
	FeedStream(stream, 1, false);

	const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operationsList = sdlDocumentType.GetOperationsList();
	for (const auto & operationIter : operationsList){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("#include \"C");
		stream << GetCapitalizedValue(operationIter.GetName());
		stream << QStringLiteral("GqlRequest.h\"");

		FeedStream(stream, 1, false);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodsForDocument(
			QTextStream& stream,
			const imtsdl::CSdlDocumentType& sdlDocumentType,
			uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// ");
	stream << sdlDocumentType.GetName();
	stream << QStringLiteral(" methods");
	FeedStream(stream, 1, false);

	const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operationsList = sdlDocumentType.GetOperationsList();

	QList<imtsdl::CSdlRequest> implementedGetRequests;

#if QT_VERSION >= 0x050500 && QT_VERSION < 0x060000
	QMapIterator operationsIter(operationsList);
#else
	QMultiMapIterator operationsIter(operationsList);
#endif

	imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(false);
	imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(false);

	while (operationsIter.hasNext()){
		auto operation = operationsIter.next();
		const imtsdl::CSdlRequest& sdlRequest = operation.value();
		imtsdl::CSdlDocumentType::OperationType operationType = operation.key();

		const QString documentClassName = CSdlGenTools::GetCollectionReferenceName(sdlDocumentType, typeList, unionList, operationType);

		if (operationType == imtsdl::CSdlDocumentType::OT_GET ||
			operationType == imtsdl::CSdlDocumentType::OT_LIST ||
			operationType == imtsdl::CSdlDocumentType::OT_UPDATE ||
			operationType == imtsdl::CSdlDocumentType::OT_INSERT)
		{
			if (!implementedGetRequests.contains(sdlRequest)){
				AddMethodForDocument(stream, operation.value(), operationType, documentClassName, sdlDocumentType, hIndents);
				implementedGetRequests << sdlRequest;
			}
		}
		else if (operationType != imtsdl::CSdlDocumentType::OT_UPDATE &&
				 operationType != imtsdl::CSdlDocumentType::OT_INSERT &&
				 operationType != imtsdl::CSdlDocumentType::OT_GET &&
				 operationType != imtsdl::CSdlDocumentType::OT_LIST &&
				 operationType != imtsdl::CSdlDocumentType::OT_DELETE &&
				 operationType != imtsdl::CSdlDocumentType::OT_ELEMENT_IDS &&
				 operationType != imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT)
		{
			AddMethodForDocument(stream, operation.value(), operationType, documentClassName, sdlDocumentType, hIndents);
		}
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodForDocument(
			QTextStream& stream,
			const imtsdl::CSdlRequest& sdlRequest,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const QString& itemClassName,
			const imtsdl::CSdlDocumentType& sdlDocumentType,
			uint hIndents) const
{
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		&sdlRequest.GetSchemaParams(),
		m_argumentParserCompPtr.GetPtr(),
		false);

	CStructNamespaceConverter structNameConverter;
	structNameConverter.relatedNamespace = sdlNamespace;
	structNameConverter.typeListProviderPtr = &*m_sdlTypeListCompPtr;
	structNameConverter.enumListProviderPtr = &*m_sdlEnumListCompPtr;
	structNameConverter.unionListProviderPtr = &*m_sdlUnionListCompPtr;
	structNameConverter.addVersion = true;

	imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(false);
	imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(false);
	auto referenceType = CSdlGenTools::GetCollectionReferenceForDocument(sdlDocumentType, typeList, unionList, operationType);
	QString documentClassName;
	if (referenceType){
		documentClassName = referenceType->GetName();
	}

	if (operationType == imtsdl::CSdlDocumentType::OT_GET ||
		operationType == imtsdl::CSdlDocumentType::OT_LIST)
	{
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual bool CreateRepresentationFromObject(");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const ");

		/// \todo do this for all versions
		if (operationType == imtsdl::CSdlDocumentType::OT_GET){
			stream << QStringLiteral("istd::IChangeable& data,");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << QStringLiteral("const C");
			stream << GetCapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("GqlRequest& ");
			stream << GetDecapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("Request,");
			FeedStream(stream, 1, false);

			imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
			structNameConverter.sdlFieldPtr = &outputArgument;
			structNameConverter.sdlEntryPtr = nullptr;

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << structNameConverter.GetString();
			stream << QStringLiteral("& representationPayload");
		}
		else {
			Q_ASSERT(referenceType);

			stream << QStringLiteral("::imtbase::IObjectCollectionIterator& objectCollectionIterator,");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << QStringLiteral("const C");
			stream << GetCapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("GqlRequest& ");
			stream << GetDecapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("Request,");
			FeedStream(stream, 1, false);

			structNameConverter.sdlEntryPtr = referenceType.get();
			structNameConverter.sdlFieldPtr = nullptr;

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << structNameConverter.GetString();
			stream << QStringLiteral("& representationObject");
		}
		stream << ',';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (operationType == imtsdl::CSdlDocumentType::OT_INSERT){
		Q_ASSERT(referenceType);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(");
		FeedStream(stream, 1, false);

		structNameConverter.sdlEntryPtr = referenceType.get();
		structNameConverter.sdlFieldPtr = nullptr;

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const ");
		stream << structNameConverter.GetString();
		stream << QStringLiteral("& ");
		stream << GetDecapitalizedValue(itemClassName);
		stream << QStringLiteral("Representation,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QByteArray& newObjectId,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual bool UpdateObjectFromRepresentationRequest(");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const ::imtgql::CGqlRequest& rawGqlRequest,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const C");
		stream << GetCapitalizedValue(sdlRequest.GetName());
		stream << QStringLiteral("GqlRequest& ");
		stream << GetDecapitalizedValue(sdlRequest.GetName());
		stream << QStringLiteral("Request,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("istd::IChangeable& object,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (		operationType == imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION ||
			 operationType == imtsdl::CSdlDocumentType::OT_RENAME ||
			 operationType == imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION ||
			 operationType == imtsdl::CSdlDocumentType::OT_HEADERS ||
			 operationType == imtsdl::CSdlDocumentType::OT_INFO ||
			 operationType == imtsdl::CSdlDocumentType::OT_METAINFO ||
			 operationType == imtsdl::CSdlDocumentType::OT_DATAMETAINFO ||
			 operationType == imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY ||
			 operationType == imtsdl::CSdlDocumentType::OT_IMPORT ||
			 operationType == imtsdl::CSdlDocumentType::OT_EXPORT ||
			 operationType == imtsdl::CSdlDocumentType::OT_GET_VIEW)
	{
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual ");
		AddMethodDeclarationForOperationType(stream, operationType, sdlRequest);
		stream << QStringLiteral(" = 0;");
		FeedStream(stream, 1, false);
	}

	else {
		SendCriticalMessage(0, QString("Unexpected request method: %1").arg(qPrintable(imtsdl::CSdlDocumentType::ToString(operationType))));
		I_CRITICAL();
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForSpecialRequest(
			QTextStream& stream,
			const imtsdl::CSdlRequest& sdlRequest,
			imtsdl::CSdlDocumentType::OperationType operationType,
			uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	QString functionName = s_nonTrivialOperationMethodsMap[operationType];
	if (functionName.isEmpty()){
		functionName = GetCapitalizedValue(sdlRequest.GetName());
	}
	stream << '/' << '/' << ' ' << functionName;
	FeedStream(stream, 1, false);

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);

	const QString requestClassName = sdlRequest.GetName() + QStringLiteral("GqlRequest");

	// [1] command ID check
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (commandId == C");
	stream << requestClassName;
	stream << QStringLiteral("::GetCommandId()){");
	FeedStream(stream, 1, false);

	// [1] create SDL request variable
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << 'C' <<  requestClassName;
	stream << ' ' << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral("(gqlRequest, ");
	if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		stream << QStringLiteral("true");
	}
	else {
		stream << QStringLiteral("false");
	}
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	// [1->2] SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(".IsValid()){");
	FeedStream(stream, 1, false);

	// [2] add error message if SDL request is not valid
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected request for command-ID: '%1'\").arg(qPrintable(commandId));");
	FeedStream(stream, 1, false);

	// [2] add log message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	CStructNamespaceConverter structNamespaceConverter(
		outputArgument,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		false);

	// [1] create payload variable by calling reimplemented method
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << structNamespaceConverter.GetString();
	stream << QStringLiteral(" replyPayload = On");
	stream << functionName << '(';
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(", errorMessage);");
	FeedStream(stream, 1, false);

	// [1->2] ensure, the derived call didn't return an error
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!errorMessage.isEmpty()){");
	FeedStream(stream, 1, false);

	// [2] send error message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, QString(\"The derived call [");
	stream << QStringLiteral("On") << functionName;
	stream << QStringLiteral("] returned an error: %1\").arg(errorMessage));");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of derived error checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	AddPayloadModelWriteCode(stream, sdlRequest, operationType, hIndents);

	// [2->1] end of payload write to TreeModel checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	// [1] return pop ptr
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return modelPtr.PopPtr();");
	FeedStream(stream, 1, false);

	// end of section
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 2, false);
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddPayloadModelWriteCode(
			QTextStream& stream,
			const imtsdl::CSdlRequest& sdlRequest,
			imtsdl::CSdlDocumentType::OperationType /*operationType*/,
			uint hIndents) const
{
	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();

	imtsdl::CSdlUnion foundUnion;
	const bool isUnion = GetSdlUnionForField(outputArgument, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
	if (isUnion){
		const static QString unionSourceVarName = QStringLiteral("&replyPayload");
		CSdlUnionConverter::WriteConversionFromUnion(stream,
													 foundUnion,
													 unionSourceVarName,
													 outputArgument.GetId(),
													 m_originalSchemaNamespaceCompPtr->GetText(),
													 outputArgument.GetId(),
													 QString(),
													 *m_sdlTypeListCompPtr,
													 *m_sdlEnumListCompPtr,
													 *m_sdlUnionListCompPtr,
													 hIndents,
													 CSdlUnionConverter::CT_MODEL_SCALAR,
													 QString(),
													 QString(),
													 QStringLiteral("false"));
	}
	else{
		// [1] write payload variable in model and create variable, to check if it success
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);");
		FeedStream(stream, 1, false);

		// [1->2] check if payload write to TreeModel is failed
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (!isModelCreated){");
		FeedStream(stream, 1, false);

		// [2] set error message
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("errorMessage = QString(\"Internal error. Unable to create response for command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// [2] add log message
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("SendCriticalMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// [2] return
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return nullptr;");
		FeedStream(stream, 1, false);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddOperationRequestMethodImplForDocument(
			QTextStream& stream,
			const imtsdl::CSdlDocumentType& sdlDocumentType) const
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");

	// add IsRequestSupported() method
	imtsdl::SdlRequestList requestList;
	imtsdl::SdlDocumentTypeList sdlDocumentTypeList = m_sdlDocumentListCompPtr->GetDocumentTypes(true);

	// add requests for operations only
	for (const imtsdl::CSdlDocumentType &documentType : sdlDocumentTypeList){
		const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operations = sdlDocumentType.GetOperationsList();

#if QT_VERSION >= 0x050500 && QT_VERSION < 0x060000
		QMapIterator operationIter(operations);
#else
		QMultiMapIterator operationIter(operations);
#endif
		while(operationIter.hasNext()){
			auto operationIterValue = operationIter.next();
			if (!requestList.contains(*operationIterValue)){
				requestList << *operationIterValue;
			}
		}

		imtsdl::SdlDocumentTypeList subtypes = documentType.GetSubtypes();
		for (const imtsdl::CSdlDocumentType& documentSubtype: subtypes){
			const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> suboperations = documentSubtype.GetOperationsList();
#if QT_VERSION >= 0x050500 && QT_VERSION < 0x060000
			QMapIterator suboperationIter(suboperations);
#else
			QMultiMapIterator suboperationIter(suboperations);
#endif			
			while(suboperationIter.hasNext()){
				auto suboperationIterValue = suboperationIter.next();
				if (!requestList.contains(*suboperationIterValue)){
					requestList << *suboperationIterValue;
				}
			}
		}
	}

	GenerateIsRequestSupportedMethodImpl(stream, requestList, className);
	FeedStream(stream, 2, false);

	// add GetOperationFromRequest() method
	stream << QStringLiteral("bool ");
	stream << className << ':' << ':';
	stream << QStringLiteral("GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
	FeedStream(stream, 2, false);

	AddOperationRequestCheck(stream, sdlDocumentType);

	imtsdl::SdlDocumentTypeList subtypes = sdlDocumentType.GetSubtypes();
	for (const imtsdl::CSdlDocumentType& documentSubtype: subtypes){
		AddOperationRequestCheck(stream, documentSubtype);
	}

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType);");
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddOperationRequestCheck(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const
{
	QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operations = sdlDocumentType.GetOperationsList();

	for (auto operationsIter = operations.cbegin(); operationsIter != operations.cend(); ++operationsIter){
		const QString requestClassName = operationsIter->GetName() + QStringLiteral("GqlRequest");

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (commandId == C");
		stream << requestClassName;
		stream << QStringLiteral("::GetCommandId()){");
		FeedStream(stream, 1, false);

		if (!s_operationsAliasList[operationsIter.key()].isEmpty()){
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("operationType =  ");
			stream << s_operationsAliasList[operationsIter.key()];
			stream << ';';
			FeedStream(stream, 2, false);
		}

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return true;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 2, false);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddOperationMapPairs(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const
{
	QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operations = sdlDocumentType.GetOperationsList();

	for (auto operationsIter = operations.cbegin(); operationsIter != operations.cend(); ++operationsIter){

		if (	!s_operationsAliasList.contains(operationsIter.key()) ||
			s_operationsAliasList[operationsIter.key()].isEmpty())
		{
			continue;
		}

		const QString requestClassName = operationsIter->GetName() + QStringLiteral("GqlRequest");

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("std::make_pair(");
		stream << s_operationsAliasList[operationsIter.key()];

		stream << QStringLiteral(", C");
		stream << requestClassName;
		stream << QStringLiteral("::GetCommandId()),");
		FeedStream(stream, 1, false);
	}
}


bool CGqlCollectionControllerBaseClassGeneratorComp::AddCollectionMethodsImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");

	QMultiMap<imtsdl::CSdlDocumentType::OperationType, ImplGenerationInfo> requestInfoMultiMap;
	QMultiMap requestMap(sdlDocumentType.GetOperationsList());
	imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(false);
	imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(false);

	for (auto mapIter = requestMap.cbegin(); mapIter != requestMap.cend(); ++mapIter){
		auto operationType = mapIter.key();
		const QString typeClassName = CSdlGenTools::GetCollectionReferenceName(sdlDocumentType, typeList, unionList, operationType);
		requestInfoMultiMap.insert(operationType, ImplGenerationInfo(mapIter.value(), typeClassName));
	}

	for (const imtsdl::CSdlDocumentType& documentType: sdlDocumentType.GetSubtypes()){
		QMultiMap subtypeRequestMap(documentType.GetOperationsList());
		for (auto mapIter = subtypeRequestMap.cbegin(); mapIter != subtypeRequestMap.cend(); ++mapIter){
			auto operationType = mapIter.key();
			const QString subtypeClassName = CSdlGenTools::GetCollectionReferenceName(documentType, typeList, unionList, operationType);
			requestInfoMultiMap.insert(operationType, ImplGenerationInfo(mapIter.value(), subtypeClassName));
		}
	}
	QList remainingOperations({
					imtsdl::CSdlDocumentType::OT_GET,
					imtsdl::CSdlDocumentType::OT_INSERT,
					imtsdl::CSdlDocumentType::OT_UPDATE,
					imtsdl::CSdlDocumentType::OT_LIST});

	for (imtsdl::CSdlDocumentType::OperationType operationType: requestInfoMultiMap.uniqueKeys()){
		const QList<ImplGenerationInfo> requestList = requestInfoMultiMap.values(operationType);
		bool isCorrect = AddImplCodeForRequests(stream, operationType, requestList, className, sdlDocumentType);
		if (!isCorrect){
			return false;
		}
		remainingOperations.removeAll(operationType);
	}

	return true;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForRequests(
			QTextStream& stream,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const QList<ImplGenerationInfo>& requestList,
			const QString& className,
			const imtsdl::CSdlDocumentType& sdlDocumentType,
			uint hIndents) const
{
	// declare method

	// a) type
	switch (operationType){
	case imtsdl::CSdlDocumentType::OT_GET:
	case imtsdl::CSdlDocumentType::OT_LIST:
	case imtsdl::CSdlDocumentType::OT_UPDATE:
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("bool ");
		break;
	case imtsdl::CSdlDocumentType::OT_INSERT:
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("istd::IChangeableUniquePtr ");
		break;
	case imtsdl::CSdlDocumentType::OT_DELETE:
	case imtsdl::CSdlDocumentType::OT_ELEMENT_IDS:
	case imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT:
		// nothing todo (only basic implementation)
	case imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION:
	case imtsdl::CSdlDocumentType::OT_RENAME:
	case imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION:
	case imtsdl::CSdlDocumentType::OT_HEADERS:
	case imtsdl::CSdlDocumentType::OT_INFO:
	case imtsdl::CSdlDocumentType::OT_METAINFO:
	case imtsdl::CSdlDocumentType::OT_DATAMETAINFO:
	case imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY:
	case imtsdl::CSdlDocumentType::OT_IMPORT:
	case imtsdl::CSdlDocumentType::OT_EXPORT:
	case imtsdl::CSdlDocumentType::OT_GET_VIEW:
		// special non-trivial methods
		return true;
		break;
	default:
		SendCriticalMessage(0, QString("Unexpected type: %1").arg(QString::number(operationType)));
		I_CRITICAL();
		break;
	}

	stream << className << ':' << ':';

	// b) name and argset
	switch (operationType){
	case imtsdl::CSdlDocumentType::OT_LIST:
		stream << QStringLiteral("SetupGqlItem(const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const");
		break;
	case imtsdl::CSdlDocumentType::OT_GET:
		stream << QStringLiteral("CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const");
		break;
	case imtsdl::CSdlDocumentType::OT_INSERT:
		stream << QStringLiteral("CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const");
		break;
	case imtsdl::CSdlDocumentType::OT_UPDATE:
		stream << QStringLiteral("UpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const");
		break;
	default:
		SendCriticalMessage(0, QString("Unexpected type: %1").arg(QString::number(operationType)));
		I_CRITICAL();
		break;
	}

	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '{';
	FeedStream(stream, 1, false);

	// checks for unused arguments
	if (operationType == imtsdl::CSdlDocumentType::OT_GET) {
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("Q_UNUSED(objectTypeId);");
		FeedStream(stream, 2, false);
	}

	// method's body
	// for list we must ensure, the iterator is not null
	if (operationType == imtsdl::CSdlDocumentType::OT_LIST){
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (objectCollectionIterator == nullptr){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Unable to create object iterator."), hIndents);

		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("}");
		FeedStream(stream, 2, false);
	}

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
	FeedStream(stream, 2, false);

	// create sections for expected command IDs
	for (const ImplGenerationInfo& sdlRequest: requestList){
		/// \todo do this for all versions
		bool isCorrect = AddImplCodeForRequest(stream, sdlRequest, operationType, sdlDocumentType, hIndents + 1);
		if (!isCorrect){
			return false;
		}
	}

	switch (operationType){
	case imtsdl::CSdlDocumentType::OT_LIST:
	case imtsdl::CSdlDocumentType::OT_GET:
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 2, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		break;
	case imtsdl::CSdlDocumentType::OT_INSERT:
		// create default section
		// add error message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return nullptr;");
		break;
	case imtsdl::CSdlDocumentType::OT_UPDATE:
		// unexpected section
		// add error message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// return after error report
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		break;
	default:
		break;
	}
	FeedStream(stream, 1, false);

	// end of method
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 3, false);

	return true;
}



void CGqlCollectionControllerBaseClassGeneratorComp::AddSpecialMethodImplCodeForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType) const
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");

	QMultiMap<imtsdl::CSdlDocumentType::OperationType, ImplGenerationInfo> requestInfoMultiMap;
	QMultiMap requestMap(sdlDocumentType.GetOperationsList());
	imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(false);
	imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(false);

	for (auto mapIter = requestMap.cbegin(); mapIter != requestMap.cend(); ++mapIter){
		auto operationType = mapIter.key();
		if (s_nonTrivialOperationMethodsMap.contains(operationType)){
			const QString typeClassName = CSdlGenTools::GetCollectionReferenceName(sdlDocumentType, typeList, unionList, operationType);
			requestInfoMultiMap.insert(operationType, ImplGenerationInfo(mapIter.value(), typeClassName));
		}
	}

	for (const imtsdl::CSdlDocumentType& documentType: sdlDocumentType.GetSubtypes()){
		QMultiMap subtypeRequestMap(documentType.GetOperationsList());
		for (auto mapIter = subtypeRequestMap.cbegin(); mapIter != subtypeRequestMap.cend(); ++mapIter){
			auto operationType = mapIter.key();
			if (s_nonTrivialOperationMethodsMap.contains(operationType)){
				const QString subtypeClassName = CSdlGenTools::GetCollectionReferenceName(documentType, typeList, unionList, operationType);
				requestInfoMultiMap.insert(operationType, ImplGenerationInfo(mapIter.value(), subtypeClassName));
			}
		}
	}

	QList remainingOperations(requestInfoMultiMap.uniqueKeys());
	QMutableListIterator remainingOperationsIter(remainingOperations);
	while(remainingOperationsIter.hasNext()){
		imtsdl::CSdlDocumentType::OperationType operationType = remainingOperationsIter.next();
		const QList<ImplGenerationInfo> requestList = requestInfoMultiMap.values(operationType);
		AddSpecialMethodImplCode(stream, operationType, requestList, className);
		remainingOperationsIter.remove();
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddSpecialMethodImplCode(
			QTextStream& stream,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const QList<ImplGenerationInfo>& requestList,
			const QString& className,
			uint hIndents) const
{
	// declare method
	AddBaseMethodDeclarationForOperationType(stream, operationType, className);
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	// declare and extract command ID
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const QString commandId = gqlRequest.GetCommandId();");
	FeedStream(stream, 1, false);


	/// create a carrier model GQL spec	\link https://spec.graphql.org/draft/#sec-Response-Format
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);");
	FeedStream(stream, 1, false);

	/// \todo add errors model and don't forget to fill it	\link https://spec.graphql.org/draft/#sec-Errors

	// create dataModel - child of a carrier model
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel(\"data\");");
	FeedStream(stream, 2, false);

	for (const ImplGenerationInfo& requestInfo: requestList){
		AddImplCodeForSpecialRequest(stream, requestInfo.request, operationType, hIndents);
	}

	// create default section

	// if get view => we in 'create internal response', => we MUST call base class method
	if (operationType == imtsdl::CSdlDocumentType::OT_GET_VIEW){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("return BaseClass::CreateInternalResponse(gqlRequest, errorMessage);");
		FeedStream(stream, 1, false);
	}

	// otherwise we MUST generate an error message
	else{
		// add error message
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// return
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("return nullptr;");
		FeedStream(stream, 1, false);
	}

	stream << '}';
	FeedStream(stream, 3, false);
}


bool CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForRequest(
			QTextStream& stream,
			const ImplGenerationInfo& sdlRequestInfo,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const imtsdl::CSdlDocumentType& sdlDocumentType,
			uint hIndents) const
{
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		&sdlRequestInfo.request.GetSchemaParams(),
		m_argumentParserCompPtr.GetPtr(),
		false);

	imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr->GetSdlTypes(false);
	imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(false);
	auto referenceType = CSdlGenTools::GetCollectionReferenceForDocument(sdlDocumentType, typeList, unionList, operationType);
	if (!referenceType){
		SendErrorMessage(0, QString("Unable to get collection reference for document %1").arg(sdlDocumentType.GetName()));

		return false;
	}

	CStructNamespaceConverter structNameConverter(
		*referenceType,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		false);
	structNameConverter.addVersion = true;

	imtsdl::CSdlField outputArgument = sdlRequestInfo.request.GetOutputArgument();
	CStructNamespaceConverter getStructNameConverter(
		outputArgument,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		false);
	getStructNameConverter.addVersion = true;

	FeedStreamHorizontally(stream, hIndents);
	stream << '/' << '/' << sdlRequestInfo.request.GetName();
	FeedStream(stream, 1, false);

	const QString requestClassName = sdlRequestInfo.request.GetName() + QStringLiteral("GqlRequest");

	/// \todo fix horizontal/vertical feed
	// [1] command ID check
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (commandId == C");
	stream << requestClassName;
	stream << QStringLiteral("::GetCommandId()){");
	FeedStream(stream, 1, false);

	// [1] create SDL request variable
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << 'C' <<  requestClassName;
	stream << ' ' << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral("(gqlRequest, ");
	if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		stream << QStringLiteral("true");
	}
	else {
		stream << QStringLiteral("false");
	}
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	// GET
	if (	operationType == imtsdl::CSdlDocumentType::OT_GET ||
		operationType == imtsdl::CSdlDocumentType::OT_LIST)
	{
		FeedStreamHorizontally(stream, hIndents + 1);
		if (operationType == imtsdl::CSdlDocumentType::OT_GET){
			stream << getStructNameConverter.GetString();
			stream << QStringLiteral(" representationObject;");
		}
		else {
			stream << structNameConverter.GetString();
			stream << QStringLiteral(" representationObject;");
		}

		FeedStream(stream, 1, false);

		// [1] create check variable
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const bool isRepresentationCreated = CreateRepresentationFromObject(");
		if (operationType == imtsdl::CSdlDocumentType::OT_GET){
			stream << QStringLiteral("data, ");
		}
		else {
			stream << QStringLiteral("*objectCollectionIterator, ");
		}
		stream << GetDecapitalizedValue(sdlRequestInfo.request.GetName());
		stream << QStringLiteral("GqlRequest, representationObject, errorMessage);");
		FeedStream(stream, 1, false);

		// [1->2] checks validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (!isRepresentationCreated){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Unable to create representation"), hIndents);

		// [2] return
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		// [2->1] end of checks validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 2, false);

		imtsdl::CSdlUnion foundUnion;
		const bool isUnion = GetSdlUnionForField(outputArgument, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
		if (isUnion){
			const static QString unionSourceVarName = QStringLiteral("&representationObject");
			CSdlUnionConverter::WriteConversionFromUnion(stream,
														 foundUnion,
														 unionSourceVarName,
														 outputArgument.GetId(),
														 m_originalSchemaNamespaceCompPtr->GetText(),
														 QStringLiteral("representationObject"),
														 QString(),
														 *m_sdlTypeListCompPtr,
														 *m_sdlEnumListCompPtr,
														 *m_sdlUnionListCompPtr,
														 hIndents + 1,
														 CSdlUnionConverter::CT_MODEL_SCALAR,
														 QStringLiteral("dataModel.SetData("), //QString(),
														 QStringLiteral("dataModel"),
														 QStringLiteral("false"));
		}
		else{
			// [1] create write check variable
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("const bool isRepresentationWritten = ");

			stream << QStringLiteral("representationObject.WriteToModel(dataModel");
			// [-||-] add index for list
			if (operationType == imtsdl::CSdlDocumentType::OT_LIST){
				stream << QStringLiteral(", itemIndex");
			}
			stream << QStringLiteral(");");
			FeedStream(stream, 1, false);

			// [1->2] checks write validate
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("if (!isRepresentationWritten){");
			FeedStream(stream, 1, false);

			AddErrorReport(stream, "Unable to Write TreeModel", hIndents + 2);

			// [2] return
			FeedStreamHorizontally(stream, hIndents + 2);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			// [2->1] end of write validate
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << '}';
			FeedStream(stream, 1, false);
		}


		FeedStream(stream, 1, false);
		// [1] error respond
		FeedStreamHorizontally(stream, hIndents + 1);
		stream <<  QStringLiteral("return true;");
		FeedStream(stream, 1, false);
	}

	// UPDATE
	else if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		// [1] return
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return UpdateObjectFromRepresentationRequest(gqlRequest, ");
		stream << GetDecapitalizedValue(requestClassName);
		stream << QStringLiteral(", object, errorMessage);");
		FeedStream(stream, 1, false);
	}
	// INSERT
	else if (operationType == imtsdl::CSdlDocumentType::OT_INSERT)
	{
		// [1->2] SDL request validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (!");
		stream << GetDecapitalizedValue(requestClassName);
		stream << QStringLiteral(".IsValid()){");
		FeedStream(stream, 1, false);

		// [2] add error message if SDL request is not valid
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected request for command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// [2] add log message
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// [2] return
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return nullptr;");
		FeedStream(stream, 1, false);

		// [2->1] end of SDL request validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 2, false);

		bool isCorrect = false;
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return CreateObjectFromRepresentation(*");
		stream << GetDecapitalizedValue(requestClassName);
		stream << GetInputExtractionStringForTypeName(
			sdlRequestInfo.request,
			sdlRequestInfo.containerClassName,
			QStringLiteral("Version_") + GetSdlEntryVersion(*referenceType, false),& isCorrect);
		stream << QStringLiteral(", newObjectId, errorMessage);");
		FeedStream(stream, 1, false);
		if (!isCorrect){
			return false;
		}
	}

	// [1] end of section
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 2, false);

	return true;
}


QString CGqlCollectionControllerBaseClassGeneratorComp::GetInputExtractionStringForTypeName(
			const imtsdl::CSdlRequest& sdlRequest,
			const QString& typeName,
			const QString& version,
			bool* okPtr) const
{
	QString retVal = QStringLiteral(".GetRequestedArguments().");

	imtsdl::SdlFieldList requestArguments = sdlRequest.GetInputArguments();

	for (const imtsdl::CSdlField& sdlField: requestArguments){
		QString callChain;
		if (FindCallChainForField(sdlField, typeName, callChain)){
			retVal.append(sdlField.GetId());
			retVal.append('.');
			if (!version.isEmpty()){
				retVal.append(version);
			}
			if (!callChain.isEmpty()){
				retVal.append('-');
				retVal.append('>');
				retVal.append(callChain);
			}

			if (okPtr != nullptr){
				*okPtr = true;
			}

			return retVal;
		}
	}

	if (okPtr != nullptr){
		*okPtr = false;
	}

	SendErrorMessage(0, QString("'collectionSchema' section processing error: Unable to find reference name '%1' in input arguments for '%2'").arg(sdlRequest.GetName(), typeName));

	return {};
}


bool CGqlCollectionControllerBaseClassGeneratorComp::FindCallChainForField(const imtsdl::CSdlField& aSdlField, const QString& typeName, QString& callChain, bool _isRoot) const
{
	bool isCustom = false;
	ConvertType(aSdlField, &isCustom);
	if (!isCustom){
		return false;
	}

	std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = FindEntryByName(
		aSdlField.GetType(),
		m_sdlTypeListCompPtr->GetSdlTypes(false),
		m_sdlEnumListCompPtr->GetEnums(false),
		m_sdlUnionListCompPtr->GetUnions(false));

	Q_ASSERT(foundEntry);

	// special case if a ref is the required type
	if (_isRoot && aSdlField.GetType() == typeName){

		return true;
	}

	auto sdlTypePtr = dynamic_cast<imtsdl::CSdlType*>(foundEntry.get());
	if (sdlTypePtr != nullptr){
		for (const imtsdl::CSdlField& sdlField: sdlTypePtr->GetFields()){
			if (sdlField.GetType() == typeName || FindCallChainForField(sdlField, typeName, callChain, false)){
				callChain.prepend(sdlField.GetId());

				// add '->' if reference is inside other object, because they are "pointers"/optional
				if (!_isRoot){
					callChain.prepend('>');
					callChain.prepend('-');
				}

				return true;
			}
		}
	}

	if (aSdlField.GetType() == typeName){
		return true;
	}

	return false;
}


} // namespace imtsdlgencpp

