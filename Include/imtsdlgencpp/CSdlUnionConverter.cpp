// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CSdlUnionConverter.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlField.h>
#include <imtsdlgencpp/CSdlGenTools.h>



namespace imtsdlgencpp
{


// static helpers
void FeedStreamHorizontally(QTextStream& stream, uint indents, char indentDelimiter = '\t')
{
	imtsdl::CSdlTools::FeedStreamHorizontally(stream, indents, indentDelimiter);
}


void FeedStream(QTextStream& stream, uint indents, bool flush)
{
	imtsdl::CSdlTools::FeedStream(stream, indents, flush);
}


static bool IsFloatingScalarType(const QString& sdlType)
{
	return sdlType == QStringLiteral("Double") || sdlType == QStringLiteral("Float");
}


static bool IsIntegralScalarType(const QString& sdlType)
{
	return	sdlType == QStringLiteral("Int") ||
			sdlType == QStringLiteral("Integer") ||
			sdlType == QStringLiteral("Long") ||
			sdlType == QStringLiteral("LongLong");
}


static QString GetMetaTypeCondition(const QString& variantExpression, const QStringList& metaTypes)
{
	QStringList conditions;
	for (const QString& metaType : metaTypes){
		conditions << variantExpression + QStringLiteral(".userType() == QMetaType::") + metaType;
	}

	return conditions.join(QStringLiteral(" || "));
}


static QString GetSignedIntegralCondition(const QString& variantExpression)
{
	return GetMetaTypeCondition(variantExpression, {QStringLiteral("Int"), QStringLiteral("Long"), QStringLiteral("LongLong"), QStringLiteral("Short")});
}


static QString GetUnsignedIntegralCondition(const QString& variantExpression)
{
	return GetMetaTypeCondition(variantExpression, {QStringLiteral("UInt"), QStringLiteral("ULong"), QStringLiteral("ULongLong"), QStringLiteral("UShort"), QStringLiteral("UChar")});
}


static QString GetFloatingCondition(const QString& variantExpression)
{
	return GetMetaTypeCondition(variantExpression, {QStringLiteral("Double"), QStringLiteral("Float")});
}


static QString GetIntegralValuedCondition(const QString& variantExpression)
{
	return variantExpression + QStringLiteral(".toDouble() == double(") + variantExpression + QStringLiteral(".toLongLong())");
}


static QString GetExactScalarMatchCondition(const QString& sdlType, const QString& variantExpression, const QString& convertedType, bool isJsonSource)
{
	if (sdlType == QStringLiteral("Bool") || sdlType == QStringLiteral("Boolean")){
		return GetMetaTypeCondition(variantExpression, {QStringLiteral("Bool")});
	}

	if (sdlType == QStringLiteral("String") || sdlType == QStringLiteral("ID")){
		return GetMetaTypeCondition(variantExpression, {QStringLiteral("QString"), QStringLiteral("QByteArray")});
	}

	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		return GetMetaTypeCondition(variantExpression, {QStringLiteral("Int")});
	}

	if (sdlType == QStringLiteral("Long")){
		return GetMetaTypeCondition(variantExpression, {QStringLiteral("Long")});
	}

	if (sdlType == QStringLiteral("LongLong")){
		return GetMetaTypeCondition(variantExpression, {QStringLiteral("LongLong")});
	}

	if (IsFloatingScalarType(sdlType)){
		if (isJsonSource){
			// integral-valued JSON numbers are ambiguous, they are matched by the fallback conditions
			return QStringLiteral("(") + GetFloatingCondition(variantExpression) + QStringLiteral(") && !(") + GetIntegralValuedCondition(variantExpression) + ')';
		}

		return GetFloatingCondition(variantExpression);
	}

	return variantExpression + QStringLiteral(".canConvert<") + convertedType + QStringLiteral(">()");
}


