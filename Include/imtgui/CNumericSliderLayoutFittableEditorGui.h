#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>



// ACF includes
#include <istd/TRange.h>
#include <istd/TPointerVector.h>
#include <imod/TModelWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <imtgui/ILayoutFittableModelEditor.h>

// ACF-Solutions includes
#include <imeas/INumericValue.h>

namespace imtgui
{


class CNumericSliderLayoutFittableEditorGui:
			public QObject,
			public ILayoutFittableModelEditor,
			public imod::TSingleModelObserverBase<imeas::INumericValue>
{
	Q_OBJECT

public:
	typedef imod::TSingleModelObserverBase<imeas::INumericValue> BaseClass;

	CNumericSliderLayoutFittableEditorGui(QObject* parentPtr = nullptr);

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

private:
	void SetValueToSlider(QSlider* sliderPtr, double value);
	double GetValueFromSlider(const QSlider* sliderPtr) const;

private Q_SLOTS:
	void OnObjectNameChanged(const QString newObjectName);
	void OnSliderValueChanged();
	void OnSliderPressed();
	void OnSliderReleased();

private:
	QList<QPointer<QSlider>> m_valueSliderPtrList;
	QList<QPointer<QLabel>> m_valueLabelPtrList;
	QList<bool> m_eventIgnoreFlags;

	QList<double> m_unitMultiplicationFactorList;
	QList<double> m_sliderScaleFactorList;
	QList<int> m_maxEditPrecisionList;
	QList<istd::CRange> m_valueRangeList;
};


} // namespace imtgui



