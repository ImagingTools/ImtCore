// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/ISuperuserProvider.h>


namespace imtauthdb
{


class CSuperuserProviderComp: public icomp::CComponentBase, virtual public imtauth::ISuperuserProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSuperuserProviderComp);
		I_REGISTER_INTERFACE(imtauth::ISuperuserProvider);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_superuserIdAttrPtr, "SuperuserId", "ID of the superuser", true, "su");
	I_END_COMPONENT;

	// reimplemented (imtauth::ISuperuserProvider)
	virtual ExistsStatus SuperuserExists(QString& errorMessage) const override;
	virtual QByteArray GetSuperuserId() const override;

protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_ATTR(QByteArray, m_superuserIdAttrPtr);
};


} // namespace imtauthdb


