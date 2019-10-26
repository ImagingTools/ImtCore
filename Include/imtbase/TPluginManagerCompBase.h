#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QLibrary>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

// Windows includes
#ifdef Q_OS_WIN
	#include <windows.h>
#endif

// ACF includes
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>
#include <iprm/IParamsManager.h>
#include <iprm/IOptionsList.h>
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>


namespace imtbase
{


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
class TPluginManagerCompBase:
			public ilog::CLoggerComponentBase,
			protected imod::CSingleModelObserverBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::CSingleModelObserverBase BaseClass2;

	I_BEGIN_BASE_COMPONENT(TPluginManagerCompBase);
		I_ASSIGN(m_settingsCompPtr, "Settings", "Plugin settings", true, "Settings");
		I_ASSIGN_TO(m_settingsModelCompPtr, m_settingsCompPtr, true);
		I_ASSIGN(m_pluginPathsManagerIdAttrPtr, "PluginPathsManagerId", "ID of the plugin path manager parameter in the application settings", true, "PluginPathsManager");
		I_ASSIGN(m_pluginPathIdAttrPtr, "PluginPathId", "ID of the plugin path parameter in the parameter set", true, "PluginPath");
		I_ASSIGN(m_defaultPluginPathIdAttrPtr, "DefaultPluginPathId", "ID of the default plug-in path parameter in the settings", true, "DefaultPluginPath");
		I_ASSIGN(m_pluginExtensionAttrPtr, "PluginFileExtension", "File extension for the plugin DLL", true, "dll");
	I_END_COMPONENT;

	TPluginManagerCompBase(const QByteArray& createMethodName, const QByteArray& destroyMethodName);

protected:
	virtual void LoadPlugins();
	virtual bool LoadPluginDirectory(const QString& pluginDirectoryPath);

	/**
		Do some initialization for concrete plugin implementation.
		You should re-implement this method, if you want to execute some actions during the initialization stage of the plugin.
		The default implementation does nothing.
	*/
	virtual bool InitializePlugin(PluginInterface* pluginPtr);

	/**
		Callback after creation of the plug-in list. The default implementation does nothing.
	*/
	virtual void OnPluginsCreated();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

protected:
	struct PluginInfo
	{
		PluginInfo()
			:pluginPtr(NULL)
		{
		}

		PluginInterface* pluginPtr;
		QString pluginPath;
		DestroyFunction destroyFunc;
	};

	typedef QList<PluginInfo> Plugins;
	Plugins m_plugins;

private:
	I_REF(iprm::IParamsSet, m_settingsCompPtr);
	I_REF(imod::IModel, m_settingsModelCompPtr);
	I_ATTR(QByteArray, m_pluginPathsManagerIdAttrPtr);
	I_ATTR(QByteArray, m_pluginPathIdAttrPtr);
	I_ATTR(QByteArray, m_defaultPluginPathIdAttrPtr);
	I_ATTR(QByteArray, m_pluginExtensionAttrPtr);

	QByteArray m_createMethodName;
	QByteArray m_destroyMethodName;
};


// public methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::TPluginManagerCompBase(const QByteArray& createMethodName, const QByteArray& destroyMethodName)
	:m_createMethodName(createMethodName),
	m_destroyMethodName(destroyMethodName)
{
}


