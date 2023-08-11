#include <imtgui/CMultiParamsSetGuiComp.h>


// ACF includes
#include <imeas/INumericValue.h>
#include <imeas/INumericConstraints.h>
#include <iprm/INameParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtgui/CNumericSliderLayoutFittableEditorGui.h>
#include <imtgui/CNameLayoutFittableEditorGui.h>
#include <imtgui/CEnableableLayoutFittableEditorGui.h>
#include <imtgui/CSelectionParamLayoutFittableEditorGui.h>


namespace imtgui
{


void CMultiParamsSetGuiComp::SetReadOnly(bool state)
{
	ParamsFrame->setEnabled(state);
}


void CMultiParamsSetGuiComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CMultiParamsSetGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imod::IModel* modelPtr = GetObservedModel();
	ModelType* modelObjectPtr = GetObservedObject();
	QGridLayout* gridLayoutPtr = dynamic_cast<QGridLayout*>(ParamsFrame->layout());

	Q_ASSERT(modelPtr != nullptr);
	Q_ASSERT(modelObjectPtr != nullptr);
	Q_ASSERT(gridLayoutPtr != nullptr);

	if (modelPtr == nullptr || modelObjectPtr == nullptr || gridLayoutPtr == nullptr){
		return;
	}

	const int idsCount = m_idsAttrPtr.GetCount();
	for (int idIndex = 0; idIndex < idsCount; ++idIndex){

		QString labelText;
		if (idIndex < m_labelTextsAttrPtr.GetCount()){
			labelText = m_labelTextsAttrPtr[idIndex];
		}
		QByteArray objectId;
		if (idIndex < m_objectIdsAttrPtr.GetCount()){
			objectId = m_objectIdsAttrPtr[idIndex];
		}

		const int rowToInsert = gridLayoutPtr->rowCount();
		QLabel* descriptionLabelPtr = new QLabel(ParamsFrame);
		descriptionLabelPtr->setText(labelText);
		gridLayoutPtr->addWidget(descriptionLabelPtr, rowToInsert, 0);

		if (!objectId.isEmpty()){
			descriptionLabelPtr->setObjectName(QString(objectId) + "DescLabel");
		}

		const QByteArray paramId = m_idsAttrPtr[idIndex];
		if (paramId.trimmed().isEmpty()){
			const QFontMetrics descLabelFontMetrix = descriptionLabelPtr->fontMetrics();
			/// \workaround set max width to aviod expanding
			descriptionLabelPtr->setMaximumHeight(descLabelFontMetrix.height());

			continue;
		}

		if (paramId != QByteArrayLiteral("*")){
			iser::ISerializable* paramPtr = modelObjectPtr->GetEditableParameter(paramId);
			Q_ASSERT(paramPtr != nullptr);
			modelPtr = dynamic_cast<imod::IModel*>(paramPtr);
		}

		ILayoutFittableModelEditor* guiEditorPtr = CreateEditorFromModel(modelPtr, QString(objectId));
		if (guiEditorPtr == nullptr){
			I_CRITICAL();

			continue;
		}

		if (labelText.isEmpty()){
			imeas::INumericValue* numericParamPtr = dynamic_cast<imeas::INumericValue*>(modelPtr);
			if (numericParamPtr != nullptr){
				const imeas::INumericConstraints* numericConstraints = numericParamPtr->GetNumericConstraints();
				if (numericConstraints != nullptr){
					const iprm::IOptionsList& valueListInfo = numericConstraints->GetValueListInfo();
					const int valueListInfoCount = valueListInfo.GetOptionsCount();
					if (valueListInfoCount > 0){
						descriptionLabelPtr->setText(valueListInfo.GetOptionName(0));
					}
				}
			}
		}

		Q_ASSERT(modelPtr != nullptr);
		if (guiEditorPtr != nullptr && modelPtr != nullptr){
			const bool isModelAttached = modelPtr->AttachObserver(guiEditorPtr);
			Q_ASSERT(isModelAttached);
			if (!isModelAttached){
				return;
			}
		}

		const bool isWidgetCreated = guiEditorPtr->PlaceWidgetToGridLayout(*gridLayoutPtr, rowToInsert);
		Q_ASSERT(isWidgetCreated);
		if (!isWidgetCreated){
			return;
		}

		m_modelEditors.PushBack(guiEditorPtr);
	}
}


