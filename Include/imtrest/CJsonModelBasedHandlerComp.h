#pragma once


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QAction>
#include <QtCore/QJsonDocument>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>
#include <imod/TSingleModelObserverBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <iqtgui/IGuiObject.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IRequestHandler.h>
#include <imtbase/ICollectionInfo.h>
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IContactInfo.h>
#include <imtauth/IContactInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>
#include <imtauthgui/IDataController.h>


namespace imtrest
{


/**
 * \brief The CJsonModelBasedHandlerComp class generates JSON-model
 */
class CJsonModelBasedHandlerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	CJsonModelBasedHandlerComp();

	I_BEGIN_COMPONENT(CJsonModelBasedHandlerComp);
		I_REGISTER_INTERFACE(IRequestHandler);
		I_ASSIGN(m_jsonModelProcessor, "ModelProcessor", "description", false, "ClassName");
		I_ASSIGN(m_commandIdAttrPtr, "CommandId", "List of command-ID used with corresponded handlers", true, "");
	I_END_COMPONENT


	// reimplemented (IRequestHandler)
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

private:
	I_REF(ifile::IFileNameParam, m_jsonModelProcessor);
	I_ATTR(QByteArray, m_commandIdAttrPtr);

	imtbase::CTreeItemModel* m_treeItemModel;
};


} // namespace imtrest


