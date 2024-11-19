#pragma once


// ImtCore includes
#include <imtrest/CBinaryDataProviderComp.h>


namespace imtrest
{


class CIconDataProviderComp: public imtrest::CBinaryDataProviderComp
{
public:
	typedef imtrest::CBinaryDataProviderComp BaseClass;

	I_BEGIN_COMPONENT(CIconDataProviderComp);
	I_END_COMPONENT;

	// reimplemented (IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;
};


} // namespace imtrest


