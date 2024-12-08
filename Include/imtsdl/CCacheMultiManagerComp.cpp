#include <imtsdl/CCacheMultiManagerComp.h>


// Qt includes
#include <QtCore/QDir>

// ImtCore includes
#include <imtsdl/CCacheMultiManager.h>
#include <imtsdl/CCacheController.h>


namespace imtsdl
{

// public methods
CCacheMultiManagerComp::CCacheMultiManagerComp()
	: m_cacheMultiManagerPtr(new CCacheMultiManager()),
	m_mainCacheControllerPtr(new CCacheController())
{
}


// protected methods

void CCacheMultiManagerComp::SetLog(istd::IPolymorphic& loggable, bool reset)
{
	try {
		ilog::ILoggable& castedLoggable = dynamic_cast<ilog::ILoggable&>(loggable);
		if (reset){
			castedLoggable.SetLogPtr(nullptr);
		}
		else {
			castedLoggable.SetLogPtr(GetLogPtr());
		}
	}
	catch(const std::bad_cast& ){
		I_CRITICAL();
	}
}


// reimplemented (ICacheMultiManager)

void CCacheMultiManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_argumentParserCompPtr.IsValid());

	SetLog(*m_cacheMultiManagerPtr);
	SetLog(*m_mainCacheControllerPtr);

	const QString cachePath = QDir::cleanPath(m_argumentParserCompPtr->GetCachePath());
	if (!cachePath.isEmpty()){
		std::shared_ptr<QFile> cacheFilePtr(new QFile(cachePath));
		bool deviceSetted = m_mainCacheControllerPtr->SetOperationalDevice(cacheFilePtr);
		if (!deviceSetted){
			SendErrorMessage(0, QString("Unable to set operational file for cache controller '%1'").arg(cachePath));
		}

		ICacheController::LoadError loadError = m_mainCacheControllerPtr->LoadFromData();
		if (loadError != ICacheController::LE_OK){
			SendWarningMessage(0, QString("Unable to load additional cache from file '%1'").arg(cachePath));
		}

		m_additionalCacheControllerPtrList << m_mainCacheControllerPtr;
	}

	const QStringList additionalCaches = m_argumentParserCompPtr->GetAdditionalCachePaths();
	for (const QString& additionalCachePath: additionalCaches){
		std::shared_ptr<CCacheController> cacheControllerPtr;
		SetLog(*cacheControllerPtr);
		std::shared_ptr<QFile> additionalCacheFilePtr(new QFile(additionalCachePath));
		cacheControllerPtr->SetOperationalDevice(additionalCacheFilePtr);
		ICacheController::LoadError loadError = cacheControllerPtr->LoadFromData();
		if (loadError != ICacheController::LE_OK){
			SendWarningMessage(0, QString("Unable to load additional cache from file '%1'").arg(additionalCachePath));
		}

		m_additionalCacheControllerPtrList << cacheControllerPtr;
	}

	bool isInitialized = m_cacheMultiManagerPtr->Init(m_additionalCacheControllerPtrList);
	if (!isInitialized){
		SendCriticalMessage(0, "Unable to initialize caches");
		I_CRITICAL();
	}
}

void CCacheMultiManagerComp::OnComponentDestroyed()
{
	SetLog(*m_cacheMultiManagerPtr, true);

	// reset all cache controllers
	for (auto& cachePtr: m_additionalCacheControllerPtrList){
		SetLog(*cachePtr, true);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtsdl


