#include "imeas/INumericConstraints.h"
#include <imtgui/CNumericSliderLayoutFittableEditorGui.h>



namespace imtgui
{


CNumericSliderLayoutFittableEditorGui::CNumericSliderLayoutFittableEditorGui(QObject* parentPtr)
	: QObject(parentPtr)
{
	connect(this, &QObject::objectNameChanged, this, &CNumericSliderLayoutFittableEditorGui::OnObjectNameChanged);
}


bool CNumericSliderLayoutFittableEditorGui::PlaceWidgetToGridLayout(QGridLayout& layout, int rowIndex, int columnIndex)
{
	int rowToInsert = rowIndex > -1 ? rowIndex : layout.rowCount();

	if (!IsModelAttached()){
		I_IF_DEBUG(qWarning("Creating gui for CNumericSliderLayoutFittableEditorGui BEFORE attaching model!"));

		QSlider* valueSlider = new QSlider(layout.widget());
		layout.addWidget(valueSlider, rowToInsert, columnIndex);
		m_valueSliderPtrList << valueSlider;
		QLabel* valueLabel = new QLabel(layout.widget());
		layout.addWidget(valueLabel, rowToInsert, columnIndex + 1);
		m_valueLabelPtrList << valueLabel;

		double unitMultiplicationFactor = 1;
		int precision = 2;
		double sliderScaleFactor = 100;
		istd::CRange range(1, 100);

		m_unitMultiplicationFactorList << unitMultiplicationFactor;
		m_maxEditPrecisionList << precision;
		m_valueRangeList << range;
		m_sliderScaleFactorList << sliderScaleFactor;

		connect(valueSlider, &QSlider::valueChanged, this, &CNumericSliderLayoutFittableEditorGui::OnSliderValueChanged);

		// trigger method to apply value to child objects
		OnObjectNameChanged(objectName());

		return true;
	}

	ModelType* observedObject = GetObservedObject();
	imath::CVarVector values = observedObject->GetValues();
	const int valuesCount = values.GetElementsCount();
	QString possibleMaxValueText;

	const imeas::INumericConstraints* numericConstraintsPtr = observedObject->GetNumericConstraints();
	if (numericConstraintsPtr != nullptr){
		const int constraintsCount = numericConstraintsPtr->GetValueListInfo().GetOptionsCount();

		for (int constraintIndex = 0; constraintIndex < constraintsCount; ++constraintIndex){
			const imath::IUnitInfo*  unitInfoPtr = numericConstraintsPtr->GetNumericValueUnitInfo(constraintIndex);
			if (unitInfoPtr != nullptr){

				double unitMultiplicationFactor = unitInfoPtr->GetDisplayMultiplicationFactor();
				int precision = unitInfoPtr->GetValueManip().GetPrecision();
				int multiplicationPrecision = int(log10(unitMultiplicationFactor) + 0.5);
				int displayPrecision = qMax(0, precision - multiplicationPrecision);
				double sliderScaleFactor = qPow(10.0, double(displayPrecision + multiplicationPrecision));

				istd::CRange valueRange = unitInfoPtr->GetValueRange();

				m_unitMultiplicationFactorList << unitMultiplicationFactor;
				m_maxEditPrecisionList << precision;
				m_valueRangeList << valueRange;
				m_sliderScaleFactorList << sliderScaleFactor;
			}
		}
	}

	// set default values if constraints is not provided
	if (m_unitMultiplicationFactorList.isEmpty()){
		double unitMultiplicationFactor = 1;
		int precision = 2;
		double sliderScaleFactor = 100;
		istd::CRange range(1, 100);

		m_unitMultiplicationFactorList << unitMultiplicationFactor;
		m_maxEditPrecisionList << precision;
		m_valueRangeList << range;
		m_sliderScaleFactorList << sliderScaleFactor;
	}

	for (int valueIndex = 0; valueIndex < valuesCount; ++valueIndex){
		m_eventIgnoreFlags << false;

		istd::CRange valueRange = m_valueRangeList[qMin(valueIndex, m_valueRangeList.count()-1)];
		double sliderScaleFactor = m_sliderScaleFactorList[qMin(valueIndex, m_valueRangeList.count()-1)];
		possibleMaxValueText = QString::number(valueRange.GetMaxValue(), 'f', m_maxEditPrecisionList[qMin(valueIndex, m_valueRangeList.count()-1)]);

		// replace all digits to '4' because it has maximum width in most fonts
		for (QChar& ch: possibleMaxValueText){
			if (ch.isDigit()){
				ch = '4';
			}
		}
		// add one more sign for small padding
		possibleMaxValueText += '4';

		QSlider* valueSliderPtr = new QSlider(Qt::Horizontal, layout.widget());
		valueSliderPtr->setTickPosition(QSlider::TicksAbove);
		valueSliderPtr->setRange(valueRange.GetMinValue() * sliderScaleFactor, valueRange.GetMaxValue() * sliderScaleFactor);
		valueSliderPtr->setPageStep(10 * valueSliderPtr->singleStep());
		valueSliderPtr->setTickInterval(valueSliderPtr->pageStep() * sliderScaleFactor);
		layout.addWidget(valueSliderPtr, rowToInsert, columnIndex);
		m_valueSliderPtrList << valueSliderPtr;

		QLabel* valueLabel = new QLabel(layout.widget());
		const QFontMetrics labelFontMetrix = valueLabel->fontMetrics();
		const int maximumWidth = labelFontMetrix.horizontalAdvance(possibleMaxValueText);
		valueLabel->setMinimumWidth(maximumWidth);

		valueLabel->setText("0");
		layout.addWidget(valueLabel, rowToInsert, columnIndex + 1);
		m_valueLabelPtrList << valueLabel;

		connect(valueSliderPtr, &QSlider::valueChanged, this, &CNumericSliderLayoutFittableEditorGui::OnSliderValueChanged);
		connect(valueSliderPtr, &QSlider::sliderPressed, this, &CNumericSliderLayoutFittableEditorGui::OnSliderPressed);
		connect(valueSliderPtr, &QSlider::sliderReleased, this, &CNumericSliderLayoutFittableEditorGui::OnSliderReleased);
		SetValueToSlider(valueSliderPtr, values.GetElement(valueIndex));

		++rowToInsert;
	}

	// trigger method to apply value to child objects
	OnObjectNameChanged(objectName());

	return true;
}


bool CNumericSliderLayoutFittableEditorGui::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	bool isModelAttached = BaseClass::OnModelAttached(modelPtr, changeMask);

