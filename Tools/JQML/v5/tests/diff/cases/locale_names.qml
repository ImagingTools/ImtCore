import QtQuick

// Locale: monthName / standaloneMonthName (month 0 - 11), dayName / standaloneDayName (day 0 - 6, Sunday 0), the formats,
// firstDayOfWeek (Sunday 0)
Item {
    Component.onCompleted: {
        let en = Qt.locale("en_US"), ru = Qt.locale("ru_RU"), de = Qt.locale("de_DE")
        console.log(en.monthName(0), en.monthName(10), en.monthName(11, Locale.ShortFormat), en.standaloneMonthName(10, Locale.NarrowFormat))
        console.log(ru.monthName(10), ru.standaloneMonthName(10), ru.monthName(10, Locale.ShortFormat))
        console.log(en.dayName(0), en.dayName(1), en.dayName(6, Locale.ShortFormat), en.dayName(1, Locale.NarrowFormat), de.standaloneDayName(0))
        console.log(en.firstDayOfWeek, ru.firstDayOfWeek, de.firstDayOfWeek, Locale.Sunday, Locale.Monday)
        Qt.quit()
    }
}
