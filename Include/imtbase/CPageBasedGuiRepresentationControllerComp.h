#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


class CPageBasedGuiRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPageBasedGuiRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable &dataModel, CTreeItemModel &representation) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel &representation, istd::IChangeable &dataModel) const override;
};


} // namespace imtbase


