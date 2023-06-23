#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <ilog/TLoggerCompWrap.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <imod/IModel.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


class CCommandsObserverQmlComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public ibase::ICommandsProvider,
			public iser::ISerializable,
			protected imod::TSingleModelObserverBase<iprm::IEnableableParam>
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCommandsObserverQmlComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_quickObjectCompPtr, "QuickObject", "Main QML Component", true, "QuickObject");
		I_ASSIGN(m_quickItemCreatedCompPtr, "QuickItemCreated", "Quick item created", false, "QuickItemCreated");
		I_ASSIGN_TO(m_quickItemCreatedModelCompPtr, m_quickItemCreatedCompPtr, false);
	I_END_COMPONENT;

	CCommandsObserverQmlComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimpemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (imod::TSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private Q_SLOTS:
	void OnCommandsModelChanged(QVariant variant);
	void OnTriggered();

private:
	iqtgui::CHierarchicalCommand m_commandsList;

private:
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);
	I_REF(iprm::IEnableableParam, m_quickItemCreatedCompPtr);
	I_REF(imod::IModel, m_quickItemCreatedModelCompPtr);
};


} // namespace imtqml


