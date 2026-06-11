#include <imtmdbx/CMdbxDatabaseControllerBase.h>


namespace imtmdbx
{


// public methods

CMdbxDatabaseControllerBase::CMdbxDatabaseControllerBase():
	m_timer(nullptr),
	m_debounceTimer(nullptr),
	m_dateFormat(Qt::ISODateWithMs),
	m_updateIsRunning(false),
	m_isForcedUpdate(false),
	m_pgChangeCount(0),
	m_collectionObserver(this)
{
	/// MDBX general data tables
	m_creationTableParamHash.insert(imtmdbx::MdbxGeneralData::INFO,				DocumentTableConfig(mdbx::key_mode::reverse, mdbx::value_mode::single, false));
	m_creationTableParamHash.insert(imtmdbx::MdbxGeneralData::REVISION_TIME,	DocumentTableConfig(mdbx::key_mode::reverse, mdbx::value_mode::single, false));
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CMdbxDatabaseControllerBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_debounceTimer = new QTimer(this);
	m_debounceTimer->setSingleShot(true);
	m_debounceTimer->setInterval(std::chrono::milliseconds{ m_updateDebounceIntervalMs->GetValue() });

	connect(m_debounceTimer, &QTimer::timeout, this, [this]() {
		Update();
	});

	// resize change group list to avoid resizing during update
	m_changeGroupList.resize(m_collectionListCompPtr.GetCount());
	AttachCollectionObservers();

	if(!m_updateIntervalSec.IsValid()){
		return;
	}

	m_timer = new QTimer(this);

	QTimer::singleShot(std::chrono::seconds{5}, this, [this]() {
		DataBaseUpdateSlot();

		connect(m_timer, &QTimer::timeout, this, &CMdbxDatabaseControllerBase::DataBaseUpdateSlot);
		m_timer->setInterval(std::chrono::seconds{m_updateIntervalSec->GetValue()});
		m_timer->start();
	});
}


void CMdbxDatabaseControllerBase::OnComponentDestroyed()
{
	DetachCollectionObservers();

	if(m_timer != nullptr){
		delete m_timer;
		m_timer = nullptr;
	}

	if(m_debounceTimer != nullptr){
		delete m_debounceTimer;
		m_debounceTimer = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imtbase::ITransactionManager)

bool CMdbxDatabaseControllerBase::StartTransaction()
{
	QMutexLocker locker(&m_mutex);

	if(m_pgChangeCount == 0){
		const int observedCollectionCount = m_collectionListCompPtr.GetCount();
		for (int i = 0; i < observedCollectionCount; ++i){
			if (m_changeGroupList[i] != nullptr){
				continue;
			}

			m_changeGroupList[i] = std::make_shared<istd::CChangeGroup>(m_collectionListCompPtr[i]);
		}
	}

	return ++m_pgChangeCount;
}


bool CMdbxDatabaseControllerBase::EndTransaction()
{
	QMutexLocker locker(&m_mutex);

	if (m_pgChangeCount <= 0){
		return false;
	}

	if (--m_pgChangeCount == 0){
		for (std::shared_ptr<istd::CChangeGroup>& changeGroupPtr : m_changeGroupList){
			changeGroupPtr.reset();
		}
	}

	return true;
}


// public methods

bool CMdbxDatabaseControllerBase::Update()
{
	if(m_updateIsRunning){
		return false;
	}

	qDebug() << "MDBX:: Forced update called";

	m_isForcedUpdate = true;
	DataBaseUpdateSlot();

	return true;
}


bool CMdbxDatabaseControllerBase::SetTableCreationTime(imtmdbx::IDocumentTable* infoTable)
{
	if(infoTable == nullptr){
		return false;
	}

	QString creationTimeStr = QDateTime::currentDateTimeUtc().toString(m_dateFormat);

	infoTable->AddDocument(imtmdbx::MdbxGeneralData::CREATION_TIME.toLocal8Bit(), creationTimeStr.toLocal8Bit());

	return true;
}


bool CMdbxDatabaseControllerBase::IsTableCreated(imtmdbx::IDocumentTable* infoTable)
{
	if(infoTable == nullptr){
		return false;
	}

	QDateTime date;
	QString dateStr = infoTable->GetDocument(imtmdbx::MdbxGeneralData::CREATION_TIME.toLocal8Bit());
	if(!dateStr.isEmpty()){
		date = QDateTime::fromString(dateStr, m_dateFormat);
	}

	return date.isValid();
}



QDateTime CMdbxDatabaseControllerBase::GetRevisionTime(const QString& tableName, mdbx::txn_managed& txn)
{
	DocumentTablePtr documentTableTime = CreateGeneralDataTable(imtmdbx::MdbxGeneralData::REVISION_TIME, txn);
	if(documentTableTime.isNull()){
		return QDateTime();
	}

	QDateTime time;
	QByteArray dateValue = documentTableTime->GetDocument(tableName.toLocal8Bit());
	if(!dateValue.isEmpty()){
		time = QDateTime::fromString(QString(dateValue), m_dateFormat);
	}

	return time;
}


bool CMdbxDatabaseControllerBase::SetRevisionTime(
			const QString& tableName,
			const QString& time,
			mdbx::txn_managed& txn)
{
	DocumentTablePtr documentTableTime = CreateGeneralDataTable(imtmdbx::MdbxGeneralData::REVISION_TIME, txn);
	if(documentTableTime.isNull()){
		return false;
	}

	QByteArray value = documentTableTime->GetDocument(tableName.toLocal8Bit());
	if(!value.isEmpty()){
		documentTableTime->UpdateDocument(tableName.toLocal8Bit(), time.toLocal8Bit());
	}
	else {
		documentTableTime->AddDocument(tableName.toLocal8Bit(), time.toLocal8Bit());
	}

	return true;
}


// protected methods

QDateTime CMdbxDatabaseControllerBase::GetTableLastUpdateTime(const QString& tableName, mdbx::txn_managed& txn)
{
	const QDateTime epochStartDate = QDateTime::fromMSecsSinceEpoch(0, QTimeZone::utc());
	const QDateTime revisionTime = GetRevisionTime(tableName, txn);

	return revisionTime.isValid() ? revisionTime : epochStartDate;
}


QByteArray CMdbxDatabaseControllerBase::GetFloatingPointNumberKey(const QString& number, int fractionalPartCount) const
{
	QByteArray keyBA;

	const double coeff	= std::pow(10, fractionalPartCount);
	const double num	= std::round(number.toDouble() * coeff);

	QString numStr = QString::number(num, 'f', 0);
	int count = numStr.length();

	QDataStream streamKey(&keyBA, QIODeviceBase::WriteOnly);
	streamKey << count;

	const QByteArray numBA = numStr.toUtf8();
	streamKey.writeRawData(numBA.constData(), numBA.length());

	return keyBA;
}


// private methods

DocumentTablePtr CMdbxDatabaseControllerBase::CreateGeneralDataTable(const QString& tableName, mdbx::txn_managed& txn) const
{
	if (!m_mdbxDatabaseEngineCompPtr.IsValid()){
		return DocumentTablePtr();
	}

	const DocumentTableConfig tableConfig = m_creationTableParamHash[tableName];

	return QSharedPointer<imtmdbx::CDocumentTable>::create(
				tableName,
				txn,
				tableConfig.keyMode,
				tableConfig.valueMode,
				tableConfig.hasIndex);
}


void CMdbxDatabaseControllerBase::AttachCollectionObservers()
{
	if (!m_collectionListCompPtr.IsValid()){
		return;
	}

	for (int i = 0; i < m_collectionListCompPtr.GetCount(); i++){
		if (auto* modelPtr = dynamic_cast<imod::IModel*>(m_collectionListCompPtr[i])){
			modelPtr->AttachObserver(&m_collectionObserver);
		}
	}
}


void CMdbxDatabaseControllerBase::DetachCollectionObservers()
{
	if (!m_collectionListCompPtr.IsValid()){
		return;
	}

	for (int i = 0; i < m_collectionListCompPtr.GetCount(); i++){
		if (auto* modelPtr = dynamic_cast<imod::IModel*>(m_collectionListCompPtr[i])){
			modelPtr->DetachObserver(&m_collectionObserver);
		}
	}
}


bool CMdbxDatabaseControllerBase::CheckConfiguration() const
{
	bool retVal = true;
	if (!m_mdbxDatabaseEngineCompPtr.IsValid()){
		retVal = false;
		SendCriticalMessage(0, QStringLiteral("Attribute 'MdbxDatabaseEngine' was not set."), __func__);
	}

	if (!m_databaseEngineCompPtr.IsValid()){
		retVal = false;
		SendCriticalMessage(0, QStringLiteral("Attribute 'DatabaseEngine' was not set."), __func__);
	}

	return retVal;
}


void CMdbxDatabaseControllerBase::TriggerDebouncedUpdate()
{
	const int debounceInterval = m_updateDebounceIntervalMs->GetValue();
	if (m_debounceTimer != nullptr && debounceInterval > 0) {
		QMetaObject::invokeMethod(m_debounceTimer, "start", Qt::AutoConnection);
	}
	else {
		Update();
	}
}


// CollectionObserver: public methods

CMdbxDatabaseControllerBase::CollectionObserver::CollectionObserver(CMdbxDatabaseControllerBase* mdbxDatabaseControllerPtr):
	m_mdbxDatabaseControllerPtr(mdbxDatabaseControllerPtr)
{
}


// CollectionObserver: private methods

void CMdbxDatabaseControllerBase::CollectionObserver::OnUpdate(imod::IModel* modelPtr, const ChangeSet& changeSet)
{
	if (m_mdbxDatabaseControllerPtr != nullptr){
		m_mdbxDatabaseControllerPtr->TriggerDebouncedUpdate();
	}
}


} // namespace imtmdbx


