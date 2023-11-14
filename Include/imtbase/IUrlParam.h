#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


/**
	Interface for a object containing URL.
*/
class IUrlParam: virtual public iser::ISerializable
{
public:
	/**
		Get the object URL.
	*/
	virtual const QUrl& GetUrl() const = 0;

	/**
		Set the object URL.
	*/
	virtual bool SetUrl(const QUrl& url) = 0;

	/**
		Return \c true, if the URL is readonly.
	*/
	virtual bool IsReadOnly() const = 0;
};


} // namespace imtbase


