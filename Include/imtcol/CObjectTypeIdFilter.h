// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcol/IObjectTypeIdFilter.h>


namespace imtcol
{


class CObjectTypeIdFilter : virtual public IObjectTypeIdFilter
{
public:
	// reimplemented (imtcol::IObjectTypeIdFilter)
	virtual QByteArray GetObjectTypeId() const override;
	virtual void SetObjectTypeId(const QByteArray& typeId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_typeId;
};


} // namespace imtcol


