#pragma once

// Qt includes
#include <QtCore/QPropertyAnimation>
#include <QtCore/QModelIndex>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class CMenuPanel; }
QT_END_NAMESPACE

namespace imtwidgets
{

class CMenuPanel : public QWidget
{
    Q_OBJECT

public:
    //Q_PROPERTY(int MinWidth READ GetMinWidth WRITE SetMinWidth)
    Q_PROPERTY(int MaxWidth READ GetMaxWidth WRITE SetMaxWidth)
    Q_PROPERTY(QByteArray ActivePage READ GetActivePage WRITE SetActivePage)

    enum ItemDataType {
        IDT_PAGE_ID = Qt::UserRole,
        IDT_PAGE_ENABLED,
        IDT_PAGE_VISIBLE
    };

    explicit CMenuPanel(QWidget *parent = nullptr);
    ~CMenuPanel() override;

    /**
        Get MenuPanel max width.
    */
    virtual int GetMaxWidth() const;

    /**
        Set MenuPanel max width.
    */
    virtual void SetMaxWidth(int maxWidth);

    /**
        Get MenuPanel active page.
    */
    virtual QByteArray GetActivePage() const;

    /**
        Set MenuPanel active page.
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
    virtual bool InsertPage(const QByteArray &pageId, const QByteArray& parentPageId = QByteArray());

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
        Set the page Name.
    */
    virtual bool SetPageName(const QByteArray& pageId, const QString& pageName);

Q_SIGNALS:
    /**
        Signal will be emitted whenever the currently selected page will be changed.
    */
    void EmitPageIdChanged(QByteArray& oldIndex, QByteArray& newIndex);

protected Q_SLOTS:
    void OnPageIdChanged(const QByteArray& pageId);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    Ui::CMenuPanel *ui;

    //int m_minWidth;
    int m_maxWidth;
    int m_indent;
    QByteArray m_activePage;

    QStandardItemModel *m_modelPtr;
    QPropertyAnimation *m_animationWidthPtr;
    QPropertyAnimation *m_animationIndentPtr;

    QModelIndex GetModelIndex(const QByteArray& pageId) const;
};

} //namespace imtwidgets
