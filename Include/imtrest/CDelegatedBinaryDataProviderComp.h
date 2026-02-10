// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IBinaryDataProvider.h>


namespace imtrest
{


class CDelegatedBinaryDataProviderComp:
			public icomp::CComponentBase,
			virtual public imtrest::IBinaryDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedBinaryDataProviderComp);
		I_REGISTER_INTERFACE(imtrest::IBinaryDataProvider);
		I_ASSIGN_MULTI_0(m_slaveBinaryDataProvidersCompPtr, "SlaveBinaryDataProviders", "Slave binary data provider", false);
	I_END_COMPONENT

	// reimplemented (IBinaryDataProvider)
	virtual bool GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 readFromPosition = 0,
			qint64 readMaxLength = -1) const override;
private:
	I_MULTIREF(imtrest::IBinaryDataProvider, m_slaveBinaryDataProvidersCompPtr);
};


} // namespace imtrest


