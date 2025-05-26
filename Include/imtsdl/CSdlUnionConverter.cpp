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
			const QString& addCommand)
{
	QString typeNamespace = CSdlTools::BuildNamespaceFromParams(sdlUnion.GetSchemaParams(), false, true);
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

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if (const ") << convertedType << QStringLiteral("* val = std::get_if<") << convertedType << QStringLiteral(">((");
		stream << sourceVariableName;
		stream << QStringLiteral(").get())){");
		CSdlTools::FeedStream(stream, 1, false);
		if (conversionType == CT_MODEL_ARRAY){
			CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetName;
			stream << QStringLiteral("->InsertNewItem();");
			CSdlTools::FeedStream(stream, 1, false);

		}

		if (isCustom){
			if (conversionType == CT_MODEL_ARRAY){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if(!val->WriteToModel(*");
				stream << targetName;
				stream << QStringLiteral(", ") << modelIndex << QStringLiteral(")){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_MODEL_SCALAR){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!val->WriteToModel(*");
				stream << QStringLiteral("(model.AddTreeModel(\"") << targetName << QStringLiteral("\", ") << modelIndex << QStringLiteral(")), 0)){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_GQL_SCALAR){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!val->WriteToGraphQlObject(") << targetVariableName << QStringLiteral(")){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_SCALAR){
				QString jsonVariable = targetName + QString("JsonObject");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("QJsonObject ") << jsonVariable << QStringLiteral(";");
				CSdlTools::FeedStream(stream, 1, false);

				QString isAddedVariableName = QString("is") + targetVariableName + QString("Added");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("val->WriteToJsonObject(") << jsonVariable << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << isAddedVariableName << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("jsonObject[\"") << targetVariableName << QStringLiteral("\"] = ") << jsonVariable << QStringLiteral(";");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_ARRAY){
				QString jsonVariable = targetName + QString("JsonObject");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("QJsonObject ") << jsonVariable << QStringLiteral(";");
				CSdlTools::FeedStream(stream, 1, false);

				QString isAddedVariableName = QString("is") + targetName + QString("Added");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << isAddedVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("val->WriteToJsonObject(") << jsonVariable << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << isAddedVariableName << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" << ") << jsonVariable << QStringLiteral(";");
				CSdlTools::FeedStream(stream, 1, false);
			}
		}
		else{
			if (conversionType == CT_GQL_SCALAR){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName;
				stream << QStringLiteral(".InsertField(\"") << targetName << QStringLiteral("\", *val);");
				CSdlTools::FeedStream(stream, 1, false);

			}
			else if (conversionType == CT_JSON_SCALAR){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << targetName << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("jsonObject[\"") << targetName << QStringLiteral("\"] = QJsonValue::fromVariant(*val);");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_ARRAY){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << targetName << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" << QJsonValue::fromVariant(*val);");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else{
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName;
				stream << QStringLiteral(" = *val;");
				CSdlTools::FeedStream(stream, 1, false);
			}

			if (!addCommand.isEmpty()){
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << addCommand << QStringLiteral("\"") << targetName;
				stream << QStringLiteral("\", ") << targetVariableName << QStringLiteral(", ");
				stream << modelIndex << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);
			}
		}

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		CSdlTools::FeedStream(stream, 1, false);

		isFirstIteration = false;
	}
}


void CSdlUnionConverter::WriteUnionConversionFromString(
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

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}

		stream << QStringLiteral("if (") << sourceVariableName << QStringLiteral(".canConvert<") << convertedType << QStringLiteral(">()){");
		CSdlTools::FeedStream(stream, 1, false);

		if (isCustom){
			CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
			QString tempVar = targetVariableName + QStringLiteral("Convert");
			stream << convertedType << QStringLiteral(" ") << tempVar << QStringLiteral(";");
			CSdlTools::FeedStream(stream, 1, false);

			if (conversionType == CT_MODEL_ARRAY || conversionType == CT_MODEL_SCALAR){
				QString readVariable = QString("is") + targetVariableName + QString("Read");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
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
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << readVariable << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
				stream << tempVar << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else if (conversionType == CT_JSON_SCALAR || conversionType == CT_JSON_ARRAY){
				QString readVariable = QString("is") + targetVariableName + QString("Read");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << readVariable << QStringLiteral(" = ");
				stream << tempVar<< QStringLiteral(".ReadFromJsonObject(");
				if (conversionType == CT_JSON_SCALAR){
					stream << QStringLiteral("jsonObject[\"") << targetVariableName << QStringLiteral("\"");
				}
				else{
					stream << targetName << QStringLiteral("[") << modelIndex;
				}
				stream << QStringLiteral("].toObject());");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << readVariable << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
				stream << tempVar << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);
			}
			else{
				QString gqlVariable = targetName + QString("DataObjectPtr");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const ::imtgql::CGqlParamObject* ") << gqlVariable;
				stream << QStringLiteral(" = request.GetFieldArgumentObjectPtr(\"");
				if (conversionType == CT_GQL_SCALAR){
					stream << targetVariableName;
				}
				else{
					stream << targetName;
				}
				stream << QStringLiteral("\");");
				CSdlTools::FeedStream(stream, 1, false);

				QString readVariable = QString("is") + targetName + QString("Read");
				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("const bool ") << readVariable << QStringLiteral(" = ");
				stream << tempVar << QStringLiteral(".ReadFromGraphQlObject(*") << gqlVariable << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("if (!") << readVariable << QStringLiteral("){");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 2);
				stream << QStringLiteral("return false;");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << QStringLiteral("}");
				CSdlTools::FeedStream(stream, 1, false);

				CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
				stream << targetVariableName << QStringLiteral(" = ");
				stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
				stream << tempVar << QStringLiteral(");");
				CSdlTools::FeedStream(stream, 1, false);
			}
		}
		else{
			CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
			stream << targetVariableName;
			stream << QStringLiteral(" = ");
			stream << QStringLiteral("std::make_shared<") << sdlUnion.GetName() << QStringLiteral(">(");
			stream << sourceVariableName << QStringLiteral(".value<") << convertedType << QStringLiteral(">());");
			CSdlTools::FeedStream(stream, 1, false);
		}


		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		CSdlTools::FeedStream(stream, 1, false);

		isFirstIteration = false;
	}

	if (conversionType == CT_MODEL_ARRAY 
		|| conversionType == CT_JSON_ARRAY
		|| conversionType == CT_GQL_ARRAY){
		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("else{");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		CSdlTools::FeedStream(stream, 1, false);
	}
}



} // namespace imtsdl
