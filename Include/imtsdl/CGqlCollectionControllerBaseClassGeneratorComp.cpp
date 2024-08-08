#include "CGqlCollectionControllerBaseClassGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <iprm/CEnableableParam.h>
#include <ifile/CFileNameParam.h>

// imtsdl includes
#include <imtfile/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{


int CGqlCollectionControllerBaseClassGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlDocumentListCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsGqlEnabled()){
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

	const QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_sourceFileType);

	SdlDocumentTypeList sdlDocumentTypeList = m_sdlDocumentListCompPtr->GetDocumentTypes();
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
			if (!joinHeaders) {
				std::cout << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath).toStdString() << std::endl;
			}
			if (!joinSources){
				std::cout << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath).toStdString() << std::endl;
			}
		}
		if (joinHeaders){
			std::cout << joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType].toStdString() << std::endl;
		}
		if (joinSources){
			std::cout << joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType].toStdString() << std::endl;
		}

		return TS_OK;
	}

	for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
		m_headerFilePtr.SetPtr(new QFile(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath)));
		m_sourceFilePtr.SetPtr(new QFile(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath)));

		if (!ProcessFiles(sdlDocumentType, !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to finalize files"));
			I_CRITICAL();

			return TS_INVALID;
		}
	}

	// join files if required
	if (!joinRules.isEmpty()){
		if (m_filesJoinerCompPtr.IsValid()){
			iprm::CParamsSet inputParams;
			ifile::CFileNameParam sourceDirPathParam;
			sourceDirPathParam.SetPath(outputDirectoryPath);
			inputParams.SetEditableParameter(imtfile::CSimpleFileJoinerComp::s_sourceDirPathParamId, &sourceDirPathParam);
			ifile::CFileNameParam outputFileNameParam;
			inputParams.SetEditableParameter(imtfile::CSimpleFileJoinerComp::s_targetFilePathParamId, &outputFileNameParam);
			iprm::CEnableableParam appendEnableParam;
			appendEnableParam.SetEnabled(true);
			inputParams.SetEditableParameter(imtfile::CSimpleFileJoinerComp::s_appendModeParamId, &appendEnableParam);

			iprm::COptionsManager filterParams;

			if (joinHeaders){
				filterParams.ResetOptions();
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					filterParams.InsertOption(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h")), QByteArray::number(filterParams.GetOptionsCount()));
				}

				outputFileNameParam.SetPath(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					QFile::remove(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath));
				}
			}
			if (joinSources){
				filterParams.ResetOptions();
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					filterParams.InsertOption(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp")), QByteArray::number(filterParams.GetOptionsCount()));
				}

				const QString sourceFilePath = joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
				outputFileNameParam.SetPath(sourceFilePath);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join cource  files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					QFile::remove(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath));
				}

				// add joined header include directive
				if (joinHeaders){
					QFile joinedSourceFile(sourceFilePath);
					if (!joinedSourceFile.open(QIODevice::ReadWrite)){
						SendCriticalMessage(0, QString("Unable to open joined filee '%1'").arg(sourceFilePath));
						I_CRITICAL();

						return TS_INVALID;
					}
					QFileInfo headerFileInfo(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
					QByteArray sourceReadData = joinedSourceFile.readAll();
					joinedSourceFile.seek(0);
					QByteArray includeDirective = QByteArrayLiteral("#include ");
					includeDirective.append('"').append(headerFileInfo.fileName().toUtf8()).append('"');
					includeDirective.append('\n').append('\n').append('\n');
					sourceReadData.prepend(includeDirective);
					joinedSourceFile.write(sourceReadData);
				}
			}
		}
	}

	return TS_OK;
}


// private static methods

QString CGqlCollectionControllerBaseClassGeneratorComp::WrapFileName(const QString& baseName, const QString& ext, const QString& directoryPath)
{
	QString retVal;
	Q_ASSERT_X(!ext.isEmpty(), __func__, "Extension for file MUST be specified");

	if (!directoryPath.isEmpty()){
		retVal.append(directoryPath);
		if (!retVal.endsWith('/') || !retVal.endsWith(QDir::separator())){
			retVal.append('/');
		}
	}
	retVal += 'C';
	retVal.append(baseName);
	retVal.append(QStringLiteral("CollectionControllerCompBase."));
	retVal.append(ext);

	return retVal;
}


// private methods

bool CGqlCollectionControllerBaseClassGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	Q_ASSERT(retVal);

	retVal = m_sourceFilePtr->flush() && retVal;
	Q_ASSERT(retVal);

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessFiles(const CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude, bool addSelfHeaderInclude)
{
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	bool retVal = true;
	retVal = retVal && ProcessHeaderClassFile(sdlDocumentType, addDependenciesInclude);
	retVal = retVal && ProcessSourceClassFile(sdlDocumentType, addSelfHeaderInclude);

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessHeaderClassFile(const CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	// namespace begin
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream <<  sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream <<  QStringLiteral("{");
		FeedStream(ifStream, 1, false);
	}

	// public section
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	// base class methods override definition
	ifStream << QStringLiteral("// reimplemented (imtgql::CObjectCollectionControllerCompBase)");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	ifStream << QStringLiteral("virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	ifStream << QStringLiteral("virtual imtbase::CTreeItemModel* InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	ifStream << QStringLiteral("virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	ifStream << QStringLiteral("virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	ifStream << QStringLiteral("virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);

	// protected section
	// definition of pure virtual methods (to be reimplemented)




	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessSourceClassFile(const CSdlDocumentType& sdlDocumentType, bool addSelfHeaderInclude)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	if (addSelfHeaderInclude){
		ifStream << QStringLiteral("#include \"");
		ifStream << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h")) << '"';
		FeedStream(ifStream, 2);
	}
	FeedStream(ifStream, 1);

	// namespace begin
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream << sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream << '{';
		FeedStream(ifStream, 1, false);
	}

	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");
	// implementation of methods



	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


void CGqlCollectionControllerBaseClassGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


} // namespace imtsdl

