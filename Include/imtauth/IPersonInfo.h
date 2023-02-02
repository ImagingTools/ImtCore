#pragma once

#include <imtauth/IAccountBaseInfo.h>

namespace imtauth
{

class IPersonInfo: virtual public IAccountBaseInfo{
public:
    enum NameFieldType
    {
        NFT_FIRST_NAME,
        NFT_LAST_NAME,
        NFT_NICKNAME
    };

    I_DECLARE_ENUM(NameFieldType, NFT_FIRST_NAME, NFT_LAST_NAME, NFT_NICKNAME);

    enum MetaInfoTypes
    {
        /**
            Contact first name given as QString.
        */
        MIT_FIRST_NAME,

        /**
            Contact last name given as QString.
        */
        MIT_LAST_NAME,

        /**
            Contact nickname given as QString.
        */
        MIT_NICKNAME,

        /**
            Contact gender type given as GenderType.
        */
        MIT_BIRTHDAY,
    };

    virtual QDate GetBirthday() const = 0;
    virtual void SetBirthday(const QDate& birthday) = 0;
    virtual QString GetNameField(NameFieldType fieldType) const = 0;
    virtual void SetNameField(NameFieldType fieldType, const QString& value) = 0;
};


} // namespace imtauth
