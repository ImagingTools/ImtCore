#include "CSdlUnionGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgencpp
{


iproc::IProcessor::TaskState CSdlUnionGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());
	Q_ASSERT(m_customSchemaParamsCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsCppEnabled()){
		return TS_OK;
	}

	const QString outputDirectoryPath = imtsdl::CSdlTools::GetCompleteOutputPath(m_customSchemaParamsCompPtr, *m_argumentParserCompPtr, true, true);
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return TS_INVALID;
	}

	const QString defaultName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		if (!m_customSchemaParamsCompPtr.IsValid()){
			SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

			return TS_INVALID;
		}

		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, *m_argumentParserCompPtr);
	}

	imtsdl::SdlUnionList sdlUnionList = m_sdlUnionListCompPtr->GetUnions(true);
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			// in case if auto join, nothing todo, because files already defined.
			return TS_OK;
		}
		else {
			const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
			QStringList cumulatedFiles;
			for (const imtsdl::CSdlUnion& sdlUnion: sdlUnionList){
				if (!joinHeaders){
					cumulatedFiles << QString(outputDirectoryPath + "/" + sdlUnion.GetName() + ".h");
					cumulatedFiles << QString(outputDirectoryPath + "/" + sdlUnion.GetName() + "_ImplClass.h");
				}
				PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);
				PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
			}
		}

		return TS_OK;
	}

	const QString tempDirectoryPath = GetTempOutputPathFromParams(paramsPtr, outputDirectoryPath);
	for (imtsdl::CSdlUnion& sdlUnion: sdlUnionList){
		QFile unionFile(QString(tempDirectoryPath + "/" + sdlUnion.GetName() + ".h"));
		if (!unionFile.open(QIODevice::WriteOnly)){
			SendErrorMessage(0, QString("Unable to open file '%1'. Error: %2").arg(unionFile.fileName(), unionFile.errorString()));

			return TS_INVALID;
		}
		{
			QTextStream stream(&unionFile);

			stream << QStringLiteral("#pragma once");
			FeedStream(stream, 2, false);

			// namespace begin
			QString sdlNamespace = GetNamespaceFromSchemaParams(sdlUnion.GetSchemaParams());

			stream << QStringLiteral("namespace ");
			stream << sdlNamespace;
			stream << QStringLiteral("\n{");
			FeedStream(stream, 2, false);

			// union forward declaration create
			stream << QStringLiteral("class ");
			stream << sdlUnion.GetName();
			stream << QStringLiteral(";");
			FeedStream(stream, 1, false);

			stream << QStringLiteral("class ");
			stream << QStringLiteral("C") << sdlUnion.GetName() << QStringLiteral("Object;");
			FeedStream(stream, 2, false);

			// end of namespace
			stream << QStringLiteral("} // namespace ");
			stream << sdlNamespace;
			FeedStream(stream, 2, true);
		}

		QFile unionImplFile(QString(tempDirectoryPath + "/" + sdlUnion.GetName() + "_ClassDef.h"));
		if (!unionImplFile.open(QIODevice::WriteOnly)){
			SendErrorMessage(0, QString("Unable to open file '%1'. Error: %2").arg(unionImplFile.fileName(), unionImplFile.errorString()));

			return TS_INVALID;
		}

		QTextStream stream(&unionImplFile);

		stream << QStringLiteral("#pragma once");
		FeedStream(stream, 2, false);
		stream << QStringLiteral("// STD includes");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#include <variant>");
		FeedStream(stream, 2, false);

		QList<QString> customIncludeList;
		QList<QString> convertedTypeList;
		bool isQtCommentAdded = false;
		for (const auto& sdlType : sdlUnion.GetTypes()){
			imtsdl::CSdlField field;
			field.SetType(sdlType);
			bool isCustom = false;
			QString convertedType = OptListConvertTypeWithNamespaceStruct(
				field,
				GetNamespaceFromSchemaParams(sdlUnion.GetSchemaParams()),
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				*m_sdlUnionListCompPtr,
				true,
				&isCustom,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				false);
			convertedTypeList << convertedType;

			// Add Qt types
			if (convertedType.contains(QStringLiteral("QByteArray"))){
				if (!isQtCommentAdded){
					stream << QStringLiteral("// Qt includes");
					FeedStream(stream, 1, false);
					isQtCommentAdded = true;
				}
				stream << QStringLiteral("#include <QtCore/QByteArray>");
				FeedStream(stream, 1, false);
			}
			if (convertedType.contains(QStringLiteral("QString"))){
				if (!isQtCommentAdded){
					stream << QStringLiteral("// Qt includes");
					FeedStream(stream, 1, false);
					isQtCommentAdded = true;
				}
				stream << QStringLiteral("#include <QtCore/QString>");
				FeedStream(stream, 1, false);
			}
			if (convertedType.contains(QStringLiteral("QList"))){
				if (!isQtCommentAdded){
					stream << QStringLiteral("// Qt includes");
					FeedStream(stream, 1, false);
					isQtCommentAdded = true;
				}
				stream << QStringLiteral("#include <QtCore/QList>");
				FeedStream(stream, 1, false);
			}

			if (isCustom){
				imtsdl::CSdlField lookupField;
				lookupField.SetType(sdlType);
				std::shared_ptr<imtsdl::CSdlEntryBase> foundType = GetSdlTypeOrEnumOrUnionForField(lookupField, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
				if (!foundType){
					SendCriticalMessage(0, QString("Unable to find type %1").arg(sdlType));
					I_CRITICAL();
				}

				if (foundType->IsExternal()){
					QString resolvedPath = foundType->GetTargetHeaderFilePath();
					const QString relativeIncludePath = '<' + resolvedPath + '>';
					if (!resolvedPath.isEmpty() && !customIncludeList.contains(relativeIncludePath)){
						customIncludeList << relativeIncludePath;
					}
				}
			}

			for (const QString& customInclude : customIncludeList){
				stream << QStringLiteral("#include ");
				stream << customInclude;
				FeedStream(stream, 1, false);
			}
		}

		FeedStream(stream, 2, false);

		// namespace begin
		QString sdlNamespace = GetNamespaceFromSchemaParams(sdlUnion.GetSchemaParams());

		stream << QStringLiteral("namespace ");
		stream << sdlNamespace;
		stream << QStringLiteral("\n{");
		FeedStream(stream, 3, false);

		// union class create
		stream << QStringLiteral("class ") << sdlUnion.GetName() << QStringLiteral(": public");
		stream << QStringLiteral(" std::variant<");

		// add all union types
		int countTypes = convertedTypeList.count();
		for (int i = 0; i < countTypes; ++i){
			stream << convertedTypeList[i];
			if (i < countTypes - 1){
				stream << QStringLiteral(", ");
			}
		}
		stream << QStringLiteral("> ");
		stream << QStringLiteral("{");
		FeedStream(stream, 2, false);
		
		stream << QStringLiteral("public:");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("typedef std::variant<");

		// add all union types
		for (int i = 0; i < countTypes; ++i){
			stream << convertedTypeList[i];
			if (i < countTypes - 1){
				stream << QStringLiteral(", ");
			}
		}
		stream << QStringLiteral("> BaseClass;");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 1);
		stream << sdlUnion.GetName() << QStringLiteral("(){};");
		FeedStream(stream, 1, false);

		for (int i = 0; i < countTypes; ++i){
			FeedStreamHorizontally(stream, 1);
			stream << sdlUnion.GetName() << QStringLiteral("(const ") << convertedTypeList[i] << QStringLiteral("& ref)");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral(": BaseClass(ref){};");
			FeedStream(stream, 2, false);
		}
		stream << QStringLiteral("};");
		FeedStream(stream, 2, false);
		
		QString unionTypeObjectName = QStringLiteral("C%1Object").arg(sdlUnion.GetName());
		stream << QStringLiteral("class %1: public ::imtbase::CItemModelBase").arg(unionTypeObjectName);
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 0);
		stream << QStringLiteral("{");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_OBJECT");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 0);
		stream << QStringLiteral("public:");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("typedef ::imtbase::CItemModelBase BaseClass;");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("%1(QObject* parent = nullptr): BaseClass(parent) {}").arg(unionTypeObjectName);
		FeedStream(stream, 2, false);

		// GetType function begin
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_INVOKABLE QString GetType() const{");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return m_type;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("}");
		FeedStream(stream, 2, false);
		// GetType function end

		// SetValue function begin
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_INVOKABLE void SetValue(const QVariant& value){");
		FeedStream(stream, 1, false);

		// for (const QString& unionType : convertedTypeList){
		QStringList unionTypeList = sdlUnion.GetTypes();
		for (int unionIndex = 0; unionIndex < unionTypeList.count(); unionIndex++){
			QString unionTypeWithNamespace = convertedTypeList[unionIndex];
			QString unionType = unionTypeList[unionIndex];
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (value.canConvert<") << unionTypeWithNamespace;
			stream << QStringLiteral(">()){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("m_type = ") << QStringLiteral("\"") << unionType << QStringLiteral("\";");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("}");
			FeedStream(stream, 2, false);
		}

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("m_value = value;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("}");
		FeedStream(stream, 2, false);
		// SetValue function end

		// GetValue function begin
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_INVOKABLE QVariant GetValue(){");
		FeedStream(stream, 1, false);

		// for (const QString& unionType : convertedTypeList){
		// 	QString unionTypeObjectClassName = unionType + QStringLiteral("Object");
		// 	QString unionTypeVarName = QStringLiteral("m_%1Object").arg(unionType.toLower());

		// 	FeedStreamHorizontally(stream, 2);
		// 	stream << QStringLiteral("if (const %1* val = std::get_if<%1>((*Version_1_0).get())){").arg(unionType);
		// 	FeedStream(stream, 1, false);

		// 	FeedStreamHorizontally(stream, 3);
		// 	stream << QStringLiteral("if (!%1){").arg(unionTypeVarName);
		// 	FeedStream(stream, 1, false);

		// 	FeedStreamHorizontally(stream, 4);
		// 	stream << unionTypeVarName << QStringLiteral("= new %1(this);").arg(unionTypeObjectClassName);
		// 	FeedStream(stream, 1, false);

		// 	FeedStreamHorizontally(stream, 3);
		// 	stream << QStringLiteral("}");
		// 	FeedStream(stream, 2, false);

		// 	FeedStreamHorizontally(stream, 3);
		// 	stream << QStringLiteral("return QVariant::fromValue(%1);").arg(unionTypeVarName);
		// 	FeedStream(stream, 1, false);

		// 	FeedStreamHorizontally(stream, 2);
		// 	stream << QStringLiteral("}");
		// 	FeedStream(stream, 2, false);
		// }

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return QVariant();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("}");
		FeedStream(stream, 2, false);
		// GetValue function end

		FeedStreamHorizontally(stream, 0);
		stream << QStringLiteral("signals:");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("void typeChanged();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("void valueChanged();");
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 0);
		stream << QStringLiteral("public:");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("istd::TSharedNullable<std::shared_ptr<%1>> Version_1_0;").arg(sdlUnion.GetName());
		FeedStream(stream, 1, false);

		// for (const QString& unionType : unionTypeList){
		// 	QString unionTypeObjectClassName = unionType + QStringLiteral("Object");
		// 	QString unionTypeVarName = QStringLiteral("m_%1Object").arg(unionType.toLower());

		// 	FeedStreamHorizontally(stream, 1);
		// 	stream << QStringLiteral("%1* %2;").arg(unionTypeObjectClassName, unionTypeVarName);
		// 	FeedStream(stream, 1, false);
		// }

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("QVariant m_value;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("QString m_type;");
		FeedStream(stream, 1, false);
	
		FeedStreamHorizontally(stream, 0);
		stream << QStringLiteral("};");
		FeedStream(stream, 2, false);
		// Union Type end

		// end of namespace
		stream << QStringLiteral("} // namespace ");
		stream << sdlNamespace;
		FeedStream(stream, 2, true);
	}

	return TS_OK;
}



} // namespace imtsdlgencpp

