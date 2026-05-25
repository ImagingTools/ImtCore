import QtQuick

// Reproduces the issue: imperative assignment in onHeightChanged breaks
// the declarative binding "height: inner.height" in JS/Web runtime.
// After the first imperative write, the binding is severed and subsequent
// height changes from inner are no longer reflected in outer.

Item {
    id: root

    Item {
        id: outer
        // Declarative binding: outer should always match inner
        width: inner.width
        height: inner.height

        onHeightChanged: {
            // Imperative assignment — this breaks the binding in web runtime
            if (inner.height > 50) {
                outer.height = inner.height
            }
        }

        Item {
            id: inner
            width: 100
            height: 100
        }
    }

    Component.onCompleted: {
        // At this point outer.height should equal inner.height (100)
        var h0 = outer.height  // expected: 100

        // Now grow inner — outer binding should follow
        inner.height = 200
        var h1 = outer.height  // expected: 200

        // Shrink inner — outer binding should still follow
        inner.height = 80
        var h2 = outer.height  // expected: 80

        console.log("wd137", h0, h1, h2)
        Qt.quit()
    }
}
