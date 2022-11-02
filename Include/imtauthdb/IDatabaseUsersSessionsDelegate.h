#pragma once

//Qt includes
#include <QByteArray>
#include <QList>
#include <QUuid>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauthdb
{


class IDatabaseUsersSessionsDelegate: virtual public istd::IPolymorphic
{
public:
	virtual QList<QUuid> GetAllTokens(const QByteArray& userId) const = 0;
	virtual QByteArray CreateInsertNewTokenQuery(const QByteArray& userId) const = 0;
	virtual QByteArray CreateDeleteTokenQuery(const QUuid& uuid) const = 0;

	virtual QByteArray GetCountQuery() const = 0;

	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray()) const = 0;

	virtual QByteArray CreateDeleteObjectQuery(const QByteArray& objectId) const = 0;

	virtual QByteArray CreateUpdateObjectQuery(const QByteArray& objectId) const = 0;
};


} // namespace imtauthdb


