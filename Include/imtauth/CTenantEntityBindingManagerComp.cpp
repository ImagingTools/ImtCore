// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantEntityBindingManagerComp.h>


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauth
{


// reimplemented (imtauth::ITenantEntityBindingManager)

bool CTenantEntityBindingManagerComp::AddBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId)
{
	if (!m_bindingsCollectionCompPtr.IsValid() || !m_bindingFactoryCompPtr.IsValid()){
		SendErrorMessage(0, "Bindings collection or factory not configured", "CTenantEntityBindingManagerComp");
		return false;
	}

	if (tenantId.isEmpty() || entityType.isEmpty() || entityId.isEmpty()){
		SendErrorMessage(0, "AddBinding: tenantId, entityType, and entityId must not be empty", "CTenantEntityBindingManagerComp");
		return false;
	}

	ITenantEntityBindingUniquePtr bindingPtr = m_bindingFactoryCompPtr.CreateInstance();
	if (!bindingPtr.IsValid()){
		SendErrorMessage(0, "Failed to create binding object", "CTenantEntityBindingManagerComp");
		return false;
	}

	bindingPtr->SetTenantId(tenantId);
	bindingPtr->SetEntityType(entityType);
	bindingPtr->SetEntityId(entityId);
	bindingPtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

	QByteArray retVal = m_bindingsCollectionCompPtr->InsertNewObject("TenantEntityBinding", QString(), QString(), bindingPtr.GetPtr());
	if (retVal.isEmpty()){
		// ON CONFLICT DO NOTHING means the insert succeeds silently for duplicates,
		// but if this collection doesn't support that, log and return true (idempotent)
		SendInfoMessage(0, QString("Binding may already exist for tenant '%1', entity '%2'/'%3'")
			.arg(QString::fromUtf8(tenantId), QString::fromUtf8(entityType), QString::fromUtf8(entityId)), "CTenantEntityBindingManagerComp");
		return true;
	}

	return true;
}


bool CTenantEntityBindingManagerComp::RemoveBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId)
{
	if (!m_bindingsCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Bindings collection not configured", "CTenantEntityBindingManagerComp");
		return false;
	}

	QByteArrayList idsToRemove = FindBindingIds(tenantId, entityType, entityId);
	if (idsToRemove.isEmpty()){
		return true;
	}

	if (!m_bindingsCollectionCompPtr->RemoveElements(idsToRemove)){
		SendErrorMessage(0, QString("Failed to remove binding for tenant '%1', entity '%2'/'%3'")
			.arg(QString::fromUtf8(tenantId), QString::fromUtf8(entityType), QString::fromUtf8(entityId)), "CTenantEntityBindingManagerComp");
		return false;
	}

	return true;
}


bool CTenantEntityBindingManagerComp::RemoveAllBindingsForEntity(const QByteArray& entityType, const QByteArray& entityId)
{
	if (!m_bindingsCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Bindings collection not configured", "CTenantEntityBindingManagerComp");
		return false;
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(entityType));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityId", QString::fromUtf8(entityId));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	if (!m_bindingsCollectionCompPtr->RemoveElementSet(&filterParams)){
		SendErrorMessage(0, QString("Failed to remove all bindings for entity '%1'/'%2'")
			.arg(QString::fromUtf8(entityType), QString::fromUtf8(entityId)), "CTenantEntityBindingManagerComp");
		return false;
	}

	return true;
}


bool CTenantEntityBindingManagerComp::RemoveAllBindingsForTenant(const QByteArray& tenantId, const QByteArray& entityType)
{
	if (!m_bindingsCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Bindings collection not configured", "CTenantEntityBindingManagerComp");
		return false;
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("TenantId", QString::fromUtf8(tenantId));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(entityType));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	if (!m_bindingsCollectionCompPtr->RemoveElementSet(&filterParams)){
		SendErrorMessage(0, QString("Failed to remove all bindings for tenant '%1', type '%2'")
			.arg(QString::fromUtf8(tenantId), QString::fromUtf8(entityType)), "CTenantEntityBindingManagerComp");
		return false;
	}

	return true;
}


bool CTenantEntityBindingManagerComp::HasBinding(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) const
{
	return !FindBindingIds(tenantId, entityType, entityId).isEmpty();
}


QByteArrayList CTenantEntityBindingManagerComp::GetEntityIds(const QByteArray& tenantId, const QByteArray& entityType) const
{
	if (!m_bindingsCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("TenantId", QString::fromUtf8(tenantId));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(entityType));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	QByteArrayList bindingIds = m_bindingsCollectionCompPtr->GetElementIds(0, -1, &filterParams);

	QByteArrayList entityIds;
	for (const QByteArray& bindingId : bindingIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_bindingsCollectionCompPtr->GetObjectData(bindingId, dataPtr)){
			const ITenantEntityBinding* bindingPtr = dynamic_cast<const ITenantEntityBinding*>(dataPtr.GetPtr());
			if (bindingPtr != nullptr){
				entityIds.append(bindingPtr->GetEntityId());
			}
		}
	}

	return entityIds;
}


bool CTenantEntityBindingManagerComp::HasAnyTenantBinding(const QByteArray& entityType, const QByteArray& entityId) const
{
	if (!m_bindingsCollectionCompPtr.IsValid() || entityType.isEmpty() || entityId.isEmpty()){
		return false;
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(entityType));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityId", QString::fromUtf8(entityId));
	// Do not restrict by TenantId: any tenant binding means it is tenant-scoped.

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	QByteArrayList bindingIds = m_bindingsCollectionCompPtr->GetElementIds(0, -1, &filterParams);
	return !bindingIds.isEmpty();
}


// private methods

QByteArrayList CTenantEntityBindingManagerComp::FindBindingIds(const QByteArray& tenantId, const QByteArray& entityType, const QByteArray& entityId) const
{
	if (!m_bindingsCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	imtbase::IComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("TenantId", QString::fromUtf8(tenantId));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityType", QString::fromUtf8(entityType));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("EntityId", QString::fromUtf8(entityId));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	return m_bindingsCollectionCompPtr->GetElementIds(0, -1, &filterParams);
}


} // namespace imtauth
