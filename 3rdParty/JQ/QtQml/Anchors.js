const GroupProperty = require("./GroupProperty")
const AnchorLine = require("./AnchorLine")
const Real = require("./Real")
const Var = require("./Var")
const Signal = require("./Signal")

class Anchors extends GroupProperty {
    static meta = {
        fill: { type: Var, value: undefined, signalName: 'fillChanged' },
        centerIn: { type: Var, value: undefined, signalName: 'centerInChanged' },

        // left: { type: var, value: undefined, signalName: 'mainChanged' },
        // right: { type: var, value: undefined, signalName: 'mainChanged' },
        // top: { type: var, value: undefined, signalName: 'mainChanged' },
        // bottom: { type: var, value: undefined, signalName: 'mainChanged' },
        horizontalCenter: { type: Var, value: undefined, signalName: 'horizontalCenterChanged' },
        verticalCenter: { type: Var, value: undefined, signalName: 'verticalCenterChanged' },

        leftMargin: { type: Real, value: 0, signalName: 'leftMarginChanged' },
        topMargin: { type: Real, value: 0, signalName: 'topMarginChanged' },
        rightMargin: { type: Real, value: 0, signalName: 'rightMarginChanged' },
        bottomMargin: { type: Real, value: 0, signalName: 'bottomMarginChanged' },
        margins: { type: Real, value: 0, signalName: 'marginsChanged' },
        // horizontalCenterOffset: { type: real, value: 0, signalName: 'numberChanged' },
        // verticalCenterOffset: { type: real, value: 0, signalName: 'numberChanged' },

        fillChanged: {type:Signal, slotName:'onFillChanged', args:[]},
        centerInChanged: {type:Signal, slotName:'onCenterInChanged', args:[]},


        leftMarginChanged: {type:Signal, slotName:'onLeftMarginChanged', args:[]},
        topMarginChanged: {type:Signal, slotName:'onTopMarginChanged', args:[]},
        rightMarginChanged: {type:Signal, slotName:'onRightMarginChanged', args:[]},
        bottomMarginChanged: {type:Signal, slotName:'onBottomMarginChanged', args:[]},

        horizontalCenterChanged: {type:Signal, slotName:'onHorizontalCenterChanged', args:[]},
        verticalCenterChanged: {type:Signal, slotName:'onVerticalCenterChanged', args:[]},

        marginsChanged: {type:Signal, slotName:'onMarginsChanged', args:[]},
    }

    onMarginsChanged(){
        this.leftMargin = this.margins
        this.topMargin = this.margins
        this.rightMargin = this.margins
        this.bottomMargin = this.margins
    }

    onFillChanged(){
        let target = this.fill.__get()
        if(this.__parent && target){
            this.__parent.x.__setCompute(()=>{return this.leftMargin})
            this.__parent.y.__setCompute(()=>{return this.topMargin})
            this.__parent.width.__setCompute(()=>{return target.width - (this.leftMargin + this.rightMargin)})
            this.__parent.height.__setCompute(()=>{return target.height - (this.topMargin + this.bottomMargin)})

            this.__parent.x.__update()
            this.__parent.y.__update()
            this.__parent.width.__update()
            this.__parent.height.__update()
        }
    }

    onCenterInChanged(){
        let target = this.centerIn.__get()
        if(this.__parent && target){
            this.__parent.x.__setCompute(()=>{return target.width / 2 - this.__parent.width / 2})
            this.__parent.y.__setCompute(()=>{return target.height / 2 - this.__parent.height / 2})

            this.__parent.x.__update()
            this.__parent.y.__update()
        }
    }

    onHorizontalCenterChanged(){
        let target = this.horizontalCenter.__get()
        if(this.__parent && target){
            if(target.__float === AnchorLine.HorizontalCenter){
                this.__parent.x.__setCompute(()=>{return target.__parent.width / 2 - this.__parent.width / 2})
                this.__parent.x.__update()
            }
        }
    }

    onVerticalCenterChanged(){
        let target = this.verticalCenter.__get()
        if(this.__parent && target){
            if(target.__float === AnchorLine.VerticalCenter){
                this.__parent.y.__setCompute(()=>{return target.__parent.height / 2 - this.__parent.height / 2})
                this.__parent.y.__update()
            }
        }
    }
}

module.exports = Anchors