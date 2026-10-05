// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CPublisherSubscriberBridgeComp.h>


namespace imtservergql
{


/**
	Forwards the collection document-service subscriptions of selected collections
	to a remote server.

	The document-service subscriptions are a single command per operation
	(\c OnDocumentManagerChanged, \c OnUndoRedoChanged, \c OnDocumentChanged) that
	names its collection in the \c input argument, where they used to be one command
	per collection (\c OnUsersDocumentChanged and friends). A plain
	\c CPublisherSubscriberBridgeComp selects on the command alone, so it can no
	longer tell a remotely served collection from a locally served one and would
	forward both upstream.

	This component adds that missing half of the routing decision - and only that -
	so the bridge itself stays unaware of collections.
*/
class CDocumentServiceSubscriberBridgeComp: public CPublisherSubscriberBridgeComp
{
public:
	typedef CPublisherSubscriberBridgeComp BaseClass;

	I_BEGIN_COMPONENT(CDocumentServiceSubscriberBridgeComp);
		I_ASSIGN_MULTI_0(m_collectionIdsAttrPtr, "CollectionIds", "Collections whose document service is served by the remote server", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriberController via CGqlPublisherCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

private:
	I_MULTIATTR(QByteArray, m_collectionIdsAttrPtr);
};


} // namespace imtservergql
