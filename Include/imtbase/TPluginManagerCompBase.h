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
#include <ifile/IFileNameParam.h>
#include <imod/IModel.h>
#include <iprm/IOptionsList.h>
#include <iprm/IParamsManager.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/TParamsPtr.h>
#include <imod/CSingleModelObserverBase.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>


namespace imtbase
{


template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass = ilog::CLoggerComponentBase>
class TPluginManagerCompBase: public BaseComponentClass
{
public:
	typedef BaseComponentClass BaseClass;

	I_BEGIN_BASE_COMPONENT(TPluginManagerCompBase);
		I_ASSIGN(m_settingsCompPtr, "Settings", "Plugin settings", true, "Settings");
		I_ASSIGN_TO(m_settingsModelCompPtr, m_settingsCompPtr, true);
		I_ASSIGN(m_pluginStatusMonitorCompPtr, "PluginStatusMonitor", "Plugin status monitor", true, "PluginStatusMonitor");
		I_ASSIGN(m_pluginPathsManagerIdAttrPtr, "PluginPathsManagerId", "ID of the plugin path manager parameter in the application settings", true, "PluginPathsManager");
		I_ASSIGN(m_pluginPathIdAttrPtr, "PluginPathId", "ID of the plugin path parameter in the parameter set", true, "PluginPath");
		I_ASSIGN(m_defaultPluginPathIdAttrPtr, "DefaultPluginPathId", "ID of the default plug-in path parameter in the settings", true, "DefaultPluginPath");
		I_ASSIGN(m_pluginExtensionAttrPtr, "PluginFileExtension", "File extension for the plugin DLL", true, "dll");
		I_ASSIGN(m_pluginTypeIdAttrPtr, "PluginTypeId", "Type-ID of supported plugins", true, "");
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

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	class SettingsObserver: public imod::CSingleModelObserverBase
	{
	public:
		SettingsObserver(TPluginManagerCompBase& parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	private:
		TPluginManagerCompBase& m_parent;
	};

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
	I_REF(imtbase::IPluginStatusMonitor, m_pluginStatusMonitorCompPtr);
	I_ATTR(QByteArray, m_pluginPathsManagerIdAttrPtr);
	I_ATTR(QByteArray, m_pluginPathIdAttrPtr);
	I_ATTR(QByteArray, m_defaultPluginPathIdAttrPtr);
	I_ATTR(QByteArray, m_pluginExtensionAttrPtr);
	I_ATTR(QByteArray, m_pluginTypeIdAttrPtr);

	QByteArray m_createMethodName;
	QByteArray m_destroyMethodName;

	SettingsObserver m_settingsObserver;
};


// public methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::TPluginManagerCompBase(const QByteArray& createMethodName, const QByteArray& destroyMethodName)
	:m_createMethodName(createMethodName),
	m_destroyMethodName(destroyMethodName),
	m_settingsObserver(*this)
{
}


// protected methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::LoadPlugins()
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


template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
bool TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::LoadPluginDirectory(const QString& pluginDirectoryPath)
{
	ilog::CLoggerBase::SendInfoMessage(0, QString("Looking for the plug-ins in '%1'").arg(pluginDirectoryPath));

	if (!pluginDirectoryPath.isEmpty() && QFileInfo(pluginDirectoryPath).exists()){
		QDir pluginsDirectory(pluginDirectoryPath);

		QFileInfoList pluginsList = pluginsDirectory.entryInfoList(QStringList() << (QString("*.") + *m_pluginExtensionAttrPtr));

		for (const QFileInfo& pluginPath : pluginsList){
#ifdef Q_OS_WIN
			SetDllDirectory(pluginPath.absolutePath().toStdWString().c_str());
#endif
			ilog::CLoggerBase::SendInfoMessage(0, QString("Load: '%1'").arg(pluginPath.canonicalFilePath()));

			QString pluginName;
			QByteArray pluginTypeId;
			istd::IInformationProvider::InformationCategory category;
			QString statusMessage;

			QLibrary library(pluginPath.canonicalFilePath());
			if (library.load()){
				CreateFunction createPluginFunc = (CreateFunction)library.resolve(m_createMethodName);
				if (createPluginFunc != NULL){
					istd::TDelPtr<PluginInterface> pluginInstancePtr = createPluginFunc();
					if (pluginInstancePtr.IsValid()){
						pluginName = pluginInstancePtr->GetPluginName();
						pluginTypeId = pluginInstancePtr->GetPluginTypeId();

						if (pluginInstancePtr->GetPluginTypeId() == *m_pluginTypeIdAttrPtr){
							PluginInfo pluginInfo;
							pluginInfo.pluginPath = pluginPath.canonicalFilePath();
							pluginInfo.pluginPtr = pluginInstancePtr.PopPtr();
							pluginInfo.destroyFunc = (DestroyFunction)library.resolve(m_destroyMethodName);

							if (InitializePlugin(pluginInfo.pluginPtr)){
								m_plugins.push_back(pluginInfo);

								category = istd::IInformationProvider::IC_INFO;
								statusMessage = QObject::tr("Plug-in loaded");
							}
							else{
								ilog::CLoggerBase::SendInfoMessage(0, QString("Plug-in initialization failed for: '%1'").arg(pluginPath.canonicalFilePath()));

								category = istd::IInformationProvider::IC_ERROR;
								statusMessage = QObject::tr("Plug-in initialization failed");
							}
						}
						else{
							category = istd::IInformationProvider::IC_WARNING;
							statusMessage = QObject::tr("Plug-in unsupported type-ID");
						}
					}
					else{
						category = istd::IInformationProvider::IC_ERROR;
						statusMessage = QObject::tr("Plug-in instance creation failed");
					}

					if (m_pluginStatusMonitorCompPtr.IsValid()) {
						m_pluginStatusMonitorCompPtr->OnPluginStatusChanged(
									pluginPath.canonicalFilePath(),
									pluginName,
									pluginTypeId,
									category,
									statusMessage);
					}
				}
				else{
					ilog::CLoggerBase::SendErrorMessage(0, QString("Plug-in entry point was not found: '%1'. %2").arg(pluginPath.canonicalFilePath()).arg(library.errorString()));

					category = istd::IInformationProvider::IC_ERROR;
					statusMessage = QObject::tr("Plug-in entry point was not found: '%1'").arg(library.errorString());
				}
			}
			else{
				ilog::CLoggerBase::SendErrorMessage(0, QString("%1").arg(library.errorString()));

				category = istd::IInformationProvider::IC_ERROR;
				statusMessage = QObject::tr("%1").arg(library.errorString());
			}
		}

		return true;
	}

	return false;
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
inline bool TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::InitializePlugin(PluginInterface * pluginPtr)
{
	Q_UNUSED(pluginPtr);

	return true;
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
inline void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::OnPluginsCreated()
{
}


// reimplemented (icomp::CComponentBase)

template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_settingsModelCompPtr.IsValid()){
		m_settingsModelCompPtr->AttachObserver(&m_settingsObserver);
	}
}


template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::OnComponentDestroyed()
{
	m_settingsObserver.EnsureModelDetached();

	for (typename Plugins::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter){
		iter->destroyFunc(iter->pluginPtr);
	}

	m_plugins.clear();

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class SettingsObserver

template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::SettingsObserver::SettingsObserver(TPluginManagerCompBase& parent)
	:m_parent(parent)
{
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::SettingsObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.LoadPlugins();
}


} // namespace imtbase


