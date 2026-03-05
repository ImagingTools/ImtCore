// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcol/ICollectionImportParam.h>

// Qt includes
#include <QtCore/QVector>


namespace imtcol
{


class CCollectionImportParam: virtual public ICollectionImportParam
{
public:
	// reimplemented (imtcol::ICollectionImportParam)
	virtual QByteArray GetCollectionId() const override;
	virtual bool SetCollectionId(const QByteArray& collectionId) override;

	virtual int GetFileCount() const override;
	virtual FileImportInfo* GetFileImportInfo(int index) const override;
	virtual bool InsertFileImportInfo(const FileImportInfo& info, int index = -1) override;
	virtual bool RemoveFileImportInfo(int index) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_collectionId;
	mutable QVector<FileImportInfo> m_fileImportInfos;
};


} // namespace imtcol


