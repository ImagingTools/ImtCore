// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/TIdentifiableWrap.h>


namespace imtauth
{


/**
	The permission tree a product published, in all its languages. It is a reference for showing the tree and for
	checking what is granted: it gives no permission itself, roles keep their paths whatever the tree says.
	Kept apart from the catalog entry of the product, whose saving must not overwrite a fresh publication.
*/
class CProductPermissionSnapshot: virtual public iser::ISerializable
{
public:
	QByteArray GetProductId() const;
	void SetProductId(const QByteArray& productId);
	QString GetProductVersion() const;
	void SetProductVersion(const QString& version);
	QByteArray GetTreeHash() const;
	void SetTreeHash(const QByteArray& hash);
	QByteArray GetTree() const;
	void SetTree(const QByteArray& tree);
	QDateTime GetPublishedAt() const;
	void SetPublishedAt(const QDateTime& publishedAt);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_productId;
	QString m_productVersion;
	QByteArray m_treeHash;
	QByteArray m_tree;
	QDateTime m_publishedAt;
};


typedef imtbase::TIdentifiableWrap<CProductPermissionSnapshot> CIdentifiableProductPermissionSnapshot;


/**
	Get the snapshot of a product, null when the product has not published yet.
	\param dataPtr Keeps the returned snapshot alive.
	\param isReadPtr [optional] False when the collection or the found snapshot could not be read, so null says nothing about the product.
	\param idPtr [optional] ID of the found snapshot, empty when the product has not published yet.
*/
const CProductPermissionSnapshot* GetProductPermissionSnapshot(
			const imtbase::IObjectCollection& snapshotCollection,
			const QByteArray& productId,
			imtbase::IObjectCollection::DataPtr& dataPtr,
			bool* isReadPtr = nullptr,
			QByteArray* idPtr = nullptr);


} // namespace imtauth
