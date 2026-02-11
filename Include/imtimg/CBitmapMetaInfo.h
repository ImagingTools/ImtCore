// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtimg/IBitmapMetaInfo.h>


namespace imtimg
{


/**
	Meta-info creator for bitmap object.
*/
class CBitmapMetaInfo: public idoc::CStandardDocumentMetaInfo, virtual public imtimg::IBitmapMetaInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QString GetMetaInfoName(int metaInfoType) const override;
};


} // namespace imtimg


