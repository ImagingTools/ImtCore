#pragma once


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtauth/ISession.h>
#include <imtauth/IUserInfo.h>


namespace imtauth
{

class CSessionInfo: virtual public ISession
{

public:
	// reimplemented (iser::ISession)
	virtual QUuid GetToken() const override;
	virtual void SetToken(const QUuid &token) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray &userId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

private:
	QUuid m_token;
	QByteArray m_userId;
};


} // namespace imtauth



