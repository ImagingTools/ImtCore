#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


class ISuperuserController: virtual public istd::IPolymorphic
{
public:
	/**
		Set superuser password
	*/
	virtual bool SetSuperuserPassword(const QByteArray& password) const = 0;
};


} // namespace imtauth


