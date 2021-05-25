#pragma once


// Qt includes
#include <QtGui/QFont>
#include <QtGui/QPalette>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtbase/CJsonHelper.h>
#include <imtstyle/IColorPaletteProvider.h>
#include <imtstyle/IFontProvider.h>


namespace imtstyle
{


class CDesignTokenBasedResourceProviderComp:
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			private imtbase::CJsonHelper,
			virtual public IColorPaletteProvider,
			virtual public IFontProvider
{
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenBasedResourceProviderComp);
		I_REGISTER_INTERFACE(IDesignSchemaInfoProvider);
		I_REGISTER_INTERFACE(IColorPaletteProvider);
		I_REGISTER_INTERFACE(IFontProvider);
		I_ASSIGN(m_resourceFileNameAttrPtr, "ResourceFileName", "Resource file name", false, "");
		I_ASSIGN_MULTI_0(m_designShemaIdAttrPtr, "DesignSchemaIds", "Design schema ids", false);
		I_ASSIGN_MULTI_0(m_paletteModeAttrPtr, "PaletteModes", "Palette modes", false);
	I_END_COMPONENT;

	// reimplemented (IColorPaletteProvider)
	virtual bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const override;

	// reimplemented (IFontProvider)
	virtual const imtbase::ICollectionInfo& GetFontList() const override;
	virtual bool GetFont(const QByteArray& fontId, QFont& font) const override;

	// reimplemented (IDesignSchemaInfoProvider)
	virtual const imtbase::ICollectionInfo& GetDesignSchemaList() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	int GetCount() const;
	void CreateDefaultPalettes();
	bool StringToColor(const QString& colorString, QColor& color) const;

private:
	I_ATTR(QByteArray, m_resourceFileNameAttrPtr);
	I_MULTIATTR(QByteArray, m_designShemaIdAttrPtr);
	I_MULTIATTR(QByteArray, m_paletteModeAttrPtr);

	imtbase::CCollectionInfo m_designSchemaList;
	imtbase::CCollectionInfo m_fontList;

	QMap<QByteArray, QPalette> m_paletteMap;
	QMap<QByteArray, QFont> m_fontMap;
};


} // namespace imtstyle


