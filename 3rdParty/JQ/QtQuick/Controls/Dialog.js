const Item = require("../Item")
const Popup = require("./Popup")
const Real = require("../../QtQml/Real")
const Int = require("../../QtQml/Int")
const Var = require("../../QtQml/Var")
const String = require("../../QtQml/String")
const Variant = require("../../QtQml/Variant")
const Signal = require("../../QtQml/Signal")

class Dialog extends Popup {
    static OkAn = 0
    static OpenAn = 1
    static SaveA = 2
    static CancelA = 3
    static CloseA = 4
    static DiscardA = 5
    static ApplyAn = 6
    static ResetA = 7
    static RestoreDefaultsA = 8
    static HelpA = 9
    static SaveAllA = 10
    static YesA = 11
    static YesToAllA = 12
    static NoA = 13
    static NoToAllA = 14
    static AbortAn = 15
    static RetryA = 16
    static IgnoreAn = 17
    static NoButtonAn = 18

    static meta = Object.assign({}, Popup.meta, {
        footer: { type: Variant, targetType: Item},
        header: { type: Variant, targetType: Item},
        implicitFooterHeight: { type: Real, value:0},
        implicitFooterWidth: { type: Real, value:0},
        implicitHeaderHeight: { type: Real, value:0},
        implicitHeaderWidth: { type: Real, value:0},
        result: { type: Int, value:0},
        standardButtons: { type: Real, value:0},
        title: { type: String, value:''},

        footerChanged: {type:Signal, args:[]},
        headerChanged: {type:Signal, args:[]},
        implicitFooterHeightChanged: {type:Signal, args:[]},
        implicitFooterWidthChanged: {type:Signal, args:[]},
        implicitHeaderHeightChanged: {type:Signal, args:[]},
        implicitHeaderWidthChanged: {type:Signal, args:[]},
        resultChanged: {type:Signal, args:[]},
        standardButtonsChanged: {type:Signal, args:[]},
        titleChanged: {type:Signal, args:[]},

        accepted: {type:Signal, args:[]},
        applied: {type:Signal, args:[]},
        discarded: {type:Signal, args:[]},
        helpRequested: {type:Signal, args:[]},
        rejected: {type:Signal, args:[]},
        reset: {type:Signal, args:[]},
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Dialog')

        return obj
    }

    accept(){}
    done(result){}
    reject(){}
    standardButton(button){}
}



module.exports = Dialog