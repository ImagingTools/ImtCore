#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QPalette;

namespace imtbase
{
	class ICollectionInfo;
}

namespace imtstyle
{


class IColorPaletteProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::ICollectionInfo& GetDesignSchemaList() const = 0;
	virtual bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const = 0;
};


} // namespace imtstyle


