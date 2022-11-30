#include <imtbase/CCommandLineParamsAdapterComp.h>


// ACF includes
#include <imeas/CGeneralNumericValue.h>
#include <iprm/CNameParam.h>
#include <istd/CChangeGroup.h>


namespace imtbase
{


// protected methods

int CCommandLineParamsAdapterComp::GetParameterCount()
{
	int count = m_nameAttrPtr.GetCount();
	count = qMin(count, m_idAttrPtr.GetCount());
	count = qMin(count, m_typeAttrPtr.GetCount());

	return count;
}


// reimplemented (imod::CSingleModelObserverBase)

void CCommandLineParamsAdapterComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	istd::CChangeGroup group(this);

	ResetData();

	const iprm::IOptionsList* optionListPtr = GetObservedObject();
	int listItemCount = optionListPtr->GetOptionsCount();

	QString optionName;
	QString optionValue;

	for (int i = 0; i < listItemCount; i++){
		QString param = optionListPtr->GetOptionName(i);
		if (param.startsWith(*m_prefixAttrPtr)){
			optionName = param.last(param.size() - (*m_prefixAttrPtr).size());
			optionValue.clear();
		}
		else{
			if (!optionName.isEmpty()){
				optionValue = param;
			}
		}

		if (!optionName.isEmpty() && !optionValue.isEmpty()){
			int paramCount = GetParameterCount();
			bool exit = false;
			for (int paramIndex = 0; paramIndex < paramCount && !exit; paramIndex++){
				if (m_nameAttrPtr[paramIndex] == optionName){
					switch (m_typeAttrPtr[paramIndex]){
					case PT_STRING:
						{
							iprm::CNameParam* paramPtr = new iprm::CNameParam();
							paramPtr->SetName(optionValue);
							SetEditableParameter(m_idAttrPtr[paramIndex], paramPtr, true);
							exit = true;
						}
						break;
					case PT_INT:
						{
							bool isOk;
							int value = optionValue.toInt(&isOk);
							if (!isOk){
								ResetData();
								return;
							}

							imeas::CGeneralNumericValue* paramPtr = new imeas::CGeneralNumericValue();
							imath::CVarVector varVector(1);
							varVector.SetElement(0, value);
							paramPtr->SetValues(varVector);
							SetEditableParameter(m_idAttrPtr[paramIndex], paramPtr, true);
							exit = true;
						}
						break;
					case PT_DOUBLE:
						{
							bool isOk;
							int value = optionValue.toDouble(&isOk);
							if (!isOk){
								ResetData();
								return;
							}

							imeas::CGeneralNumericValue* paramPtr = new imeas::CGeneralNumericValue();
							imath::CVarVector varVector(1);
							varVector.SetElement(0, value);
							paramPtr->SetValues(varVector);
							SetEditableParameter(m_idAttrPtr[paramIndex], paramPtr, true);
							exit = true;
						}
						break;
					default:
						ResetData();
						Q_ASSERT(false);
					}
				}
			}

			optionName.clear();
			optionValue.clear();
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CCommandLineParamsAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(!(*m_prefixAttrPtr).isEmpty());

	int count = GetParameterCount();
	for (int i = 0; i < count; i++){
		Q_ASSERT(!m_nameAttrPtr[i].isEmpty());
		Q_ASSERT(!m_idAttrPtr[i].isEmpty());
		Q_ASSERT(m_typeAttrPtr[i] >= PT_STRING && m_typeAttrPtr[i] <= PT_DOUBLE);
	}

	if (m_paramListModelCompPtr.IsValid()){
		m_paramListModelCompPtr->AttachObserver(this);
	}
}


void CCommandLineParamsAdapterComp::OnComponentDestroyed()
{
	EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


