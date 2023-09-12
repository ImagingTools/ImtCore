#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtauth
{


class ISuperuserProvider: virtual public istd::IChangeable
{
public:
	/**
		Data model change notification flags.
	*/
	enum ChangeFlags
	{
		CF_SU_EXISTS = 0x36e7b88
	};

	/**
		Get information for exists superuser.
	*/
	virtual bool SuperuserExists(QString& errorMessage) const = 0;

	/**
		Get information for superuser ID.
	*/
	virtual QByteArray GetSuperuserId() const = 0;
};


} // namespace imtauth


