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
	virtual QByteArray GetMetaInfoId(int metaInfoType) const;
	virtual QString GetMetaInfoName(int metaInfoType) const;
	virtual QString GetMetaInfoDescription(int metaInfoType) const;
};


} // namespace imtbase


