#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


class ICredentialController: virtual public istd::IPolymorphic
{
public:
	virtual bool CheckCredential(const QByteArray& login, const QByteArray& password) const = 0;
};


} // namespace imtauth


