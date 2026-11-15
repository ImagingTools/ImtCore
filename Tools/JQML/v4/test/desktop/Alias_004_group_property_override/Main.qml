import QtQuick

Item {
    width: 200
    height: 100

    Deco { id: plain }

    Deco {
        id: flat
        color: "transparent"
        border.width: 0
        radius: 1
    }

    Deco {
        id: colored
        border.color: "blue"
        border.width: 2
    }

    Deco {
        id: computed
        border.width: plain.base + 4
    }

    function report(tag, d) {
        var b = d.backgroundItem
        console.log(tag, b.border.width, Qt.colorEqual(b.border.color, "blue") ? "blue" : "black", b.radius)
    }

    Component.onCompleted: {
        report("plain", plain)
        report("flat", flat)
        report("colored", colored)
        report("computed", computed)
        plain.base = 5
        report("plain after base change", plain)
        flat.base = 5
        report("flat after base change", flat)
        colored.base = 5
        report("colored after base change", colored)
        flat.border.width = 4
        report("flat after script", flat)
        Qt.quit()
    }
}
