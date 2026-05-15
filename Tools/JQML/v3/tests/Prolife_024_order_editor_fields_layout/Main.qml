import QtQuick 2.0

// Mirrors OrderEditor field-level layout: form column with labelled input rows.
// OrderEditor has: instanceIdInput, purchaseIdInput, descriptionInput,
// customerCB, orderStatusCB, productsView, additionalRolesView.
// Here we replicate a representative form geometry and test:
//  - that each row fills the form width
//  - that fields are positioned sequentially in y

Item {
    id: root
    width: 900
    height: 640

    property int rowHeight: 36
    property int labelWidth: 160
    property int spacing: 8

    Item {
        id: formContainer
        anchors.fill: parent
        anchors.margins: 20

        Column {
            id: fieldsColumn
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: root.spacing

            // instanceIdInput row
            Row {
                id: instanceIdRow
                width: parent.width
                height: root.rowHeight
                spacing: root.spacing

                Item { id: instanceIdLabel; width: root.labelWidth; height: parent.height }
                Item { id: instanceIdInput; width: parent.width - root.labelWidth - root.spacing; height: parent.height }
            }

            // purchaseIdInput row
            Row {
                id: purchaseIdRow
                width: parent.width
                height: root.rowHeight
                spacing: root.spacing

                Item { id: purchaseIdLabel; width: root.labelWidth; height: parent.height }
                Item { id: purchaseIdInput; width: parent.width - root.labelWidth - root.spacing; height: parent.height }
            }

            // descriptionInput row
            Row {
                id: descriptionRow
                width: parent.width
                height: root.rowHeight * 2
                spacing: root.spacing

                Item { id: descLabel; width: root.labelWidth; height: parent.height }
                Item { id: descriptionInput; width: parent.width - root.labelWidth - root.spacing; height: parent.height }
            }

            // customerCB row
            Row {
                id: customerRow
                width: parent.width
                height: root.rowHeight
                spacing: root.spacing

                Item { id: customerLabel; width: root.labelWidth; height: parent.height }
                Item { id: customerCB; width: parent.width - root.labelWidth - root.spacing; height: parent.height }
            }

            // orderStatusCB row
            Row {
                id: orderStatusRow
                width: parent.width
                height: root.rowHeight
                spacing: root.spacing

                Item { id: statusLabel; width: root.labelWidth; height: parent.height }
                Item { id: orderStatusCB; width: parent.width - root.labelWidth - root.spacing; height: parent.height }
            }
        }
    }

    Component.onCompleted: {
        console.log(
            "prolife24",
            root.width,
            root.height,
            formContainer.width,
            formContainer.height,
            fieldsColumn.width,
            instanceIdRow.y,
            purchaseIdRow.y,
            descriptionRow.y,
            customerRow.y,
            orderStatusRow.y,
            instanceIdInput.width,
            instanceIdInput.height,
            purchaseIdInput.width,
            customerCB.width,
            orderStatusCB.width
        )
        Qt.quit()
    }
}
