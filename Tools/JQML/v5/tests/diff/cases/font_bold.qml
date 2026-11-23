import QtQuick
import QtQuick.Controls

// QFont: bold sets the weight Bold or Normal, a weight above Medium is bold; on a Text and on the font of a control
Item {
    Text { id: t; font.bold: true }
    Text { id: t2; font.weight: Font.DemiBold }
    Text { id: t3; font.weight: Font.Medium }
    Label { id: l; font.bold: true }
    Control { id: c; font.weight: Font.Black; Control { id: inner } }
    Component.onCompleted: {
        console.log(t.font.bold, t.font.weight, t2.font.bold, t2.font.weight, t3.font.bold, l.font.weight)
        t.font.bold = false; console.log(t.font.bold, t.font.weight)
        t.font.weight = Font.Light; t.font.bold = true; console.log(t.font.weight); t.font.bold = false; console.log(t.font.weight)
        console.log(c.font.bold, inner.font.bold, inner.font.weight)
        c.font.bold = false; console.log(c.font.weight, inner.font.weight, inner.font.bold)
        Qt.quit()
    }
}
