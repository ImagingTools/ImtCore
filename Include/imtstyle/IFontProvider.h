#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QFont;

namespace imtbase
{
	class ICollectionInfo;
}

namespace imtstyle
{


class IFontProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetFontList() const = 0;
	virtual bool GetFont(const QByteArray& fontId, QFont& font) const = 0;
};


} // namespace imtstyle


