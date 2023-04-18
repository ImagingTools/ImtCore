#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class ISuperuserProvider: virtual public istd::IChangeable
{
public:
	/**
		Get information for exists superuser.
	*/
	virtual bool SuperuserExists() const = 0;

	/**
		Get information for superuser ID.
	*/
	virtual QByteArray GetSuperuserId() const = 0;
};


} // namespace imtauth


