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


namespace imtsdlgenv2
{


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

QString CSdlGenTools::GetTypeVerstion(const imtsdl::CSdlType& type)
{
	QString retVal = QStringLiteral("V");

	const iprm::IParamsSet& schemaParams = type.GetSchemaParams();
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

QString CSdlGenTools::OptListConvertTypeWithNamespaceStruct(const imtsdl::CSdlField& sdlField, const QString& relatedNamespace, imtsdl::ISdlTypeListProvider& listProvider, bool listWrap,  bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr)
{
	bool isCustom = false;
	QString retVal = imtsdl::CSdlTools::OptListConvertTypeWithNamespace(sdlField, relatedNamespace, listProvider, listWrap, &isCustom, isComplexPtr, isArrayPtr);
	if (isCustomPtr != nullptr){
		*isCustomPtr = isCustom;
	}
	if (isCustom){
		imtsdl::CSdlType typeForField;
		const bool isFound = imtsdl::CSdlTools::GetSdlTypeForField(sdlField, listProvider.GetSdlTypes(false), typeForField);
		Q_ASSERT(isFound);
		retVal += QStringLiteral("::") + GetTypeVerstion(typeForField);
	}

	if (listWrap && sdlField.IsArray()){
		imtsdl::CSdlTools::WrapTypeToList(retVal);
	}

	return retVal;
}


} // namespace imtsdlgenv2
