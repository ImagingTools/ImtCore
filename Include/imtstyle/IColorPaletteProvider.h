#pragma once


// ImtCore includes
#include <imtstyle/IDesignSchemaInfoProvider.h>


class QPalette;


namespace imtstyle
{


class IColorPaletteProvider: virtual public IDesignSchemaInfoProvider
{
public:
	virtual bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const = 0;
};


} // namespace imtstyle


