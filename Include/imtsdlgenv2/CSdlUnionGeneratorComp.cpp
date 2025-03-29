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
#include <imtfile/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


iproc::IProcessor::TaskState CSdlUnionGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
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

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
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

		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
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
				}
				PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);
				PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
			}
		}

		return TS_OK;
	}

	for (const imtsdl::CSdlUnion& sdlUnion: sdlUnionList){
		QFile unionFile(QString(outputDirectoryPath + "/" + sdlUnion.GetName() + ".h"));
		if (!unionFile.open(QIODevice::WriteOnly)){
			SendErrorMessage(0, QString("Unable to open file '%1'. Error: %2").arg(unionFile.fileName(), unionFile.errorString()));

			return TS_INVALID;
		}

		QTextStream stream(&unionFile);

		stream << QStringLiteral("#pragma once");
		FeedStream(stream, 2, false);
		stream << QStringLiteral("// STD includes");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#include <variant>");
		FeedStream(stream, 2, false);

		bool isQtCommentAdded = false;
		for (const auto& sdlType : sdlUnion.GetTypes()){
			// Add Qt types
			if (sdlType.first.contains(QStringLiteral("QByteArray"))){
				if (!isQtCommentAdded){
					stream << QStringLiteral("// Qt includes");
					FeedStream(stream, 1, false);
					isQtCommentAdded = true;
				}
				stream << QStringLiteral("#include <QtCore/QByteArray>");
				FeedStream(stream, 1, false);
			}
			if (sdlType.first.contains(QStringLiteral("QString"))){
				if (!isQtCommentAdded){
					stream << QStringLiteral("// Qt includes");
					FeedStream(stream, 1, false);
					isQtCommentAdded = true;
				}
				stream << QStringLiteral("#include <QtCore/QString>");
				FeedStream(stream, 1, false);
			}
			if (sdlType.first.contains(QStringLiteral("QList"))){
				if (!isQtCommentAdded){
					stream << QStringLiteral("// Qt includes");
					FeedStream(stream, 1, false);
					isQtCommentAdded = true;
				}
				stream << QStringLiteral("#include <QtCore/QList>");
				FeedStream(stream, 1, false);
			}

			if (sdlType.second){
				stream << QStringLiteral("// ") << GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr) << QStringLiteral(" includes");
				FeedStream(stream, 1, false);
				stream << QStringLiteral("#include \"") << sdlType.first << QStringLiteral(".h\"");
				FeedStream(stream, 1, false);
			}

			//// Add user's custom types
			//if (addDependenciesInclude){
			//	// first add include comment
			//	if (!complexTypeList.isEmpty()){
			//		stream << QStringLiteral("// ") << GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr) << QStringLiteral(" includes");
			//		FeedStream(stream, 1, false);
			//	}

			//	// then add inclides
			//	for (QSet<QString>::const_iterator complexIter = complexTypeList.cbegin(); complexIter != complexTypeList.cend(); ++complexIter){
			//		const QString& complexTypeName = *complexIter;
			//		stream << QStringLiteral("#include \"") << complexTypeName << QStringLiteral(".h\"");
			//		FeedStream(stream, 1, false);
			//	}
			//}
		}

		FeedStream(stream, 2, false);

		// namespace begin
		QString sdlNamespace = GetNamespaceFromSchemaParams(sdlUnion.GetSchemaParams());

		stream << QStringLiteral("namespace ");
		stream << sdlNamespace;
		stream << QStringLiteral("\n{");
		FeedStream(stream, 3, false);

		// enum create
		stream << QStringLiteral("typedef std::variant<");

		// add all union types
		int countTypes = sdlUnion.GetTypes().count();
		for (int i = 0; i < countTypes; ++i){
			stream << sdlUnion.GetTypes()[i].first;
			if (i < countTypes - 1){
				stream << QStringLiteral(", ");
			}
		}

		stream << QStringLiteral("> ");
		stream << sdlUnion.GetName();
		stream << QStringLiteral(";");
		FeedStream(stream, 2, false);

		// end of namespace
		stream << QStringLiteral("} // namespace ");
		stream << sdlNamespace;
		FeedStream(stream, 2, true);
	}

	return TS_OK;
}



} // namespace imtsdlgenv2

