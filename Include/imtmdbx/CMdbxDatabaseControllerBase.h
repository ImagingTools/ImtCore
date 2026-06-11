#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelObserverBase.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtbase/ITransactionManager.h>
#include <imtmdbx/IMdbxDatabaseEngine.h>
#include <imtmdbx/CMdbxDocumentTableManager.h>


namespace imtmdbx
{


class CMdbxDatabaseControllerBase:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtbase::ITransactionManager
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	using DocumentTableConfig	= CMdbxDocumentTableManager::DocumentTableConfig;

	I_BEGIN_BASE_COMPONENT(CMdbxDatabaseControllerBase);
		I_REGISTER_INTERFACE(imtbase::ITransactionManager);
		I_ASSIGN(m_transactionCountLimitAttr, "TransactionCheckpointInterval", "Max elements to process before performing a transactional checkpoint and reopen (100000000 - for Windows; 1000 - Others)", true, 1000);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_mdbxDatabaseEngineCompPtr, "MdbxDatabaseEngine", "MdbxDatabase engine for analytics data", true, "MdbxDatabaseEngine");
		I_ASSIGN(m_updateDebounceIntervalMs, "UpdateDebounceIntervalMs", "Delay in milliseconds to debounce database updates triggered by collection changes.", true, 500);
		I_ASSIGN(m_updateIntervalSec, "UpdateIntervalSec", "Interval between database updates in seconds. No automatic updates if disabled.", false, 300); // 5 minutes
		I_ASSIGN_MULTI_0(m_collectionListCompPtr, "DatabaseCollections", "Database collections. Connect to trigger MDBX update on every detected collection change", false);
	I_END_COMPONENT;

	CMdbxDatabaseControllerBase();

	virtual bool Update();
	virtual void TriggerDebouncedUpdate();

Q_SIGNALS:
	void updateFinished();

protected Q_SLOTS:
	virtual void DataBaseUpdateSlot() = 0;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	virtual bool CheckConfiguration() const;

	virtual bool CreateTables() = 0;
	virtual bool CreateRevisionTime (mdbx::txn_managed& txn) = 0;

	virtual bool IsTableCreated(imtmdbx::IDocumentTable* infoTable);
	virtual bool SetTableCreationTime(imtmdbx::IDocumentTable* infoTable);
	virtual QDateTime GetRevisionTime(const QString& tableName, mdbx::txn_managed& txn);
	virtual bool SetRevisionTime(
				const QString& tableName,
				const QString& time,
				mdbx::txn_managed& txn);

	// reimplemented (imtbase::ITransactionManager)
	virtual bool StartTransaction() override;
	virtual bool CancelTransaction() override { return true; }
	virtual bool EndTransaction() override;

protected:
	/**
		\brief Get last update time of the table with \param tableName from the document table with revision times.
		\param tableName - name of the table to get last update time for, \sa CMdbxDocumentTableManager table keys.
		\param txn - MDBX transaction to use for getting last update time.
		\return Last update time of the table if found and valid, otherwise QDateTime with epoch time (i.e. 1970-01-01T00:00:00).
	*/
	QDateTime GetTableLastUpdateTime(const QString& tableName, mdbx::txn_managed& txn);
	/**
		\brief Get key for floating point number \param number with fixed length of fractional part \param fractionalPartCount.
		\param number - floating point number in string representation to get key for.
		\param fractionalPartCount - number of digits in fractional part to use for key generation, default is 2.
		\return QByteArray key for the floating point number.
	*/
	QByteArray GetFloatingPointNumberKey(const QString& number, int fractionalPartCount = 2) const;
	/**
		\brief Create and return initialized document table with \param tableName and configuration from \sa m_creationTableParamHash.
		\param tableName - name of the table to create, should be an element of \sa m_creationTableParamHash keys.
		\param txn - MDBX transaction to use for table creation.
		\return Initialized document table if created successfully, otherwise uninitialized imtmdbx::CDocumentTable.
	*/
	DocumentTablePtr CreateGeneralDataTable(const QString& tableName, mdbx::txn_managed& txn) const;

private:
	// observer for collections, used for triggering database update on collection change
	class CollectionObserver: public imod::CMultiModelObserverBase
	{
	public:
		CollectionObserver(CMdbxDatabaseControllerBase* mdbxDatabaseController);
	private:
		void OnUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;
	private:
		CMdbxDatabaseControllerBase* m_mdbxDatabaseControllerPtr;
	};

private:
	/**
		\brief Attach \sa CollectionObserver to all collections in \sa m_collectionListCompPtr.
				Attached observer will trigger database update on collection change.
	*/
	void AttachCollectionObservers();
	/**
		\brief Detach \sa CollectionObserver from all collections in \sa m_collectionListCompPtr.
	*/
	void DetachCollectionObservers();

protected:
	I_ATTR(int, m_updateIntervalSec);
	I_ATTR(int, m_transactionCountLimitAttr);
	I_ATTR(int, m_updateDebounceIntervalMs);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(imtmdbx::IMdbxDatabaseEngine, m_mdbxDatabaseEngineCompPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_collectionListCompPtr);

protected:
	QTimer* m_timer;
	QTimer* m_debounceTimer;
	QHash<const QString/*tableName*/, DocumentTableConfig/*config*/> m_creationTableParamHash;

	Qt::DateFormat m_dateFormat;

	bool m_updateIsRunning;
	bool m_isForcedUpdate;
	int m_transactionCountLimit;

	/**
		Observer for collections, used for triggering database update on collection change.
		Attached to all collections in \sa m_collectionListCompPtr.
	*/
	CollectionObserver m_collectionObserver;
	/**
		List of change groups for collections.
		Used to group collection changes during database update and avoid multiple updates if several changes are detected.
		When update is triggered, all groups will be reset which will call update on attached collections and trigger database update only once.
		Applicaple only for StartTransaction/EndTransaction mechanism.
	*/
	QList<std::shared_ptr<istd::CChangeGroup>> m_changeGroupList;
	int m_pgChangeCount;
	QMutex m_mutex;
};


} // namespace imtmdbx


