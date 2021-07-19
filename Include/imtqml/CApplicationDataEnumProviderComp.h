#pragma once


#include <imtqml/CRepresentationDataEnumProviderCompBase.h>



namespace imtqml
{


class CApplicationDataEnumProviderComp :
		public QObject,
		public CRepresentationDataEnumProviderCompBase
{
	Q_OBJECT
	Q_PROPERTY(QString APPLICATION READ Application)
	Q_PROPERTY(QString PAGES READ Pages)
	Q_PROPERTY(QString DATA READ Data)
public:
	typedef CRepresentationDataEnumProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationDataEnumProviderComp);
	I_END_COMPONENT;

	CApplicationDataEnumProviderComp(QObject *parent = nullptr) : QObject(parent), BaseClass() {}

	static constexpr const char* APPLICATION = "Application";
	const QString Application() { return APPLICATION; }

	static constexpr const char* PAGES = "Pages";
	const QString Pages() { return PAGES; }

	static constexpr const char* DATA = "Data";
	const QString Data() { return DATA; }

protected:
	void virtual GetEnums(imtrest::QueryParams& enums) const override
	{
		enums.insert("APPLICATION", APPLICATION);
		enums.insert("PAGES", PAGES);
		enums.insert("DATA", DATA);
	}

};


} // namespace imtqml

