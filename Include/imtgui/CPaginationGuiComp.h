#pragma once


// Qt includes
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>

// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/TGuiComponentBase.h>


namespace imtgui
{


class PaginationGuiItem: public QLabel
{
	Q_OBJECT
public:
	PaginationGuiItem(QWidget* parent = nullptr);
	PaginationGuiItem(const QString& text, QWidget* parent = nullptr);

Q_SIGNALS:
	void Clicked();

protected:
	// reimplemented (QWidget)
	virtual void mousePressEvent(QMouseEvent* ev) override;
};


class CPaginationGuiComp: public iqtgui::TGuiComponentBase<QFrame>, public imod::TSingleModelObserverBase<iprm::ISelectionParam>
{
	Q_OBJECT
public:
	typedef iqtgui::TGuiComponentBase<QFrame> BaseClass;
	typedef imod::TSingleModelObserverBase<iprm::ISelectionParam> BaseClass2;

	I_BEGIN_COMPONENT(CPaginationGuiComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;

protected:
	void RefreshWidget();

private Q_SLOTS:
	void OnFirstClicked();
	void OnPrevClicked();
	void OnPageClicked();
	void OnNextClicked();
	void OnLastClicked();

private:
};


} // namespace imtgui


