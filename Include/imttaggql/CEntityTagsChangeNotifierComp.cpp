// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttaggql/CEntityTagsChangeNotifierComp.h>


// Qt includes
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imttagsdl/SDL/1.0/CPP/Tags.h>


namespace imttaggql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CEntityTagsChangeNotifierComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (gqlRequest.GetCommandId() == GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CEntityTagsChangeNotifierComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_assignmentManagerModelCompPtr.IsValid()){
		m_assignmentManagerModelCompPtr->AttachObserver(this);
	}
}


void CEntityTagsChangeNotifierComp::OnComponentDestroyed()
{
	if (m_assignmentManagerModelCompPtr.IsValid()){
		m_assignmentManagerModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CEntityTagsChangeNotifierComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(imttag::ITagAssignmentManager::CF_ASSIGNMENTS_CHANGED)){
		return;
	}

	const QByteArray entityType = changeSet.GetChangeInfo("EntityType").toByteArray();
	const QByteArray tenantId = changeSet.GetChangeInfo("TenantId").toByteArray();

	QList<sdl::V1_0::imttag::CEntityTagChange> changes;
	for (const QVariant& changeVariant : changeSet.GetChangeInfo("Changes").toList()){
		const QVariantMap changeMap = changeVariant.toMap();

		sdl::V1_0::imttag::CEntityTagChange change;
		change.entityId = changeMap.value("EntityId").toByteArray();
		change.addedTagIds.Emplace().FromList(changeMap.value("AddedTagIds").value<QByteArrayList>());
		change.removedTagIds.Emplace().FromList(changeMap.value("RemovedTagIds").value<QByteArrayList>());

		changes << change;
	}

	sdl::V1_0::imttag::CEntityTagsChangedPayload payload;
	payload.entityType = QString::fromUtf8(entityType);
	payload.changes.Emplace().FromList(changes);

	QJsonObject jsonObject;
	if (!payload.WriteToJsonObject(jsonObject)){
		return;
	}

	const QByteArray data = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);

	PublishDataFiltered(GetCommandId(), data, [entityType, tenantId](const imtgql::CGqlRequest& gqlRequest) -> bool
	{
		const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
		if ((inputPtr == nullptr) || (inputPtr->GetParamArgumentValue("entityType").toByteArray() != entityType)){
			return false;
		}

		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();

		return (contextPtr != nullptr) && (contextPtr->GetTenantId() == tenantId);
	});
}


// private methods

QByteArray CEntityTagsChangeNotifierComp::GetCommandId() const
{
	if (m_commandIdsAttrPtr.IsValid() && (m_commandIdsAttrPtr.GetCount() > 0) && !m_commandIdsAttrPtr[0].isEmpty()){
		return m_commandIdsAttrPtr[0];
	}

	return QByteArrayLiteral("OnEntityTagsChanged");
}


} // namespace imttaggql


