const GroupProperty = require("./GroupProperty")
const Var = require("./Var")
const String = require("./String")
const Signal = require("./Signal")

class KeyNavigation extends GroupProperty {
    static meta = {
        backtab: { type: Var },
        down: { type: Var },
        left: { type: Var },
        right: { type: Var },
        tab: { type: Var },
        up: { type: Var },
        priority : { type: String, value: '' },
    }


}



module.exports = KeyNavigation