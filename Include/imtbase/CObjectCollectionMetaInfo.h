// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCoreApplication>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtbase
{


/**
	\ingroup Collection
*/
class CObjectCollectionMetaInfo: public idoc::CStandardDocumentMetaInfo
{
	Q_DECLARE_TR_FUNCTIONS(CObjectCollectionMetaInfo);

public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QByteArray GetMetaInfoId(int metaInfoType) const override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;
	virtual QString GetMetaInfoDescription(int metaInfoType) const override;
};


} // namespace imtbase


