// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTagNameLookup.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IObjectCollection.h>
#include <imttag/ITag.h>


namespace imttag
{


// public static methods

QString CTagNameLookup::NormalizeName(const QString& name)
{
	return name.simplified();
}


QByteArrayList CTagNameLookup::FindTagIds(
			const imtbase::IObjectCollection& tagCollection,
			const QString& name,
			const QByteArray* tenantIdPtr,
			const QByteArray& excludedTagId)
{
	QByteArrayList retVal;

	const QString normalizedName = NormalizeName(name);
	if (normalizedName.isEmpty()){
		return retVal;
	}

	imtbase::IComplexCollectionFilter::FieldMetaInfo nameMetaInfo;
	nameMetaInfo.flags = imtbase::IComplexCollectionFilter::SO_TEXT_FILTER;

	imtbase::IComplexCollectionFilter::Fields fields;
	fields << imtbase::IComplexCollectionFilter::FieldInfo("Name", nameMetaInfo);

	// LIKE narrows the candidates, the exact comparison happens below.
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFields(fields);
	complexFilter.SetTextFilter(normalizedName);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	if (tenantIdPtr != nullptr){
		imtauth::CTenantFilterParam* tenantFilterPtr = new imtauth::CTenantFilterParam;
		tenantFilterPtr->SetTenantId(*tenantIdPtr);
		filterParams.SetEditableParameter("TenantFilter", tenantFilterPtr, true);
	}

	const imtbase::ICollectionInfo::Ids candidateIds = tagCollection.GetElementIds(0, -1, &filterParams);
	for (const QByteArray& candidateId : candidateIds){
		if (candidateId == excludedTagId){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!tagCollection.GetObjectData(candidateId, dataPtr)){
			continue;
		}

		const ITag* tagPtr = dynamic_cast<const ITag*>(dataPtr.GetPtr());
		if ((tagPtr != nullptr) && (NormalizeName(tagPtr->GetName()).compare(normalizedName, Qt::CaseInsensitive) == 0)){
			retVal << candidateId;
		}
	}

	return retVal;
}


} // namespace imttag


