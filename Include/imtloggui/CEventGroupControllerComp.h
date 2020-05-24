#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMap>
#include <QtCore/QObject>

// Acf includes
#include <icomp/CComponentBase.h>
#include <imath/CDoubleManip.h>
#include <imeas/INumericConstraints.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtloggui/IEventGroupController.h>
#include <imtloggui/IEventItemController.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CEventGroupControllerComp: public QObject, public icomp::CComponentBase, virtual public IEventGroupController
{
	Q_OBJECT

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEventGroupControllerComp)
		I_REGISTER_INTERFACE(IEventGroupController);
		I_REGISTER_SUBELEMENT(VerticalScaleConstraints);
		I_REGISTER_SUBELEMENT_INTERFACE(VerticalScaleConstraints, istd::IChangeable, ExtractVerticalScaleConstraints);
		I_REGISTER_SUBELEMENT_INTERFACE(VerticalScaleConstraints, imod::IModel, ExtractVerticalScaleConstraints);
		I_REGISTER_SUBELEMENT_INTERFACE(VerticalScaleConstraints, imeas::INumericConstraints, ExtractVerticalScaleConstraints);
		I_ASSIGN_MULTI_0(m_groupRefsCompPtr, "EventGroups", "Event groups components", false);
	I_END_COMPONENT

	CEventGroupControllerComp();

	// reimplemented (imtloggui::IEventGroupController)
	virtual void SetScene(QGraphicsScene* scenePtr) override;
	virtual void SetView(QGraphicsView* viewPtr) override;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) override;

	virtual bool CreateGraphicsItem() override;
	virtual bool DestroyGraphicsItem() override;
	virtual QGraphicsItem* GetGraphicsItem() override;

	virtual QByteArrayList GetAvailableGroupList() const override;
	virtual QByteArrayList GetActiveGroupList() const override;
	virtual IEventItemController* GetGroup(const QByteArray& groupId) const override;
	virtual QString GetGroupName(const QByteArray& groupId) const override;

	virtual IEventItemController* AddGroup(const QByteArray& groupId, const QString& groupName) override;
	virtual void AddGroups(const imtlog::IMessageGroupInfoProvider::GroupInfos& groupInfos) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;

	virtual bool SetVisible(const QByteArray& groupId, bool isVisible) const override;

	virtual void OnTimeAxisChanged() override;
	virtual void OnViewPortChanged() override;

Q_SIGNALS:
	void MinimumVerticalScaleChanged(double scale);

protected:
	class VerticalScaleConstraints:
				virtual public imeas::INumericConstraints,
				virtual public iprm::IOptionsList,
				virtual public imath::IUnitInfo,
				virtual public imath::CDoubleManip
	{
	public:
		typedef istd::IChangeable BaseClass;

		VerticalScaleConstraints();
		void SetParent(CEventGroupControllerComp* parentPtr);
		void SetMinScale(double scale);
	
		// reimplemented (istd::IChangeable)
		virtual bool IsEqual(const IChangeable& object) const override;

		// reimplemented (imeas::INumericConstraints)
		virtual const iprm::IOptionsList& GetValueListInfo() const override;
		virtual const imath::IUnitInfo* GetNumericValueUnitInfo(int index) const override;
		
		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const override;
		virtual int GetOptionsCount() const override;
		virtual QString GetOptionName(int index) const override;
		virtual QString GetOptionDescription(int index) const override;
		virtual QByteArray GetOptionId(int index) const override;
		virtual bool IsOptionEnabled(int index) const override;

		// reimplemented (imath::IUnitInfo)
		virtual int GetUnitType() const override;
		virtual QString GetUnitName() const override;
		virtual double GetDisplayMultiplicationFactor() const override;
		virtual istd::CRange GetValueRange() const override;
		virtual const imath::IDoubleManip& GetValueManip() const override;

		// reimplemented (imath::CDoubleManip)
		virtual int GetPrecision() const override;

	private:
		CEventGroupControllerComp* m_parentPtr;
		double m_minVerticalScale;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractVerticalScaleConstraints(CEventGroupControllerComp& component)
	{
		return &component.m_verticalScaleConstraints;
	}

private:
	I_MULTIREF(IEventItemController, m_groupRefsCompPtr);

	imod::TModelWrap<VerticalScaleConstraints> m_verticalScaleConstraints;

	QGraphicsItemGroup* m_graphicsItem;
	
	QMap<QByteArray, IEventItemController*> m_groups;

	QGraphicsScene* m_scenePtr;
	QGraphicsView* m_viewPtr;
	const IEventScenePositionProvider* m_timeAxisPtr;

	double m_minimumVerticalScale;
};


} // namespace imtloggui


