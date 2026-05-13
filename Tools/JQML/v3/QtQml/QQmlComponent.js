const QObject = require("./QObject")

class QQmlComponent extends QObject {
    __complete(){
        if(this.__completed) return

        this.__completed = true
    }
}



module.exports = QQmlComponent