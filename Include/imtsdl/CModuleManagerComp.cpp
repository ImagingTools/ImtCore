#include <imtsdl/CModuleManagerComp.h>


// Qt includes
#include <QtCore/QDirIterator>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include<iprm/TParamsPtr.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdl/CSdlUnion.h>
#include <imtsdl/ModuleInfo.h>


namespace imtsdl
{

// static helpers

static std::optional<CEntity> FindEntryByName(const std::optional<QList<CEntity>>& moduleEntry, const QString& itemId)
{
	if (!moduleEntry){
		return std::optional<CEntity>();
	}

	auto foundIter = std::find_if(moduleEntry->cbegin(), moduleEntry->cend(), [&itemId](const CEntity& type){
		return (type.Name && *type.Name == itemId);
	});

	if (foundIter != moduleEntry->cend()){
		return *foundIter;
	}

	return std::optional<CEntity>();
}


template <class _ContainerClass>
static std::shared_ptr<CSdlEntryBase> FindValueInContainer(const _ContainerClass& container, const QString itemName)
{
	auto foundIter = std::find_if(container.cbegin(), container.cend(),
								[&itemName](const typename _ContainerClass::value_type& containerItem)
	{
		return containerItem.GetName() == itemName;
	});

	if (foundIter != container.cend()){
		typename _ContainerClass::value_type retVal = typename _ContainerClass::value_type(*foundIter);

		return std::make_shared<typename _ContainerClass::value_type>(retVal);
	}

	return nullptr;
}


static void FillItemInfo(const std::optional<CEntity>& entry, IModuleManager::ItemInfo* itemInfoPtr)
{
	if (!entry || itemInfoPtr == nullptr){
		return;
	}
	if (entry->QmlModuleName){
		itemInfoPtr->QmlModuleName = *entry->QmlModuleName;
	}
	if (entry->TargerHeaderFilePath){
		itemInfoPtr->TargerHeaderFilePath = *entry->TargerHeaderFilePath;
	}
}


static void ResolveAbsolutePath(std::optional<QList<CEntity>>& entryList, const QString& rootPath)
{
	if (!entryList || entryList->isEmpty() || rootPath.isEmpty()){
		return;
	}

	QDir rootDir(rootPath);
	for (CEntity& entry: *entryList){
		QString& entryPath = *entry.TargerHeaderFilePath;

		if (entryPath.startsWith(s_moduleDirVariable)){
			entryPath.replace(s_moduleDirVariable, rootDir.absolutePath());
		}
	}
}


// static helpers
template <class _ContainerType>
void UpdateModuleInfo(std::optional<QList<CEntity>>& moduleContainer, const QList<_ContainerType>& fromList, const QString& moduleRootPath)
{
	if (fromList.isEmpty()){
		return;
	}

	if(!moduleContainer){
		moduleContainer.emplace();
	}

	for (auto containerElement: fromList){
		if (containerElement.IsExternal()){
			// process only
			continue;
		}

		CEntity entity;
		entity.Name = containerElement.GetName();
		QString relativeHeaderFilePath = containerElement.GetTargetHeaderFilePath();
		QFileInfo moduleRootDirInfo(moduleRootPath);
		if (moduleRootDirInfo.isDir() && moduleRootDirInfo.exists()){
			QDir moduleRootDir(moduleRootDirInfo.absoluteFilePath());
			relativeHeaderFilePath = s_moduleDirVariable + '/' + moduleRootDir.relativeFilePath(relativeHeaderFilePath);
		}
		entity.TargerHeaderFilePath = relativeHeaderFilePath;
		/// \todo add later \code{.cpp} entity.QmlModuleName = containerElement.GetQmlModuleName(); \endcode
		moduleContainer->append(entity);
	}

	if (moduleContainer->isEmpty()){
		moduleContainer.reset();
	}
}


// public methods

// reimplemented(IModuleManager)

bool CModuleManagerComp::Initialize(const QStringList& paths)
{
	if (!m_modules.isEmpty()){
		SendWarningMessage(0, "Several calls of 'SetSearchPaths'");
		m_modules.clear();
	}

	// remove potential subdirs
	QStringList pathsToProcess = paths;

	bool pathRemoved = false;
	pathsToProcess.sort();
	QMutableListIterator pathsIter(pathsToProcess);
	while(pathsIter.hasNext()){
		QString path = pathsIter.next();

		// finish if only one left
		if (pathsToProcess.size() == 1){
			break;
		}

		// check if we at last
		if (!pathsIter.hasNext()){
			if (pathRemoved){ // rescan if at least one path removed
				pathRemoved = false;
				pathsIter.toFront();
				path = pathsIter.next();
			}
			else { // finish if no paths removed
				break;
			}
		}

		if (!path.endsWith('/')){
			path.append('/');
		}
		QString nextPath = pathsIter.peekNext();
		if (!nextPath.endsWith('/')){
			nextPath.append('/');
		}
		if (path.length() < nextPath.length()){
			if (nextPath.startsWith(path)){
				pathsIter.next();
				pathsIter.remove();
				pathRemoved = true;
				if (!pathsIter.hasNext()){
					pathsIter.toFront();
				}
			}
		}
		else {
			if (path.startsWith(nextPath)){
				pathsIter.remove();
				pathRemoved = true;
			}
		}
	}

	// begin process
	QSet<QString> processedFiles;
	for (const QString& path: pathsToProcess){
		QDirIterator dirIterator(
			path,
			{QStringLiteral("*.jsmi")},
			QDir::Files,
			QDirIterator::Subdirectories);

		while (dirIterator.hasNext()){
#if QT_VERSION > QT_VERSION_CHECK(6, 3, 0)
			const QFileInfo currentDirEntry = dirIterator.nextFileInfo();
#else
			QString dirEntryPath = dirIterator.next();
			QFileInfo currentDirEntry(dirEntryPath);
#endif
			if (processedFiles.contains(currentDirEntry.absoluteFilePath())){
				continue;
			}

			processedFiles << currentDirEntry.absoluteFilePath();
			QFile moduleFile(currentDirEntry.absoluteFilePath());
			if (!moduleFile.open(QIODevice::ReadOnly)){
				SendErrorMessage(0, QString("Unable to open file '%1'").arg(moduleFile.fileName()));

				return false;
			}

			const QByteArray readData = moduleFile.readAll();
			const QJsonDocument moduleDocument = QJsonDocument::fromJson(readData);
			if (!moduleDocument.isObject()){
				SendErrorMessage(0, QString("Unexpected file data '%1'").arg(moduleFile.fileName()));

				return false;
			}

			CModule module;
			if(!module.ReadFromJsonObject(moduleDocument.object())){
				SendErrorMessage(0, QString("Unable to parse file '%1'").arg(moduleFile.fileName()));

				return false;
			}
			const QString rootPath = currentDirEntry.absoluteDir().absolutePath();
			ResolveAbsolutePath(module.Enums, rootPath);
			ResolveAbsolutePath(module.Types, rootPath);
			ResolveAbsolutePath(module.Unions, rootPath);

			m_modules << module;
		}
	}

	return true;
}


std::shared_ptr<CSdlEntryBase> CModuleManagerComp::GetItem(const QString& itemId, ItemInfo* itemInfoPtr) const
{
	for (const CModule& module: m_modules){
		std::optional<CEntity> foundEntry;

		// is type
		if (m_typeListProviderCompPtr.IsValid()){
			foundEntry = FindEntryByName(module.Types, itemId);
			if (foundEntry){
				FillItemInfo(foundEntry, itemInfoPtr);

				return FindValueInContainer(m_typeListProviderCompPtr->GetSdlTypes(false), itemId);
			}
		}

		// is enum
		if (m_enumListProviderCompPtr.IsValid()){
			foundEntry = FindEntryByName(module.Enums, itemId);
			if (foundEntry){
				FillItemInfo(foundEntry, itemInfoPtr);

				return FindValueInContainer(m_enumListProviderCompPtr->GetEnums(false), itemId);
			}
		}

		// is union
		if (m_unionListProviderCompPtr.IsValid()){
			foundEntry = FindEntryByName(module.Unions, itemId);
			if (foundEntry){
				FillItemInfo(foundEntry, itemInfoPtr);

				return FindValueInContainer(m_unionListProviderCompPtr->GetUnions(false), itemId);
			}
		}
	}

	return nullptr;
}


bool CModuleManagerComp::CreateModuleFile() const
{
	CModule jsmiModule;
	UpdateModuleInfo(jsmiModule.Types, m_typeListProviderCompPtr->GetSdlTypes(true), m_argumentParserCompPtr->GetOutputDirectoryPath());
	UpdateModuleInfo(jsmiModule.Enums, m_enumListProviderCompPtr->GetEnums(true), m_argumentParserCompPtr->GetOutputDirectoryPath());
	UpdateModuleInfo(jsmiModule.Unions, m_unionListProviderCompPtr->GetUnions(true), m_argumentParserCompPtr->GetOutputDirectoryPath());
	jsmiModule.Params.emplace();


	if (m_schemaParamsCompPtr.IsValid()){
		iprm::TParamsPtr<iprm::ITextParam> namespaceParamPtr(m_schemaParamsCompPtr.GetPtr(), SdlCustomSchemaKeys::SchemaNamespace.toUtf8(), false);
		if (namespaceParamPtr.IsValid()){
			jsmiModule.Params->Namespace = namespaceParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(m_schemaParamsCompPtr.GetPtr(), SdlCustomSchemaKeys::SchemaName.toUtf8(), false);
		if (nameParamPtr.IsValid()){
			jsmiModule.Params->Name = nameParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(m_schemaParamsCompPtr.GetPtr(), SdlCustomSchemaKeys::VersionName.toUtf8(), false);
		if (versionNameParamPtr.IsValid()){
			jsmiModule.Params->Version = versionNameParamPtr->GetText();
		}
	}

	QJsonObject jsmiJsonData;
	// data always MUST write
	if (!jsmiModule.WriteToJsonObject(jsmiJsonData)){
		SendCriticalMessage(0, "Unable to create module info");
		I_CRITICAL();

		return false;
	}

	QString jsmiFilePath = m_argumentParserCompPtr->GetModuleOutputFilePath();
	if (jsmiFilePath.isEmpty()){
		jsmiFilePath = m_argumentParserCompPtr->GetOutputDirectoryPath() + '/';
		if (jsmiModule.Params->Name){
			jsmiFilePath += *jsmiModule.Params->Name;
		}
		else {
			jsmiFilePath += QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
		}
		if (jsmiModule.Params->Version){
			jsmiFilePath += 'V' + *jsmiModule.Params->Version;
		}
	}
	if (!jsmiFilePath.endsWith(QStringLiteral(".jsmi"))){
		jsmiFilePath += QStringLiteral(".jsmi");
	}

	QFile jsmiFile(jsmiFilePath);
	if (!jsmiFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unabl to create file '%1'").arg(jsmiFile.fileName()));

		return false;
	}

	jsmiFile.write(QJsonDocument(jsmiJsonData).toJson(QJsonDocument::Compact));
	jsmiFile.close();

	return true;
}


IModuleManager::ItemInfo CModuleManagerComp::GetItemInfo(const CSdlEntryBase& item) const
{
	ItemInfo retVal;
	for (const CModule& module: m_modules){
		std::optional<CEntity> foundEntry;

		// is type
		try{
			const CSdlType& type = dynamic_cast<const CSdlType&>(item);
			if (m_typeListProviderCompPtr.IsValid()){
				foundEntry = FindEntryByName(module.Types, type.GetName());
				if (foundEntry){
					FillItemInfo(foundEntry, &retVal);
				}
			}
		}
		catch (const std::bad_cast&){
			// Not a type. Nothing todo
		}

		// is enum
		try{
			const CSdlEnum& enumValue = dynamic_cast<const CSdlEnum&>(item);
			if (m_enumListProviderCompPtr.IsValid()){
				foundEntry = FindEntryByName(module.Enums, enumValue.GetName());
				if (foundEntry){
					FillItemInfo(foundEntry, &retVal);
				}
			}
		}
		catch (const std::bad_cast&){
			// Not an enum. Nothing todo
		}

		// is union
		try{
			const CSdlUnion& unionValue = dynamic_cast<const CSdlUnion&>(item);
			if (m_unionListProviderCompPtr.IsValid()){
				foundEntry = FindEntryByName(module.Unions, unionValue.GetName());
				if (foundEntry){
					FillItemInfo(foundEntry, &retVal);
				}
			}
		}
		catch (const std::bad_cast&){
			// Not an union. Nothing todo
		}
	}

	return retVal;
}


} // namespace imtsdl
