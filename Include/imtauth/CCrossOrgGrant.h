// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ICrossOrgGrantData.h>


namespace imtauth
{


class CCrossOrgGrant: virtual public ICrossOrgGrantData, virtual public iser::ISerializable
{
public:
	CCrossOrgGrant();

	virtual CrossOrgGrantInfo GetGrantInfo() const override;
	virtual void SetGrantInfo(const CrossOrgGrantInfo& grantInfo) override;
	virtual QByteArray GetGrantId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	CrossOrgGrantInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CCrossOrgGrant> CIdentifiableCrossOrgGrant;


} // namespace imtauth
