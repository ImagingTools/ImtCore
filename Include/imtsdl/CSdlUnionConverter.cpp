#include <imtsdl/CSdlUnionConverter.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	\todo make a refactoring replace 'text' types and methods to static values.
		\example 'WriteToJsonObject' -> 'CSdlClassJsonModificatorComp::s_writeMethodName'
		\example 'QJsonObject' -> 'CSdlClassJsonModificatorComp::s_classVariableName'
		\example 'jsonObject' -> 'CSdlClassJsonModificatorComp::s_containerVariableName'
*/
void CSdlUnionConverter::WriteConversionFromUnion(
			QTextStream& stream,
			const CSdlUnion& sdlUnion,
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
			const QString returnOnFail)
{
	QString typeNamespace = BuildNamespaceFromParams(sdlUnion.GetSchemaParams(), false, true);
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

	// for GQL create a container object (GQL Spec limitaion?)
	if (conversionType == CT_GQL_SCALAR || conversionType == CT_GQL_ARRAY){
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
		QString convertedType = imtsdlgenv2::CSdlGenTools::OptListConvertTypeWithNamespaceStruct(
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

		FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if (const ") << convertedType << QStringLiteral("* val = std::get_if<") << convertedType << QStringLiteral(">((");
		stream << sourceVariableName;
		stream << QStringLiteral(").get())){");
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
				stream << QStringLiteral("if(!val->WriteToModel(*");
				stream << targetName;
				stream << QStringLiteral(", ") << modelIndex << QStringLiteral(")){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_MODEL_SCALAR){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!val->WriteToModel(");
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
			}
			else if (conversionType == CT_GQL_SCALAR || conversionType == CT_GQL_ARRAY){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!val->WriteToGraphQlObject(");
				stream << targetName;
				stream << QStringLiteral("DataObject)){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_SCALAR){
				QString jsonVariable = targetName + QString("JsonObject");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("QJsonObject ") << jsonVariable << QStringLiteral(";");
				FeedStream(stream, 1, false);

				QString isAddedVariableName = QString("is") + targetVariableName + QString("Added");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("val->WriteToJsonObject(") << jsonVariable << QStringLiteral(");");
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

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("jsonObject[\"") << targetVariableName << QStringLiteral("\"] = ") << jsonVariable << QStringLiteral(";");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_ARRAY){
				QString jsonVariable = targetName + QString("JsonObject");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("QJsonObject ") << jsonVariable << QStringLiteral(";");
				FeedStream(stream, 1, false);

				QString isAddedVariableName = QString("is") + targetName + QString("Added");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("val->WriteToJsonObject(") << jsonVariable << QStringLiteral(");");
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

				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" << ") << jsonVariable << QStringLiteral(";");
				FeedStream(stream, 1, false);
			}
		}
		else{
			if (conversionType == CT_GQL_SCALAR){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetName;
				stream << QStringLiteral("DataObject");
				stream << QStringLiteral(".InsertParam(\"") << targetName << QStringLiteral("\", *val);");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_SCALAR){
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
				stream << QStringLiteral("jsonObject[\"") << targetName << QStringLiteral("\"] = QJsonValue::fromVariant(*val);");
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
				stream << targetVariableName << QStringLiteral(" << QJsonValue::fromVariant(*val);");
				FeedStream(stream, 1, false);
			}
			else{
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName;
				stream << QStringLiteral(" = *val;");
				FeedStream(stream, 1, false);
			}

			if (!addCommand.isEmpty()){
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << addCommand << QStringLiteral("\"") << targetName;
				stream << QStringLiteral("\", ") << targetVariableName << QStringLiteral(", ");
				stream << modelIndex << QStringLiteral(");");
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

	// and finally for GQL write a *container*
	if (conversionType == CT_GQL_SCALAR){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("gqlObject.InsertParam(\"");
		stream << targetName;
		stream << QStringLiteral("\", ");
		stream << targetName;
		stream << QStringLiteral("DataObject);");
	}
}


void CSdlUnionConverter::WriteUnionConversionFromData(
			QTextStream& stream,
			const CSdlUnion& sdlUnion,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			const QString& modelIndex,
			const imtsdl::ISdlTypeListProvider& listProvider,
			const imtsdl::ISdlEnumListProvider& enumlistProvider,
			const imtsdl::ISdlUnionListProvider& unionlistProvider,
			uint hIndents,
			const ConversionType& conversionType,
			const QString& targetName)
{
	bool isFirstIteration = true;
	for (const auto& sdlType : sdlUnion.GetTypes()){
		imtsdl::CSdlField field;
		field.SetType(sdlType);
		bool isCustom = false;
		QString convertedType = imtsdlgenv2::CSdlGenTools::OptListConvertTypeWithNamespaceStruct(
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

		FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}

		stream << QStringLiteral("if (") << sourceVariableName << QStringLiteral(".canConvert<") << convertedType << QStringLiteral(">()){");
		FeedStream(stream, 1, false);

		if (isCustom){
			FeedStreamHorizontally(stream, hIndents + 1);
			QString tempVar = targetVariableName + QStringLiteral("Convert");
			stream << convertedType << QStringLiteral(" ") << tempVar << QStringLiteral(";");
			FeedStream(stream, 1, false);

			if (conversionType == CT_MODEL_ARRAY || conversionType == CT_MODEL_SCALAR){
				QString readVariable = QString("is") + targetVariableName + QString("Read");
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
				stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
				stream << tempVar << QStringLiteral(");");
				FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_SCALAR || conversionType == CT_JSON_ARRAY){
				QString readVariable = QString("is") + targetVariableName + QString("Read");
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
				stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
				stream << tempVar << QStringLiteral(");");
				FeedStream(stream, 1, false);
			}
			else{
				QString gqlVariable = targetName + QString("DataObjectPtr");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const ::imtgql::CGqlParamObject* ") << gqlVariable;
				stream << QStringLiteral(" = gqlObject.GetParamArgumentObjectPtr(\"");
				if (conversionType == CT_GQL_SCALAR){
					stream << targetVariableName;
				}
				else{
					stream << targetName;
				}
				stream << QStringLiteral("\");");
				FeedStream(stream, 1, false);

				QString readVariable = QString("is") + GetCapitalizedValue(targetName) + QString("Read");
				FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << readVariable << QStringLiteral(" = ");
				stream << tempVar << QStringLiteral(".ReadFromGraphQlObject(*") << gqlVariable << QStringLiteral(");");
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
				stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
				stream << tempVar << QStringLiteral(");");
				FeedStream(stream, 1, false);
			}
		}
		else{
			FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetVariableName;
			stream << QStringLiteral(" = ");
			stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
			stream << sourceVariableName << QStringLiteral(".value<") << convertedType << QStringLiteral(">());");
			FeedStream(stream, 1, false);
		}


		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		isFirstIteration = false;
	}

	if (conversionType == CT_MODEL_ARRAY 
		|| conversionType == CT_JSON_ARRAY
		|| conversionType == CT_GQL_ARRAY){
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



} // namespace imtsdl
