// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QLibrary>
#include <QtCore/QDir>

// Windows includes
#ifdef Q_OS_WIN
	#include <windows.h>
#endif

// ACF includes
#include <istd/TDelPtr.h>
#include <ilog/CLoggerBase.h>

// ImtCore includes
#include <imtbase/IPluginStatusMonitor.h>


namespace imtbase
{


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
class TPluginManager: virtual public ilog::ILoggable
{
public:
	TPluginManager() : m_pluginStatusMonitorPtr(nullptr)
	{
	}

	TPluginManager(const QByteArray& createMethodName, const QByteArray& destroyMethodName, IPluginStatusMonitor* pluginStatusMonitorPtr);

	virtual bool LoadPluginDirectory(
				const QString& pluginDirectoryPath,
				const QByteArray& pluginExtension,
				const QByteArray& pluginTypeId);

	// reimplemented (ilog::ILoggable)
	virtual void SetLogPtr(ilog::IMessageConsumer* logPtr) override;
	virtual ilog::IMessageConsumer* GetLogPtr() const override;

public:
	struct PluginInfo
	{
		PluginInfo()
			:pluginPtr(nullptr)
		{
		}

		PluginInterface* pluginPtr;
		QString pluginPath;
		DestroyFunction destroyFunc;
	};

	typedef QList<PluginInfo> Plugins;
	Plugins m_plugins;

protected:
	void SetStatusManager(IPluginStatusMonitor* pluginStatusMonitorPtr);

	/**
		Do some initialization for concrete plugin implementation.
		You should re-implement this method, if you want to execute some actions during the initialization stage of the plugin.
		The default implementation does nothing.
	*/
	virtual bool InitializePlugin(PluginInterface* pluginPtr);

protected:
	class Logger : public ilog::CLoggerBase
	{
	public:
		typedef ilog::CLoggerBase BaseClass;

		bool SendInfoMessage(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendWarningMessage(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendErrorMessage(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendCriticalMessage(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendInfoMessageOnce(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendWarningMessageOnce(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendErrorMessageOnce(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
		bool SendCriticalMessageOnce(
			int id,
			const QString& message,
			const QString& messageSource = QString(),
			int flags = 0) const;
	};

protected:
	QByteArray m_createMethodName;
	QByteArray m_destroyMethodName;
	IPluginStatusMonitor* m_pluginStatusMonitorPtr;
	Logger m_logger;
};


// public methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::TPluginManager(const QByteArray& createMethodName, const QByteArray& destroyMethodName, IPluginStatusMonitor* pluginStatusMonitorPtr)
			:m_createMethodName(createMethodName),
			m_destroyMethodName(destroyMethodName),
			m_pluginStatusMonitorPtr(pluginStatusMonitorPtr)
{
	Q_ASSERT(!createMethodName.isEmpty());
	Q_ASSERT(!destroyMethodName.isEmpty());
}


// public methods

// reimplemented (ilog::ILoggable)

template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline void TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::SetLogPtr(ilog::IMessageConsumer* logPtr)
{
	m_logger.SetLogPtr(logPtr);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline ilog::IMessageConsumer* TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::GetLogPtr() const
{
	return m_logger.GetLogPtr();
}


// protected methods

template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
void TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::SetStatusManager(IPluginStatusMonitor* pluginStatusMonitorPtr)
{
	m_pluginStatusMonitorPtr = pluginStatusMonitorPtr;
}


template <class PluginInterface, typename CreateFunction, typename DestroyFunction>
bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::LoadPluginDirectory(
			const QString& pluginDirectoryPath,
			const QByteArray& pluginExtension,
			const QByteArray& pluginTypeId)
{
	m_logger.SendInfoMessage(0, QString("Looking for the plug-ins in '%1'").arg(pluginDirectoryPath));

	if (!pluginDirectoryPath.isEmpty() && QFileInfo(pluginDirectoryPath).exists()){
		QDir pluginsDirectory(pluginDirectoryPath);

		QFileInfoList pluginsList = pluginsDirectory.entryInfoList(QStringList() << (QString("*.") + pluginExtension));

		for (const QFileInfo& pluginPath : pluginsList){
#ifdef Q_OS_WIN
			std::wstring path = pluginPath.absolutePath().toStdWString();
			SetDllDirectory(path.c_str());
#endif
			m_logger.SendInfoMessage(0, QString("Load: '%1'").arg(pluginPath.canonicalFilePath()));

			QString pluginName;
			QByteArray instanceTypeId;
			istd::IInformationProvider::InformationCategory category;
			QString statusMessage;

			QLibrary library(pluginPath.canonicalFilePath());
			if (library.load() == true){
				CreateFunction createPluginFunc = (CreateFunction)library.resolve(m_createMethodName);
				if (createPluginFunc != NULL){
					istd::TDelPtr<PluginInterface> pluginInstancePtr = createPluginFunc();
					if (pluginInstancePtr.IsValid()){
						pluginName = pluginInstancePtr->GetPluginName();
						instanceTypeId = pluginInstancePtr->GetPluginTypeId();

						if (instanceTypeId == pluginTypeId){
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
								m_logger.SendInfoMessage(0, QString("Plug-in initialization failed for: '%1'").arg(pluginPath.canonicalFilePath()));

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

					if (m_pluginStatusMonitorPtr != nullptr){
						m_pluginStatusMonitorPtr->OnPluginStatusChanged(
									pluginPath.canonicalFilePath(),
									pluginName,
									pluginTypeId,
									category,
									statusMessage);
					}
				}
				else{
					m_logger.SendErrorMessage(0, QString("Plug-in entry point was not found: '%1'. %2").arg(pluginPath.canonicalFilePath()).arg(library.errorString()));

					category = istd::IInformationProvider::IC_ERROR;
					statusMessage = QObject::tr("Plug-in entry point was not found: '%1'").arg(library.errorString());
				}
			}
			else{
				m_logger.SendErrorMessage(0, QString("%1").arg(library.errorString()));

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


// public methods of the embedded class Logger

template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendInfoMessage(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendInfoMessage(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendWarningMessage(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendWarningMessage(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendErrorMessage(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendErrorMessage(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendCriticalMessage(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendCriticalMessage(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendInfoMessageOnce(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendInfoMessageOnce(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendWarningMessageOnce(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendWarningMessageOnce(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendErrorMessageOnce(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendErrorMessageOnce(id, message, messageSource, flags);
}


template<class PluginInterface, typename CreateFunction, typename DestroyFunction>
inline bool TPluginManager<PluginInterface, CreateFunction, DestroyFunction>::Logger::SendCriticalMessageOnce(int id, const QString& message, const QString& messageSource, int flags) const
{
	return BaseClass::SendCriticalMessageOnce(id, message, messageSource, flags);
}


} // namespace imtbase


