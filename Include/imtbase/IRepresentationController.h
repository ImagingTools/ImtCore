#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class CTreeItemModel;


class IRepresentationController: virtual public istd::IPolymorphic
{
public:
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const = 0;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation) const = 0;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const = 0;
};


class CParamSetRepresentationControllerComp: public IRepresentationController
{
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override
	{
		const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
		if (paramsSetPtr != nullptr) {
			return true;
		}

		return false;
	}


	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation) const override
	{
		const iprm::IParamsSet* paramsSetPtr = dynamic_cast<const iprm::IParamsSet*>(&dataModel);
		if (paramsSetPtr == nullptr){
			return false;
		}

		iprm::IParamsSet::Ids parameterIds = paramsSetPtr->GetParamIds();
		for (const QByteArray& paramId : parameterIds){
			const iser::ISerializable* paramPtr = GetParameter(paramId);

			const IRepresentationController* subControllerPtr = FindSubController(*paramPtr);
			if (subControllerPtr != nullptr){
				CTreeItemModel subParameterRepresentation;
				if (subControllerPtr->GetRepresentationFromDataModel(paramPtr, subParameterRepresentation)){
					representation.AddModel(paramId, &subParameterRepresentation);
				}
				else{
					return false;
				}
			}
		}

		return true;
	}

	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const override
	{
		return false;
	}

protected:
	const IRepresentationController* FindSubController(const istd::IChangeable& dataModel)
	{
		if (IsModelSupported(dataModel)){
			return this;
		}

		// Iterate over m_paramRepresentationControllersCompPtr using IsModelSupported
	}

private:
	/**
		All needed sub-controllers according to the used data model types inside of the parameter set
	*/
	I_MULTIREF(IRepresentationController, m_paramRepresentationControllersCompPtr);
};


} // namespace imtbase


