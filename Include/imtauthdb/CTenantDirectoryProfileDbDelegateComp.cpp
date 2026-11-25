// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantDirectoryProfileDbDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/ITenantDirectory.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CTenantDirectoryProfileDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtauth::ITenantDirectoryProfile> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CTenantDirectoryProfileDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtauth::ITenantDirectoryProfile* profilePtr = dynamic_cast<const imtauth::ITenantDirectoryProfile*>(&object);
	if (profilePtr == nullptr){
		return false;
	}

	// The search compares lower case texts, the same way in every database
	const QString searchText = QStringList({profilePtr->GetTenantName(), profilePtr->GetDescription(), profilePtr->GetIndustry()}).join(' ').toLower();
	// An inactive organization is not found, its own choice is kept for its reactivation
	const bool isFound = profilePtr->IsListed() && profilePtr->IsTenantActive();
	columns.append({QStringLiteral("TenantId"), IdLiteral(profilePtr->GetTenantId())});
	columns.append({QStringLiteral("IsListed"), NumberLiteral(isFound ? 1 : 0)});
	columns.append({QStringLiteral("SearchText"), TextLiteral(searchText)});
	columns.append({QStringLiteral("UpdatedAt"), TimeLiteral(profilePtr->GetUpdatedAt())});

	return true;
}


QString CTenantDirectoryProfileDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	iprm::TParamsPtr<iprm::ITextParam> textParamPtr(&filterParams, imtauth::s_directorySearchTextParamId, false);
	if (!textParamPtr.IsValid()){
		return BaseClass::CreateAdditionalFiltersQuery(filterParams);
	}

	// The directory is searched by a text only, it is never listed whole
	const QString text = textParamPtr->GetText().trimmed().toLower();
	if (text.isEmpty()){
		return QStringLiteral("1 = 0");
	}

	QStringList conditions = {QStringLiteral(R"("IsListed" = 1)"), ContainsCondition(QStringLiteral("SearchText"), text)};

	iprm::TParamsPtr<iprm::ITextParam> excludedParamPtr(&filterParams, imtauth::s_directoryExcludedTenantIdParamId, false);
	if (excludedParamPtr.IsValid() && !excludedParamPtr->GetText().isEmpty()){
		conditions << QStringLiteral(R"("TenantId" <> %1)").arg(IdLiteral(excludedParamPtr->GetText().toUtf8()));
	}

	return conditions.join(QStringLiteral(" AND "));
}


} // namespace imtauthdb


