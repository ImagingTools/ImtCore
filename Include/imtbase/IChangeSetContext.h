// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Optional part of an operation context: the change set the operation writes into.
	A delegate that keeps revisions records every document it writes for such an operation in the
	journal of the change set (see IChangeSetJournal), so the change set can be rolled back as a whole.
	Query it from an IOperationContext with dynamic_cast; an empty ID means no change set.
	\ingroup Collection
*/
class IChangeSetContext: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetChangeSetId() const = 0;
};


} // namespace imtbase


