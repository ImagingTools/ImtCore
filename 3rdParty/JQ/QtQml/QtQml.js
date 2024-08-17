module.exports = {
    GroupProperty: require("./GroupProperty"),
    AnchorLine: require("./AnchorLine"),
    Anchors: require("./Anchors"),
    Border: require("./Border"),
    SourceSize: require("./SourceSize"),
    Font: require("./Font"),
    LinkedBool: require("./LinkedBool"),
    point: require("./Point"),

    QObject: require("./QObject"),
    QQmlComponent: require("./QQmlComponent"),
    Component: require("./Component"),
    QtObject: require("./QtObject"),
    Binding: require("./Binding"),
    Connections: require("./Connections"),
    Timer: require("./Timer"),
    
    real: require("./Real"),
    double: require("./Double"),
    int: require("./Int"),
    string: require("./String"),
    color: require("./Color"),
    bool: require("./Bool"),
    var: require("./Var"),
    alias: require("./Alias"),
    list: require("./List"),

    Signal: require("./Signal"),

    Screen: require("./Screen"),

    Models: require("./Models/Models"),

    XMLHttpRequest: {
        UNSENT: 0,
        OPENED: 1,
        HEADERS_RECEIVED: 2,
        LOADING: 3,
        DONE: 4,
    },
    FileReader: {
        EMPTY: 0,
        LOADING: 1,
        DONE: 2,
    },
}