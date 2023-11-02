#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>
#include <imtrest/imtrest.h>


namespace imtqml
{


class CRepresentationDataEnumProviderCompBase:
			public icomp::CComponentBase,
			virtual public imtbase::IBinaryDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationDataEnumProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataProvider);
		I_ASSIGN(m_dataIdAttrPtr, "DataId", "Data Id", true, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

protected:
	// abstract methods
	virtual void GetEnums(imtrest::QueryParams& enums) const = 0;

protected:
	I_ATTR(QByteArray, m_dataIdAttrPtr);
};


} // namespace imtqml


