// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/CUserRecentAction.h>
#include <imtdesk/ITicketAction.h>


namespace imtdesk
{


class CTicketActionComp:
			public icomp::CComponentBase,
			public imtauth::CUserRecentAction,
			virtual public ITicketAction
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtauth::CUserRecentAction BaseClass2;

	I_BEGIN_COMPONENT(CTicketActionComp)
		I_REGISTER_INTERFACE(ITicketAction);
		I_REGISTER_INTERFACE(imtauth::IUserRecentAction);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtdesk::ITicketAction)
	virtual QByteArray GetTicketId() const override;
	virtual void SetTicketId(const QByteArray& ticketId) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_ticketId;
};


} // namespace imtdesk


