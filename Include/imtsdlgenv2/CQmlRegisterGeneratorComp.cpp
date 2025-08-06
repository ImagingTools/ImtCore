#include "CQmlRegisterGeneratorComp.h"
#include "imtsdl/CSdlEntryBase.h"
#include "istd/istd.h"


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


namespace imtsdlgenv2
{


// static helpers
void GenerateRegisterFunction(QTextStream& stream, const imtsdl::CSdlEntryBase& entry, bool isSingleton)
{
	imtsdl::CSdlTools::FeedStreamHorizontally(stream);

	const QString sdlNamespace = imtsdl::CSdlTools::BuildQmlImportDeclarationFromParams(entry.GetSchemaParams(), "Sdl", false);

	if (isSingleton){
		stream << QStringLiteral("qmlRegisterSingletonType<Enum") << imtsdl::CSdlTools::GetCapitalizedValue(entry.GetName());
		stream << QStringLiteral(">(") ;
		stream << QStringLiteral("\"") << sdlNamespace << QStringLiteral("\", 1, 0, ");
		stream << QStringLiteral("\"") << entry.GetName() << QStringLiteral("\"");
		stream << QStringLiteral(", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("Q_UNUSED(engine)");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("Q_UNUSED(scriptEngine)");
		imtsdl::CSdlTools::FeedStream(stream, 2, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("Enum") << entry.GetName() << QStringLiteral(" *enumType = new ");
		stream << QStringLiteral("Enum") << entry.GetName() << QStringLiteral("();");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return enumType;");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream);
		stream << QStringLiteral("});");

	}
	else{
		stream << QStringLiteral("qmlRegisterType<C") << entry.GetName() << QStringLiteral("Object>(") ;
		stream << QStringLiteral("\"") << sdlNamespace << QStringLiteral("\", 1, 0, ");
		stream << QStringLiteral("\"") << entry.GetName() << QStringLiteral("\");");
	}
	imtsdl::CSdlTools::FeedStream(stream, 1, false);
}


template <class T>
void GenerateRegisterFunctions(QTextStream& stream, const QList<T>& entryList, bool isSingleton)
{
	for (const auto& entry: entryList){
		GenerateRegisterFunction(stream, entry, isSingleton);
	}
}


// public methods

// reimplemented (iproc::IProcessor)

iproc::IProcessor::TaskState CQmlRegisterGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
#ifndef DISABLE_CREATE_SDL_QOBJECT
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
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

	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		if (!m_customSchemaParamsCompPtr.IsValid()){
			SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

			return TS_INVALID;
		}

		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, *m_argumentParserCompPtr);
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			// in case if auto join, nothing todo, because files already defined.
			return TS_OK;
		}
		else {
			/// \todo implement it
			I_CRITICAL();
		}

		return TS_OK;
	}

	const QString tempDirectoryPath = GetTempOutputPathFromParams(paramsPtr, outputDirectoryPath);

	QFile qmlRegisterFile(QString(tempDirectoryPath + QStringLiteral("/QmlRegister.h")));
	if (!qmlRegisterFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'. Error: %2").arg(qmlRegisterFile.fileName(), qmlRegisterFile.errorString()));

		return TS_INVALID;
	}

	QTextStream stream(&qmlRegisterFile);

	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 3, false);

	// namespace begin
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	stream << QStringLiteral("#ifdef QT_QML_LIB");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <QtQml/QQmlEngine>");
	FeedStream(stream, 3, false);
	stream << QStringLiteral("namespace ");
	stream << sdlNamespace;
	stream << QStringLiteral("\n{");
	FeedStream(stream, 3, false);

	// add function
	stream << QStringLiteral("static void RegisterQmlTypes()");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	GenerateRegisterFunctions(stream, m_sdlTypeListCompPtr->GetSdlTypes(true), false);
	GenerateRegisterFunctions(stream, m_sdlEnumListCompPtr->GetEnums(true), true);

	stream << '}';
	FeedStream(stream, 1, false);

	// end of namespace
	stream << QStringLiteral("} // namespace ");
	stream << sdlNamespace;
	FeedStream(stream, 1, true);
	stream << QStringLiteral("#endif");
	FeedStream(stream, 2, false);
#endif

	return TS_OK;
}


} // namespace imtsdlgenv2


