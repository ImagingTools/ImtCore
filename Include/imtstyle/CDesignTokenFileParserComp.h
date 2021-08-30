#pragma once


// Qt includes
#include <QtCore/QtCore>
#include <QtGui/QPalette>

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
	bool ParseFile() override;
	bool SplitFile(const QString& outputDirPath, const QString& projectName) override;
	bool SetFile(const QByteArray& filePath) override;
	bool GetColorRoleGroup(const QString& name, QPalette::ColorGroup& group, QPalette::ColorRole& role) const override;
	QString GetColorName(QPalette::ColorGroup group, QPalette::ColorRole role) const override;

	QByteArray GetTemplateIconColor(const QByteArray& styleName) const override;
    QByteArray GetNormalColor(const QByteArray& styleName) const override;
	QByteArray GetOffNormalColor(const QByteArray& styleName) const override;
	QByteArray GetOffDisabledColor(const QByteArray& styleName) const override;
	QByteArray GetOffActiveColor(const QByteArray& styleName) const override;
	QByteArray GetOffSelectedColor(const QByteArray& styleName) const override;
	QByteArray GetOnNormalColor(const QByteArray& styleName) const override;
	QByteArray GetOnDisabledColor(const QByteArray& styleName) const override;
	QByteArray GetOnActiveColor(const QByteArray& styleName) const override;
	QByteArray GetOnSelectedColor(const QByteArray& styleName) const override;

	// reimplemented (IColorPaletteProvider)
	const imtbase::ICollectionInfo& GetDesignSchemaList() const override;
	bool GetColorPalette(const QByteArray& designSchemaId, QPalette& palette) const override;

public:
	static const QMap<QString, QPalette::ColorGroup> s_colorGroupNamesMap;
	static const QMap<QString, QPalette::ColorRole> s_colorRolesNamesMap;

protected:
	// reimplemented (ilog::CLoggerComponentBase)
	void OnComponentCreated() override;

private:
	constexpr static const char* s_offNormalColorParamName = "OffNormal";
	constexpr static const char* s_offDisabledColorParamName = "OffDisabled";
	constexpr static const char* s_offActiveColorParamName = "OffActive";
	constexpr static const char* s_offSelectedColorParamName = "OffSelected";
	constexpr static const char* s_onNormalColorParamName = "OnNormal";
	constexpr static const char* s_onDisabledColorParamName = "OnDisabled";
	constexpr static const char* s_onActiveColorParamName = "OnActive";
	constexpr static const char* s_onSelectedColorParamName = "OnSelected";

	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	imtbase::CCollectionInfo m_designSchemaList;
	QByteArray m_templateIconColor;
	QFileInfo m_designTokenFileInfo;

	QVariantMap m_iconColors;
	QMap<QString, QPalette> m_stylesPalettes;

private:
	void GetPaletteFromEntry(const QString& styleName, const QJsonValue& paletteEntry);
	bool CreateColorFromGrb(const QString& rgbString, QColor& color) const;
};


} // namespace imtstyle


