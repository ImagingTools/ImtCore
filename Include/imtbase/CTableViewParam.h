// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ITableViewParam.h>


namespace imtbase
{


class CTableViewParam: virtual public ITableViewParam
{
public:
	// reimplemented (imtbase::ITableViewParam)
	virtual QByteArrayList GetHeaderIds() const override;
	virtual HeaderInfo GetHeaderInfo(const QByteArray& headerId) const override;
	virtual bool SetHeaderInfo(QByteArray headerId, ITableViewParam::HeaderInfo headerInfo) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QMap<QByteArray, ITableViewParam::HeaderInfo> m_headerInfoMap;
};


} // namespace imtbase


