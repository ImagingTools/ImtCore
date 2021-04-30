#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IRequestHandler.h>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>




//------------------------------------------------------------------mem test
// Qt includes
#include <QtWidgets/QAction>
#include <QtCore/QJsonDocument>


// ACF includes
//#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/IGuiObject.h>
#include <imod/TSingleModelObserverBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
//#include <GeneratedFiles/imtauthgui/ui_CContactInfoEditorComp.h>
#include <imtauth/IContactInfo.h>
#include <imtauthgui/IDataController.h>
#include <imtauthgui/CTreeItemModel.h>

// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>
//------------------------------------------------------------------

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

	imtauthgui::CTreeItemModel *m_treeItemModel;
};


} // namespace imtrest


