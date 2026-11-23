pragma Singleton
import QtQuick

// a singleton that reads its own enums through its name while it is made (ComponentLibrary of QmlAppTemplate)
QtObject {
    enum ThemeNames { THEME_DEFAULT, THEME_LIGHT, THEME_DARK, THEME_LAST }
    enum ThemeColors { Light = 0, Dark = 1 }
    property int currentTheme: -1
    property int themeStatusbar
    property int first: Theme.THEME_DARK
    property string name: currentTheme === Theme.THEME_DARK ? "dark" : "other"
    Component.onCompleted: loadTheme(Theme.THEME_LIGHT)
    function loadTheme(index) {
        if (index <= Theme.THEME_DEFAULT || index >= Theme.THEME_LAST) return
        themeStatusbar = index === Theme.THEME_LIGHT ? Theme.Light : Theme.Dark
        currentTheme = index
        console.log("load", index, themeStatusbar, Theme.currentTheme)
    }
}
