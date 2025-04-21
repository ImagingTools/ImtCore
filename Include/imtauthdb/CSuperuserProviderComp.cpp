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

bool CSuperuserProviderComp::SuperuserExists(QString& /*errorMessage*/) const
{
	if (m_userCollectionCompPtr.IsValid()) {
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "Id";
		fieldFilter.filterValue = *m_superuserIdAttrPtr;
		
		imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
		groupFilter.fieldFilters << fieldFilter;
		
		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.SetFieldsFilter(groupFilter);
		
		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		imtbase::ICollectionInfo::Ids userObjectIds = m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!userObjectIds.isEmpty()){
			return true;
		}
	}

	return false;
}


QByteArray CSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthdb


