// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/IIdentifiable.h>


namespace imtbase
{


class CIdentifiable: virtual public IIdentifiable, virtual public iser::ISerializable
{
public:
	// reimplemented (IIdentifiable)
	virtual QByteArray GetObjectUuid() const override;
	virtual bool SetObjectUuid(const QByteArray& identifier) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual bool IsIdentifierAccepted(const QByteArray& identifier) const;
	virtual bool IsMutable() const;

private:
	QByteArray m_identifier;
};


} // namespace imtbase


