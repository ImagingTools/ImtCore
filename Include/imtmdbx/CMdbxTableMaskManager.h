#pragma once


// ACF includes
#include <ilog/CLoggerBase.h>

// ImtCore includes
#include <imtmdbx/imtmdbx.h>
#include <imtmdbx/IMask.h>
#include <imtmdbx/CMdbxDocumentTableManager.h>


namespace imtmdbx
{


using MaskPtr = QSharedPointer<imtmdbx::IMask>;


class CMdbxTableMaskManager: public ilog::CLoggerBase
{
public:
	enum class MaskKind {
		Normal,
		Double
	};

	struct MaskConfig {
		MaskKind kind;

		MaskConfig(MaskKind kind = MaskKind::Normal)
			: kind(kind) {}
	};

	CMdbxTableMaskManager(
		mdbx::txn_managed& txn,
		imtmdbx::CMdbxDocumentTableManager& tableManager,
		const QHash<QString, MaskConfig>& maskConfigHash = {}
	);
	virtual ~CMdbxTableMaskManager() = default;

	/**
		\brief Insert external mask to \sa m_activeMaskHash and \sa m_initializedMaskHash.

		\param maskName - any external key or element of \sa iotmdbx::MdbxMask
				if replacement of already initialized masks if warranted.
		\param maskPtr - initialized shared pointer to \sa imtmdbx::IMask interface implementation.
		\return True if inserted, otherwise false (e.g. failed to remove pre-existing mask with \param maskName).
	*/
	bool insertExternalMask(const QString& maskName, MaskPtr maskPtr);
	/**
		\brief Get an initialized mask from \sa m_initializedMaskHash.
				If it's not valid, initiaze it.
				Insert initialized mask pointer to \sa m_initializedMaskHash.
		\param maskName - expected element of \sa iotmdbx::MdbxMask
				Can also be an external key if added by \sa insertExternMask().
		\return Shared pointer to \sa imtmdbx::IMask interface implementation if found, otherwise uninitialized QSharedPointer.
	*/
	MaskPtr get(const QString& maskName);
	/**
		\brief Remove mask with \param maskName from \sa m_initializedMaskHash.
				Also remove it from \sa m_activeMaskHash if exists, so it will be unapplied.
		\param maskName - expected element of \sa iotmdbx::MdbxMask
				Can also be an external key if added by \sa insertExternMask().
		\return True if removed, otherwise false (e.g. if mask with \param maskName was not found in \sa m_initializedMaskHash).
	*/
	bool remove(const QString& maskName);
	/**
		\brief Clear \sa m_initializedMaskHash, so all masks will be uninitialized.
				Also clear \sa m_activeMaskHash, so all masks will be unapplied.
	*/
	void reset();
	/**
		\brief Reopen all initialized masks on the current transaction.
				Existing shared pointers held by callers remain valid.
	*/
	void reopen();
	/**
		\brief Get set of active masks from \sa m_activeMaskHash.
		\return Set of \sa imtmdbx::IMask \sa m_activeMaskHash.
	*/
	QSet<MaskPtr> getActiveMaskSet() const;
	/**
		\brief Check if all of \sa imtmdbx::IMask shared pointer references in \sa m_initializedMaskHash are valid.
		\return True if all are valid, otherwise false.
	*/
	bool IsValid() const;
	/**
		\brief Get initilized mask by name \param maskName from \sa m_maskMap
				If it's not valid, initialize it and insert to \sa m_initializedMaskHash and to \sa m_activeMaskHash.
		\param MaskClass - implementation of \sa imtmdbx::IMask interface.
		\param maskName - expected element of \sa iotmdbx::MdbxMask.
		\param externKey - expected paremeter for \sa imtmdbx::IMask interface implemetation in form of \sa imtmdbx::CDoubleMask.
		\return True if all are valid, otherwise false.
	*/
	template<class MaskClass = imtmdbx::CMask>
	bool insertActiveMask(const QString& maskName, const quint64& externKey = 0);

	/**
		\brief Add individual mask configuration dynamically.
	*/
	void addMaskConfig(const QString& maskName, const MaskConfig& config);

	/**
		\brief Set multiple mask configurations.
	*/
	void setMaskConfigHash(const QHash<QString, MaskConfig>& maskConfigHash);

	/**
		\brief Get current mask configurations.
	*/
	const QHash<QString, MaskConfig>& getMaskConfigHash() const;

protected:
	/**
		\brief Factory method for creating masks of different types depending on the mask name and configuration.
				Returns a null pointer if the mask name is unknown.
	*/
	virtual MaskPtr createMask(const QString& maskName);

protected:
	/**
		Constructor initialized reference to MDBX transaction, used for mask initialization.
	*/
	mdbx::txn_managed& m_txn;
	/**
		Constructor initialized reference to MDBX table manager, used for table access.
	*/
	imtmdbx::CMdbxDocumentTableManager& m_tableManager;
	/**
		Holder of mask configuration configurations.
	*/
	QHash<QString, MaskConfig> m_maskConfigHash;
	/**
		Holder of initialized masks.
		If mask is initialized, it should be stored here for future use.
	*/
	QHash<QString, MaskPtr> m_initializedMaskHash;
	/**
		Holder of applied masks.
		If mask is applied, it should be stored here for future use in mask container.
	*/
	QHash<QString, MaskPtr> m_activeMaskHash;
};


// public template functions

template<class MaskClass>
bool CMdbxTableMaskManager::insertActiveMask(const QString& maskName, const quint64& externKey)
{
	MaskPtr maskPtr = this->get(maskName);
	if(!maskPtr){
		maskPtr = m_initializedMaskHash[maskName] = QSharedPointer<MaskClass>::create(maskName, m_txn);
	}

	if (auto doubleMaskPtr = qSharedPointerDynamicCast<imtmdbx::CDoubleMask>(maskPtr)) {
		doubleMaskPtr->SetExternalKey(externKey);
	}

	m_activeMaskHash.insert(maskName, maskPtr);

	return true;
}


} // namespace imtmdbx
