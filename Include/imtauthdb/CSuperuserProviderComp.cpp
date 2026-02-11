// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CSuperuserProviderComp.h>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtauth::ISuperuserProvider)

imtauth::ISuperuserProvider::ExistsStatus CSuperuserProviderComp::SuperuserExists(QString& /*errorMessage*/) const
{
	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "Id";
		fieldFilter.filterValue = *m_superuserIdAttrPtr;
		
		imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
		groupFilter.fieldFilters << fieldFilter;
		
		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.SetFilterExpression(groupFilter);
		
		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		imtbase::ICollectionInfo::Ids userObjectIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!userObjectIds.isEmpty()){
			return ES_EXISTS;
		}

		return ES_NOT_EXISTS;
	}

	return ES_UNKNOWN;
}


QByteArray CSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthdb


