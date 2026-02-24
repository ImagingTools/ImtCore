// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial

#include "imtsdl/CSdlEntryBase.h"
#include <imtsdlgencpp/CSdlGenTools.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdl/CSdlDocumentType.h>
#include <memory>
#include <optional>


namespace imtsdlgencpp
{

// public methods of embedded CStructNamespaceConverter class


CSdlGenTools::CStructNamespaceConverter::CStructNamespaceConverter()
{
}


CSdlGenTools::CStructNamespaceConverter::CStructNamespaceConverter(
			const imtsdl::CSdlEntryBase& aSdlEntry,
			const QString& aRelatedNamespace,
			const imtsdl::ISdlTypeListProvider& aListProvider,
			const imtsdl::ISdlEnumListProvider& aEnumListProvider,
			const imtsdl::ISdlUnionListProvider& aUnionListProvider,
			bool aListWrap):
	typeListProviderPtr(&aListProvider),
	enumListProviderPtr(&aEnumListProvider),
	unionListProviderPtr(&aUnionListProvider),
	relatedNamespace(aRelatedNamespace),
	listWrap(aListWrap),
	sdlEntryPtr(&aSdlEntry)
{
}


CSdlGenTools::CStructNamespaceConverter::CStructNamespaceConverter(
			const imtsdl::CSdlField& aSdlField,
			const QString& aRelatedNamespace,
			const imtsdl::ISdlTypeListProvider& aListProvider,
			const imtsdl::ISdlEnumListProvider& aEnumListProvider,
			const imtsdl::ISdlUnionListProvider& aUnionListProvider,
			bool aListWrap):
	typeListProviderPtr(&aListProvider),
	enumListProviderPtr(&aEnumListProvider),
	unionListProviderPtr(&aUnionListProvider),
	relatedNamespace(aRelatedNamespace),
	listWrap(aListWrap),
	sdlFieldPtr(&aSdlField)
{
}


QString CSdlGenTools::CStructNamespaceConverter::GetString() const
{
	if (typeListProviderPtr == nullptr){
		Q_ASSERT_X(false, "typelist is not provided", __func__);

		return QString();
	}

	if (sdlEntryPtr == nullptr && sdlFieldPtr == nullptr){
		Q_ASSERT_X(false, "sdlEntryPtr or sdlFieldPtr is not provided", __func__);

		return QString();
	}

	if (!((sdlEntryPtr == nullptr) ^ (sdlFieldPtr == nullptr))){
		Q_ASSERT_X(false, "sdlEntryPtr AND sdlFieldPtr are both provided. You MUST provide only ONE!", __func__);

		return QString();
	}

	QString retVal;
	const imtsdl::CSdlEntryBase* namespaceEntryPtr = sdlEntryPtr;
	std::shared_ptr<imtsdl::CSdlEntryBase> typeForFieldPtr;

	if (sdlFieldPtr != nullptr){
		typeForFieldPtr = imtsdl::CSdlTools::GetSdlTypeOrEnumOrUnionForField(
			*sdlFieldPtr,
			typeListProviderPtr->GetSdlTypes(false),
			enumListProviderPtr->GetEnums(false),
			unionListProviderPtr->GetUnions(false));
		if (!typeForFieldPtr){
			qCritical() << "Unable to find entry or type for" << sdlFieldPtr->GetId() << "of" << sdlFieldPtr->GetType();
			I_CRITICAL();

			return QString();
		}
		namespaceEntryPtr = typeForFieldPtr.get();
	}

	const auto* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(namespaceEntryPtr);
	if (sdlTypePtr != nullptr){
		retVal += 'C';
		retVal +=  imtsdl::CSdlTools::GetCapitalizedValue(sdlTypePtr->GetName());
	}

	const auto* sdlDocumentTypePtr = dynamic_cast<const imtsdl::CSdlDocumentType*>(namespaceEntryPtr);
	if (sdlDocumentTypePtr != nullptr){
		retVal += 'C';
		retVal += imtsdl::CSdlTools::GetCapitalizedValue(sdlDocumentTypePtr->GetName());
	}

	const auto* sdlRequestPtr = dynamic_cast<const imtsdl::CSdlRequest*>(namespaceEntryPtr);
	if (sdlRequestPtr != nullptr){

		retVal += imtsdl::CSdlTools::GetCapitalizedValue(sdlRequestPtr->GetName());
		retVal += QStringLiteral("GqlRequest");
	}

	const auto* sdlEnumPtr = dynamic_cast<const imtsdl::CSdlEnum*>(namespaceEntryPtr);
	if (sdlEnumPtr != nullptr){
		retVal += imtsdl::CSdlTools::GetCapitalizedValue(sdlEnumPtr->GetName());
	}

	const auto* sdlUnionPtr = dynamic_cast<const imtsdl::CSdlUnion*>(namespaceEntryPtr);
	if (sdlUnionPtr != nullptr){
		retVal += imtsdl::CSdlTools::GetCapitalizedValue(sdlUnionPtr->GetName());
	}
	// versions does NOT exist for enumerators and unions
	if ((sdlEnumPtr == nullptr && sdlUnionPtr == nullptr) && addVersion){
		retVal += QStringLiteral("::");
		retVal += GetSdlEntryVersion(*namespaceEntryPtr);
	}

	if (!relatedNamespace.isEmpty()){
		QString typeNamespace = GetNamespaceFromSchemaParams(namespaceEntryPtr->GetSchemaParams());
		if (typeNamespace != relatedNamespace){

			bool namespaceCleaned = false;
			// clean namespace
			if (cleanNamespace){
				QStringList relatedNamespaceParts = relatedNamespace.split(QStringLiteral("::"));
				QMutableListIterator relatedNamespacePartsIter(relatedNamespaceParts);
				while (relatedNamespacePartsIter.hasNext()){
					QString& value = relatedNamespacePartsIter.next();
					if (value.isEmpty()){
						relatedNamespacePartsIter.remove();
					}
				}

				QStringList typeNamespaceParts = typeNamespace.split(QStringLiteral("::"));
				QMutableListIterator typeNamespacePartsIter(typeNamespaceParts);
				while (typeNamespacePartsIter.hasNext()){
					QString& value = typeNamespacePartsIter.next();
					if (value.isEmpty()){
						typeNamespacePartsIter.remove();
					}
				}

				// lookup for same parts from begin
				relatedNamespacePartsIter.toFront();
				typeNamespacePartsIter.toFront();
				while (relatedNamespacePartsIter.hasNext() && typeNamespacePartsIter.hasNext()){
					QString relatedNamespacePart = relatedNamespacePartsIter.next();
					QString typeNamespacePart = typeNamespacePartsIter.next();
					if (relatedNamespacePart != typeNamespacePart){
						// ALL parts in same order MUST be equal
						break;
					}
					namespaceCleaned = true;
					typeNamespacePartsIter.remove();
				}
				typeNamespace = typeNamespaceParts.join(QStringLiteral("::")) + QStringLiteral("::");

				retVal.prepend(typeNamespace);
			}
			// use global namespace
			if (!namespaceCleaned && !retVal.startsWith(QStringLiteral("::"))){
				retVal.prepend(QStringLiteral("::"));
			}
		}
	}

	if (sdlFieldPtr != nullptr && listWrap && sdlFieldPtr->IsArray()){
		imtsdl::CSdlTools::WrapTypeToList(retVal);
	}

	return retVal;
}


// public static methods

QString CSdlGenTools::GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix)
{
	QString retVal;
	if (!prefix.isEmpty()){
		retVal += prefix;
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::");
		}
	}

	QString schemaNamespace;
	iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(&schemaParams, imtsdl::SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
	if (namespaceParamPtr.IsValid()){
		schemaNamespace = namespaceParamPtr->GetText();
	}
	if (!schemaNamespace.isEmpty()){
		retVal += schemaNamespace;
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::");
		}
	}

	QString schemaName;
	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, imtsdl::SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		schemaName = nameParamPtr->GetText();
	}
	if (!schemaName.isEmpty()){
		retVal += schemaName;
	}

	return imtsdl::CSdlTools::GetNamespaceAcceptableString(retVal);
}


