// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtlic/ILicenseInstance.h>
#include <imtlic/CLicenseDefinition.h>


namespace imtlic
{


/**
	Common implementation of the information about a single license.
	\sa ILicenseDefinition
	\ingroup LicenseManagement
*/
class CLicenseInstance: virtual public imtlic::ILicenseInstance, public CLicenseDefinition
{
public:
	typedef CLicenseDefinition BaseClass;

	QByteArray GetTypeId() const;

	// reimplemented (imtlic::ILicenseInstance)
	virtual QDateTime GetExpiration() const override;
	virtual void SetExpiration(const QDateTime& expirationTime) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QDateTime m_expirationTime;
};


} // namespace imtlic