static QString GetIntegralFallbackMatchCondition(const QString& variantExpression, const QString& convertedType)
{
	// the value fits into the (signed) alternative type, if it is not changed by the conversion to it
	const QString signedValue = variantExpression + QStringLiteral(".toLongLong()");
	const QString unsignedValue = variantExpression + QStringLiteral(".toULongLong()");
	const QString isSignedInRange = QStringLiteral("qlonglong(") + convertedType + '(' + signedValue + QStringLiteral(")) == ") + signedValue;
	const QString isUnsignedInRange =
				convertedType + '(' + unsignedValue + QStringLiteral(") >= 0 && quint64(") +
				convertedType + '(' + unsignedValue + QStringLiteral(")) == ") + unsignedValue;

	return
				QStringLiteral("((") + GetSignedIntegralCondition(variantExpression) + QStringLiteral(") && ") + isSignedInRange + QStringLiteral(") || ") +
				QStringLiteral("((") + GetUnsignedIntegralCondition(variantExpression) + QStringLiteral(") && ") + isUnsignedInRange + QStringLiteral(") || ") +
				QStringLiteral("((") + GetFloatingCondition(variantExpression) + QStringLiteral(") && ") +
							GetIntegralValuedCondition(variantExpression) + QStringLiteral(" && ") + isSignedInRange + ')';
}


QList<QPair<QString, QString>> CSdlUnionConverter::GetScalarUnionMatchConditions(
			const QList<QPair<QString, QString>>& scalarTypes,
			const QString& variantExpression,
			bool isJsonSource)
{
	QList<QPair<QString, QString>> retVal;

	// exact matches of the native metatypes
	for (const QPair<QString, QString>& scalarType : scalarTypes){
		retVal << qMakePair(scalarType.second, GetExactScalarMatchCondition(scalarType.first, variantExpression, scalarType.second, isJsonSource));
	}

	// numbers of other metatypes are accepted by the integral alternatives, which can hold the value
	for (const QPair<QString, QString>& scalarType : scalarTypes){
		if (IsIntegralScalarType(scalarType.first)){
			retVal << qMakePair(scalarType.second, GetIntegralFallbackMatchCondition(variantExpression, scalarType.second));
		}
	}

	// floating alternatives accept any number
	for (const QPair<QString, QString>& scalarType : scalarTypes){
		if (IsFloatingScalarType(scalarType.first)){
			const QString condition =
						GetSignedIntegralCondition(variantExpression) + QStringLiteral(" || ") +
						GetUnsignedIntegralCondition(variantExpression) + QStringLiteral(" || ") +
						GetFloatingCondition(variantExpression);
			retVal << qMakePair(scalarType.second, condition);
		}
	}

	return retVal;
}


