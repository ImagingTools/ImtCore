#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtstyle/IDesignTokenFileParser.h>

namespace imtstyle
{

class CDesignTokenFileParserComp: public ilog::CLoggerComponentBase, public IDesignTokenFileParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenFileParserComp);
		I_REGISTER_INTERFACE(IDesignTokenFileParser);
	I_END_COMPONENT;

	// reimplemented (IDesignTokenFileParser)
	virtual bool ParseFile() override;
	virtual bool SetFile(const QByteArray& filePath) override;
	virtual QByteArray GetTemplateIconColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOffNormalColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOffDisabledColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOffActiveColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOffSelectedColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOnNormalColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOnDisabledColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOnActiveColor(const QByteArray& styleName) const override;
	virtual QByteArray GetOnSelectedColor(const QByteArray& styleName) const override;

	// reimplemented (IColorPaletteProvider)
	virtual const imtbase::ICollectionInfo& GetDesignSchemaList() const override;
	virtual bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const override;

protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	constexpr static const char* s_offNormalColorParamName = "OffNormal";
	constexpr static const char* s_offDisabledColorParamName = "OffDisabled";
	constexpr static const char* s_offActiveColorParamName = "OffActive";
	constexpr static const char* s_offSelectedColorParamName = "OffSelected";
	constexpr static const char* s_onNormalColorParamName = "OnNormal";
	constexpr static const char* s_onDisabledColorParamName = "OnDisabled";
	constexpr static const char* s_onActiveColorParamName = "OnActive";
	constexpr static const char* s_onSelectedColorParamName = "OnSelected";



private:
	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	QByteArray m_templateIconColor;
	QFileInfo m_designTokenFileInfo;
	imtbase::CCollectionInfo m_designSchemaList;

	/**
		m_stylesColors[styleName].toMap()[colorName].toByteArray();
	*/
	QVariantMap m_iconColors;
	QVariantMap m_stylesPalettes;

};


} // namespace imtstyle
