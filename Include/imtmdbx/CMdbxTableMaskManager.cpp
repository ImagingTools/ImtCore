#include <imtmdbx/CMdbxTableMaskManager.h>


// ImtCore includes
#include <imtmdbx/CMemoryMask.h>


namespace imtmdbx
{


// public methods

CMdbxTableMaskManager::CMdbxTableMaskManager(
	mdbx::txn_managed& txn,
	imtmdbx::CMdbxDocumentTableManager& tableManager,
	const QHash<QString, MaskConfig>& maskConfigHash
):
	m_txn(txn),
	m_tableManager(tableManager),
	m_maskConfigHash(maskConfigHash)
{
}


bool CMdbxTableMaskManager::insertExternalMask(const QString& maskName, MaskPtr maskPtr)
{
	m_initializedMaskHash.insert(maskName, maskPtr);
	m_activeMaskHash.insert(maskName, maskPtr);

	return true;
}


QSet<MaskPtr> CMdbxTableMaskManager::getActiveMaskSet() const
{
	const QList<MaskPtr> valueList = m_activeMaskHash.values();

	return QSet<MaskPtr>(valueList.begin(), valueList.end());
}


MaskPtr CMdbxTableMaskManager::get(const QString& maskName)
{
	auto it = m_initializedMaskHash.find(maskName);
	if (it != m_initializedMaskHash.end() && !it.value().isNull()) {
		return it.value();
	}

	MaskPtr retVal = createMask(maskName);
	if (retVal) {
		m_initializedMaskHash[maskName] = retVal;
	}

	return retVal;
}


MaskPtr CMdbxTableMaskManager::createMask(const QString& maskName)
{
	auto it = m_maskConfigHash.find(maskName);
	if (it == m_maskConfigHash.end()) {
		return {};
	}

	const MaskConfig& config = it.value();
	if (config.kind == MaskKind::Double) {
		return QSharedPointer<imtmdbx::CDoubleMask>::create(maskName, m_txn);
	}

	return QSharedPointer<imtmdbx::CMask>::create(maskName, m_txn);
}


bool CMdbxTableMaskManager::remove(const QString& maskName)
{
	m_activeMaskHash.remove(maskName);

	return m_initializedMaskHash.remove(maskName);
}


void CMdbxTableMaskManager::reset()
{
	m_activeMaskHash.clear();
	m_initializedMaskHash.clear();
}


void CMdbxTableMaskManager::reopen()
{
	for(auto [key, value] : m_initializedMaskHash.asKeyValueRange()){
		if (value.isNull()) {
			continue;
		}

		if (auto* docTable = dynamic_cast<imtmdbx::IDocumentTable*>(value.data())) {
			docTable->Reopen();
		}
	}
}


bool CMdbxTableMaskManager::IsValid() const{
	if(m_initializedMaskHash.isEmpty()){
		return false;
	}

	bool retVal = true;
	for(auto [key, value] : m_initializedMaskHash.asKeyValueRange()){
		retVal = retVal && !value.isNull();
	}

	return retVal;
}


void CMdbxTableMaskManager::addMaskConfig(const QString& maskName, const MaskConfig& config)
{
	m_maskConfigHash.insert(maskName, config);
}


void CMdbxTableMaskManager::setMaskConfigHash(const QHash<QString, MaskConfig>& maskConfigHash)
{
	m_maskConfigHash = maskConfigHash;
}


const QHash<QString, CMdbxTableMaskManager::MaskConfig>& CMdbxTableMaskManager::getMaskConfigHash() const
{
	return m_maskConfigHash;
}


} // namespace imtmdbx
