// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CSimObjectCollectionComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>


namespace imttest
{


CSimObjectCollectionComp::CSimObjectCollectionComp()
{
	SetOperationFlags(imtbase::IObjectCollection::OF_ALL);
}


// reimplemented (imtbase::IObjectCollection)

bool CSimObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const imtbase::IOperationContext* operationContextPtr)
{
	++saveCount;
	if (operationContextPtr != nullptr){
		++contextSaveCount;
	}

	return BaseClass2::SetObjectData(objectId, object, mode, operationContextPtr);
}


imtbase::IObjectCollectionIterator* CSimObjectCollectionComp::CreateObjectCollectionIterator(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (failsIterator){
		return nullptr;
	}

	return BaseClass2::CreateObjectCollectionIterator(objectId, offset, count, selectionParamsPtr);
}


// protected methods

QByteArray CSimObjectCollectionComp::GetFieldValue(const QByteArray& objectId, const QByteArray& fieldId) const
{
	if (fieldId == QByteArrayLiteral("DocumentId")){
		return objectId;
	}

	return fieldId == QByteArrayLiteral("TypeId") ? GetObjectTypeId(objectId) : QByteArray();
}


// reimplemented (imtbase::TFilterableCollectionWrap)

bool CSimObjectCollectionComp::IsAcceptedByFilter(const QByteArray& objectId, const iprm::IParamsSet& filterParams) const
{
	iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterPtr(&filterParams, QByteArrayLiteral("ComplexFilter"), false);
	if (!complexFilterPtr.IsValid()){
		return BaseClass2::IsAcceptedByFilter(objectId, filterParams);
	}

	return IsAcceptedByExpression(objectId, complexFilterPtr->GetFilterExpression());
}


// private methods

bool CSimObjectCollectionComp::IsAcceptedByExpression(
			const QByteArray& objectId,
			const imtbase::IComplexCollectionFilter::FilterExpression& expression) const
{
	if (expression.fieldFilters.isEmpty() && expression.filterExpressions.isEmpty()){
		return true;
	}

	const bool isOr = expression.logicalOperation == imtbase::IComplexCollectionFilter::LO_OR;

	for (const auto& fieldFilter : expression.fieldFilters){
		const bool isAccepted = fieldFilter.filterOperation == imtbase::IComplexCollectionFilter::FO_EQUAL
					&& GetFieldValue(objectId, fieldFilter.fieldId) == fieldFilter.filterValue.toByteArray();
		if (isAccepted == isOr){
			return isOr;
		}
	}

	for (const auto& subExpression : expression.filterExpressions){
		if (IsAcceptedByExpression(objectId, subExpression) == isOr){
			return isOr;
		}
	}

	return !isOr;
}


} // namespace imttest

