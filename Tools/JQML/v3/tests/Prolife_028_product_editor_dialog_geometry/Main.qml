import QtQuick 2.0

// Prolife_028: ProductEditorDialog geometry
// contentComp ProductEditor fixed width=800, height=700
// activeProductIndex = -1
// orderId = "", orderUuid = ""

Item {
    id: root
    width: 1200
    height: 800

    Item {
        id: dialog

        property int activeProductIndex: -1
        property string orderId: ""
        property string orderUuid: ""

        width: 800
        height: 700

        // contentComp (ProductEditor placeholder)
        Item {
            id: contentComp
            width: 800
            height: 700
        }
    }

    Component.onCompleted: {
        console.log("prolife28",
            dialog.width, dialog.height,
            contentComp.width, contentComp.height,
            dialog.activeProductIndex,
            dialog.orderId === "" ? "empty" : dialog.orderId,
            dialog.orderUuid === "" ? "empty" : dialog.orderUuid
        )
        Qt.quit()
    }
}
