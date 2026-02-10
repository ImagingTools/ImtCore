// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CSingleModelObserverBase.h>

#include <ifile/IFileNameParam.h>
#include <imod/IModel.h>
#include <iprm/IOptionsList.h>
#include <iprm/IParamsManager.h>
#include <iprm/TParamsPtr.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/TPluginManager.h>
#include <imtbase/IPluginStatusMonitor.h>


namespace imtbase
{


template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass = ilog::CLoggerComponentBase>
class TPluginManagerCompBase: public BaseComponentClass, public TPluginManager<PluginInterface, CreateFunction, DestroyFunction>
{
public:
	typedef BaseComponentClass BaseClass;
	typedef TPluginManager<PluginInterface, CreateFunction, DestroyFunction> BaseClass2;

	I_BEGIN_BASE_COMPONENT(TPluginManagerCompBase);
		I_ASSIGN(m_settingsCompPtr, "Settings", "Plugin settings", true, "Settings");
		I_ASSIGN_TO(m_settingsModelCompPtr, m_settingsCompPtr, true);
		I_ASSIGN(m_pluginStatusMonitorCompPtr, "PluginStatusMonitor", "Plugin status monitor", false, "PluginStatusMonitor");
		I_ASSIGN(m_pluginPathsManagerIdAttrPtr, "PluginPathsManagerId", "ID of the plugin path manager parameter in the application settings", true, "PluginPathsManager");
		I_ASSIGN(m_pluginPathIdAttrPtr, "PluginPathId", "ID of the plugin path parameter in the parameter set", true, "PluginPath");
		I_ASSIGN(m_defaultPluginPathIdAttrPtr, "DefaultPluginPathId", "ID of the default plug-in path parameter in the settings", true, "DefaultPluginPath");
		I_ASSIGN(m_pluginExtensionAttrPtr, "PluginFileExtension", "File extension for the plugin DLL", true, "dll");
		I_ASSIGN(m_pluginTypeIdAttrPtr, "PluginTypeId", "Type-ID of supported plugins", true, "");
	I_END_COMPONENT;

	TPluginManagerCompBase(const QByteArray& createMethodName, const QByteArray& destroyMethodName);

	// reimplemented (ilog::ILoggable)
	virtual void SetLogPtr(ilog::IMessageConsumer* logPtr) override;
	virtual ilog::IMessageConsumer* GetLogPtr() const override;

protected:
	virtual void LoadPlugins();

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
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		TPluginManagerCompBase& m_parent;
	};

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
	:BaseClass2(createMethodName, destroyMethodName, nullptr),
	m_settingsObserver(*this)
{
}


// reimplemented (ilog::ILoggable)

template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::SetLogPtr(ilog::IMessageConsumer* logPtr)
{
	BaseClass::SetLogPtr(logPtr);
	BaseClass2::SetLogPtr(logPtr);
}


template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
ilog::IMessageConsumer* TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::GetLogPtr() const
{
	return BaseClass::GetLogPtr();
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
				BaseClass2::LoadPluginDirectory(defaultPluginFolderPath, *m_pluginExtensionAttrPtr, *m_pluginTypeIdAttrPtr);
			}
		}

		iprm::TParamsPtr<iprm::IParamsManager> pluginPathManagerPtr(m_settingsCompPtr.GetPtr(), *m_pluginPathsManagerIdAttrPtr);
		if (pluginPathManagerPtr.IsValid()){
			const iprm::IOptionsList* paramListPtr = pluginPathManagerPtr->GetSelectionConstraints();

			int pluginPathsCount = pluginPathManagerPtr->GetParamsSetsCount();
			for (int pluginPathIndex = 0; pluginPathIndex < pluginPathsCount; ++pluginPathIndex){
				
				bool isEnabled = true;
				if (paramListPtr != nullptr){
					isEnabled = paramListPtr->IsOptionEnabled(pluginPathIndex);
				}

				if (isEnabled){
					iprm::TParamsPtr<ifile::IFileNameParam> pluginPathParamPtr(pluginPathManagerPtr->GetParamsSet(pluginPathIndex), *m_pluginPathIdAttrPtr);
					if (pluginPathParamPtr.IsValid()){
						QString pluginsDirectoryPath = pluginPathParamPtr->GetPath();

						BaseClass2::LoadPluginDirectory(pluginsDirectoryPath, *m_pluginExtensionAttrPtr, *m_pluginTypeIdAttrPtr);
					}
				}
			}
		}
	}

	OnPluginsCreated();
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

	BaseClass2::SetStatusManager(m_pluginStatusMonitorCompPtr.GetPtr());

	if (m_settingsModelCompPtr.IsValid()){
		m_settingsModelCompPtr->AttachObserver(&m_settingsObserver);
	}
}


template <class PluginInterface, typename CreateFunction, typename DestroyFunction, typename BaseComponentClass>
void TPluginManagerCompBase<PluginInterface, CreateFunction, DestroyFunction, BaseComponentClass>::OnComponentDestroyed()
{
	m_settingsObserver.EnsureModelDetached();

	for (auto iter = this->m_plugins.begin(); iter != this->m_plugins.end(); ++iter){
		iter->destroyFunc(iter->pluginPtr);
	}

	this->m_plugins.clear();

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


