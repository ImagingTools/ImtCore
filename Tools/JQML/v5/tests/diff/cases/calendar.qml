import QtQuick
import QtQuick.Controls
import QtTest

// MonthGrid, DayOfWeekRow, WeekNumberColumn, CalendarModel, Calendar: the days of a month from the first day of the week of
// the locale (some days of the month before always shown), the week numbers, the names of the days, the title, a change
// of month or locale, a press and a click on a day (fixed-size delegates), the months of a CalendarModel
Item {
    width: 600; height: 500
    property var log: []
    MonthGrid { id: g; month: Calendar.November; year: 2026; locale: Qt.locale("en_US") }
    MonthGrid { id: cg; y: 200; month: 1; year: 2021; locale: Qt.locale("en_US"); spacing: 0
        delegate: Rectangle { width: 30; height: 20; required property var model; objectName: model.day + "." + model.month; color: model.today ? "red" : "white" }
        onPressed: (date) => log.push("pressed " + date.getDate() + "." + date.getMonth())
        onReleased: (date) => log.push("released " + date.getDate())
        onClicked: (date) => log.push("clicked " + date.getDate() + "." + date.getMonth() + "." + date.getFullYear())
    }
    DayOfWeekRow { id: r; y: 400; locale: Qt.locale("en_US") }
    DayOfWeekRow { id: rr; y: 450; locale: Qt.locale("de_DE") }
    WeekNumberColumn { id: w; x: 400; month: 10; year: 2026; locale: Qt.locale("en_US") }
    CalendarModel { id: cm; from: new Date(2025, 0, 1); to: new Date(2026, 11, 31) }
    function texts(item) {
        let s = []
        for (let i = 0; i < item.children.length; i++) if (item.children[i].text !== undefined) s.push(item.children[i].text + (item.children[i].opacity < 1 ? "*" : ""))
        return s.join(" ")
    }
    TestCase { name: "Calendar"; when: windowShown
        function test_a_grid() {
            console.log("grid", g.title, g.month, g.year, g.source.count, g.contentItem.children.length, g.spacing, g.padding, g.contentItem.rows, g.contentItem.columns)
            console.log("days", texts(g.contentItem))
            let s = g.source, rows = []
            console.log("cell", g.contentItem.children[0].x, g.contentItem.children[0].y, g.contentItem.children[7].y > g.contentItem.children[0].y, g.contentItem.children[0].horizontalAlignment, g.contentItem.children[0].font.bold)
            g.month = Calendar.March; console.log("march", g.title, texts(g.contentItem))
            g.year = 2027; console.log("2027", g.title)
            g.locale = Qt.locale("de_DE"); console.log("de", g.title, texts(g.contentItem))
            g.title = "Mine"; console.log("title", g.title); g.month = 4; console.log("title kept", g.title)
        }
        function test_b_click() {
            console.log("custom", cg.implicitWidth, cg.implicitHeight, cg.contentItem.children[0].objectName, cg.contentItem.children[41].objectName)
            mousePress(cg, 5 + 30 * 2, 5 + 20 * 1); mouseRelease(cg, 5 + 30 * 2, 5 + 20 * 1)
            console.log(log.join(", ")); log = []
            mouseClick(cg, 5, 5); mousePress(cg, 5, 5); mouseRelease(cg, 100, 70); mousePress(cg, 5, 5); mouseRelease(cg, 300, 150)
            console.log(log.join(", ")); log = []
        }
        function test_c_row() {
            console.log("row", r.source.count, texts(r.contentItem), r.font.bold, r.spacing, r.topPadding, r.bottomPadding, r.leftPadding)
            console.log("de row", texts(rr.contentItem))
        }
        function test_d_column() {
            console.log("column", w.source.count, texts(w.contentItem), w.font.bold, w.leftPadding, w.rightPadding, w.topPadding)
            w.month = 0; w.year = 2021; console.log("jan 2021", texts(w.contentItem))
            w.locale = Qt.locale("de_DE"); console.log("de", texts(w.contentItem))
        }
        function test_e_model() {
            console.log("model", cm.count, cm.monthAt(0), cm.yearAt(0), cm.monthAt(13), cm.yearAt(13), cm.indexOf(2026, 3), cm.indexOf(new Date(2025, 5, 7)), cm.indexOf(2024, 1))
            cm.to = new Date(2025, 5, 1); console.log("to june", cm.count)
            console.log("months", Calendar.January, Calendar.June, Calendar.December)
        }
    }
}
