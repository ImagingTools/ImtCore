#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/IModel.h>
#include <iqtgui/IGuiObject.h>

// ImtCore includes
#include <imtgui/IClientAreaProvider.h>


namespace imtgui
{


class CStandardClientAreaProviderComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IClientAreaProvider
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStandardClientAreaProviderComp);
		I_REGISTER_INTERFACE(IClientAreaProvider);
		I_ASSIGN(m_anchorAttrPtr, "AnchorCorner", "Anchor corner. see Qt::Corner\n0 - TopLeftCorner\n1 - TopRightCorner\n2 - BottomLeftCorner\n3 - BottomRightCorner", true, 3);
		I_ASSIGN(m_clientGuiCompPtr, "GuiObject", "GuiObject to get client area", false, "");
	I_END_COMPONENT;

	CStandardClientAreaProviderComp();

	// reimplemented (IClientAreaProvider)
	virtual QPoint GetAnchor() const override;
	virtual QSize GetSize() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	// reimplemented (QObject)
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private:
	QWidget* GetWindow();
	void SetRect(const QRect& rect);
	void SetCorner(Qt::Corner corner);

private:
	I_ATTR(int, m_anchorAttrPtr);
	I_REF(iqtgui::IGuiObject, m_clientGuiCompPtr);

	Qt::Corner m_corner;
	QPoint m_anchor;
	QSize m_size;
};


} // namespace imtgui


