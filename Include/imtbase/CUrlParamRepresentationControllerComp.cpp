#include <imtbase/CUrlParamRepresentationControllerComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/IUrlParam.h>


namespace imtbase
{


// protected methods

// reimplemented (imtbase::CObjectRepresentationControllerCompBase)

bool CUrlParamRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	Q_ASSERT(urlParamPtr != nullptr);

	const QUrl url = urlParamPtr->GetUrl();

	representation.SetData("Host", url.host());
	representation.SetData("Port", url.port());

	return true;
}


// reimplemented (IRepresentationController)

bool CUrlParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::IUrlParam* urlParamPtr = dynamic_cast<const imtbase::IUrlParam*>(&dataModel);
	if (urlParamPtr != nullptr) {
		return true;
	}

	return false;
}


bool CUrlParamRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	imtbase::IUrlParam* urlParamPtr = dynamic_cast<imtbase::IUrlParam*>(&dataModel);
	Q_ASSERT(urlParamPtr != nullptr);

	QUrl url;

	if (representation.ContainsKey("Host")){
		QString host = representation.GetData("Host").toString();

		url.setHost(host);
	}

	if (representation.ContainsKey("Port")){
		int port = representation.GetData("Port").toInt();

		url.setPort(port);
	}

	urlParamPtr->SetUrl(url);

	return true;
}


} // namespace imtbase


