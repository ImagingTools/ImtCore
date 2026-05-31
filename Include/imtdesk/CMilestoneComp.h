// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtdesk/IMilestone.h>


namespace imtdesk
{


class CMilestoneComp:
			public icomp::CComponentBase,
			virtual public IMilestone
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMilestoneComp)
		I_REGISTER_INTERFACE(IMilestone);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtdesk::IMilestone)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetTitle() const override;
	virtual void SetTitle(const QString& title) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual MilestoneState GetState() const override;
	virtual void SetState(MilestoneState state) override;
	virtual QString GetDueOn() const override;
	virtual void SetDueOn(const QString& dueOn) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetClosedAt() const override;
	virtual void SetClosedAt(const QString& closedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QString m_title;
	QString m_description;
	MilestoneState m_state = MS_OPEN;
	QString m_dueOn;
	QString m_createdAt;
	QString m_closedAt;
};


} // namespace imtdesk


