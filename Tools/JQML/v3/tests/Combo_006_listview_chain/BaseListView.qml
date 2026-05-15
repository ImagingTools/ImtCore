import QtQuick

Item {
    id: root

    property int baseCount: 3

    signal ready()

    ListModel {
        id: model

        ListElement { title: "alpha" }
        ListElement { title: "beta" }
        ListElement { title: "gamma" }
    }

    ListView {
        id: view
        width: 120
        height: 80
        model: model

        delegate: Item {
            width: 20
            height: 12

            property string label: title
        }
    }

    function entryCount() {
        return baseCount
    }

    Component.onCompleted: {
        ready()
    }
}
