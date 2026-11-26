import QtQuick

Item {
    Component.onCompleted: {
        console.log(Theme.currentTheme, Theme.themeStatusbar, Theme.first, Theme.name, Theme.Dark, Theme.THEME_LAST)
        Theme.loadTheme(Theme.THEME_DARK)
        console.log(Theme.currentTheme, Theme.themeStatusbar, Theme.name)
        Qt.quit()
    }
}
