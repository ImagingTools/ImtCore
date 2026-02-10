// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class IGqlResponse: virtual public istd::IChangeable
{
public:
	typedef istd::TSharedInterfacePtr<IGqlRequest> GqlRequestPtr;
	typedef QList<QVariantMap> ErrorEntryList;

	virtual GqlRequestPtr GetOriginalRequest() const = 0;

	/**
		Returns RAW data from response
	*/
	virtual QByteArray GetResponseData() const = 0;

	/**
		Returns valuable data of payload
	*/
	virtual QByteArray GetData() const = 0;

	/**
		Returns the value of the Errors described entry section.
		\details
		If the request raised any errors, this list must contain at least one entry with key errors.
		If the request completed without raising any errors, this entry must not be present.
	*/
	virtual ErrorEntryList GetErrorList() const = 0;

	/**
		Returns the list of error messages the same thing if you call \c GetErrorList()[index]["message"]
		\details
		If the request raised any errors, this list must contain at least one message.
		If the request completed without raising any errors, this list must be empty.
	*/
	virtual QStringList GetErrorMessageList() const = 0;
};



} // namespace imtgql


