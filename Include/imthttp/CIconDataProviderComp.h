#pragma once


// ImtCore includes
#include <imthttp/CBinaryDataProviderComp.h>


namespace imthttp
{


class CIconDataProviderComp: public imthttp::CBinaryDataProviderComp
{
public:
	typedef imthttp::CBinaryDataProviderComp BaseClass;

	I_BEGIN_COMPONENT(CIconDataProviderComp);
	I_END_COMPONENT;

	// reimplemented (IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;
};


} // namespace imthttp


