#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqtgui/TDesignerGuiCompBase.h>

// ImtCore includes
#include <imtgui/IClientAreaProvider.h>
#include <imtgui/IPopupController.h>
#include <GeneratedFiles/imtgui/ui_CPopupTestGuiComp.h>


namespace imtgui
{


class CPopupTestGuiComp:
			public ilog::TLoggerCompWrap<iqtgui::TDesignerGuiCompBase<Ui::CPopupTestGuiComp>>
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<iqtgui::TDesignerGuiCompBase<Ui::CPopupTestGuiComp>> BaseClass;

	I_BEGIN_COMPONENT(CPopupTestGuiComp);
		I_REGISTER_SUBELEMENT(ClientAreaProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(ClientAreaProvider, IClientAreaProvider, ExtractClientAreaProvider);
		I_ASSIGN(m_popupControllerCompPtr, "PopupController", "Popup controller", false, "PopupController");
	I_END_COMPONENT;

	CPopupTestGuiComp();

protected:
	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (iqtgui::TDesignSchemaHandlerWrap)
	virtual void OnDesignSchemaChanged(const QByteArray& themeId) override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private Q_SLOTS:
	void on_AnchorTL_clicked();
	void on_AnchorTR_clicked();
	void on_AnchorBL_clicked();
	void on_AnchorBR_clicked();

	void on_InfoButton_clicked();
	void on_WarningButton_clicked();
	void on_ErrorButton_clicked();
	void on_CriticalButton_clicked();

	void on_MessageButton_clicked();
	void on_WidgetButton_clicked();
	void on_WidgetControlsButton_clicked();

	void on_TimeoutUser_toggled(bool checked);

	void on_AllowClosing_toggled(bool checked);
	void on_UseAreaProvider_toggled(bool checked);

private:
	QRect CalculatePopupAreaFromScreen();
	void UpdateClientArea();
	istd::IInformationProvider::InformationCategory GetCategory();
	int GetTimeout();

	static QRect Normalize(const QRect& rect);

private:
	class ClientAreaProvider: virtual public IClientAreaProvider
	{
	public:
		ClientAreaProvider();

		void SetCorner(Qt::Corner corner);
		void SetRect(const QRect& rect/*, bool isMoved*/);

		// reimplemented (IClientAreaProvider)
		virtual QPoint GetAnchor() const override;
		virtual QSize GetSize() const override;

	private:
		Qt::Corner m_corner;
		QPoint m_anchor;
		QSize m_size;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractClientAreaProvider(CPopupTestGuiComp& component)
	{
		return &component.m_clientAreaProvider;
	}

private:
	I_REF(IPopupController, m_popupControllerCompPtr);

	int m_messageCounter;
	imod::TModelWrap<ClientAreaProvider> m_clientAreaProvider;
};


} // namespace imtgui


