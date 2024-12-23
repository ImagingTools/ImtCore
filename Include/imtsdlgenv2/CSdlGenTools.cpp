#include <imtsdlgenv2/CSdlGenTools.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
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
#include <imtsdl/CSdlTools.h>
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
			bool aListWrap):
	typeListProviderPtr(&aListProvider),
	relatedNamespace(aRelatedNamespace),
	listWrap(aListWrap),
	sdlEntryPtr(&aSdlEntry)
{
}


CSdlGenTools::CStructNamespaceConverter::CStructNamespaceConverter(
			const imtsdl::CSdlField& aSdlField,
			const QString& aRelatedNamespace,
			const imtsdl::ISdlTypeListProvider& aListProvider,
			bool aListWrap):
	typeListProviderPtr(&aListProvider),
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
		std::unique_ptr<imtsdl::CSdlType> typeForFieldPtr;

		QString typeNamespace;
		if (sdlFieldPtr != nullptr){
			typeForFieldPtr.reset(new imtsdl::CSdlType);
			const bool isFound = imtsdl::CSdlTools::GetSdlTypeForField(*sdlFieldPtr, typeListProviderPtr->GetSdlTypes(false), *typeForFieldPtr);
			Q_ASSERT(isFound);
			namespaceEntryPtr = typeForFieldPtr.get();
		}

		typeNamespace = GetNamespaceFromSchemaParams(namespaceEntryPtr->GetSchemaParams());
		typeNamespace += QStringLiteral("::");

		const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(namespaceEntryPtr);
		if (sdlTypePtr != nullptr){
			typeNamespace += 'C' + imtsdl::CSdlTools::GetCapitalizedValue(sdlTypePtr->GetName());
		}

		const imtsdl::CSdlDocumentType* sdlDocumentTypePtr = dynamic_cast<const imtsdl::CSdlDocumentType*>(namespaceEntryPtr);
		if (sdlDocumentTypePtr != nullptr){
			typeNamespace += 'C' + imtsdl::CSdlTools::GetCapitalizedValue(sdlDocumentTypePtr->GetName());
		}

		const imtsdl::CSdlRequest* sdlRequestPtr = dynamic_cast<const imtsdl::CSdlRequest*>(namespaceEntryPtr);
		if (sdlRequestPtr != nullptr){
			typeNamespace += 'C' + imtsdl::CSdlTools::GetCapitalizedValue(sdlRequestPtr->GetName());
			typeNamespace += QStringLiteral("GqlRequest");
		}



		if (typeNamespace != relatedNamespace){
			if (addVersion){
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


QString CSdlGenTools::GetSchemaVerstionString(const iprm::IParamsSet& schemaParams)
{
	QString retVal = QStringLiteral("V");

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


QString CSdlGenTools::OptListConvertTypeWithNamespaceStruct(const imtsdl::CSdlField& sdlField, const QString& relatedNamespace, const imtsdl::ISdlTypeListProvider& listProvider, bool listWrap,  bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr)
{
	bool _isCustom = false;
	QString retVal = imtsdl::CSdlTools::ConvertType(sdlField.GetType(), &_isCustom, isComplexPtr);
	if (isCustomPtr != nullptr){
		*isCustomPtr = _isCustom;
	}
	if (isArrayPtr != nullptr){
		*isArrayPtr = sdlField.IsArray();
	}

	if (!_isCustom){
		// we can define namespace only for custom types
		if (listWrap && sdlField.IsArray()){
			imtsdl::CSdlTools::WrapTypeToList(retVal);
		}

		return retVal;
	}

	CStructNamespaceConverter converter(sdlField, relatedNamespace, listProvider, listWrap);
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
	retVal.append('*');

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


} // namespace imtsdlgenv2
