// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IDocumentShareData.h>


namespace imtauth
{


class CDocumentShare: virtual public IDocumentShareData, virtual public iser::ISerializable
{
public:
	CDocumentShare();

	virtual DocumentShareInfo GetShareInfo() const override;
	virtual void SetShareInfo(const DocumentShareInfo& shareInfo) override;
	virtual QByteArray GetShareId() const override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	DocumentShareInfo m_info;
};


typedef imtbase::TIdentifiableWrap<CDocumentShare> CIdentifiableDocumentShare;


} // namespace imtauth
