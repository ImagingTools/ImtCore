// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IConnectedProduct.h>
#include <imtbase/TIdentifiableWrap.h>


namespace imtauth
{


class CConnectedProduct: virtual public IConnectedProduct
{
public:
	// reimplemented (imtauth::IConnectedProduct)
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QString GetProductName() const override;
	virtual void SetProductName(const QString& name) override;
	virtual QString GetProductDescription() const override;
	virtual void SetProductDescription(const QString& description) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_productId;
	QString m_name;
	QString m_description;
};


typedef imtbase::TIdentifiableWrap<CConnectedProduct> CIdentifiableConnectedProduct;


class CConnectedProductComp:
			public icomp::CComponentBase,
			public CIdentifiableConnectedProduct
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CConnectedProductComp);
		I_REGISTER_INTERFACE(IConnectedProduct);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT;
};


} // namespace imtauth
