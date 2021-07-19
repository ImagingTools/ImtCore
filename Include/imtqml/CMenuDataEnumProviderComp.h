#pragma once


#include <imtqml/CRepresentationDataEnumProviderCompBase.h>



namespace imtqml
{


class CMenuDataEnumProviderComp :
		public QObject,
		public CRepresentationDataEnumProviderCompBase
{
	Q_OBJECT
	Q_PROPERTY(QString TITLE READ Title)
	Q_PROPERTY(QString TEXT READ Text)
	Q_PROPERTY(QString ICON READ Icon)
	Q_PROPERTY(QString URL READ Url)
public:
	typedef CRepresentationDataEnumProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CMenuDataEnumProviderComp);
	I_END_COMPONENT;

	CMenuDataEnumProviderComp(QObject *parent = nullptr) : QObject(parent), BaseClass() {}

	static constexpr const char* TITLE = "Title";
	const QString Title() { return TITLE; }

	static constexpr const char* TEXT = "Text";
	const QString Text() { return TEXT; }

	static constexpr const char* ICON = "Icon";
	const QString Icon() { return ICON; }

	static constexpr const char* URL = "Url";
	const QString Url() { return URL; }

protected:
	void virtual GetEnums(imtrest::QueryParams& enums) const override
	{
		enums.insert("TITLE", TITLE);
		enums.insert("TEXT", TEXT);
		enums.insert("ICON", ICON);
		enums.insert("URL", URL);
	}

};


} // namespace imtqml

