// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtauth/IUserInfo.h>


namespace imtservergql
{


/**
	The user, its roles and its groups read from the sources of their collections, for a check that must see the current state.
	A role or a group is read when it is asked for, once; a read that failed makes the snapshot incomplete, and nothing resolved
	through it may be used then. A cached collection is read at its source, through every caching layer.
*/
class CAuthorizationSnapshot: virtual public imtauth::IRoleInfoProvider, virtual public imtauth::IUserGroupInfoProvider
{
public:
	enum ReadStatus
	{
		/// The object was read.
		RS_READ,
		/// The store answered that the object is not there.
		RS_ABSENT,
		/// A read failed or cannot be made: nothing is known.
		RS_FAILED,
		/// The source explicitly refused access to the object.
		RS_DENIED
	};

	CAuthorizationSnapshot(
				const imtbase::IObjectCollection* userCollectionPtr,
				const imtbase::IObjectCollection* roleCollectionPtr,
				const imtbase::IObjectCollection* groupCollectionPtr);

	/**
		Read the user as a copy of its own, its roles and groups provided by this snapshot.
		The copy must be detached (DetachUser) before the snapshot is gone.
	*/
	ReadStatus ReadUser(const QByteArray& userId, istd::IChangeableUniquePtr& userPtr, QString& errorMessage) const;

	/**
		Resolve the permissions of the user, read by ReadUser, in every product it or its groups hold and in productId,
		keep them as its own and detach it. Fails when a role or a group could not be read.
	*/
	bool KeepPermissions(imtauth::IUserInfo& userInfo, const QByteArray& productId, QString& errorMessage) const;

	/// False when a role or a group asked for could not be read: what was resolved through the snapshot is incomplete.
	bool IsComplete(QString& errorMessage) const;

	/// Let the user point to no provider: its permissions are only those it holds.
	static void DetachUser(istd::IChangeable& user);

	/// Read one object as a copy of its own, distinguishing absence, access refusal and failed reads.
	static ReadStatus ReadObject(const imtbase::IObjectCollection& collection, const QByteArray& objectId, istd::IChangeableUniquePtr& objectPtr);

	/// The collection that holds the data: a cached collection is passed through.
	static const imtbase::IObjectCollection* GetSourceCollection(const imtbase::IObjectCollection* collectionPtr);

	// reimplemented (imtauth::IRoleInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRoleList() const override;
	virtual imtauth::IRoleUniquePtr GetRole(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (imtauth::IUserGroupInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserGroupList() const override;
	virtual imtauth::IUserGroupInfoSharedPtr GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* paramsPtr = nullptr) const override;

private:
	enum ObjectKind
	{
		OK_ROLE,
		OK_GROUP
	};

	/// The object read once; an invalid pointer is an absent object or a failed read.
	imtbase::IObjectCollection::DataPtr ReadOnce(ObjectKind kind, const QByteArray& objectId) const;

	const imtbase::IObjectCollection* m_userCollectionPtr;
	const imtbase::IObjectCollection* m_roleCollectionPtr;
	const imtbase::IObjectCollection* m_groupCollectionPtr;
	mutable QHash<QByteArray, imtbase::IObjectCollection::DataPtr> m_roles;
	mutable QHash<QByteArray, imtbase::IObjectCollection::DataPtr> m_groups;
	mutable QString m_errorMessage;
	imtbase::CCollectionInfo m_emptyList;
};


} // namespace imtservergql
