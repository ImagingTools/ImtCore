// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtassistgql/CSdlApiCatalog.h>


namespace imtassistgql
{


/**
	Public API of the application for the assistant, with the policy of the product:
	which commands the assistant may use and which fields it never sees or sends.

	\ingroup imtassistgql
*/
class IAssistantApiCatalog: virtual public istd::IPolymorphic
{
public:
	virtual const CSdlApiCatalog& GetCatalog() const = 0;

	/**
		Check whether the assistant may use the command.
	*/
	virtual bool IsCommandAllowed(const QString& commandName) const = 0;

	/**
		Check whether a field holds secrets, e.g. passwords or tokens: it is removed from results
		and refused in inputs.
	*/
	virtual bool IsFieldForbidden(const QString& fieldName) const = 0;
};


} // namespace imtassistgql


