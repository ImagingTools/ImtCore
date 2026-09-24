// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtbase/IDocumentHistoryEventProvider.h>
#include <imtbase/IObjectCollection.h>


namespace imttag
{


/**
	Shows the Tagged/Untagged events of a document in its history.
	The collection ID of the document is the entity type of its tag assignments.
*/
class CTagHistoryEventProviderComp:
			public icomp::CComponentBase,
			virtual public imtbase::IDocumentHistoryEventProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTagHistoryEventProviderComp);
		I_REGISTER_INTERFACE(imtbase::IDocumentHistoryEventProvider);
		I_ASSIGN(m_eventCollectionCompPtr, "EventCollection", "SQL-backed TagEvents collection", true, "TagEventCollection");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	// reimplemented (imtbase::IDocumentHistoryEventProvider)
	virtual HistoryEvents GetHistoryEvents(
				const QByteArray& collectionId,
				const QByteArray& documentId,
				const QByteArray& languageId) const override;

private:
	I_REF(imtbase::IObjectCollection, m_eventCollectionCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imttag


