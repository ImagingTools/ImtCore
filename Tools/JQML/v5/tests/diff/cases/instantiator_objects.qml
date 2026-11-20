import QtQuick
import QtQml

// Instantiator makes an object for every element of the model (TenantMembersPage makes the items of a menu with it): count, objectAt,
// objectAdded / objectRemoved, the roles of the delegate, and a model that changes.
Item {
    width: 40
    height: 40
    property var log: []

    Instantiator {
        id: inst
        model: ["a", "b", "c"]
        delegate: QtObject {
            property string label: "item-" + modelData
            property int position: index
        }
        onObjectAdded: (index, object) => console.log("added", index, object.label)
        onObjectRemoved: (index, object) => console.log("removed", index, object.label)
    }

    Component.onCompleted: {
        console.log("count", inst.count)
        console.log("objectAt", inst.objectAt(1).label, inst.objectAt(1).position, inst.objectAt(5))
        inst.model = ["x", "y"]
        console.log("count after", inst.count, inst.objectAt(0).label, inst.objectAt(1).label)
        inst.model = []
        console.log("count empty", inst.count)
        inst.model = 2
        console.log("count number", inst.count, inst.objectAt(1).position)
        console.log("active", inst.active)
        inst.active = false
        console.log("inactive", inst.active, inst.count)
        inst.active = true
        console.log("active again", inst.count)
        Qt.quit()
    }
}
