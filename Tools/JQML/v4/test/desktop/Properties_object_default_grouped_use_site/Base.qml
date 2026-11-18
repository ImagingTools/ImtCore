import QtQuick

// A property of a component type whose default is an object made in this file (FieldFilterDelegate: `property FieldFilter
// defaultFieldFilter: FieldFilter { ... }`).
Item {
    id: base
    property FilterData filter: FilterData {
        operations: ["Equal"]
    }
}
