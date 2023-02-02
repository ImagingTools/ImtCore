#include <imtauth/CPersonInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{
    QString CPersonInfo::GetAccountName() const
    {
        return m_accountName;
    }

    QString CPersonInfo::GetAccountDescription() const
    {
        return m_accountDescription;
    }

    CAddress CPersonInfo::GetAddress() const
    {
        return m_address;
    }

    QString CPersonInfo::GetMail() const
    {
        return m_mail;
    }

    QString CPersonInfo::GetNameField(NameFieldType fieldType) const
    {
        switch(fieldType){
        case NFT_FIRST_NAME:
            return m_firstName;
        case NFT_LAST_NAME:
            return m_lastName;
        case NFT_NICKNAME:
            return m_nick;
        default:
            return QString();
        };
    }

    QDate CPersonInfo::GetBirthday() const
    {
        return m_birthday;
    }

    void CPersonInfo::SetAccountName(const QString &accountName)
    {
        if (m_accountName != accountName){
            istd::CChangeNotifier changeNotifier(this);

            m_accountName = accountName;
        }
    }

    void CPersonInfo::SetAccountDescription(const QString &accountDescription)
    {
        if (m_accountDescription != accountDescription){
            istd::CChangeNotifier changeNotifier(this);

            m_accountDescription = accountDescription;
        }
    }

    void CPersonInfo::SetAddress(const CAddress &address)
    {
        if (!m_address.IsEqual(address)){
            istd::CChangeNotifier changeNotifier(this);

            m_address = address;
        }
    }

    void CPersonInfo::SetMail(const QString &mail)
    {
        if (m_mail != mail){
            istd::CChangeNotifier changeNotifier(this);

            m_mail = mail;
        }
    }

    void CPersonInfo::SetNameField(NameFieldType fieldType, const QString &value)
    {
        switch(fieldType){
        case NFT_FIRST_NAME:
            if (m_firstName != value){
                istd::CChangeNotifier changeNotifier(this);

                m_firstName = value;
            }
            break;
        case NFT_LAST_NAME:
            if (m_lastName != value){
                istd::CChangeNotifier changeNotifier(this);

                m_lastName = value;
            }
            break;
        case NFT_NICKNAME:
            if (m_nick != value){
                istd::CChangeNotifier changeNotifier(this);

                m_nick = value;
            }
            break;
        };
    }

    void CPersonInfo::SetBirthday(const QDate &birthday)
    {
        if (m_birthday != birthday){
            istd::CChangeNotifier changeNotifier(this);

            m_birthday = birthday;
        }
    }

    bool CPersonInfo::Serialize(iser::IArchive& archive)
    {
        istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

        bool retVal = true;

        static iser::CArchiveTag accountNameTag("AccountName", "Account name", iser::CArchiveTag::TT_LEAF);
        retVal = archive.BeginTag(accountNameTag);
        retVal = retVal && archive.Process(m_accountName);
        retVal = retVal && archive.EndTag(accountNameTag);

        static iser::CArchiveTag accountDescriptionTag("AccountDescription", "Account description", iser::CArchiveTag::TT_LEAF);
        retVal = retVal && archive.BeginTag(accountDescriptionTag);
        retVal = retVal && archive.Process(m_accountDescription);
        retVal = retVal && archive.EndTag(accountDescriptionTag);

        static iser::CArchiveTag addressTag("Address", "Address", iser::CArchiveTag::TT_GROUP);
        retVal = retVal && archive.BeginTag(addressTag);
        retVal = retVal && m_address.Serialize(archive);
        retVal = retVal && archive.EndTag(addressTag);

        static iser::CArchiveTag mailTag("Mail", "Mail", iser::CArchiveTag::TT_LEAF);
        retVal = retVal && archive.BeginTag(mailTag);
        retVal = retVal && archive.Process(m_mail);
        retVal = retVal && archive.EndTag(mailTag);

        static iser::CArchiveTag firstNameTag("firstName", "First name", iser::CArchiveTag::TT_LEAF);
        retVal = retVal && archive.BeginTag(firstNameTag);
        retVal = retVal && archive.Process(m_firstName);
        retVal = retVal && archive.EndTag(firstNameTag);

        // TODO over var

        return retVal;
    }
}
