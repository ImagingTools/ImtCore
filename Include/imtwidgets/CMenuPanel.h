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
	Q_PROPERTY(int MaxWidth READ GetMaxWidth WRITE SetMaxWidth)
	Q_PROPERTY(QByteArray ActivePage READ GetActivePage WRITE SetActivePage)

	enum DataRole
	{
		DR_PAGE_ID = Qt::UserRole,
		DR_PAGE_ENABLED,
		DR_PAGE_VISIBLE
	};

	enum AnimationAction
	{
		AA_NONE = 0,
		AA_EXPAND,
		AA_COLLAPSE
	};

	CMenuPanel(QWidget* parent = nullptr);

	virtual int GetMaxWidth() const;
	virtual void SetMaxWidth(int maxWidth);
	virtual QByteArray GetActivePage() const;
	virtual void SetActivePage(const QByteArray& pageId);
	virtual bool IsPageEnabled(const QByteArray& pageId) const;
	virtual bool SetPageEnabled(const QByteArray& pageId, bool isPageEnabled = true);
	virtual bool IsPageVisible(const QByteArray& pageId) const;
	virtual bool SetPageVisible(const QByteArray& pageId, bool isPageVisible = true);
	virtual void ResetPages();
	virtual bool IsPageIdExist(const QByteArray& pageId) const;
	virtual bool RemovePage(const QByteArray& pageId);
	virtual bool InsertPage(const QByteArray& pageId, const QByteArray& parentPageId = QByteArray());
	virtual int GetPageOrder(const QByteArray& pageId) const;
	virtual bool SetPageOrder(const QByteArray& pageId, int position);
	virtual QList<QByteArray> GetChilds(const QByteArray& pageId);
	virtual QIcon GetPageIcon(const QByteArray& pageId) const;
	virtual bool SetPageIcon(const QByteArray& pageId, const QIcon& pageIcon);
	virtual QString GetPageName(const QByteArray& pageId) const;
	virtual bool SetPageName(const QByteArray& pageId, const QString& pageName);
	virtual void SetItemPadding(int padding);
	virtual void SetIconSize(int size);

	virtual void SetAnimationDelay(int delay);
	virtual void SetAnimationDuration(int duration);

	virtual void SetItemTextColor(QColor color);
	virtual void SetItemSelectedColor(QColor color);
	virtual void SetItemSelectedContourColor(QColor color);
	virtual void SetItemMouserOverColor(QColor color);
	virtual void SetItemMouserOverSelectedColor(QColor color);

	virtual void SetMainWidget(QWidget *mainWidget);


Q_SIGNALS:
	/**
		Signal will be emitted whenever the currently selected page will be changed.
	*/
	void PageIdChanged(const QByteArray& selected, const QByteArray& deselected);

private Q_SLOTS:
	void OnPageIdChanged(const QModelIndex& selected, const QModelIndex& deselected);
	void OnAnimationFinished();
	void on_pushTop_clicked();
	void on_pushBottom_clicked();

protected:
	// reimplemented (QObject)
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	virtual void timerEvent(QTimerEvent *event);

	// reimplemented (QWidget)
	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void resizeEvent(QResizeEvent *event) override;

private:
	int m_padding;
	int m_maxWidth;
	int m_indent;

	AnimationAction m_animationAction;
	int m_animationTimerIdentifier;
	int m_animationDelay;
	int m_animationDuration;

	QStandardItemModel m_model;
	QPropertyAnimation m_animationWidth;
	QPropertyAnimation m_animationIndent;
	QPropertyAnimation m_animationWidthComp;

	QWidget *m_leftFrame;
	QWidget *m_mainWidget;

private:
	QModelIndex GetModelIndex(const QByteArray& pageId) const;
	int CalculateMaxItemWith();
	void CheckButtonsVisible();
};


} // namespace imtwidgets


