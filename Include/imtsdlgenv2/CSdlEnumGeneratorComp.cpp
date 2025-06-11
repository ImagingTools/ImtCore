#include "CSdlEnumGeneratorComp.h"


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


iproc::IProcessor::TaskState CSdlEnumGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
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

	imtsdl::SdlEnumList sdlEnumList = m_sdlEnumListCompPtr->GetEnums(true);
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			// in case if auto join, nothing todo, because files already defined.
			return TS_OK;
		}
		else {
			const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
			QStringList cumulatedFiles;
			for (const imtsdl::CSdlEnum& sdlEnum: sdlEnumList){
				if (!joinHeaders){
					cumulatedFiles << QString(outputDirectoryPath + "/" + sdlEnum.GetName() + ".h");
				}
				PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);
				PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
			}
		}

		return TS_OK;
	}

	for (const imtsdl::CSdlEnum& sdlEnum: sdlEnumList){
		QFile enumFile(QString(outputDirectoryPath + "/" + sdlEnum.GetName() + ".h"));
		if (!enumFile.open(QIODevice::WriteOnly)){
			SendErrorMessage(0, QString("Unable to open file '%1'. Error: %2").arg(enumFile.fileName(), enumFile.errorString()));

			return TS_INVALID;
		}

		QTextStream stream(&enumFile);

		stream << QStringLiteral("#pragma once");
		FeedStream(stream, 3, false);

		// namespace begin
		QString sdlNamespace = GetNamespaceFromSchemaParams(sdlEnum.GetSchemaParams());

		stream << QStringLiteral("namespace ");
		stream << sdlNamespace;
		stream << QStringLiteral("\n{");
		FeedStream(stream, 3, false);

		// enum create
		stream << QStringLiteral("enum class ");
		stream << sdlEnum.GetName();
		stream << QStringLiteral(" {");
		FeedStream(stream, 1, false);

		// add all enum values
		for (const auto& enumValue: sdlEnum.GetValues()){
			FeedStreamHorizontally(stream);
			stream << enumValue.first;
			stream << ',';
			FeedStream(stream, 1, false);
		}

		stream << QStringLiteral("};");
		FeedStream(stream, 2, false);

		// end of namespace
		stream << QStringLiteral("} // namespace ");
		stream << sdlNamespace;
		FeedStream(stream, 2, true);
	}

	return TS_OK;
}



} // namespace imtsdlgenv2

