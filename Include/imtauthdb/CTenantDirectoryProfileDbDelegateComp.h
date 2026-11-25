// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantDirectoryProfile.h>
#include <imtauthdb/CSerializedObjectDbDelegateCompBase.h>


namespace imtauthdb
{


/**
	SQL delegate of the profiles of the organizations in the directory.
	The parameters of imtauth::ITenantDirectory (s_directorySearchTextParamId, s_directoryExcludedTenantIdParamId) select the found profiles:
	listed by an active organization and matching the text.
*/
class CTenantDirectoryProfileDbDelegateComp: public CSerializedObjectDbDelegateCompBase
{
public:
	typedef CSerializedObjectDbDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantDirectoryProfileDbDelegateComp)
		I_ASSIGN(m_dataFactoryCompPtr, "ProfileFactory", "Factory of the stored objects", true, "TenantDirectoryProfile");
	I_END_COMPONENT

protected:
	// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)
	virtual istd::IChangeableUniquePtr CreateDataObject() const override;
	virtual bool GetColumns(const istd::IChangeable& object, Columns& columns) const override;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

private:
	I_FACT(imtauth::ITenantDirectoryProfile, m_dataFactoryCompPtr);
};


} // namespace imtauthdb


