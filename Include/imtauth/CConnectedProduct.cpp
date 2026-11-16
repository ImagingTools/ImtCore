// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CConnectedProduct.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <istd/CChangeNotifier.h>
#include <istd/TDelPtr.h>


namespace imtauth
{


// reimplemented (imtauth::IConnectedProduct)

QByteArray CConnectedProduct::GetProductId() const
{
	return m_productId;
}


void CConnectedProduct::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


QString CConnectedProduct::GetProductName() const
{
	return m_name;
}


void CConnectedProduct::SetProductName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QString CConnectedProduct::GetProductDescription() const
{
	return m_description;
}


void CConnectedProduct::SetProductDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


// reimplemented (iser::ISerializable)

bool CConnectedProduct::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	iser::CArchiveTag nameTag("ProductName", "Name of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag descriptionTag("ProductDescription", "Description of the product", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CConnectedProduct::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CConnectedProduct::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CConnectedProduct* sourcePtr = dynamic_cast<const CConnectedProduct*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_productId = sourcePtr->m_productId;
	m_name = sourcePtr->m_name;
	m_description = sourcePtr->m_description;

	return true;
}


bool CConnectedProduct::IsEqual(const istd::IChangeable& object) const
{
	const CConnectedProduct* sourcePtr = dynamic_cast<const CConnectedProduct*>(&object);

	return sourcePtr != nullptr
				&& m_productId == sourcePtr->m_productId
				&& m_name == sourcePtr->m_name
				&& m_description == sourcePtr->m_description;
}


istd::IChangeableUniquePtr CConnectedProduct::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CConnectedProduct> clonePtr(new CConnectedProduct);
	if (clonePtr->CopyFrom(*this, mode)){
		return istd::IChangeableUniquePtr(clonePtr.PopPtr());
	}

	return nullptr;
}


bool CConnectedProduct::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId.clear();
	m_name.clear();
	m_description.clear();

	return true;
}


} // namespace imtauth
