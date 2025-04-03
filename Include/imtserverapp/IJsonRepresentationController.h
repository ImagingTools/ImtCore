#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <iprm/IParamsSet.h>


namespace imtserverapp
{


class IJsonRepresentationController: virtual public istd::IPolymorphic
{
public:
	struct RepresentationInfo
	{
		QByteArray modelId;
		QString name;
		QString description;
	};
	
	virtual RepresentationInfo GetRepresentationInfo() const = 0;
	virtual QByteArray GetTypeId() const = 0;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const = 0;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				QJsonObject& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
	virtual bool GetDataModelFromRepresentation(
				const QJsonObject& representation,
				istd::IChangeable& dataModel) const = 0;
};


} // namespace imtserverapp


