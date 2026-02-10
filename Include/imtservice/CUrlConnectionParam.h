// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservice/CServiceConnectionInfo.h>
#include <imtservice/IServiceConnectionParam.h>


namespace imtservice
{


class CUrlConnectionParam:
			public CServiceConnectionInfo,
			virtual public IServiceConnectionParam
{
public:
	typedef CServiceConnectionInfo BaseClass;

	CUrlConnectionParam();

	void AddExternConnection(const imtservice::IServiceConnectionParam::IncomingConnectionParam& externConnection);

	// reimplemented (imtservice::IServiceConnectionParam)
	virtual QList<IncomingConnectionParam> GetIncomingConnections() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QList<IncomingConnectionParam> m_externConnectionList;
};


} // namespace imtservice