bool CMultiParamsSetGuiComp::OnModelDetached(imod::IModel* modelPtr)
{
	ModelType* modelObjectPtr = GetObservedObject();

	Q_ASSERT(modelPtr != nullptr && modelObjectPtr != nullptr);
	if (modelPtr == nullptr || modelObjectPtr == nullptr){
		return false;
	}

	const int guisCount = m_modelEditors.GetCount();
	const int idsCount = m_idsAttrPtr.GetCount();
	for (int guiIndex = 0; guiIndex < guisCount; ++guiIndex){
		ILayoutFittableModelEditor* editorPtr =  m_modelEditors.GetAt(guiIndex);
		imod::IModel* currentModelPtr = modelPtr;
		if (guiIndex < idsCount){
			const QByteArray paramId = m_idsAttrPtr[guiIndex];
			if (paramId.isEmpty()){
				continue;
			}

			if (paramId != QByteArrayLiteral("*")){
				iser::ISerializable* paramPtr = modelObjectPtr->GetEditableParameter(paramId);
				Q_ASSERT(paramPtr != nullptr);
				currentModelPtr = dynamic_cast<imod::IModel*>(paramPtr);
			}
		}

		Q_ASSERT(currentModelPtr != nullptr);
		if (currentModelPtr != nullptr && editorPtr != nullptr){
			if (currentModelPtr->IsAttached(editorPtr)){
				currentModelPtr->DetachObserver(editorPtr);
			}
		}
	}

	m_modelEditors.Reset();

	return BaseClass::OnModelDetached(modelPtr);
}


ILayoutFittableModelEditor* CMultiParamsSetGuiComp::CreateEditorFromModel(const imod::IModel* modelPtr, const QString& objectName) const
{
	ILayoutFittableModelEditor* retVal = nullptr;

	const imeas::INumericValue* numericParamPtr = dynamic_cast<const imeas::INumericValue*>(modelPtr);
	if (numericParamPtr != nullptr){
		CNumericSliderLayoutFittableEditorGui* guiPtr = new CNumericSliderLayoutFittableEditorGui;
		if (!objectName.isNull()){
			guiPtr->setObjectName(objectName);
		}

		return guiPtr;
	}

	const iprm::INameParam* nameParamPtr = dynamic_cast<const iprm::INameParam*>(modelPtr);
	if (nameParamPtr != nullptr){
		CNameLayoutFittableEditorGui* guiPtr = new CNameLayoutFittableEditorGui;
		if (!objectName.isNull()){
			guiPtr->setObjectName(objectName);
		}

		return guiPtr;
	}

	const iprm::IEnableableParam* enableableParamPtr = dynamic_cast<const iprm::IEnableableParam*>(modelPtr);
	if (enableableParamPtr != nullptr){
		CEnableableLayoutFittableEditorGui* guiPtr = new CEnableableLayoutFittableEditorGui;
		if (!objectName.isNull()){
			guiPtr->setObjectName(objectName);
		}

		return guiPtr;
	}

	const iprm::ISelectionParam* selectionParamPtr = dynamic_cast<const iprm::ISelectionParam*>(modelPtr);
	if (selectionParamPtr != nullptr){
		CSelectionParamLayoutFittableEditorGui* guiPtr = new CSelectionParamLayoutFittableEditorGui;
		if (!objectName.isNull()){
			guiPtr->setObjectName(objectName);
		}

		return guiPtr;
	}

	Q_ASSERT_X(retVal != nullptr, "CreateEditorFromModel", "Provided model is not supported");

	return retVal;
}


} // namespace imtgui


