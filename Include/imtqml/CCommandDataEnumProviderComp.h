#pragma once


// ImtCore includes
#include <imtqml/CRepresentationDataEnumProviderCompBase.h>


namespace imtqml
{


class CCommandDataEnumProviderComp :
		public QObject,
		public CRepresentationDataEnumProviderCompBase
{
	Q_OBJECT
	Q_PROPERTY(QString ID READ Id)
	Q_PROPERTY(QString NAME READ Name)
	Q_PROPERTY(QString ICON READ Icon)
	Q_PROPERTY(QString ENABLED READ Enabled)
public:
	typedef CRepresentationDataEnumProviderCompBase BaseClass;

	CCommandDataEnumProviderComp(QObject *parent = nullptr) : QObject(parent), BaseClass() {}

	static constexpr const char* ID = "Id";
	const QString Id() { return ID; }
	static constexpr const char* NAME = "Name";
	const QString Name() { return NAME; }
	static constexpr const char* ICON = "Icon";
	const QString Icon() { return ICON; }
	static constexpr const char* ENABLED = "Enabled";
	const QString Enabled() { return ENABLED; }

protected:
	void SetEnums(imtbase::CTreeItemModel& rootModel) override
	{
		rootModel.SetData(NAME, NAME);
		rootModel.SetData(ICON, ICON);
		rootModel.SetData(ENABLED, ENABLED);
		rootModel.SetData(ID, ID);
	}

};


} // namespace imtqml

