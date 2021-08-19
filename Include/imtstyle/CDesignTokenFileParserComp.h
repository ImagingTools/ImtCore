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
	static const QMap<QString, QPalette::ColorGroup> s_colorGroupNamesMap;
	static const QMap<QString, QPalette::ColorRole> s_colorRolesNamesMap;

	static QString s_GetColorName(QPalette::ColorGroup group, QPalette::ColorRole role);
	static bool s_GetColorRoleGroup(const QString& name, QPalette::ColorGroup& group,  QPalette::ColorRole& role);

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

	I_REF(ifile::IFileNameParam, m_designTokenFilePathAttrPtr);

	imtbase::CCollectionInfo m_designSchemaList;
	QByteArray m_templateIconColor;
	QFileInfo m_designTokenFileInfo;

	QVariantMap m_iconColors;
	QMap<QString, QPalette> m_stylesPalettes;

private:
	void GetPaletteFromEntry(const QString& styleName, const QJsonValue& paletteEntry);
};


} // namespace imtstyle


