// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <ilog/IMessageConsumer.h>


namespace imtbase
{


class IValidationContext;


/**
	Common interface for validation of a data object.
	\ingroup DataManagement
*/
class IDataValidator: virtual public istd::IPolymorphic
{
public:
	virtual istd::IInformationProvider::InformationCategory Validate(
				const IValidationContext* validationContextPtr,
				const istd::IChangeable& data,
				ilog::IMessageConsumer* validationConsumerPtr) const = 0;
};


} // namespace imtbase


