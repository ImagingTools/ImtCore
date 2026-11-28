// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Optional part of an operation context: the document action the operation writes into.
	A delegate that keeps revisions records every document it writes for such an operation in the
	journal of the document action (see imtdb::IDocumentActionJournal), so the document action can be rolled back as a whole.
	Query it from an IOperationContext with dynamic_cast; an empty ID means no document action.
	\ingroup Collection
*/
class IDocumentActionContext: virtual public istd::IChangeable
{
public:
	virtual QByteArray GetDocumentActionId() const = 0;
};


} // namespace imtbase


