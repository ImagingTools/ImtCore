#pragma once


// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtauth/CUserBaseInfo.h>


namespace imtauth
{


class CUserInfo: virtual public IUserInfo, public CUserBaseInfo
{
	typedef CUserBaseInfo BaseClass;

public:
	// reimplemented (iser::IUserInfo)
	virtual QByteArray GetPasswordHash() const override;
	virtual void SetPasswordHash(const QByteArray& passwordHash) override;
	virtual QString GetMail() const override;
	virtual void SetMail(const QString& mail) override;
	virtual bool IsAdmin() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	QByteArray m_passwordHash;
	QString m_mail;
};


} // namespace imtauth



