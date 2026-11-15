import QtQuick

Item {
    id: root

    Source {
        id: source
    }

    Component.onCompleted: {
        console.log("kinds", Source.Kind.Own, Source.Kind.View, Source.Kind.External)
        console.log("levels", Source.Level.Low, Source.Level.High)
        console.log("short", Source.External, Source.High)
        console.log("inside", source.isOwn(Source.Kind.Own), source.isOwn(Source.Kind.External), source.isHigh(20), source.isHigh(10))
        Qt.quit()
    }
}
