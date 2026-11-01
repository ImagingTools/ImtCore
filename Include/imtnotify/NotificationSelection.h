// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtnotify/NotificationTypes.h>


namespace imtnotify
{


/**
	Fields of the collection of the notifications that can be filtered or sorted.
*/
namespace NotificationField
{
static const inline QByteArray s_severity = QByteArrayLiteral("severity");
static const inline QByteArray s_isRead = QByteArrayLiteral("isRead");
static const inline QByteArray s_isDelivered = QByteArrayLiteral("isDelivered");
static const inline QByteArray s_typeId = QByteArrayLiteral("typeId");
static const inline QByteArray s_createdAt = QByteArrayLiteral("createdAt");
static const inline QByteArray s_productId = QByteArrayLiteral("productId");
static const inline QByteArray s_tenantId = QByteArrayLiteral("tenantId");
static const inline QByteArray s_groupKey = QByteArrayLiteral("groupKey");
}


typedef QVector<imtbase::IComplexCollectionFilter::FieldFilter> FieldFilters;


/**
	Restrict a selection of the collection of the notifications to an inbox; the fields of the filter are joined with AND.
*/
inline void SetInboxSelection(
			const NotificationScope& scope,
			const FieldFilters& fieldFilters,
			iprm::CParamsSet& params)
{
	const QList<QPair<QByteArray, QByteArray>> ids = {
		{s_inboxProductIdParamId, scope.productId},
		{s_inboxTenantIdParamId, scope.tenantId},
		{s_inboxUserIdParamId, scope.userId}};
	for (const QPair<QByteArray, QByteArray>& id : ids){
		iprm::CIdParam* idParamPtr = new iprm::CIdParam;
		idParamPtr->SetId(id.second);
		params.SetEditableParameter(id.first, idParamPtr, true);
	}

	if (!fieldFilters.isEmpty()){
		imtbase::CComplexCollectionFilter* filterPtr = new imtbase::CComplexCollectionFilter;
		imtbase::IComplexCollectionFilter::FilterExpression expression;
		expression.fieldFilters = fieldFilters;
		filterPtr->SetFilterExpression(expression);
		params.SetEditableParameter(QByteArrayLiteral("ComplexFilter"), filterPtr, true);
	}
}


} // namespace imtnotify


