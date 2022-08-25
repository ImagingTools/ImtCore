#pragma once


// ImtCore includes
#include "imtbase/CCollectionInfo.h"
#include <imtauth/IUserInfo.h>
#include <imtauth/CRole.h>


namespace imtauth
{

class CUserInfo: virtual public IUserInfo, public CRole
{

public:
	typedef CRole BaseClass;

	// reimplemented (iser::IUserInfo)
	virtual QByteArray GetId() const override;
	virtual void SetId(QByteArray id) override;
	virtual QString GetUsername() const override;
	virtual void SetUsername(QString username) override;
	virtual QString GetName() const override;
	virtual void SetName(QString name) override;
	virtual QByteArray GetPasswordHash() const override;
	virtual void SetPasswordHash(QByteArray passwordHash) override;
	virtual QString GetMail() const override;
	virtual void SetMail(QString mail) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	QString m_username;
	QString m_name;
	QByteArray m_passwordHash;
	QString m_mail;

};


} // namespace imtauth



