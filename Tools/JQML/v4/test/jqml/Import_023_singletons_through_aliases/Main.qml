import QtQuick
import ModA 1.0 as A
import ModB 1.0 as B

Item {


    Component.onCompleted: {
        console.log("styles", A.Style.name, B.Style.name)
        Qt.quit()
    }
}
