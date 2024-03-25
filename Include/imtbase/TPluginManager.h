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
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>


namespace imtbase
{


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
class TPluginManager: public ilog::CLoggerBase
{
public:
	TPluginManager() {}
	TPluginManager(const QByteArray& createMethodName, const QByteArray& destroyMethodName, imtbase::IPluginStatusMonitor* pluginStatusMonitorPtr);

	virtual bool LoadPluginDirectory(
				const QString& pluginDirectoryPath,
				const QByteArray& pluginExtension,
				const QByteArray& pluginTypeId);

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

protected:
	/**
		Do some initialization for concrete plugin implementation.
		You should re-implement this method, if you want to execute some actions during the initialization stage of the plugin.
		The default implementation does nothing.
	*/
	virtual bool InitializePlugin(PluginInterface* pluginPtr);

protected:
	QByteArray m_createMethodName;
	QByteArray m_destroyMethodName;
	imtbase::IPluginStatusMonitor* m_pluginStatusMonitorPtr;
};


// public methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::TPluginManager(const QByteArray& createMethodName, const QByteArray& destroyMethodName, IPluginStatusMonitor* pluginStatusMonitorPtr)
			:m_createMethodName(createMethodName),
			m_destroyMethodName(destroyMethodName),
			m_pluginStatusMonitorPtr(pluginStatusMonitorPtr)
{
}


// protected methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::LoadPluginDirectory(
			const QString& pluginDirectoryPath,
			const QByteArray& pluginExtension,
			const QByteArray& pluginTypeId)
{
	ilog::CLoggerBase::SendInfoMessage(0, QString("Looking for the plug-ins in '%1'").arg(pluginDirectoryPath));

	if (!pluginDirectoryPath.isEmpty() && QFileInfo(pluginDirectoryPath).exists()){
		QDir pluginsDirectory(pluginDirectoryPath);

		QFileInfoList pluginsList = pluginsDirectory.entryInfoList(QStringList() << (QString("*.") + pluginExtension));

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

						if (pluginInstancePtr->GetPluginTypeId() == pluginTypeId){
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

					if (m_pluginStatusMonitorPtr != nullptr) {
						m_pluginStatusMonitorPtr->OnPluginStatusChanged(
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


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::InitializePlugin(PluginInterface * pluginPtr)
{
	Q_UNUSED(pluginPtr);

	return true;
}


} // namespace imtbase


