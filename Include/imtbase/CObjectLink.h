// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ICollectionObjectLink.h>


namespace imtbase
{


class CObjectLink: virtual public ICollectionObjectLink
{
public:
	virtual void SetFactoryId(const QByteArray& objectType);
	virtual void SetCollectionPtr(const imtbase::IObjectCollection* collectionPtr);

	// reimplemented (ICollectionObjectLink)
	virtual const imtbase::IObjectCollection* GetCollection() const override;

	// reimplemented (IIdentifiable)
	virtual QByteArray GetObjectUuid() const override;
	virtual bool SetObjectUuid(const QByteArray& objectUuid) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_objectUuid;
	QByteArray m_objectTypeId;
	const imtbase::IObjectCollection* m_collectionPtr;
};


} // namespace imtbase


