import QtQuick 2.12

QtObject {
    /*
        Technical column name. Use it as a stable identifier in handlers,
        settings, sorting, etc. Header text is controlled by title.
    */
    property string name: ""

    /*
        Text displayed in the header. If empty, TreeView uses name.
    */
    property string title: ""

    property int width: 120

    /*
        Path to the value inside normalized TreeView node.

        Examples:
            display: "text"
            display: "checked"
            display: "enabled"
            display: "data.opacity"
            display: "data.type"
    */
    property string display: ""

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
        If true, this column contains tree indentation and expand/collapse arrow.
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