	/// \todo implement creating widget before attaching model
	Q_ASSERT(m_valueSliderPtrList.count() == 0);
	Q_ASSERT(m_valueLabelPtrList.count() == 0);

	return isModelAttached;
}


void CNumericSliderLayoutFittableEditorGui::UpdateEditor(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	ModelType* observedObject = GetObservedObject();
	imath::CVarVector values = observedObject->GetValues();
	const int valuesCount = qMin(m_valueSliderPtrList.count(), values.GetElementsCount());

	for (int valueIndex = 0; valueIndex < valuesCount; ++valueIndex){
		double value = values.GetElement(valueIndex);
		SetValueToSlider(m_valueSliderPtrList[valueIndex], value);
	}
}


void CNumericSliderLayoutFittableEditorGui::UpdateModelFromEditor() const
{
	ModelType* observedObjectPtr = GetObservedObject();
	imath::CVarVector values = observedObjectPtr->GetValues();
	const int valuesCount = values.GetElementsCount();

	for (int valueIndex = 0; valueIndex < valuesCount; ++valueIndex){
		double newValue = GetValueFromSlider(m_valueSliderPtrList[valueIndex]);
		values.SetElement(valueIndex, newValue);
	}
	observedObjectPtr->SetValues(values);
}


bool CNumericSliderLayoutFittableEditorGui::IsReadOnly() const
{
	return !m_valueSliderPtrList.isEmpty() && !m_valueSliderPtrList[0].isNull() && m_valueSliderPtrList[0]->isEnabled();
}


void CNumericSliderLayoutFittableEditorGui::SetReadOnly(bool state)
{
	if (m_valueSliderPtrList.isEmpty() || m_valueSliderPtrList[0].isNull()){
		return;
	}

	if (m_valueSliderPtrList[0]->isEnabled() != state){
		for (QSlider* sliderPtr: qAsConst(m_valueSliderPtrList)){
			if (sliderPtr!= nullptr){
				sliderPtr->setEnabled(!state);
			}
		}
		for (QLabel* labelPtr: qAsConst(m_valueLabelPtrList)){
			if (labelPtr!= nullptr){
				labelPtr->setEnabled(!state);
			}
		}
	}
}


