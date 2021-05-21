#pragma once


// Qt includes
#include <QtGui/QPalette>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtstyle/IColorPaletteProvider.h>


namespace imtstyle
{


class CDesignTokenBasedPaletteProviderComp:
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public IColorPaletteProvider
{
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenBasedPaletteProviderComp);
		I_REGISTER_INTERFACE(IColorPaletteProvider);
		I_ASSIGN(m_resourceNameAttrPtr, "PaletteResourceName", "Palette resource name", false, ":/Palette/Palette");
		I_ASSIGN_MULTI_0(m_designShemaIdAttrPtr, "DesignSchemaIds", "Design schema ids", false);
		I_ASSIGN_MULTI_0(m_paletteModeAttrPtr, "PaletteModes", "Palette modes", false);
	I_END_COMPONENT;

	virtual const imtbase::ICollectionInfo& GetDesignSchemaList() const override;
	virtual bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void CreateDefaultPalettes();
	bool StringToColor(const QString& colorString, QColor& color) const;

private:
	I_ATTR(QByteArray, m_resourceNameAttrPtr);
	I_MULTIATTR(QByteArray, m_designShemaIdAttrPtr);
	I_MULTIATTR(QByteArray, m_paletteModeAttrPtr);

	imtbase::CCollectionInfo m_designSchemaList;
	QMap<QByteArray, QPalette> m_paletteMap;
};


} // namespace imtstyle


