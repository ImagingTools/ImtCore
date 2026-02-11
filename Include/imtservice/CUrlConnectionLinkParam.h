// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservice/CServiceConnectionInfo.h>
#include <imtservice/IServiceConnectionLinkParam.h>


namespace imtservice
{


class CUrlConnectionLinkParam:
			public CServiceConnectionInfo,
			public virtual IServiceConnectionLinkParam
{
public:
	typedef CServiceConnectionInfo BaseClass;

	CUrlConnectionLinkParam();

	void SetDependantServiceConnectionId(const QByteArray& connectionId);

	// reimplemented (IServiceConnectionLinkParam)
	virtual QByteArray GetDependantServiceConnectionId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_dependantServiceConnectionId;
};


} // namespace imtservice



