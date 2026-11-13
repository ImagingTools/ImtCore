import QtQuick

// String.arg(): each call fills the lowest numbered place marker, as QString::arg does
Item {
    Component.onCompleted: {
        console.log("chained:", "%1 of %2".arg(3).arg(0))
        console.log("reversed:", "%2 then %1".arg("a").arg("b"))
        console.log("repeated:", "%1 and %1".arg(5))
        console.log("two digits:", "%1 %10".arg("x").arg("y"))
        console.log("no marker:", "plain".arg(1))
        console.log("percent:", "%1%".arg(50))
        Qt.quit();
    }
}
