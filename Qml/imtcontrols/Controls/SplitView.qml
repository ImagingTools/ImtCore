import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype SplitView
    \inqmlmodule imtcontrols

    Resizable split layout, API-compatible with Qt Quick Controls
    \c SplitView.

    SplitView arranges its children (its default content) in a row
    (\c Qt.Horizontal) or column (\c Qt.Vertical), separated by draggable
    handles that let the user redistribute the available space between the
    adjacent panes.

    \qml
    SplitView {
        orientation: Qt.Horizontal
        Rectangle { color: "#eee" }
        Rectangle { color: "#ddd" }
    }
    \endqml

    \sa StackView
*/
Item {
    id: split
    objectName: "ImtControlsSplitView"

    /*! Layout direction: \c Qt.Horizontal (default) or \c Qt.Vertical. */
    property int orientation: Qt.Horizontal
    readonly property bool horizontal: orientation === Qt.Horizontal

    /*! Thickness of the drag handles. */
    property real handleSize: Style.spacingS

    /*! Minimum size of any pane. */
    property real minimumSize: Style.spacingXXL

    /*! Panes declared inside the view. */
    default property alias contentData: holder.data

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightL

    property var _sizes: []
    property var _handles: []

    Item {
        id: holder
        anchors.fill: parent
    }

    Component {
        id: handleComponent
        Rectangle {
            property int sliceIndex: 0
            color: handleArea.pressed ? Style.buttonPressedColor
                   : handleArea.containsMouse ? Style.buttonHoverColor
                   : Style.borderColor

            MouseArea {
                id: handleArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: split.horizontal ? Qt.SplitHCursor : Qt.SplitVCursor
                property real pressPos: 0
                onPressed: pressPos = split.horizontal
                    ? mapToItem(split, mouse.x, mouse.y).x
                    : mapToItem(split, mouse.x, mouse.y).y
                onPositionChanged: {
                    if (!pressed)
                        return;
                    var cur = split.horizontal
                        ? mapToItem(split, mouse.x, mouse.y).x
                        : mapToItem(split, mouse.x, mouse.y).y;
                    split._resize(parent.sliceIndex, cur - pressPos);
                    pressPos = cur;
                }
            }
        }
    }

    Component.onCompleted: _rebuild()
    onWidthChanged: _relayout()
    onHeightChanged: _relayout()

    function _rebuild() {
        for (var h = 0; h < _handles.length; ++h) {
            if (_handles[h])
                _handles[h].destroy();
        }
        _handles = [];
        var n = holder.children.length;
        for (var i = 0; i < n - 1; ++i)
            _handles.push(handleComponent.createObject(split, { "sliceIndex": i }));
        _initSizes(n);
        _relayout();
    }

    function _initSizes(n) {
        _sizes = [];
        var total = (horizontal ? width : height) - handleSize * (n - 1);
        var each = n > 0 ? total / n : 0;
        for (var i = 0; i < n; ++i)
            _sizes.push(Math.max(minimumSize, each));
    }

    function _resize(index, delta) {
        if (index < 0 || index + 1 >= _sizes.length)
            return;
        var a = _sizes[index] + delta;
        var b = _sizes[index + 1] - delta;
        if (a < minimumSize || b < minimumSize)
            return;
        _sizes[index] = a;
        _sizes[index + 1] = b;
        _relayout();
    }

    function _relayout() {
        var n = holder.children.length;
        if (n === 0 || _sizes.length !== n)
            return;
        var total = (horizontal ? width : height) - handleSize * (n - 1);
        var sum = 0;
        for (var s = 0; s < n; ++s)
            sum += _sizes[s];
        if (sum <= 0)
            return;
        var scale = total / sum;
        var off = 0;
        for (var i = 0; i < n; ++i) {
            var size = _sizes[i] * scale;
            var pane = holder.children[i];
            if (horizontal) {
                pane.x = off; pane.y = 0; pane.width = size; pane.height = height;
            } else {
                pane.x = 0; pane.y = off; pane.width = width; pane.height = size;
            }
            _sizes[i] = size;
            off += size;
            if (i < n - 1) {
                var handle = _handles[i];
                if (handle) {
                    if (horizontal) {
                        handle.x = off; handle.y = 0; handle.width = handleSize; handle.height = height;
                    } else {
                        handle.x = 0; handle.y = off; handle.width = width; handle.height = handleSize;
                    }
                }
                off += handleSize;
            }
        }
    }
}
