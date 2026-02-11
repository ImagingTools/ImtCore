// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtstyle/IDesignSchemaInfoProvider.h>


class QFont;


namespace imtbase
{
	class ICollectionInfo;
}

namespace imtstyle
{


class IFontProvider: virtual public IDesignSchemaInfoProvider
{
public:
	virtual const imtbase::ICollectionInfo& GetFontList(const QByteArray& designSchemaId) const = 0;
	virtual bool GetFont(const QByteArray& designSchemaId, const QByteArray& fontId, QFont& font) const = 0;
};


} // namespace imtstyle