/**
	\todo make a refactoring replace 'text' types and methods to static values.
		\example 'WriteToJsonObject' -> 'CSdlClassJsonModificatorComp::s_writeMethodName'
		\example 'QJsonObject' -> 'CSdlClassJsonModificatorComp::s_classVariableName'
		\example 'jsonObject' -> 'CSdlClassJsonModificatorComp::s_containerVariableName'
*/
void CSdlUnionConverter::WriteConversionFromUnion(
			QTextStream& stream,
			const imtsdl::CSdlUnion& sdlUnion,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			const QString& targetName,
			const QString& modelIndex,
			const imtsdl::ISdlTypeListProvider& listProvider,
			const imtsdl::ISdlEnumListProvider& enumlistProvider,
			const imtsdl::ISdlUnionListProvider& unionlistProvider,
			uint hIndents,
			const ConversionType& conversionType,
			const QString& addCommand,
			const QString& customModelTarget,
			const QString& returnOnFail)
{
	QString typeNamespace = imtsdl::CSdlTools::BuildNamespaceFromParams(sdlUnion.GetSchemaParams(), false, true);
	if (typeNamespace != relatedNamespace){
		while (!typeNamespace.endsWith(QStringLiteral("::"))){
			typeNamespace.append(':');
		}
		// use global namespace
		if (!typeNamespace.startsWith(QStringLiteral("::"))){
			typeNamespace.prepend(QStringLiteral("::"));
		}
	}
	else {
		typeNamespace.clear();
	}
	const QString completeTypeName = typeNamespace + sdlUnion.GetName();

	// for GQL arrays create a container object (GQL Spec limitaion?)
	if (conversionType == CT_GQL_ARRAY){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("::imtgql::CGqlParamObject ");
		stream << targetName;
		stream << QStringLiteral("DataObject;");
		FeedStream(stream, 1, false);
	}

	bool isFirstIteration = true;
	for (const auto& sdlType : sdlUnion.GetTypes()){
		imtsdl::CSdlField field;
		field.SetType(sdlType);
		bool isCustom = false;
		QString convertedType = imtsdlgencpp::CSdlGenTools::OptListConvertTypeWithNamespaceStruct(
			field,
			relatedNamespace,
			listProvider,
			enumlistProvider,
			unionlistProvider,
			true,
			&isCustom,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			false);

		const QString valName = imtsdl::CSdlTools::GetDecapitalizedValue(sdlType) + QStringLiteral("Val");

		FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}

		stream << QStringLiteral("if (const ") << convertedType << QStringLiteral("* ") << valName << QStringLiteral(" = std::get_if<") << convertedType << QStringLiteral(">(");
		stream << sourceVariableName;
		stream << QStringLiteral(")){");

		FeedStream(stream, 1, false);
		if (conversionType == CT_MODEL_ARRAY){
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetName;
			stream << QStringLiteral("->InsertNewItem();");
			FeedStream(stream, 1, false);
		}

		if (isCustom){
			if (conversionType == CT_MODEL_ARRAY){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if(!") << valName << QStringLiteral("->WriteToModel(*");
				stream << targetName;
				stream << QStringLiteral(", ") << modelIndex << QStringLiteral(")){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);

				// add typename
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetName;
				stream << QStringLiteral("->SetData(\"__typename\", \"");
				stream << sdlType;
				stream << QStringLiteral("\", ");
				stream << modelIndex;
				stream << QStringLiteral(");");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_MODEL_SCALAR){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << valName << QStringLiteral("->WriteToModel(");
				if (!customModelTarget.isEmpty()){
					stream << customModelTarget;
				}
				else{
					stream << QStringLiteral("*(model.AddTreeModel(\"") << targetName << QStringLiteral("\", ") << modelIndex << QStringLiteral(")), 0");
				}
				stream << QStringLiteral(")){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return ");
				stream << returnOnFail;
				stream << ';';
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);

				// add typename
				FeedStreamHorizontally(stream, hIndents + 1);
				QString newModelPtrVarName = customModelTarget;
				bool isNewModelPointer = false;
				if (customModelTarget.isEmpty()){
					newModelPtrVarName = QStringLiteral("model.GetTreeItemModel(\"") + targetName + QStringLiteral("\", ") + modelIndex + ')';
					isNewModelPointer = true;
				}
				else {
					if (newModelPtrVarName.startsWith('*')){
						isNewModelPointer = true;
						newModelPtrVarName.remove(0,1);
					}
				}
				if (isNewModelPointer){
					stream <<  QStringLiteral("if(");
					stream << newModelPtrVarName;
					stream << QStringLiteral(" != nullptr){");
					FeedStream(stream, 1, false);
					FeedStreamHorizontally(stream, hIndents + 2);
				}

				stream << newModelPtrVarName;
				stream << (isNewModelPointer ? "->" : ".");
				stream << QStringLiteral("SetData(\"__typename\", \"");
				stream << sdlType;
				stream << QStringLiteral("\", 0);");
				FeedStream(stream, 1, false);

				if (isNewModelPointer){
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << '}';
					FeedStream(stream, 1, false);
				}
			}
			else if (conversionType == CT_GQL_SCALAR || conversionType == CT_GQL_ARRAY){
				if (conversionType == CT_GQL_SCALAR){
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("::imtgql::CGqlParamObject ") << targetName << QStringLiteral("DataObject;");
					FeedStream(stream, 1, false);
				}

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << valName << QStringLiteral("->WriteToGraphQlObject(");
				stream << targetName;
				stream << QStringLiteral("DataObject)){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);

				// add typename
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetName;
				stream << QStringLiteral("DataObject.InsertParam(\"__typename\", QVariant(\"");
				stream << sdlType << QStringLiteral("\"));");
				FeedStream(stream, 1, false);

				if (conversionType == CT_GQL_SCALAR){
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("gqlObject.InsertParam(\"") << targetName << QStringLiteral("\", ") << targetName << QStringLiteral("DataObject);");
					FeedStream(stream, 1, false);
				}
			}
			else if (conversionType == CT_JSON_SCALAR){
				if (!customModelTarget.isEmpty()){
					// direct write to custom JSON target object
					QString isAddedVariableName = QStringLiteral("is") + targetVariableName + QStringLiteral("Added");
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
					stream << valName << QStringLiteral("->WriteToJsonObject(") << customModelTarget << QStringLiteral(");");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("if (!") << isAddedVariableName << QStringLiteral("){");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 2);
					stream << QStringLiteral("return ") << returnOnFail << ';';
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("}");
					FeedStream(stream, 1, false);

					// add typename
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << customModelTarget;
					stream << QStringLiteral("[\"__typename\"] = \"") << sdlType << QStringLiteral("\";");
					FeedStream(stream, 1, false);
				}
				else{
					QString jsonVariable = targetName + QStringLiteral("JsonObject");
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("QJsonObject ") << jsonVariable << QStringLiteral(";");
					FeedStream(stream, 1, false);

					QString isAddedVariableName = QStringLiteral("is") + targetVariableName + QStringLiteral("Added");
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
					stream << valName << QStringLiteral("->WriteToJsonObject(") << jsonVariable << QStringLiteral(");");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("if (!") << isAddedVariableName << QStringLiteral("){");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 2);
					stream << QStringLiteral("return false;");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("}");
					FeedStream(stream, 1, false);

					// add typename
					FeedStreamHorizontally(stream, hIndents + 1);
					stream << jsonVariable;
					stream << QStringLiteral("[\"__typename\"] = \"") << sdlType << QStringLiteral("\";");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("jsonObject[\"") << targetVariableName << QStringLiteral("\"] = ") << jsonVariable << QStringLiteral(";");
					FeedStream(stream, 1, false);
				}
			}
			else if (conversionType == CT_JSON_ARRAY){
				QString jsonVariable = targetName + QStringLiteral("JsonObject");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("QJsonObject ") << jsonVariable << QStringLiteral(";");
				FeedStream(stream, 1, false);

				QString isAddedVariableName = QStringLiteral("is") + targetName + QStringLiteral("Added");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
				stream << valName << QStringLiteral("->WriteToJsonObject(") << jsonVariable << QStringLiteral(");");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << isAddedVariableName << QStringLiteral("){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);

				// add typename
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << jsonVariable;
				stream << QStringLiteral("[\"__typename\"] = \"") << sdlType << QStringLiteral("\";");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" << ") << jsonVariable << QStringLiteral(";");
				FeedStream(stream, 1, false);
			}
		}
		else{
			if (conversionType == CT_GQL_SCALAR){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("gqlObject.InsertParam(\"") << targetName << QStringLiteral("\", QVariant(*") << valName << QStringLiteral("));");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_GQL_ARRAY){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetName;
				stream << QStringLiteral("DataObject");
				stream << QStringLiteral(".InsertParam(\"") << targetName << QStringLiteral("\", *") << valName << QStringLiteral(");");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_SCALAR){
				FeedStreamHorizontally(stream, hIndents + 1);
				if (!customModelTarget.isEmpty()){
					if (!addCommand.isEmpty()){
						// addCommand inserts with empty key for primitive union members (equivalent of CTreeItemModel::SetData("", *val))
						stream << addCommand << QStringLiteral("\"\", *") << valName << QStringLiteral(");");
					}
					else{
						stream << customModelTarget << QStringLiteral("[\"") << targetName << QStringLiteral("\"] = QJsonValue::fromVariant(*") << valName << QStringLiteral(");");
					}
				}
				else{
					stream << QStringLiteral("if (!") << targetName << QStringLiteral("){");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 2);
					stream << QStringLiteral("return false;");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("}");
					FeedStream(stream, 1, false);

					FeedStreamHorizontally(stream, hIndents + 1);
					stream << QStringLiteral("jsonObject[\"") << targetName << QStringLiteral("\"] = QJsonValue::fromVariant(*") << valName << QStringLiteral(");");
				}
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_ARRAY){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << targetName << QStringLiteral("){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" << QJsonValue::fromVariant(*") << valName << QStringLiteral(");");
				FeedStream(stream, 1, false);
			}
			else{
				FeedStreamHorizontally(stream, hIndents + 1);


				if (!addCommand.isEmpty() && conversionType == CT_MODEL_SCALAR){
					stream << addCommand << '"' << targetName << QStringLiteral("\", *") << valName << QStringLiteral(", ") << modelIndex << QStringLiteral(");");
				}
				else if (!addCommand.isEmpty() && conversionType == CT_MODEL_ARRAY){
					stream << addCommand << QStringLiteral("QByteArray(), *") << valName << QStringLiteral(", ") << modelIndex << QStringLiteral(");");
				}
				else if (!addCommand.isEmpty()){
					stream << addCommand << QStringLiteral("\"\", *") << valName << QStringLiteral(");");
				}
				else{
					stream << targetVariableName;
					stream << QStringLiteral(" = *") << valName << QStringLiteral(";");
				}
				FeedStream(stream, 1, false);
			}
		}

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		isFirstIteration = false;
	}

	if (conversionType == CT_GQL_ARRAY){
		FeedStreamHorizontally(stream, hIndents);
		stream << targetVariableName;
		stream << QStringLiteral(" << ");
		stream << targetName;
		stream << QStringLiteral("DataObject;");
		FeedStream(stream, 1, false);
	}
}