QString CSdlGenTools::GetNamespaceFromTypeSchemaParams(const imtsdl::CSdlType& type, const QString& prefix)
{
	return GetNamespaceFromSchemaParams(type.GetSchemaParams(), prefix);
}


QString CSdlGenTools::GetSchemaVerstionString(const iprm::IParamsSet& schemaParams, bool addVPrefix)
{
	QString retVal;
	if (addVPrefix){
		retVal += QStringLiteral("V");
	}

	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, imtsdl::SdlCustomSchemaKeys::VersionName.toUtf8());
	if (versionNameParamPtr.IsValid()){
		QString versionName = versionNameParamPtr->GetText();
		retVal += versionName;
	}
	else {
		retVal += QStringLiteral("0");
	}

	static QRegularExpression nonWordRegexp("[^\\w]");
	retVal.replace(nonWordRegexp, QStringLiteral("_"));

	return retVal;
}


QString CSdlGenTools::OptListConvertTypeWithNamespaceStruct(
	const imtsdl::CSdlField& sdlField,
	const QString& relatedNamespace,
	const imtsdl::ISdlTypeListProvider& listProvider,
	const imtsdl::ISdlEnumListProvider& enumlistProvider,
	const imtsdl::ISdlUnionListProvider& unionlistProvider,
	bool listWrap,
	bool* isCustomPtr,
	bool* isComplexPtr,
	bool* isArrayPtr,
	bool* isEnumPtr,
	bool* isUnionPtr,
	const bool& addVersion)
{
	bool _isCustom = false;
	QString retVal = imtsdl::CSdlTools::ConvertTypeOrEnumOrUnion(sdlField, enumlistProvider.GetEnums(false), unionlistProvider.GetUnions(false), &_isCustom, isComplexPtr, isArrayPtr, isEnumPtr, isUnionPtr);
	if (isCustomPtr != nullptr){
		*isCustomPtr = _isCustom;
	}
	if (isArrayPtr != nullptr){
		*isArrayPtr = sdlField.IsArray();
	}

	// no processing is required if a type is scalar
	if (!_isCustom){
		if (listWrap && sdlField.IsArray()){
			imtsdl::CSdlTools::WrapTypeToList(retVal);
		}

		return retVal;
	}

	CStructNamespaceConverter converter(sdlField, relatedNamespace, listProvider, enumlistProvider, unionlistProvider, listWrap);
	converter.addVersion = addVersion;

	return converter.GetString();
}


