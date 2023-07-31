#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QCheckBox>

// ACF includes
#include <istd/TRange.h>
#include <istd/TPointerVector.h>
#include <imod/TModelWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iprm/IEnableableParam.h>

#include <imtgui/ILayoutFittableModelEditor.h>


namespace imtgui
{


class CEnableableLayoutFittableEditorGui:
			public QObject,
			public ILayoutFittableModelEditor,
			public imod::TSingleModelObserverBase<iprm::IEnableableParam>
{
	Q_OBJECT

public:
	typedef imod::TSingleModelObserverBase<iprm::IEnableableParam> BaseClass;

	CEnableableLayoutFittableEditorGui(QObject* parentPtr = nullptr);

	// reimplemented (ILayoutFittableWidget)
	bool PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex = -1, int columnIndex = 1) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;

	// reimplemented (imod::IModelEditor)
	virtual void UpdateEditor(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void UpdateModelFromEditor() const override;
	virtual bool IsReadOnly() const override;
	virtual void SetReadOnly(bool state) override;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private Q_SLOTS:
	void OnObjectNameChanged(const QString newObjectName);
	void OnCheckBoxClicked();

private:
	QPointer<QCheckBox> m_checkBoxPtr;
};


} // namespace imtgui



