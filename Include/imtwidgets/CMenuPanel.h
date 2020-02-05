#pragma once


// Qt includes
#include <QtCore/QPropertyAnimation>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QTreeView>

// ACF includes
#include <iwidgets/CFocusDecorator.h>

// ImtCore includes
#include <GeneratedFiles/imtwidgets/ui_CMenuPanel.h>


namespace imtwidgets
{


class CMenuPanel: public QWidget, public Ui::CMenuPanel
{
	Q_OBJECT

public:
	Q_PROPERTY(int MinWidth READ GetMinWidth WRITE SetMinWidth)
	Q_PROPERTY(int MaxWidth READ GetMaxWidth WRITE SetMaxWidth)
	Q_PROPERTY(QByteArray ActivePage READ GetActivePage WRITE SetActivePage)

	enum DataRole
	{
		DR_PAGE_ID = Qt::UserRole,
		DR_PAGE_ENABLED,
		DR_PAGE_VISIBLE
	};

	CMenuPanel(QWidget* parent = nullptr);

	/**
		Get the minimum width of the menu panel.
	*/
	virtual int GetMinWidth() const;

	/**
		Set the minimum width of the menu panel.
	*/
	virtual void SetMinWidth(int minWidth);

	/**
		Get the maximal width of the menu panel.
	*/
	virtual int GetMaxWidth() const;

	/**
		Set the maximal width of the menu panel.
	*/
	virtual void SetMaxWidth(int maxWidth);

	/**
		Get currently active page.
	*/
	virtual QByteArray GetActivePage() const;

	/**
		Set active page.
	*/
	virtual void SetActivePage(const QByteArray& pageId);

	/**
		Get if the page enabled/disabled.
	*/
	virtual bool IsPageEnabled(const QByteArray& pageId) const;

	/**
		Set page enabled/disabled.
	*/
	virtual bool SetPageEnabled(const QByteArray& pageId, bool isPageEnabled = true);

	/**
		Get if the page visible/hidden.
	*/
	virtual bool IsPageVisible(const QByteArray& pageId) const;

	/**
		Set page visible/hidden.
	*/
	virtual bool SetPageVisible(const QByteArray& pageId, bool isPageVisible = true);

	/**
		Remove all pages from the MenuPanel.
	*/
	virtual void ResetPages();

	/**
		Checking page existence.
	*/
	virtual bool IsPageIdExist(const QByteArray& pageId) const;

	/**
		Remove page.
	*/
	virtual bool RemovePage(const QByteArray& pageId);

	/**
		Insert new page.
	*/
	virtual bool InsertPage(const QByteArray& pageId, const QByteArray& parentPageId = QByteArray());

	/**
		Get page order position in the list pages.
	*/
	virtual int GetPageOrder(const QByteArray& pageId) const;

	/**
		Set page order position in the list pages.
	*/
	virtual bool SetPageOrder(const QByteArray& pageId, int position);

	/**
		Get all childs pages from page.
	*/
	virtual QList<QByteArray> GetChilds(const QByteArray& pageId);

	/**
		Get the page icon.
	*/
	virtual QIcon GetPageIcon(const QByteArray& pageId) const;

	/**
		Set the page icon.
	*/
	virtual bool SetPageIcon(const QByteArray& pageId, const QIcon& pageIcon);

	/**
		Get the page name.
	*/
	virtual QString GetPageName(const QByteArray& pageId) const;

	/**
		Set the page name.
	*/
	virtual bool SetPageName(const QByteArray& pageId, const QString& pageName);

Q_SIGNALS:
	/**
		Signal will be emitted whenever the currently selected page will be changed.
	*/
	void PageIdChanged(const QByteArray& selected, const QByteArray& deselected);

private Q_SLOTS:
	void OnPageIdChanged(const QModelIndex& selected, const QModelIndex& deselected);

protected:
	//class FocusDecorationFactory: public iwidgets::CFocusDecorator::GraphicsEffectFactory
	//{
	//public:
	//	// reimplemented (iGraphicsEffectFactory)
	//	virtual QGraphicsEffect* CreateInstance(const QByteArray& keyId = "") const;
	//	virtual KeyList GetFactoryKeys() const;
	//};

protected:
	// reimplemented (QWidget)
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;

private:
	int m_minWidth;
	int m_maxWidth;
	int m_indent;

	QStandardItemModel m_model;
	QPropertyAnimation m_animationWidth;
	QPropertyAnimation m_animationIndent;
	
	//iwidgets::CFocusDecorator* m_focusDecoratorPtr;
	//FocusDecorationFactory m_graphicsEffectFactory;

private:
	QModelIndex GetModelIndex(const QByteArray& pageId) const;
};


} // namespace imtwidgets