QString CSdlGenTools::GetNullCheckString(const imtsdl::CSdlField& sdlField, bool checkNull, const QString& objectName)
{
	QString retVal;
	if (checkNull){
		retVal += '!';
	}
	if (!objectName.isEmpty()){
		retVal = objectName + '.';
	}

	retVal += sdlField.GetId();
	return retVal;
}


QString CSdlGenTools::GetSettingValueString(
			const imtsdl::CSdlField& sdlField,
			const QString& /*relatedNamespace*/,
			const imtsdl::ISdlTypeListProvider& /*listProvider*/,
			const QString& variableName,
			const QString& objectName)
{
	QString retVal;

	if (!objectName.isEmpty()){
		retVal += objectName + '.';
	}

	retVal += sdlField.GetId();
	retVal += QStringLiteral(" = ");
	retVal += variableName;
	retVal += QStringLiteral(";");

	return retVal;
}


void CSdlGenTools::AddArrayInternalChecksFail(QTextStream& stream, const imtsdl::CSdlField& field, bool checkEmpty, const QString& objectName, uint hIndents)
{
	imtsdl::CSdlTools::FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (");
	stream << GetNullCheckString(field, true);
	if (checkEmpty){
		stream << QStringLiteral(" || ");
		if (!objectName.isEmpty()){
			stream << objectName;
			stream << '.';
		}
		stream << field.GetId();
		stream << QStringLiteral("->isEmpty()");
	}
	stream << QStringLiteral("){");
	imtsdl::CSdlTools::FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Field: '%3' doesn't exist, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));
	
	imtsdl::CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;\n\t}");
	imtsdl::CSdlTools::FeedStream(stream, 1, false);
}


