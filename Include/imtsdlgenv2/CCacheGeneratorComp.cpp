#include "CCacheGeneratorComp.h"


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QLockFile>
#include <QtCore/QDir>

// Acf includes
#include <istd/CSystem.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


int CCacheGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_cacheControllerCompPtr.IsValid());

	const QString cachePath = QDir::cleanPath(m_argumentParserCompPtr->GetCachePath());
	if (cachePath.isEmpty()){
		// nothing todo
		return TS_OK;
	}

	// init cache controller and check required data
	std::shared_ptr<QFile> cacheFilePtr(new QFile(cachePath));
	bool deviceSetted = m_cacheControllerCompPtr->SetOperationalDevice(cacheFilePtr);
	if (!deviceSetted){
		SendErrorMessage(0, QString("Unable to set operational file for cache controller '%1'").arg(cachePath));

		return TS_INVALID;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	const QString schemaFileNameName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();

	// guard a cache file, while a cache is generating...
	QString lockFilePath = cachePath + QStringLiteral(".lock");
	QLockFile cacheLockFile(lockFilePath);
	bool isLocked = cacheLockFile.lock();
	if (!isLocked){
		SendErrorMessage(0, QString("Unable to guard cache file at '%1'. ErrorCode: %2").arg(lockFilePath, cacheLockFile.error()));

		return TS_INVALID;
	}

	imtsdl::ICacheController::LoadError loadResult = m_cacheControllerCompPtr->LoadFromData();

	// expected OK or NULL CACHE, because we need to create a cache. It might be empty
	if (loadResult != imtsdl::ICacheController::LE_OK && loadResult != imtsdl::ICacheController::LE_NULL_CACHE){
		SendErrorMessage(0, "Unable to load cache");

		return TS_INVALID;
	}

	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		if (!m_schemaParamsCompPtr.IsValid()){
			SendErrorMessage(0, "Application is not configured with schema parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

			return TS_INVALID;
		}

		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_schemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), schemaFileNameName);
	}

	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);

	imtsdl::CCache& cache = m_cacheControllerCompPtr->GetCache();
	std::unique_ptr<istd::CChangeGroup> changeGroupPtr(new istd::CChangeGroup(&cache));

	// set include paths
	QStringList includePaths = m_argumentParserCompPtr->GetHeadersIncludePaths();
	QMutableListIterator includesIter(includePaths);

	// clean all paths
	while (includesIter.hasNext()){
		QString& path = includesIter.next();
		path = QDir::cleanPath(path);
	}
	cache.SetIncludePaths(includePaths);

	// generate cache for all LOCAL types
	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		imtsdl::CCacheEntry cacheTypeEntry;
		cacheTypeEntry.SetTypeId(sdlType.GetName());
		cacheTypeEntry.SetQmlEntryName(sdlType.GetQmlImportDeclaration());
		cacheTypeEntry.SetNamespace(GetNamespaceFromTypeSchemaParams(sdlType));
		cacheTypeEntry.SetVersion(GetTypeVersion(sdlType));

		QString targetHeaderPath = sdlType.GetTargetHeaderFilePath();
		if (joinHeaders){
			targetHeaderPath = joinRules.value(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
		}
		targetHeaderPath = QDir::cleanPath(targetHeaderPath);
		cacheTypeEntry.SetTargetHeaderPath(targetHeaderPath);

		cache.AddTypesElement(cacheTypeEntry);
	}

	// complete change group
	changeGroupPtr.reset();

	// save cache
	bool saved = m_cacheControllerCompPtr->Save();
	if (!saved){
		SendErrorMessage(0, "Unable to save cache");

		return TS_INVALID;
	}

	return TS_OK;
}


} // namespace imtsdlgenv2

