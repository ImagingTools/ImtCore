import QtQuick

Item {
    id: window
    property DeepProvider designProvider: DeepProvider {}

    Item {
        Component.onCompleted: {
            designProvider.setDesignSchema("Light")
            window.designProvider.setDesignSchema("Dark")
        }
    }
}
