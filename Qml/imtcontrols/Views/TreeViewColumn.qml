import QtQuick 2.12

/*
    Declarative column descriptor for BasicTreeView.

    A column can be supplied to BasicTreeView in two equivalent ways:

      1) As a plain JS object inside the `columns` array:

            columns: [
                { id: "featureName", name: qsTr("Feature Name"), tree: true },
                { id: "featureId",   name: qsTr("Feature-ID") }
            ]

      2) As a declarative TreeViewColumn child of the tree:

            BasicTreeView {
                TreeViewColumn { columnId: "featureName"; name: qsTr("Feature Name"); tree: true }
                TreeViewColumn { columnId: "featureId";   name: qsTr("Feature-ID") }
            }

    NOTE on the field identifier:
      The JS-object form uses the key `id`. In the declarative form the same
      role is exposed as `columnId`, because `id` is reserved by QML for object
      identifiers and cannot be used as a normal property. BasicTreeView reads
      the field key from `id` first and falls back to `columnId`, so both forms
      behave identically.
*/
QtObject {
    // Marker used by BasicTreeView to discover declarative columns among its
    // children. Do not change.
    readonly property bool isTreeViewColumn: true

    /*
        Field identifier / data role. This is the key looked up inside the
        node's data dictionary (node.data[columnId]) to obtain the cell value.
        Equivalent to the `id` key of a plain-object column declaration.
    */
    property string columnId: ""

    /*
        Header text shown for this column. If empty, the tree falls back to the
        field identifier.
    */
    property string name: ""

    /*
        Fixed column width in pixels. 0 (the default) means the column stretches
        to share the remaining width with other stretch columns.
    */
    property int width: 0
    property int minWidth: 0

    /*
        Optional live source property read directly from the underlying QObject
        (node.data.sourceItem[source]). Use it when the tree must reflect
        external mutations of the model object immediately.
    */
    property string source: ""

    /*
        Supported values:
            "auto"
            "string"
            "number"
            "bool"
            "checkState"
            "combo"
    */
    property string type: "auto"

    /*
        If true, this column contains tree indentation and the expand/collapse arrow.
    */
    property bool tree: false

    /*
        If true, cells in this column can be edited. Header itself is never edited.
    */
    property bool editable: false

    /*
        Values for combo editor.
    */
    property var options: []

    property int horizontalAlignment: Text.AlignLeft
}
