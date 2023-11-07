#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>


namespace imtbase
{


class CDelegatedBinaryDataProviderComp:
			public icomp::CComponentBase,
			virtual public IBinaryDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedBinaryDataProviderComp);
		I_REGISTER_INTERFACE(IBinaryDataProvider);
		I_ASSIGN_MULTI_0(m_slaveBinaryDataProvidersCompPtr, "SlaveBinaryDataProviders", "Slave binary data provider", false);
	I_END_COMPONENT

	// reimplemented (IBinaryDataProvider)
	virtual bool GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 readFromPosition = 0,
			qint64 readMaxLength = -1) const override;
private:
	I_MULTIREF(IBinaryDataProvider, m_slaveBinaryDataProvidersCompPtr);
};


} // namespace imtbase


