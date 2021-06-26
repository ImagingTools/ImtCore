#pragma once


// ACF includes
#include <iauth/ILogin.h>
#include <ilog/IMessageConsumer.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtgui/CMultiDocumentWorkspaceGuiCompBase.h>
#include <imtgui/TStandardDocumentViewDecorator.h>
#include <GeneratedFiles/imtgui/ui_CStandardDocumentViewDecorator.h>


namespace imtgui
{


class CMultiDocumentWorkspaceGuiComp: public imtgui::CMultiDocumentWorkspaceGuiCompBase
{
	Q_OBJECT

public:
	typedef imtgui::CMultiDocumentWorkspaceGuiCompBase BaseClass;

	I_BEGIN_COMPONENT(CMultiDocumentWorkspaceGuiComp);
		I_ASSIGN(m_loginCompPtr, "Login", "Login component", false, "Login");
		I_ASSIGN(m_logCompPtr, "Log", "Message consumer component", false, "Log");
		I_ASSIGN(m_tabStyleSheetPropertyAttrPtr, "TabStyleSheetProperty", "Enable a stylesheet with a property for the tab", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgui::CMultiDocumentWorkspaceGuiCompBase)
	virtual IDocumentViewDecorator* CreateDocumentViewDecorator(
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr) override;

	// reimplemented (idoc::CMultiDocumentManagerBase)
	virtual QString GetSaveFilePath(const QByteArray& documentTypeId, const istd::IChangeable* dataObjectPtr, const QString& currentFilePath) const override;
	virtual QStringList GetOpenFilePaths(const QByteArray* documentTypeIdPtr = NULL) const override;

	// reimplemented (iqt:CGuiComponentBase)
	virtual void OnGuiCreated() override;

protected Q_SLOTS:
	void OnSaveDocument();

private:
	enum DataRole
	{
		DR_PATH = Qt::UserRole,
		DR_TYPE_ID
	};

	friend class MdiDocumentViewDecorator;
	friend class TStandardDocumentViewDecorator<CMultiDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator>;

	I_ATTR(QByteArray, m_tabStyleSheetPropertyAttrPtr);
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(ilog::IMessageConsumer, m_logCompPtr);
};


class MdiDocumentViewDecorator:
			public TStandardDocumentViewDecorator<CMultiDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator>
{
public:
	typedef TStandardDocumentViewDecorator<CMultiDocumentWorkspaceGuiComp, Ui::CStandardDocumentViewDecorator> BaseClass;

	MdiDocumentViewDecorator(
				CMultiDocumentWorkspaceGuiComp* parentPtr,
				istd::IPolymorphic* viewPtr,
				QWidget* parentWidgetPtr,
				const ifile::IFilePersistence* persistencePtr,
				const DecoratorConfiguration& configuration);
};


} // namespace imtgui


