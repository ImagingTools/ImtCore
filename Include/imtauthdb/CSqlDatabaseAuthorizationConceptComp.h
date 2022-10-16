#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauthgql/IAuthorizationConcept.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtauthdb
{


class CSqlDatabaseAuthorizationConceptComp:
		public ilog::CLoggerComponentBase,
		virtual public imtauthgql::IAuthorizationConcept

{
public:
	I_BEGIN_COMPONENT(CSqlDatabaseAuthorizationConceptComp);
	I_END_COMPONENT;

	// reimplemented (imtauthgql::IAuthorizationConcept)
	virtual int GetUserMode() const override;
	virtual QUuid GetUserToken(const QByteArray &login) const override;
	virtual bool LoginMatch(const QByteArray &login) const override;
	virtual bool Authorization(const QByteArray &login, const QByteArray &password) const override;
	virtual bool ChangePassword(const QByteArray &userId, const QString &oldPassword, const QString &newPassword) const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive &archive) override;

protected:
	I_REF(imtdb::IDatabaseEngine, m_dbEngineCompPtr);

};


} // namespace imtauthdb


