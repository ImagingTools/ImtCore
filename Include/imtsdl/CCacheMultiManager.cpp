#include <imtsdl/CCacheMultiManager.h>


// Qt includes
#include <QtCore/QDir>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


// public methods

// reimplemented (ICacheMultiManager)

bool CCacheMultiManager::Init(const QList<std::shared_ptr<ICacheController>>& controllers)
{
	// ensure, all caches are sxists and has a valid cache
	for (const std::shared_ptr<ICacheController>& controllerPtr: controllers){
		if (!controllerPtr){
			SendLogMessage(istd::IInformationProvider::IC_CRITICAL, 0, "Uninitialized coltroller provided!", "CCacheMultiManager");
			I_CRITICAL();

			return false;
		}
	}

	m_controllersPtrList = controllers;

	return true;
}


bool CCacheMultiManager::ResolveIncludePathForType(const CSdlType& type, QString& foundPath) const
{
	// lookup through all cache controllers
	for (const std::shared_ptr<ICacheController>& controllerPtr: m_controllersPtrList){
		Q_ASSERT(controllerPtr);

		const CCache& cache = controllerPtr->GetCache();
		const istd::TPointerVector<CCacheEntry> entryList = cache.GetTypes();

		// lookup through all entries
		for (int index = 0; index < entryList.GetCount(); ++index){
			const CCacheEntry* entryPtr = entryList.GetAt(index);
			Q_ASSERT(entryPtr != nullptr);

			if (entryPtr->GetTypeId() == type.GetName()){
				const iprm::IParamsSet& schemaParams = type.GetSchemaParams();
				const QString typeNamespace = BuildNamespaceFromParams(schemaParams);

				if (entryPtr->GetNamespace() == typeNamespace){
					QString typeVersionName = GetTypeVersion(type);

					if (entryPtr->GetVersion() == typeVersionName){
						const QStringList includePaths = cache.GetIncludePaths();
						const QString entryTargetFile = entryPtr->GetTargetHeaderPath();

						for (const QString& includePath: includePaths){
							if (entryTargetFile.startsWith(includePath)){
								QDir includeDir(includePath);
								foundPath = includeDir.relativeFilePath(entryTargetFile);

								return true;
							}
						}

						Q_ASSERT_X(false, "UNEXPECTED! Unable to find include path for found type.", __func__);
					}
				}
			}
		}
	}

	return false;
}


} // namespace imtsdl


