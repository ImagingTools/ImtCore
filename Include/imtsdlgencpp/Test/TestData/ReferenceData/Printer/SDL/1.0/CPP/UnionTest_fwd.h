#pragma once


#include <QtCore/QObject>
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace sdl::V1_0::modsdl
{

inline namespace UnionTestSdlEnums
{

Q_NAMESPACE

enum class LinkStatus {
	OPEN,
	CLOSE,
};

Q_ENUM_NS(LinkStatus)


class EnumLinkStatus: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString OPEN READ GetOPEN NOTIFY OPENChanged)
	Q_PROPERTY(QString CLOSE READ GetCLOSE NOTIFY CLOSEChanged)

protected:
	static QString GetOPEN() { return "OPEN"; }
	static QString GetCLOSE() { return "CLOSE"; }

signals:
	void OPENChanged();
	void CLOSEChanged();
};


} // inline namespace UnionTestSdlEnums

// type forward declarations
class CCoords;
class CPrinterSpecificationBase;
class CLink;
class CPrinterBase;

// union forward declarations
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;

// request forward declarations
class CGetSpecificationsGqlRequest;

class CPrinterCollectionControllerCompBase: public ::imtservergql::CObjectCollectionControllerCompBase
{

public:
	typedef ::imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CPrinterCollectionControllerCompBase)
	I_END_COMPONENT

	virtual QMap<int, QByteArray> GetSupportedCommandIds() const override;

protected:
	// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;
	virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, QJsonObject& dataObj, QString& errorMessage) const override;

protected:
	// Printer methods
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const CGetSpecificationsGqlRequest& getSpecificationsRequest,
				PrinterSpecification& representationPayload,
				QString& errorMessage) const = 0;

};

// GqlHandlerCompBase forward declaration
class CUnionTestGqlHandlerCompBase;

} // namespace sdl::V1_0::modsdl
