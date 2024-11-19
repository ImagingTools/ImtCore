#include <imtrest/CUrlParamRepresentationControllerComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/IUrlParam.h>


namespace imtrest
{


// protected methods

// reimplemented (imtrest::CObjectRepresentationControllerCompBase)

bool CUrlParamRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	Q_ASSERT(urlParamPtr != nullptr);

	QUrl url = urlParamPtr->GetUrl();

	if (url.host().isEmpty()){
		url.setHost("localhost");
	}

	if (url.scheme().isEmpty()){
		url.setScheme("http");
	}

	if (url.port() == -1){
		url.setPort(80);
	}

	representation.SetData("Value", url.toString());

	return true;
}


// reimplemented (IRepresentationController)

bool CUrlParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	if (urlParamPtr != nullptr){
		return true;
	}

	return false;
}


bool CUrlParamRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	imtbase::IUrlParam* urlParamPtr = dynamic_cast<imtbase::IUrlParam*>(&dataModel);
	Q_ASSERT(urlParamPtr != nullptr);

	QUrl url;

	if (representation.ContainsKey("Value")){
		QString urlValue = representation.GetData("Value").toString();

		url = QUrl(urlValue);
	}

	urlParamPtr->SetUrl(url);

	return true;
}


} // namespace imtrest


