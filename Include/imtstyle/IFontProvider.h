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


