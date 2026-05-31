import QtQuick

Item {
   width: column.width
   height: column.height

   onWidthChanged: {
       console.log('Item.onWidthChanged', width)
   }
   onHeightChanged: {
       console.log('Item.onHeightChanged', height)
   }

   Component.onCompleted: {
        console.log("Item.onCompleted")
        Qt.quit()
    }

   Column {
        id: column
        spacing: 10
        onWidthChanged: {
            console.log('Column.onWidthChanged', width)
        }
        onHeightChanged: {
            console.log('Column.onHeightChanged', height)
        }

        Component.onCompleted: {
            console.log("Column.onCompleted")
        }

        Item {
            width: 50
            height: 50
        }

        Item {
            width: 35
            height: 35
        }

        Item {
            width: 75
            height: 75
        }
   }
}
