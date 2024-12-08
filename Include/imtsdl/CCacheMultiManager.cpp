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
		ICacheController::LoadError loadError =  controllerPtr->LoadFromData();
		if (loadError != ICacheController::LE_OK && loadError != ICacheController::LE_NULL_CACHE){
			SendErrorMessage(0, "Unable to load cache");

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

		const istd::TPointerVector<CCacheEntry>& entryList = cache.GetTypes();

		// lookup through all entries
		for (int index = 0; index < entryList.GetCount(); ++index){
			const CCacheEntry* entryPtr = entryList.GetAt(index);
			Q_ASSERT(entryPtr != nullptr);

			if (entryPtr->GetTypeId() == type.GetName()){
				const iprm::IParamsSet& schemaParams = type.GetSchemaParams();
				const QString typeNamespace = GetNamespaceFromSchemaParams(schemaParams);

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

						SendLogMessage(
							istd::IInformationProvider::IC_CRITICAL,
							0,
							QString("UNEXPECTED! Unable to find include path for found type. Type: %1").arg(type.GetName()),
							"CCacheMultiManager");
						Q_ASSERT_X(false, "UNEXPECTED! Unable to find include path for found type.", __func__);
					}
				}
			}
		}
	}

	return false;
}


QString CCacheMultiManager::GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix)
{
	QString retVal;
	if (!prefix.isEmpty()){
		retVal += prefix;
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::");
		}
	}

	QString schemaNamespace;
	iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(&schemaParams, imtsdl::SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
	if (namespaceParamPtr.IsValid()){
		schemaNamespace = namespaceParamPtr->GetText();
	}
	if (!schemaNamespace.isEmpty()){
		retVal += schemaNamespace;
		if (!retVal.isEmpty()){
			retVal += QStringLiteral("::");
		}
	}

	QString schemaName;
	iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&schemaParams, imtsdl::SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
	if (nameParamPtr.IsValid()){
		schemaName = nameParamPtr->GetText();
	}
	if (!schemaName.isEmpty()){
		retVal += schemaName;
	}

	return imtsdl::CSdlTools::GetNamespaceAcceptableString(retVal);
}


} // namespace imtsdl


