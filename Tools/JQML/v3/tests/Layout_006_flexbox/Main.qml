import QtQuick
import QtQuick.Layouts
Item {
    FlexboxLayout {
        id: fl
        width: 300
        height: 200
        direction: FlexboxLayout.Row
        wrap: FlexboxLayout.Wrap
        justifyContent: FlexboxLayout.JustifySpaceAround
        alignItems: FlexboxLayout.AlignCenter
        columnGap: 10
        rowGap: 8

        Rectangle { id: r0; color: "red";    width: 80; height: 40 }
        Rectangle { id: r1; color: "green";  width: 80; height: 60 }
        Rectangle { id: r2; color: "blue";   width: 80; height: 50 }
        Rectangle { id: r3; color: "orange"; Layout.fillWidth: true; height: 50 }
    }

    Component.onCompleted: {
        console.log("fl.w", fl.width)
        console.log("fl.h", fl.height)
        console.log("r0.x", r0.x)
        console.log("r0.y", r0.y)
        console.log("r0.w", r0.width)
        console.log("r0.h", r0.height)
        console.log("r1.x", r1.x)
        console.log("r1.y", r1.y)
        console.log("r3.w", r3.width)
        Qt.quit()
    }
}
