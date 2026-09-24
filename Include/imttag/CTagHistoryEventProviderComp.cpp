// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTagHistoryEventProviderComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imttag/ITagEvent.h>


namespace imttag
{


// reimplemented (imtbase::IDocumentHistoryEventProvider)

imtbase::IDocumentHistoryEventProvider::HistoryEvents CTagHistoryEventProviderComp::GetHistoryEvents(
			const QByteArray& collectionId,
			const QByteArray& documentId,
			const QByteArray& languageId) const
{
	HistoryEvents retVal;

	if (!m_eventCollectionCompPtr.IsValid() || collectionId.isEmpty() || documentId.isEmpty()){
		return retVal;
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpression;
	filterExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(collectionId));
	filterExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityId", QString::fromUtf8(documentId));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpression);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	const QString taggedText = QString::fromUtf8(iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TRANSLATE_NOOP("Tags", "Added tag \"%1\""), languageId, "Tags"));
	const QString untaggedText = QString::fromUtf8(iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TRANSLATE_NOOP("Tags", "Removed tag \"%1\""), languageId, "Tags"));

	const imtbase::ICollectionInfo::Ids eventIds = m_eventCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	for (const QByteArray& eventId : eventIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_eventCollectionCompPtr->GetObjectData(eventId, dataPtr)){
			continue;
		}

		const ITagEvent* eventPtr = dynamic_cast<const ITagEvent*>(dataPtr.GetPtr());
		if (eventPtr == nullptr){
			continue;
		}

		HistoryEvent historyEvent;
		historyEvent.timestamp = eventPtr->GetTimestamp();
		historyEvent.user = eventPtr->GetActorName();
		historyEvent.revision = eventPtr->GetEntityRevision();
		historyEvent.kind = QByteArrayLiteral("TagEvent");
		historyEvent.description = ((eventPtr->GetAction() == ITagEvent::A_UNTAGGED) ? untaggedText : taggedText).arg(eventPtr->GetTagName());

		retVal << historyEvent;
	}

	return retVal;
}


} // namespace imttag


