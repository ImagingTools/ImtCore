#pragma once


// Qt includes
#include <QtCore/QDate>

// ACF includes
#include <imtauth/IPersonInfo.h>


namespace imtauth
{


class CPersonInfo: virtual public IPersonInfo
{
public:
    virtual QString GetAccountName() const override;
    virtual void SetAccountName(const QString &accountName) override;
    virtual QString GetAccountDescription() const override;
    virtual void SetAccountDescription(const QString &accountDescription) override;
    virtual CAddress GetAddress() const override;
    virtual void SetAddress(const CAddress &address) override;
    virtual QString GetMail() const override;
    virtual void SetMail(const QString &mail) override;

    virtual QDate GetBirthday() const override;
    virtual void SetBirthday(const QDate &birthday) override;
    virtual QString GetNameField(NameFieldType fieldType) const override;
    virtual void SetNameField(NameFieldType fieldType, const QString &value) override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;

private:
    QString m_accountName;
    QString m_accountDescription;
    CAddress m_address;
    QString m_mail;

    QString m_firstName;
    QString m_lastName;
    QString m_nick;
    QDate m_birthday;
};


}
