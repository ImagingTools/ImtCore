#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/CBinaryDataProviderComp.h>


namespace imtrest
{


class CServerLogDataProviderComp: public imtrest::CBinaryDataProviderComp
{
public:
	typedef imtrest::CBinaryDataProviderComp BaseClass;

	I_BEGIN_COMPONENT(CServerLogDataProviderComp);
		I_ASSIGN(m_commandIdAttrPtr, "Command-ID", "Command ID", true, "");
	I_END_COMPONENT;

	// reimplemented (imtrest::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

protected:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtrest


