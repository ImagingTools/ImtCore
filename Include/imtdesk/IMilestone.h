// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtdesk
{


/**
	Interface representing a milestone (like GitHub Milestones).

	Milestones group tickets with an optional due date and track
	progress via open/closed issue counts.

	\ingroup imtdesk
*/
class IMilestone: virtual public iser::ISerializable
{
public:
	/**
		Milestone state — open or closed.
	*/
	enum MilestoneState
	{
		MS_OPEN,
		MS_CLOSED
	};
	I_DECLARE_ENUM(MilestoneState,
				MS_OPEN,
				MS_CLOSED);

	/**
		Get the unique milestone identifier.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set the unique milestone identifier.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get the milestone title.
	*/
	virtual QString GetTitle() const = 0;

	/**
		Set the milestone title.
	*/
	virtual void SetTitle(const QString& title) = 0;

	/**
		Get the milestone description.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the milestone description.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get the milestone state (Open / Closed).
	*/
	virtual MilestoneState GetState() const = 0;

	/**
		Set the milestone state.
	*/
	virtual void SetState(MilestoneState state) = 0;

	/**
		Get the ISO 8601 due date (empty if no due date).
	*/
	virtual QString GetDueOn() const = 0;

	/**
		Set the due date.
	*/
	virtual void SetDueOn(const QString& dueOn) = 0;

	/**
		Get the ISO 8601 creation timestamp.
	*/
	virtual QString GetCreatedAt() const = 0;

	/**
		Set the creation timestamp.
	*/
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	/**
		Get the ISO 8601 closed timestamp (empty if not closed).
	*/
	virtual QString GetClosedAt() const = 0;

	/**
		Set the closed timestamp.
	*/
	virtual void SetClosedAt(const QString& closedAt) = 0;
};

typedef istd::TUniqueInterfacePtr<IMilestone> IMilestoneUniquePtr;


} // namespace imtdesk