void CSdlUnionConverter::WriteUnionConversionFromData(
			QTextStream& stream,
			const imtsdl::CSdlUnion& sdlUnion,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			const QString& modelIndex,
			const imtsdl::ISdlTypeListProvider& listProvider,
			const imtsdl::ISdlEnumListProvider& enumlistProvider,
			const imtsdl::ISdlUnionListProvider& unionlistProvider,
			uint hIndents,
			const ConversionType& conversionType,
			const QString& targetName,
			const QString& arraySourceVariableName)
{
	bool isFirstIteration = true;
	QList<QPair<QString, QString>> scalarTypes;
	QStringList enumTypes;
	for (const auto& sdlType : sdlUnion.GetTypes()){
		imtsdl::CSdlField field;
		field.SetType(sdlType);
		bool isCustom = false;
		bool isEnum = false;
		QString convertedType = imtsdlgencpp::CSdlGenTools::OptListConvertTypeWithNamespaceStruct(
			field,
			relatedNamespace,
			listProvider,
			enumlistProvider,
			unionlistProvider,
			true,
			&isCustom,
			nullptr,
			nullptr,
			&isEnum,
			nullptr,
			false);

		if (isEnum){
			enumTypes << convertedType;

			continue;
		}

		// scalar alternatives are checked after the custom ones in the special order
		if (!isCustom){
			scalarTypes << qMakePair(sdlType, convertedType);

			continue;
		}

		FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}

		QString tempVar = targetVariableName + QStringLiteral("Convert");
		stream << QStringLiteral("if (");
		stream << imtsdl::CSdlTools::GetDecapitalizedValue(targetVariableName);
		stream << QStringLiteral("Typename == \"") <<  sdlType << QStringLiteral("\") {");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents + 1);

		stream << convertedType << QStringLiteral(" ") << tempVar << QStringLiteral(";");
		FeedStream(stream, 1, false);

		if (conversionType == CT_MODEL_ARRAY || conversionType == CT_MODEL_SCALAR){
			QString readVariable = QStringLiteral("is") + targetVariableName + QStringLiteral("Read");
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("const bool ") << readVariable << QStringLiteral(" = ");
			stream << tempVar << QStringLiteral(".ReadFromModel(*model.GetTreeItemModel(\"");
			if (conversionType == CT_MODEL_SCALAR){
				stream << targetVariableName;
			}
			else{
				stream << targetName;
			}
			stream << QStringLiteral("\", ");
			stream << modelIndex << QStringLiteral(")); ");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("if (!") << readVariable << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 2);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetVariableName << QStringLiteral(" = ");
			stream << sdlUnion.GetName() << QStringLiteral("(");
			stream << tempVar << QStringLiteral(");");
			FeedStream(stream, 1, false);
		}
		else if (conversionType == CT_JSON_SCALAR || conversionType == CT_JSON_ARRAY){
			QString readVariable = QStringLiteral("is") + targetVariableName + QStringLiteral("Read");
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("const bool ") << readVariable << QStringLiteral(" = ");
			stream << tempVar<< QStringLiteral(".ReadFromJsonObject(");
			if (conversionType == CT_JSON_SCALAR){
				stream << QStringLiteral("jsonObject[\"") << targetVariableName << QStringLiteral("\"");
			}
			else{
				stream << targetName << QStringLiteral("JsonArray[") << modelIndex;
			}
			stream << QStringLiteral("].toObject());");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("if (!") << readVariable << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 2);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetVariableName << QStringLiteral(" = ");
			stream << sdlUnion.GetName() << QStringLiteral("(");
			stream << tempVar << QStringLiteral(");");
			FeedStream(stream, 1, false);
		}
		else{
			QString readVariable = QStringLiteral("is") + imtsdl::CSdlTools::GetCapitalizedValue(targetName) + QStringLiteral("Read");
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("const bool ") << readVariable << QStringLiteral(" = ");
			stream << tempVar << QStringLiteral(".ReadFromGraphQlObject(*");
			if (conversionType == CT_GQL_SCALAR){
				stream << QStringLiteral("gqlObject.GetParamArgumentObjectPtr(\"") << targetVariableName << QStringLiteral("\")");
			}
			else{
				stream << arraySourceVariableName;
			}
			stream << QStringLiteral(");");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("if (!") << readVariable << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 2);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetVariableName << QStringLiteral(" = ");
			stream << sdlUnion.GetName() << QStringLiteral("(");
			stream << tempVar << QStringLiteral(");");
			FeedStream(stream, 1, false);
		}


		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		isFirstIteration = false;
	}

	QString variantExpression = sourceVariableName;
	if (conversionType == CT_GQL_ARRAY){
		variantExpression += QStringLiteral("->GetParamArgumentValue(\"\")");
	}
	for (const QString& enumType : std::as_const(enumTypes)){
		const QString enumValue = QStringLiteral("QMetaEnum::fromType<") + enumType + QStringLiteral(">().keyToValue(") + variantExpression + QStringLiteral(".toString().toUtf8())");

		FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if ((") << variantExpression << QStringLiteral(".userType() == QMetaType::QString || ")
			<< variantExpression << QStringLiteral(".userType() == QMetaType::QByteArray) && ") << enumValue << QStringLiteral(" > -1){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << targetVariableName << QStringLiteral(" = ") << sdlUnion.GetName() << QStringLiteral("(static_cast<") << enumType << QStringLiteral(">(") << enumValue << QStringLiteral("));");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		isFirstIteration = false;
	}

	const bool isJsonSource = conversionType == CT_JSON_SCALAR || conversionType == CT_JSON_ARRAY;
	const QList<QPair<QString, QString>> scalarMatches = GetScalarUnionMatchConditions(scalarTypes, variantExpression, isJsonSource);
	for (const QPair<QString, QString>& scalarMatch : scalarMatches){
		FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if (") << scalarMatch.second << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << targetVariableName;
		stream << QStringLiteral(" = ");
		stream << sdlUnion.GetName() << QStringLiteral("(");
		stream << variantExpression;
		stream << QStringLiteral(".value<") << scalarMatch.first << QStringLiteral(">());");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		isFirstIteration = false;
	}

	if (	conversionType == CT_MODEL_ARRAY ||
			conversionType == CT_JSON_ARRAY ||
			conversionType == CT_GQL_ARRAY)
	{
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("else{");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);
	}
}



} // namespace imtsdlgencpp
