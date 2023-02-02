#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iser/IObject.h>
#include <imtauth/CAddress.h>


namespace imtauth
{

/**
    Interface describing the company information.
	\ingroup Authentification
*/

class IAccountBaseInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
        /**
            Account name given as QString.
        */
        MIT_ACCOUNT_NAME,

        /**
            Account description given as QString.
        */
        MIT_ACCOUNT_DESCRIPTION,

        /**
            Adress given as CAddress.
        */
        MIT_ADDRESS,

        /**
            Mail given as QString.
        */
        MIT_MAIL
	};

    virtual QString GetAccountName() const = 0;
    virtual void SetAccountName(const QString& accountName) = 0;
    virtual QString GetAccountDescription() const = 0;
    virtual void SetAccountDescription(const QString& accountDescription) = 0;
    virtual CAddress GetAddress() const = 0;
    virtual void SetAddress(const CAddress& address) = 0;
    virtual QString GetMail() const = 0;
    virtual void SetMail(const QString& mail) = 0;
};


} // namespace imtauth


