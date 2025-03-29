#pragma once


// ImtCore includes
#include <imtrest/TJsonRepresentationControllerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtrest
{


class CSchedulerParamsRepresentationControllerComp: public imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>
{
public:
	typedef imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0> BaseClass;

	I_BEGIN_COMPONENT(CSchedulerParamsRepresentationControllerComp)
		I_ASSIGN(m_dateFormatAttrPtr, "DateFormat", "Date format", false, "dd-MM-yyyy HH:mm");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0& sdlRepresentation) const override;
private:
	I_ATTR(QString, m_dateFormatAttrPtr);
};


} // namespace imtrest


