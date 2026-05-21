// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Observer interface for document lifecycle events.

	Implementations receive all document-lifecycle notifications produced by
	an \c IDocumentService via a single \c ProcessEvent dispatch point.
	The concrete event type can be determined by calling \c CEventBase::Type()
	and then down-casting to the appropriate subclass (e.g.\
	\c CDocumentOpenedEvent, \c CDocumentChangedEvent, etc.).

	Register handlers at runtime with
	\c IDocumentService::RegisterEventHandler, or wire them up statically
	through the ACF component system using the
	\c "DocumentServiceEventHandler" multi-reference slot on
	\c CDocumentServiceCompBase.
*/
class IDocumentServiceEventHandler : virtual public istd::IPolymorphic
{
public:
	/**
		\brief Process a document lifecycle event.

		\param eventPtr  Non-null pointer to the event object.  The object is
		                 owned by the caller; the handler must not delete it.
		\return          \c true when the handler accepted the event;
		                 \c false to indicate that the event was rejected or
		                 not handled (interpretation is implementation-defined).
	*/
	virtual bool ProcessEvent(CEventBase* eventPtr) = 0;
};


} // namespace imtdoc


