// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantFilterParam.h>


namespace imtauth
{


class CTenantFilterParam: virtual public ITenantFilterParam
{
public:
	CTenantFilterParam();

	// reimplemented (imtauth::ITenantFilterParam)
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual QByteArray GetOwnerId() const override;
	virtual void SetOwnerId(const QByteArray& ownerId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_tenantId;
	QByteArray m_ownerId;
};


} // namespace imtauth
