import QtQuick

// `value as Type` (a type assertion of QML): the value when it is an instance of the type, else null; with a member
// access, in a condition, of null
Item {
    Rectangle { id: r; width: 10 }
    Text { id: t; text: "t" }
    property Item it: r
    Component.onCompleted: {
        console.log((it as Rectangle).width, (t as Rectangle), (it as Item) === r, (null as Item))
        let x = t as Text
        console.log(x.text, x as Text ? "yes" : "no", typeof (5 as Item))
        Qt.quit()
    }
}
