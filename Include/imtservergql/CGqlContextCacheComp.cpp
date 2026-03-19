// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlContextCacheComp.h>

#include <iterator>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDateTime>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>


namespace imtservergql
{

namespace
{


constexpr qint64 s_jwtContextCacheTtlMs = 5000;
constexpr int s_jwtContextCacheLimit = 256;


bool IsJwtToken(const QByteArray& token)
{
	return !token.isEmpty() && !token.startsWith("imt_pat_");
}


QByteArray GetTokenCacheKey(const QByteArray& token)
{
	return QCryptographicHash::hash(token, QCryptographicHash::Sha256);
}


qint64 GetJwtExpirationTimeMs(const QByteArray& token)
{
	QByteArrayList parts = token.split('.');
	if (parts.size() != 3){
		return 0;
	}

	QByteArray payload = QByteArray::fromBase64(parts[1], QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(payload, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()){
		return 0;
	}

	QJsonValue expValue = document.object().value("exp");
	if (!expValue.isDouble()){
		return 0;
	}

	qint64 expSecs = static_cast<qint64>(expValue.toDouble());
	if (expSecs <= 0){
		return 0;
	}

	return expSecs * 1000;
}


} // namespace


// public methods

// reimplemented (imtservergql::IGqlContextCache)

bool CGqlContextCacheComp::TryCreateRequestContext(
			const QByteArray& token,
			const QByteArray& productId,
			const imtgql::IGqlContext::Headers& headers,
			imtgql::IGqlContextSharedPtr& gqlContextPtr) const
{
	if (!IsJwtToken(token)){
		return false;
	}

	CachedContextItem item;
	{
		QReadLocker locker(&m_lock);
		auto itemIter = m_cacheItems.constFind(GetTokenCacheKey(token));
		if (itemIter == m_cacheItems.cend()){
			return false;
		}

		item = itemIter.value();
	}

	if (!item.prototypeContextPtr.IsValid() || item.expirationTimeMs <= QDateTime::currentMSecsSinceEpoch()){
		Remove(token);
		return false;
	}

	gqlContextPtr.MoveCastedPtr(item.prototypeContextPtr->CloneMe());
	if (!gqlContextPtr.IsValid()){
		Remove(token);
		return false;
	}

	gqlContextPtr->SetToken(token);
	gqlContextPtr->SetProductId(productId);
	gqlContextPtr->SetHeaders(headers);

	return true;
}


void CGqlContextCacheComp::Insert(
			const QByteArray& token,
			const imtgql::IGqlContext* gqlContextPtr) const
{
	if (!IsJwtToken(token) || gqlContextPtr == nullptr){
		return;
	}

	qint64 currentTimeMs = QDateTime::currentMSecsSinceEpoch();
	qint64 expirationTimeMs = currentTimeMs + s_jwtContextCacheTtlMs;
	qint64 jwtExpirationTimeMs = GetJwtExpirationTimeMs(token);
	if (jwtExpirationTimeMs > 0 && jwtExpirationTimeMs < expirationTimeMs){
		expirationTimeMs = jwtExpirationTimeMs;
	}

	if (expirationTimeMs <= currentTimeMs){
		return;
	}

	imtgql::IGqlContextSharedPtr prototypeContextPtr;
	prototypeContextPtr.MoveCastedPtr(gqlContextPtr->CloneMe());
	if (!prototypeContextPtr.IsValid()){
		return;
	}

	prototypeContextPtr->SetToken(QByteArray());
	prototypeContextPtr->SetProductId(QByteArray());
	prototypeContextPtr->SetHeaders(imtgql::IGqlContext::Headers());

	QWriteLocker locker(&m_lock);
	RemoveExpiredItems(currentTimeMs);
	EnsureSpaceForOneMoreItem();
	m_cacheItems.insert(GetTokenCacheKey(token), {prototypeContextPtr, expirationTimeMs, currentTimeMs});
}


// private methods

void CGqlContextCacheComp::Remove(const QByteArray& token) const
{
	QWriteLocker locker(&m_lock);
	m_cacheItems.remove(GetTokenCacheKey(token));
}


void CGqlContextCacheComp::RemoveExpiredItems(qint64 currentTimeMs) const
{
	for (auto itemIter = m_cacheItems.begin(); itemIter != m_cacheItems.end();){
		if (!itemIter.value().prototypeContextPtr.IsValid() || itemIter.value().expirationTimeMs <= currentTimeMs){
			itemIter = m_cacheItems.erase(itemIter);
		}
		else{
			++itemIter;
		}
	}
}


void CGqlContextCacheComp::EnsureSpaceForOneMoreItem() const
{
	while (m_cacheItems.size() >= s_jwtContextCacheLimit){
		auto oldestIter = m_cacheItems.begin();
		for (auto itemIter = std::next(m_cacheItems.begin()); itemIter != m_cacheItems.end(); ++itemIter){
			if (itemIter.value().createdAtMs < oldestIter.value().createdAtMs){
				oldestIter = itemIter;
			}
		}

		m_cacheItems.erase(oldestIter);
	}
}


} // namespace imtservergql

