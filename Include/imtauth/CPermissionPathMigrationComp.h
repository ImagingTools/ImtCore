// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/COperationContext.h>
#include <imtlic/IProductInfo.h>


namespace imtauth
{


/**
	Rewrites the permissions stored by feature id alone ("AddLicense") as full feature paths
	("/LicenseManagement/EditLicense/AddLicense"), using the feature tree of the product.

	Covers the local permissions and prohibitions of the product's roles and of all users and groups.
	Runs once after the start of the application; paths and unknown or ambiguous ids are left as they are,
	so running it again changes nothing. Every rewritten object gets a new revision.
*/
class CPermissionPathMigrationComp: public QObject, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionPathMigrationComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product whose feature tree gives the permission paths", false, "ProductInfo");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Roles collection", false, "RoleCollection");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Users collection", false, "UserCollection");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "Groups collection", false, "UserGroupCollection");
	I_END_COMPONENT;

protected:
	typedef QHash<QByteArray, QByteArray> PathMap;

	void Migrate();

	/**
		Feature id to full path of every permission of the product. An id found in several places is left out.
	*/
	PathMap CreatePathMap(QByteArrayList& ambiguousIds) const;

	/**
		Replace the ids found in \c pathMap by their paths and describe each replacement in \c operationContext.
		\return \c true if anything was replaced.
	*/
	bool ConvertIds(
				QByteArrayList& ids,
				const PathMap& pathMap,
				const QByteArray& addOperationTypeId,
				const QByteArray& removeOperationTypeId,
				const QByteArray& key,
				const QString& keyName,
				imtbase::COperationContext& operationContext) const;

	/**
		Apply \c convertFunction to every active object of the collection and store the converted ones.
	*/
	template<class ObjectInterface, class ConvertFunction>
	void MigrateCollection(imtbase::IObjectCollection& collection, const QString& objectKind, ConvertFunction convertFunction);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauth


