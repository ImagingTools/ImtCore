// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ICrossTenantMessageData.h>


namespace imtauth
{


class CCrossTenantMessage: virtual public ICrossTenantMessageData, virtual public iser::ISerializable
{
public:
	CCrossTenantMessage();

	virtual CrossTenantMessageInfo GetMessageInfo() const override;
	virtual void SetMessageInfo(const CrossTenantMessageInfo& messageInfo) override;
	virtual QByteArray GetMessageId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	CrossTenantMessageInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CCrossTenantMessage> CIdentifiableCrossTenantMessage;


} // namespace imtauth
