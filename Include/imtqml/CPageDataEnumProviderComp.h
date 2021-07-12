#pragma once


// ImtCore includes
#include <imtqml/CRepresentationDataEnumProviderCompBase.h>


namespace imtqml
{


class CPageDataEnumProviderComp :
		public QObject,
		public CRepresentationDataEnumProviderCompBase
{
	Q_OBJECT
	Q_PROPERTY(QString ID READ Id)
	Q_PROPERTY(QString NAME READ Name)
	Q_PROPERTY(QString ICON READ Icon)
	Q_PROPERTY(QString ICON_ON_SELECTED READ IconOnSelected)
	Q_PROPERTY(QString ICON_OFF_SELECTED READ IconOffSelected)
	Q_PROPERTY(QString COMMANDS READ Commands)
	Q_PROPERTY(QString ENABLED READ Enabled)
public:
	typedef CRepresentationDataEnumProviderCompBase BaseClass;

//	I_BEGIN_COMPONENT(CEnumApplicationDataProviderComp);
//	//	I_REGISTER_INTERFACE(imtrest::IRepresentationDataProvider);
//	//	I_ASSIGN(m_slaveRepresentationDataProviderCompPtr, "ItemBasedRepresentationDataProvider", "Item based representation data provider", true, "ItemBasedRepresentationDataProvider");
//	I_END_COMPONENT;

	CPageDataEnumProviderComp(QObject *parent = nullptr) : QObject(parent), BaseClass() {}

	static constexpr const char* ID = "PageId";
	const QString Id() { return ID; }
	static constexpr const char* NAME = "name";
	const QString Name() { return NAME; }
	static constexpr const char* ICON = "Icon";
	const QString Icon() { return ICON; }
	static constexpr const char* ICON_ON_SELECTED = "IconOnSelected";
	const QString IconOnSelected() { return ICON_ON_SELECTED; }
	static constexpr const char* ICON_OFF_SELECTED = "IconOffSelected";
	const QString IconOffSelected() { return ICON_OFF_SELECTED; }
	static constexpr const char* COMMANDS = "Commands";
	const QString Commands() { return COMMANDS; }
	static constexpr const char* ENABLED = "Enabled";
	const QString Enabled() { return ENABLED; }

protected:
	void SetEnums(imtbase::CTreeItemModel& rootModel) override
	{
		rootModel.SetData(ID, ID);
		rootModel.SetData(NAME, NAME);
		rootModel.SetData(ICON, ICON);
		rootModel.SetData(ICON_ON_SELECTED, ICON_ON_SELECTED);
		rootModel.SetData(ICON_OFF_SELECTED, ICON_OFF_SELECTED);
		rootModel.SetData(COMMANDS, COMMANDS);
		rootModel.SetData(ENABLED, ENABLED);
	}

};


} // namespace imtqml

