#include <imtsdl/CCacheController.h>


// Qt includes
#include<QtCore/QFile>
#include<QtCore/QReadLocker>
#include<QtCore/QWriteLocker>

//ACF includes
#include <istd/CChangeGroup.h>


namespace imtsdl
{


// public methods

CCacheController::CCacheController()
	: m_cachePtr(new imod::TModelWrap<CCache>),
	m_isAutosaveEnabled(false)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_cachePtr.get());
	Q_ASSERT(modelPtr != nullptr);

	[[maybe_unused]] const bool attached = modelPtr->AttachObserver(this);
	Q_ASSERT_X(attached && BaseClass::IsModelAttached(), "Unable to observe model", __func__);
}


// reimplemented (imod::CSingleModelObserverBase)

void CCacheController::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_isAutosaveEnabled){
		Save();
	}
}


// reimplemented (ICacheController)

CCache* CCacheController::GetCache()
{
	QWriteLocker lock(&m_cacheGuard);

	return m_cachePtr.get();
}


const CCache* CCacheController::GetCache() const
{
	QReadLocker lock(&m_cacheGuard);

	return m_cachePtr.get();
}


bool CCacheController::SetOperationalDevice(const std::shared_ptr<QIODevice>& devicePtr)
{
	QWriteLocker devLock(&m_deviceGuard);

	if (!devicePtr){
		SendCriticalMessage(0, "NULL device provided!");
		Q_ASSERT_X(false, "NULL operational device is provided", __func__);

		return false;
	}

	if (devicePtr->isSequential()){
		SendCriticalMessage(0, "Sequential device is not supported!");
		Q_ASSERT_X(false, "Sequential operational device is provided", __func__);

		return false;
	}

	if (!devicePtr->isOpen()){
		const bool isOpen = devicePtr->open(QIODevice::ReadWrite | QIODevice::Text);
		if (!isOpen) {
			QFile* filePtr = dynamic_cast<QFile*>(devicePtr.get());
			QString fileName;
			QString errorString = devicePtr->errorString();
			if (filePtr != nullptr){
				fileName = filePtr->fileName();
			}
			if (filePtr != nullptr){
				SendErrorMessage(0, QString("Unable to open device! Error: %1 '%2'").arg(errorString, fileName));

				return false;
			}
		}
	}
	else {
		QIODevice::OpenMode openMode = devicePtr->openMode();
		if (!openMode.testFlag(QIODevice::ReadWrite)){
			SendErrorMessage(0, QString("Operational device is already opened, but with wrong mode!. Mode: %1").arg(QString::number(openMode, 16)));

			return false;
		}
	}

	m_operationalDevicePtr = devicePtr;

	return true;
}


bool CCacheController::Save() const
{
	QReadLocker lock(&m_cacheGuard);
	QWriteLocker devLock(&m_deviceGuard);

	if (!m_operationalDevicePtr){
		SendCriticalMessage(0, "Operational device is NOT initialized!");
		Q_ASSERT_X(false, "Operational device is NOT initialized", __func__);

		return false;
	}

	QJsonObject store;
	bool ok = m_cachePtr->WriteToJsonObject(store);
	if (!ok){
		SendErrorMessage(0, "Unable to write cache. Dnsufficient data in cache!");

		return false;
	}

	ok = m_operationalDevicePtr->seek(0);
	if (!ok){
		SendErrorMessage(0, "Unable to change position of operational device");

		return false;
	}

	QByteArray cacheData = QJsonDocument(store).toJson();
	qsizetype cacheSize = cacheData.size();
	qint64 writtenBytes = m_operationalDevicePtr->write(cacheData);

	ok = bool(writtenBytes == cacheSize);
	if (!ok){
		SendErrorMessage(0, QString("Unable to write data. Expected: %1. Actual: %2. Error: %3").arg(QString::number(cacheSize), QString::number(writtenBytes), m_operationalDevicePtr->errorString()));

		return false;
	}

	return ok;
}


bool CCacheController::LoadFromData(QIODevice* dataDevicePtr)
{
	QWriteLocker lock(&m_cacheGuard);
	QWriteLocker devLock(&m_deviceGuard);

	QIODevice* readDevicePtr;
	if (dataDevicePtr != nullptr){
		readDevicePtr = dataDevicePtr;
	}
	else if (m_operationalDevicePtr){
		readDevicePtr = m_operationalDevicePtr.get();
	}
	else {
		SendCriticalMessage(0, "Unable to load cache. devicePtr is NOT provided, operationalDevice is NOT initialized.");
		Q_ASSERT_X(false, "Unable to load cache. devicePtr is NOT provided, operationalDevice is NOT initialized.", __func__);

		return false;
	}

	bool ok = readDevicePtr->seek(0);
	if (!ok){
		SendErrorMessage(0, "Unable to change position of operational device");

		return false;
	}

	QByteArray readData = readDevicePtr->readAll();
	QJsonParseError jsonError;
	QJsonObject cacheObject = QJsonDocument::fromJson(readData, &jsonError).object();
	if (jsonError.error != QJsonParseError::NoError){
		SendErrorMessage(0, QString("Unexpected cache data. Expected JSON object. Error: %1 .Position: %2").arg(jsonError.errorString(), QString::number(jsonError.offset)));

		return false;
	}

	istd::CChangeGroup changGroup(m_cachePtr.get());
	ok = m_cachePtr->ResetData();
	if (!ok){
		SendCriticalMessage(0, "Unable to reset cache.");
		I_CRITICAL();

		return false;
	}

	ok = CCache::ReadFromJsonObject(*m_cachePtr, cacheObject);
	if (!ok){
		SendErrorMessage(0, "Unable to read cache. Unexpected data!");

		return false;
	}

	// observe model
	BaseClass::SetModelPtr(dynamic_cast<imod::IModel*>(m_cachePtr.get()));

	return true;
}


void CCacheController::EnableAutoSave(bool enableAutosave)
{
	m_isAutosaveEnabled = enableAutosave;
}



} // namespace imtsdl