void CSdlGenTools::AddErrorReport(
			QTextStream& stream,
			const QString& errorMessage,
			uint hIndents,
			const QStringList& argset,
			bool onlyDebug)
{
	imtsdl::CSdlTools::FeedStreamHorizontally(stream, hIndents);

	if (onlyDebug){
		stream << QStringLiteral("I_IF_DEBUG(");
	}

	stream << QStringLiteral("qWarning() << QString(\"%1:%2 Error: ");
	stream << errorMessage;
	stream << QStringLiteral("\")");
	stream << QStringLiteral(".arg(__FILE__, QString::number(__LINE__)");
	for (const QString& argument: argset){
		stream << ',' << ' ' << argument;
	}
	stream << QStringLiteral(").toLocal8Bit().constData();");

	if (onlyDebug){
		stream << ')';
	}

	imtsdl::CSdlTools::FeedStream(stream, 2, false);
}


void CSdlGenTools::GenerateIsRequestSupportedMethodImpl(
			QTextStream& stream,
			const imtsdl::SdlRequestList& requestList,
			const QString& className)
{
	stream << QStringLiteral("bool ");
	stream << className;
	stream << ("::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const");
	imtsdl::CSdlTools::FeedStream(stream);

	stream << '{';
	imtsdl::CSdlTools::FeedStream(stream);

	imtsdl::CSdlTools::FeedStreamHorizontally(stream);
	stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
	imtsdl::CSdlTools::FeedStream(stream, 1, false);

	bool isFirst = true;
	for (const imtsdl::CSdlRequest& sdlRequest: requestList){
		const QString requestClassName = sdlRequest.GetName() + QStringLiteral("GqlRequest");

		imtsdl::CSdlTools::FeedStreamHorizontally(stream);
		if (!isFirst){
			stream << QStringLiteral("else ");
		}
		else {
			isFirst = false;
		}
		stream << QStringLiteral("if (commandId == C");
		stream << requestClassName;
		stream << QStringLiteral("::GetCommandId()){");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);

		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return true;");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);

		imtsdl::CSdlTools::FeedStreamHorizontally(stream);
		stream << '}';
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
	}
	imtsdl::CSdlTools::FeedStream(stream, 1, false);
	
	// default call

	imtsdl::CSdlTools::FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::IsRequestSupported(gqlRequest);");
	imtsdl::CSdlTools::FeedStream(stream, 1, false);

	stream << '}';
	imtsdl::CSdlTools::FeedStream(stream, 1, false);
}


