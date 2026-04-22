// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IBinaryDataController.h>


namespace imtrest
{


class CDelegatedBinaryDataControllerComp:
			public icomp::CComponentBase,
			virtual public imtrest::IBinaryDataController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedBinaryDataControllerComp);
		I_REGISTER_INTERFACE(imtrest::IBinaryDataController);
		I_ASSIGN_MULTI_0(m_slaveBinaryDataControllersCompPtr, "SlaveBinaryDataControllers", "Slave binary data controller", false);
	I_END_COMPONENT

	// reimplemented (IBinaryDataController)
	virtual bool SetData(const QByteArray& data, QByteArray& dataId) const override;
	virtual bool RemoveData(const QByteArray& dataId) const override;
	virtual bool EnsureDataExists(const QByteArray& dataId) const override;
private:
	I_MULTIREF(imtrest::IBinaryDataController, m_slaveBinaryDataControllersCompPtr);
};


} // namespace imtrest


