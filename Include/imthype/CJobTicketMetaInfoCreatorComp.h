// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>


namespace imthype
{


class CJobTicketMetaInfoCreatorComp: public imtbase::CObjectMetaInfoCreatorCompBase
{
public:
	typedef imtbase::CObjectMetaInfoCreatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CJobTicketMetaInfoCreatorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const override;

private:
	class MetaInfo: public idoc::CStandardDocumentMetaInfo
	{
	public:
		typedef idoc::CStandardDocumentMetaInfo BaseClass;

		// reimplemented (idoc::IDocumentMetaInfo)
		virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
		virtual QByteArray GetMetaInfoId(int metaInfoType) const override;
		virtual QString GetMetaInfoName(int metaInfoType) const override;
	};
};


} // namespace imthype


