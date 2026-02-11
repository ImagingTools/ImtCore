// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CBinaryDataProviderComp.h>


namespace imtserverapp
{


class CServerLogDataProviderComp: public imtrest::CBinaryDataProviderComp
{
public:
	typedef imtrest::CBinaryDataProviderComp BaseClass;

	I_BEGIN_COMPONENT(CServerLogDataProviderComp);
		I_ASSIGN(m_commandIdAttrPtr, "Command-ID", "Command ID", true, "");
	I_END_COMPONENT;

	// reimplemented (imtserverapp::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

protected:
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtserverapp


