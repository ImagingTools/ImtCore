#pragma once


// Qt includes
#include <QtGui/QPalette>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtstyle
{


class IColorPaletteProvider: virtual public istd::IChangable
{
public:
	const imtbase::ICollectionInfo& GetDesignSchemaList() const = 0;
	virtual QPalette GetColorPalette(const QByteArray& designSchemaId) const = 0
};


} // namespace imtstyle


