#pragma once


// ACF includes
#include <ilog/CLoggerBase.h>

// ImtCore includes
#include <imtmdbx/CMaskContainer.h>
#include <imtmdbx/CDoubleMask.h>


namespace imtmdbx
{


using DocumentTablePtr = QSharedPointer<imtmdbx::IDocumentTable>;


class CMdbxDocumentTableManager: public ilog::CLoggerBase
{
public:
	struct DocumentTableConfig {
		mdbx::key_mode keyMode;
		mdbx::value_mode valueMode;
		bool hasIndex;

		DocumentTableConfig(
			mdbx::key_mode keyMode = mdbx::key_mode::usual,
			mdbx::value_mode valueMode = mdbx::value_mode::single,
			bool hasIndex = false
			)
			: keyMode(keyMode), valueMode(valueMode), hasIndex(hasIndex) {}
	};

	CMdbxDocumentTableManager(mdbx::txn_managed& txn, const QHash<QString/*tableName*/, DocumentTableConfig>& tableConfigHash = {});

public:
	/**
		\brief Get hash with table configuration. Access to configuration parameters for each table.
		\return Hash with table name as key and \sa DocumentTableConfig as value.
	*/
	const QHash<QString/*tableName*/, DocumentTableConfig>& GetTableConfigHash() const;
	/**
		\brief Set hash with table configuration. Access to configuration parameters for each table.
		\param tableConfigHash - hash with table name as key and \sa DocumentTableConfig as value.
	*/
	void SetTableConfigHash(const QHash<QString/*tableName*/, DocumentTableConfig>& tableConfigHash);

	/**
		\brief Try to find initialized table in \sa m_initializedTableHash.
				If it is not found, try to initialize it if configuration exists in \sa m_tableConfigHash.
				If configuration does not exist, return null pointer.
		\param tableId - expected element of \sa iotmdbx::MdbxTable related structs.
		\return Shared pointer to \sa imtmdbx::IDocumentTable interface implementation if found, otherwise uninitialized QSharedPointer.
	*/
	DocumentTablePtr get(const QString& tableId);
	/**
		\brief Add table configuration to \sa m_tableConfigHash.
		\param tableName - expected element of \sa iotmdbx::MdbxTable, \sa iotmdbx::MeterInfoMTable or \sa iotmdbx::MeterDataMTable.
		\param config - struct with table configuration parameters.
	*/
	void addTableConfig(const QString& tableName, const DocumentTableConfig& config);
	/**
		\brief Get hash with table configuration. Access to configuration parameters for each table.
		\return Hash with table name as key and \sa DocumentTableConfig as value.
	*/
	const QHash<QString/*tableName*/, DocumentTableConfig>& getTableConfigHash() const;
	/**
		\brief Check if all of \sa imtmdbx::IDocumentTable shared pointer references in \sa m_initializedTableHash are valid.
		\return True if all are valid, otherwise false.
	*/
	bool AreInitTablesValid() const;
	/**
		\brief Remove initialized table from \sa m_initializedTableHash.
		\param tableId - expected element of \sa iotmdbx::MdbxTable related structs.
		\return True if removed, otherwise false (e.g. table with \param tableId was not found in \sa m_initializedTableHash).
	*/
	bool remove(const QString& tableId);
	/**
		\brief Remove all initialized tables from \sa m_initializedTableHash.
				Does not remove table configuration from \sa m_tableConfigHash, so tables can be re-initialized by \sa get() or \sa add().
	*/
	void reset();
	/**
		\brief Reopen all initialized tables on the current transaction.
				Existing shared pointers held by callers remain valid.
	*/
	void reopen();
	/**
		\brief  Get initilized table by name \param tableId from \sa m_initializedTableHash.
				If it's not valid, initiaze it with parameters.
				Insert initialized table pointer to \sa m_initializedTableHash.
		\param TableClass - implementation of \sa imtmdbx::IDocumentTable interface.
		\param tableId - expected element of \sa iotmdbx::MdbxTable, \sa iotmdbx::MeterInfoMTable or \sa iotmdbx::MeterDataMTable.
		\param tableConfig - struct with table configuration parameters.
		\return Shared pointer to \sa imtmdbx::IDocumentTable interface implementation.
	*/
	template<class TableClass = imtmdbx::CDocumentTable>
	DocumentTablePtr add(const QString& tableId, const DocumentTableConfig& tableConfig);
private:
	/**
		\brief  Initilize table by name \param tableId.
				Insert initialized table pointer to \sa m_initializedTableHash.
		\params \sa CMdbxDocumentTableManager::add()
	*/
	template<class TableClass = imtmdbx::CDocumentTable>
	DocumentTablePtr InitTable(const QString& tableId, const DocumentTableConfig& tableConfig);

private:
	// constructor initialized reference to MDBX transaction, used for table initialization.
	mdbx::txn_managed& m_txn;
	/**
		Holder of configuration for tables.
		If table configuration exists here, it can be initialized when requested.
	*/
	QHash<QString/*tableName*/, DocumentTableConfig> m_tableConfigHash;
	/**
		Holder of initialized tables.
		If table is initialized, it should be stored here for future use.
	*/
	QHash<QString/*tableName*/, DocumentTablePtr> m_initializedTableHash;
};


// public template methods

template<class TableClass>
DocumentTablePtr CMdbxDocumentTableManager::add(
			const QString& tableId,
			const DocumentTableConfig& tableConfig)
{
	DocumentTablePtr retVal = get(tableId);
	if(!retVal.isNull()){
		return retVal;
	}

	return InitTable<TableClass>(tableId, tableConfig);
}


// private template methods

template<class TableClass>
DocumentTablePtr CMdbxDocumentTableManager::InitTable(
			const QString& tableId,
			const DocumentTableConfig& tableConfig)
{
	m_initializedTableHash[tableId] = QSharedPointer<TableClass>::create(
											tableId,
											m_txn,
											tableConfig.keyMode,
											tableConfig.valueMode,
											tableConfig.hasIndex
									);

	return m_initializedTableHash[tableId];
}


} // namespace imtmdbx


