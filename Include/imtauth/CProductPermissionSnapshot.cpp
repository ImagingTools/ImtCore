// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CProductPermissionSnapshot.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>


namespace imtauth
{


QByteArray CProductPermissionSnapshot::GetProductId() const
{
	return m_productId;
}


void CProductPermissionSnapshot::SetProductId(const QByteArray& productId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId = productId;
}


QString CProductPermissionSnapshot::GetProductVersion() const
{
	return m_productVersion;
}


void CProductPermissionSnapshot::SetProductVersion(const QString& version)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productVersion = version;
}


QByteArray CProductPermissionSnapshot::GetTreeHash() const
{
	return m_treeHash;
}


void CProductPermissionSnapshot::SetTreeHash(const QByteArray& hash)
{
	istd::CChangeNotifier changeNotifier(this);

	m_treeHash = hash;
}


QByteArray CProductPermissionSnapshot::GetTree() const
{
	return m_tree;
}


void CProductPermissionSnapshot::SetTree(const QByteArray& tree)
{
	istd::CChangeNotifier changeNotifier(this);

	m_tree = tree;
}


QDateTime CProductPermissionSnapshot::GetPublishedAt() const
{
	return m_publishedAt;
}


void CProductPermissionSnapshot::SetPublishedAt(const QDateTime& publishedAt)
{
	istd::CChangeNotifier changeNotifier(this);

	m_publishedAt = publishedAt;
}


// reimplemented (iser::ISerializable)

bool CProductPermissionSnapshot::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	iser::CArchiveTag versionTag("ProductVersion", "Version of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(versionTag);
	retVal = retVal && archive.Process(m_productVersion);
	retVal = retVal && archive.EndTag(versionTag);

	iser::CArchiveTag hashTag("TreeHash", "Hash of the tree", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(hashTag);
	retVal = retVal && archive.Process(m_treeHash);
	retVal = retVal && archive.EndTag(hashTag);

	iser::CArchiveTag treeTag("Tree", "Permission tree in all languages, as JSON", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(treeTag);
	retVal = retVal && archive.Process(m_tree);
	retVal = retVal && archive.EndTag(treeTag);

	iser::CArchiveTag publishedAtTag("PublishedAt", "Time of the publication", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(publishedAtTag);
	iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_publishedAt);
	retVal = retVal && archive.EndTag(publishedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CProductPermissionSnapshot::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CProductPermissionSnapshot::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CProductPermissionSnapshot* sourcePtr = dynamic_cast<const CProductPermissionSnapshot*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_productId = sourcePtr->m_productId;
	m_productVersion = sourcePtr->m_productVersion;
	m_treeHash = sourcePtr->m_treeHash;
	m_tree = sourcePtr->m_tree;
	m_publishedAt = sourcePtr->m_publishedAt;

	return true;
}


bool CProductPermissionSnapshot::IsEqual(const istd::IChangeable& object) const
{
	const CProductPermissionSnapshot* sourcePtr = dynamic_cast<const CProductPermissionSnapshot*>(&object);

	return sourcePtr != nullptr
				&& m_productId == sourcePtr->m_productId
				&& m_productVersion == sourcePtr->m_productVersion
				&& m_treeHash == sourcePtr->m_treeHash
				&& m_tree == sourcePtr->m_tree
				&& m_publishedAt == sourcePtr->m_publishedAt;
}


istd::IChangeableUniquePtr CProductPermissionSnapshot::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CProductPermissionSnapshot> clonePtr(new CProductPermissionSnapshot);
	if (clonePtr->CopyFrom(*this, mode)){
		return istd::IChangeableUniquePtr(clonePtr.PopPtr());
	}

	return nullptr;
}


bool CProductPermissionSnapshot::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId.clear();
	m_productVersion.clear();
	m_treeHash.clear();
	m_tree.clear();
	m_publishedAt = QDateTime();

	return true;
}


QByteArray FindProductPermissionSnapshot(const imtbase::IObjectCollection& snapshotCollection, const QByteArray& productId)
{
	imtbase::CComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("ProductId", QString::fromUtf8(productId));

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	const imtbase::ICollectionInfo::Ids ids = snapshotCollection.GetElementIds(0, 1, &filterParams);

	return ids.isEmpty() ? QByteArray() : ids.first();
}


} // namespace imtauth
