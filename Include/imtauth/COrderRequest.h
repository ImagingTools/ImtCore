// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IOrderRequestData.h>


namespace imtauth
{


class COrderRequest: virtual public IOrderRequestData, virtual public iser::ISerializable
{
public:
	COrderRequest();

	virtual OrderRequestInfo GetOrderRequestInfo() const override;
	virtual void SetOrderRequestInfo(const OrderRequestInfo& orderRequestInfo) override;
	virtual QByteArray GetOrderRequestId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	OrderRequestInfo m_info;
};


typedef imtbase::TIdentifiableWrap<COrderRequest> CIdentifiableOrderRequest;


} // namespace imtauth
