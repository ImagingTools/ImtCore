#include <imtsdlgenv2/CSdlGenTools.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdl/CSdlDocumentType.h>


namespace imtsdlgenv2
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
	if (!relatedNamespace.isEmpty()){

		const imtsdl::CSdlEntryBase* namespaceEntryPtr = sdlEntryPtr;
		std::shared_ptr<imtsdl::CSdlEntryBase> typeForFieldPtr;

		QString typeNamespace;
		if (sdlFieldPtr != nullptr){
			typeForFieldPtr = imtsdl::CSdlTools::GetSdlTypeOrEnumOrUnionForField(
				*sdlFieldPtr,
				typeListProviderPtr->GetSdlTypes(false),
				enumListProviderPtr->GetEnums(false),
				unionListProviderPtr->GetUnions(false));
			if (!typeForFieldPtr){
				qCritical() << "Unable to find enum or type for" << sdlFieldPtr->GetId() << "of" << sdlFieldPtr->GetType();
				I_CRITICAL();

				return QString();
			}
			namespaceEntryPtr = typeForFieldPtr.get();
		}

		typeNamespace = GetNamespaceFromSchemaParams(namespaceEntryPtr->GetSchemaParams());

		const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(namespaceEntryPtr);
		if (sdlTypePtr != nullptr){
			typeNamespace += QStringLiteral("::C") + imtsdl::CSdlTools::GetCapitalizedValue(sdlTypePtr->GetName());
		}

		const imtsdl::CSdlDocumentType* sdlDocumentTypePtr = dynamic_cast<const imtsdl::CSdlDocumentType*>(namespaceEntryPtr);
		if (sdlDocumentTypePtr != nullptr){
			typeNamespace += QStringLiteral("::C") + imtsdl::CSdlTools::GetCapitalizedValue(sdlDocumentTypePtr->GetName());
		}

		const imtsdl::CSdlRequest* sdlRequestPtr = dynamic_cast<const imtsdl::CSdlRequest*>(namespaceEntryPtr);
		if (sdlRequestPtr != nullptr){
			typeNamespace += QStringLiteral("::C") + imtsdl::CSdlTools::GetCapitalizedValue(sdlRequestPtr->GetName());
			typeNamespace += QStringLiteral("GqlRequest");
		}

		const imtsdl::CSdlEnum* sdlEnumPtr = dynamic_cast<const imtsdl::CSdlEnum*>(namespaceEntryPtr);
		if (sdlEnumPtr != nullptr){
			typeNamespace += QStringLiteral("::");
			typeNamespace += imtsdl::CSdlTools::GetCapitalizedValue(sdlEnumPtr->GetName());
		}

		const imtsdl::CSdlUnion* sdlUnionPtr = dynamic_cast<const imtsdl::CSdlUnion*>(namespaceEntryPtr);
		if (sdlUnionPtr != nullptr){
			typeNamespace += QStringLiteral("::");
			typeNamespace += imtsdl::CSdlTools::GetCapitalizedValue(sdlUnionPtr->GetName());
		}

		if (typeNamespace != relatedNamespace){
			// versions does NOT exists for enumerators and unions
			if ((sdlEnumPtr == nullptr && sdlUnionPtr == nullptr) && addVersion){
				typeNamespace += QStringLiteral("::");
				typeNamespace += GetSdlEntryVersion(*namespaceEntryPtr);
			}

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
				while (relatedNamespacePartsIter.hasNext() && typeNamespacePartsIter.hasNext()) {
					QString relatedNamespacePart = relatedNamespacePartsIter.next();
					QString typeNamespacePart = typeNamespacePartsIter.next();
					if (relatedNamespacePart != typeNamespacePart) {
						// ALL parts in same order MUST be equal
						break;
					}
					namespaceCleaned = true;
					typeNamespacePartsIter.remove();
				}
				typeNamespace = typeNamespaceParts.join(QStringLiteral("::"));

				retVal = typeNamespace;
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

	QString versionName;
	iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(&schemaParams, imtsdl::SdlCustomSchemaKeys::VersionName.toUtf8());
	if (versionNameParamPtr.IsValid()){
		versionName = versionNameParamPtr->GetText();
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
	bool* isUnionPtr)
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
	converter.addVersion = true;

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
			const QString& relatedNamespace,
			const imtsdl::ISdlTypeListProvider& listProvider,
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

	imtsdl::CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;\n\t}");
	imtsdl::CSdlTools::FeedStream(stream, 1, false);
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


} // namespace imtsdlgenv2