// protected methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::LoadPlugins()
{
	if (m_settingsCompPtr.IsValid()){
		// Load default plug-in directory:
		iprm::TParamsPtr<ifile::IFileNameParam> defaultPluginPathParamPtr(m_settingsCompPtr.GetPtr(), *m_defaultPluginPathIdAttrPtr);
		if (defaultPluginPathParamPtr.IsValid()){
			QString defaultPluginFolderPath = defaultPluginPathParamPtr->GetPath();
			if (QFileInfo(defaultPluginFolderPath).exists()){
				LoadPluginDirectory(defaultPluginFolderPath);
			}
		}

		iprm::TParamsPtr<iprm::IParamsManager> pluginPathManagerPtr(m_settingsCompPtr.GetPtr(), *m_pluginPathsManagerIdAttrPtr);
		if (pluginPathManagerPtr.IsValid()){
			const iprm::IOptionsList* paramListPtr = pluginPathManagerPtr->GetSelectionConstraints();

			int pluginPathsCount = pluginPathManagerPtr->GetParamsSetsCount();
			for (int pluginPathIndex = 0; pluginPathIndex < pluginPathsCount; ++pluginPathIndex){
				
				bool isEnabled = true;
				if (paramListPtr != NULL){
					isEnabled = paramListPtr->IsOptionEnabled(pluginPathIndex);
				}

				if (isEnabled){
					iprm::TParamsPtr<ifile::IFileNameParam> pluginPathParamPtr(pluginPathManagerPtr->GetParamsSet(pluginPathIndex), *m_pluginPathIdAttrPtr);
					if (pluginPathParamPtr.IsValid()){
						QString pluginsDirectoryPath = pluginPathParamPtr->GetPath();

						LoadPluginDirectory(pluginsDirectoryPath);
					}
				}
			}
		}
	}

	OnPluginsCreated();
}


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
bool TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::LoadPluginDirectory(const QString& pluginDirectoryPath)
{
	SendInfoMessage(0, QString("Looking for the plug-ins in '%1'").arg(pluginDirectoryPath));

	if (!pluginDirectoryPath.isEmpty() && QFileInfo(pluginDirectoryPath).exists()){
		QDir pluginsDirectory(pluginDirectoryPath);

		QFileInfoList pluginsList = pluginsDirectory.entryInfoList(QStringList() << (QString("*.") + *m_pluginExtensionAttrPtr));

		for (const QFileInfo& pluginPath : pluginsList){
#ifdef Q_OS_WIN
			SetDllDirectory(pluginPath.absolutePath().toStdWString().c_str());
#endif
			SendInfoMessage(0, QString("Load: '%1'").arg(pluginPath.canonicalFilePath()));

			QLibrary library(pluginPath.canonicalFilePath());
			if (library.load()){
				CreateFunction createPluginFunc = (CreateFunction)library.resolve(m_createMethodName);
				if (createPluginFunc != NULL){
					istd::TDelPtr<PluginInterface> pluginInstancePtr = createPluginFunc();
					if (pluginInstancePtr.IsValid()){
						PluginInfo pluginInfo;
						pluginInfo.pluginPath = pluginPath.canonicalFilePath();
						pluginInfo.pluginPtr = pluginInstancePtr.PopPtr();
						pluginInfo.destroyFunc = (DestroyFunction)library.resolve(m_destroyMethodName);

						if (InitializePlugin(pluginInfo.pluginPtr)){
							m_plugins.push_back(pluginInfo);
						}
						else{
							SendInfoMessage(0, QString("Plug-in initialization failed for: '%1'").arg(pluginPath.canonicalFilePath()));
						}
					}
				}
				else{
					SendErrorMessage(0, QString("Plug-in entry point was not found: '%1'").arg(pluginPath.canonicalFilePath()).arg(library.errorString()));
				}
			}
			else{
				SendErrorMessage(0, QString("%1").arg(library.errorString()));
			}
		}

		return true;
	}

	return false;
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::InitializePlugin(PluginInterface * pluginPtr)
{
	Q_UNUSED(pluginPtr);

	return true;
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::OnPluginsCreated()
{
}


// reimplemented (imod::CSingleModelObserverBase)

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	LoadPlugins();
}


// reimplemented (icomp::CComponentBase)

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_settingsModelCompPtr.IsValid()){
		m_settingsModelCompPtr->AttachObserver(this);
	}
}


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction>::OnComponentDestroyed()
{
	BaseClass2::EnsureModelDetached();

	for (auto iter = m_plugins.begin(); iter != m_plugins.end(); ++iter){
		iter->destroyFunc(iter->pluginPtr);
	}

	m_plugins.clear();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