QString CSdlGenTools::GetQObjectTypeName(const imtsdl::CSdlField& sdlField,
			const imtsdl::SdlTypeList& typeList,
			const imtsdl::SdlEnumList& enumList,
			const imtsdl::SdlUnionList& unionList,
			bool withPointer,
			bool treatArrayAsElement)
{
	QString retVal = "";
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;

	const QString convertedType = imtsdl::CSdlTools::ConvertTypeOrEnumOrUnion(sdlField, enumList, unionList, &isCustom, nullptr, &isArray, &isEnum, &isUnion);
	std::shared_ptr<imtsdl::CSdlEntryBase> sdlEntryBase = imtsdl::CSdlTools::GetSdlTypeOrEnumOrUnionForField(
		sdlField,
		typeList,
		enumList,
		unionList);

	QString sdlNamespace;
	if (sdlEntryBase != nullptr){
		sdlNamespace = GetNamespaceFromSchemaParams(sdlEntryBase->GetSchemaParams());
	}

	if (!isCustom || isEnum){
		// For primitive types and enums
		if (isArray && !treatArrayAsElement){
			retVal = QStringLiteral("QList<");
		}

		if (sdlField.GetType() == "String" || isEnum){
			retVal += QStringLiteral("QString");
		}
		else if (sdlField.GetType() == "Integer" || sdlField.GetType() == "Int"){
			retVal += QStringLiteral("int");
		}
		else if (sdlField.GetType() == "Double" || sdlField.GetType() == "Float"){
			retVal += QStringLiteral("double");
		}
		else if (sdlField.GetType() == "Boolean" || sdlField.GetType() == "Bool"){
			retVal += QStringLiteral("bool");
		}
		else if (sdlField.GetType() == "LongLong" || sdlField.GetType() == "longLong"){
			retVal += QStringLiteral("int");
		}
		else if (sdlField.GetType() == "ID"){
			retVal += QStringLiteral("QString");
		}
		else {
			Q_ASSERT_X(false, "CSdlGenTools::GetQObjectTypeName", sdlField.GetType().toUtf8() + " field.GetType() not implemented");
		}

		if (isArray && !treatArrayAsElement){
			retVal += QStringLiteral(">");
		}
	}
	else if(isArray && !treatArrayAsElement){
		// For custom array types, return ObjectList
		retVal = sdlNamespace;
		retVal += QStringLiteral("::C") + sdlField.GetType();
		retVal += QStringLiteral("ObjectList");

		if (withPointer){
			retVal += "*";
		}
	}
	else if(isUnion){
		retVal = "QVariant";
	}
	else{
		// For custom non-array types, or when treating arrays as elements, return Object
		retVal = sdlNamespace;
		retVal += QStringLiteral("::C") + sdlField.GetType();
		retVal += QStringLiteral("Object");

		if (withPointer){
			retVal += "*";
		}
	}

	return retVal;
}


QString CSdlGenTools::GetTempVariableWrappedValue(const QString& variableName)
{
	return QStringLiteral("t_%1").arg(imtsdl::CSdlTools::GetDecapitalizedValue(variableName));
}

std::shared_ptr<imtsdl::CSdlEntryBase> CSdlGenTools::GetCollectionReferenceForDocument(
			const imtsdl::CSdlDocumentType& documentType,
			const imtsdl::SdlTypeList& typeList,
			const imtsdl::SdlUnionList& unionList,
			imtsdl::CSdlDocumentType::OperationType operationType)
{
	using namespace imtsdl;

	if (!documentType.HasRequest(operationType)) {
		return {};
	}

	CSdlRequest request = documentType.GetRequest(operationType);
	std::optional<CSdlField> fieldForType;
	switch (operationType) {
		case CSdlDocumentType::OT_GET:
		case CSdlDocumentType::OT_LIST:
		case CSdlDocumentType::OT_INSERT:
		case CSdlDocumentType::OT_UPDATE:
		
		case CSdlDocumentType::OT_GET_VIEW:
		case CSdlDocumentType::OT_DELETE:
		case CSdlDocumentType::OT_UPDATE_COLLECTION:
		case CSdlDocumentType::OT_RENAME:
		case CSdlDocumentType::OT_SET_DESCRIPTION:
		case CSdlDocumentType::OT_HEADERS:
		case CSdlDocumentType::OT_INFO:
		case CSdlDocumentType::OT_METAINFO:
		case CSdlDocumentType::OT_DATAMETAINFO:
		case CSdlDocumentType::OT_ELEMENTS_COUNT:
		case CSdlDocumentType::OT_ELEMENT_IDS:
		case CSdlDocumentType::OT_ELEMENT_HISTORY:
		case CSdlDocumentType::OT_IMPORT:
		case CSdlDocumentType::OT_EXPORT:
		default:
			qWarning() << "Operation type not implemented";
			break;
		}

	if (!fieldForType){
		return {};
	}

	auto typeForField =  CSdlTools::GetSdlTypeOrEnumOrUnionForField(
		*fieldForType,
		typeList,
		{},
		unionList);

	return typeForField;
}


} // namespace imtsdlgencpp