void CNumericSliderLayoutFittableEditorGui::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	UpdateEditor(changeSet);
}


void CNumericSliderLayoutFittableEditorGui::SetValueToSlider(QSlider* sliderPtr, double value)
{
	const int indexOfSlider = m_valueSliderPtrList.indexOf(sliderPtr);
	Q_ASSERT(indexOfSlider > -1);

	const double sliderScaleFactor = m_sliderScaleFactorList[qMin(indexOfSlider, m_sliderScaleFactorList.count() - 1)];
	sliderPtr->setValue(value * sliderScaleFactor);
}

double CNumericSliderLayoutFittableEditorGui::GetValueFromSlider(const QSlider* sliderPtr) const
{	
	QSlider* nonConstSliderPtr = const_cast<QSlider*>(sliderPtr);
	const int indexOfSlider = m_valueSliderPtrList.indexOf(nonConstSliderPtr);
	Q_ASSERT(indexOfSlider > -1);

	const double sliderValue = sliderPtr->value() / m_unitMultiplicationFactorList[qMin(indexOfSlider, m_unitMultiplicationFactorList.count() - 1)];

	return sliderValue;
}


void CNumericSliderLayoutFittableEditorGui::OnObjectNameChanged(const QString newObjectName)
{
	for (int index = 0; index < m_valueSliderPtrList.count(); ++ index){
		QPointer<QSlider> sliderPtr =  m_valueSliderPtrList[index];
		QPointer<QLabel> labelPtr = m_valueLabelPtrList[index];
		QString objectNameBase = newObjectName;
		if (index > 0){
			objectNameBase += QString::number(index);
		}
		if (!sliderPtr.isNull()){
			sliderPtr->setObjectName(objectNameBase + "ValueSlider");
		}
		if (!labelPtr.isNull()){
			labelPtr->setObjectName(objectNameBase + "ValueLabel");
		}
	}
}


void CNumericSliderLayoutFittableEditorGui::OnSliderValueChanged()
{
	QObject* senderPtr = sender();
	QSlider* sliderPtr = dynamic_cast<QSlider*>(senderPtr);
	const int indexOfSlider = m_valueSliderPtrList.indexOf(sliderPtr);
	const double sliderValue = GetValueFromSlider(sliderPtr);
	const int maxEditPrecision = m_maxEditPrecisionList[qMin(indexOfSlider, m_sliderScaleFactorList.count() - 1)];
	m_valueLabelPtrList[indexOfSlider]->setText(QString::number(sliderValue, 'f', maxEditPrecision));

	bool& ignoreEvents = m_eventIgnoreFlags[indexOfSlider];
	if (!ignoreEvents){
		ignoreEvents = true;

		ModelType* observedObjectPtr = GetObservedObject();
		imath::CVarVector values = observedObjectPtr->GetValues();
		values.SetElement(indexOfSlider, sliderValue);
		observedObjectPtr->SetValues(values);

		ignoreEvents = false;
	}
}



void CNumericSliderLayoutFittableEditorGui::OnSliderPressed()
{
	QObject* senderPtr = sender();
	QSlider* sliderPtr = dynamic_cast<QSlider*>(senderPtr);
	int indexOfSlider = m_valueSliderPtrList.indexOf(sliderPtr);
	bool& ignoreEvents = m_eventIgnoreFlags[indexOfSlider];
	ignoreEvents = true;
}


void CNumericSliderLayoutFittableEditorGui::OnSliderReleased()
{
	QObject* senderPtr = sender();
	QSlider* sliderPtr = dynamic_cast<QSlider*>(senderPtr);
	const int indexOfSlider = m_valueSliderPtrList.indexOf(sliderPtr);
	const double sliderValue = GetValueFromSlider(sliderPtr);
	const int maxEditPrecision = m_maxEditPrecisionList[qMin(indexOfSlider, m_sliderScaleFactorList.count() - 1)];
	m_valueLabelPtrList[indexOfSlider]->setText(QString::number(sliderValue, 'f', maxEditPrecision));

	ModelType* observedObjectPtr = GetObservedObject();
	imath::CVarVector values = observedObjectPtr->GetValues();
	values.SetElement(indexOfSlider, sliderValue);
	observedObjectPtr->SetValues(values);

	bool& ignoreEvents = m_eventIgnoreFlags[indexOfSlider];
	ignoreEvents = false;
}


} // namespace imtgui
