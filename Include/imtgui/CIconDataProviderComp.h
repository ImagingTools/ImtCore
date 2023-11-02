#pragma once


// ImtCore includes
#include <imtbase/CBinaryDataProviderComp.h>


namespace imtgui
{


class CIconDataProviderComp: public imtbase::CBinaryDataProviderComp
{
public:
	typedef imtbase::CBinaryDataProviderComp BaseClass;

	I_BEGIN_COMPONENT(CIconDataProviderComp);
	I_END_COMPONENT;

	// reimplemented (IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;
};


} // namespace imtgui


