const GroupProperty = require("./GroupProperty")
const AnchorLine = require("./AnchorLine")
const Real = require("./Real")
const Var = require("./Var")
const Signal = require("./Signal")
const QtFunctions = require("../Qt/functions")

class Anchors extends GroupProperty {
    static meta = {
        fill: { type: Var, value: undefined, signalName: 'fillChanged' },
        centerIn: { type: Var, value: undefined, signalName: 'centerInChanged' },

        left: { type: Var, value: undefined, signalName: 'leftChanged' },
        right: { type: Var, value: undefined, signalName: 'rightChanged' },
        top: { type: Var, value: undefined, signalName: 'topChanged' },
        bottom: { type: Var, value: undefined, signalName: 'bottomChanged' },
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

        leftChanged: {type:Signal, slotName:'onLeftChanged', args:[]},
        topChanged: {type:Signal, slotName:'onTopChanged', args:[]},
        rightChanged: {type:Signal, slotName:'onRightChanged', args:[]},
        bottomChanged: {type:Signal, slotName:'onBottomChanged', args:[]},
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
        let self = this.__parent

        if(self && target){
            let updateList = []

            if(self.parent === target){
                self.x = QtFunctions.binding(()=>{return this.leftMargin},updateList)
                self.y = QtFunctions.binding(()=>{return this.topMargin},updateList)
                self.width = QtFunctions.binding(()=>{return target.width - (this.leftMargin + this.rightMargin)},updateList)
                self.height = QtFunctions.binding(()=>{return target.height - (this.topMargin + this.bottomMargin)},updateList)
            } else {
                self.x = QtFunctions.binding(()=>{return target.x + this.leftMargin},updateList)
                self.y = QtFunctions.binding(()=>{return target.y + this.topMargin},updateList)
                self.width = QtFunctions.binding(()=>{return target.width - (this.leftMargin + this.rightMargin)},updateList)
                self.height = QtFunctions.binding(()=>{return target.height - (this.topMargin + this.bottomMargin)},updateList)
            }
            

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onCenterInChanged(){
        let target = this.centerIn.__get()
        let self = this.__parent

        if(self && target){
            let updateList = []

            if(self.parent === target){
                self.x = QtFunctions.binding(()=>{return target.width / 2 - self.width / 2},updateList)
                self.y = QtFunctions.binding(()=>{return target.height / 2 - self.height / 2},updateList)
            } else {
                self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 - self.width / 2},updateList)
                self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 - self.height / 2},updateList)
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onHorizontalCenterChanged(){
        let target = this.horizontalCenter.__get()
        let targetFloat = target.__float
        target = target.__parent

        let self = this.__parent

        if(self && target){
            let updateList = []

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return -self.width / 2},updateList)
                } else if(targetFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return target.width - self.width / 2},updateList)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return target.width / 2 - self.width / 2},updateList)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return target.x - self.width / 2},updateList)
                } else if(targetFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return target.x + target.width - self.width / 2},updateList)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 - self.width / 2},updateList)
                }
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onVerticalCenterChanged(){
        let target = this.verticalCenter.__get()
        let targetFloat = target.__float
        target = target.__parent

        let self = this.__parent

        if(self && target){
            let updateList = []

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return -self.height / 2},updateList)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return target.height - self.height / 2},updateList)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return target.height / 2 - self.height / 2},updateList)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return target.y - self.height / 2},updateList)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return target.y + target.height - self.height / 2},updateList)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 - self.height / 2},updateList)
                }               
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onLeftChanged(){
        let target = this.left.__get()
        let targetFloat = target.__float
        target = target.__parent

        let self = this.__parent

        if(target && this.right.__get()) {
            this.onLeftRightChanged()
            return
        }

        if(self && target){
            let updateList = []

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return this.leftMargin},updateList)
                } else if(targetFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return target.width + this.leftMargin},updateList)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return target.width / 2 + this.leftMargin},updateList)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return target.x + this.leftMargin},updateList)
                } else if(targetFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return target.x + target.width + this.leftMargin},updateList)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 + this.leftMargin},updateList)
                }            
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onRightChanged(){
        let target = this.right.__get()
        let targetFloat = target.__float
        target = target.__parent

        let self = this.__parent

        if(target && this.left.__get()) {
            this.onLeftRightChanged()
            return
        }

        if(self && target){
            let updateList = []

            if(self.parent === target){
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return -self.width - this.rightMargin},updateList)
                } else if(targetFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return target.width - self.width - this.rightMargin},updateList)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return target.width / 2 - self.width - this.rightMargin},updateList)
                }
            } else {
                if(targetFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return target.x - self.width - this.rightMargin},updateList)
                } else if(targetFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return target.x + target.width - self.width - this.rightMargin},updateList)
                } else if(targetFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return target.x + target.width / 2 - self.width - this.rightMargin},updateList)
                }              
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onLeftRightChanged(){
        let left = this.left.__get()
        let leftFloat = left.__float
        left = left.__parent

        let right = this.right.__get()
        let rightFloat = right.__float
        right = right.__parent

        let self = this.__parent

        if(self && left && right){
            let updateList = []

            if(self.parent === left){
                if(leftFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return this.leftMargin},updateList)
                } else if(leftFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return left.width + this.leftMargin},updateList)
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return left.width / 2 + this.leftMargin},updateList)
                }
            } else {
                if(leftFloat === AnchorLine.Left){
                    self.x = QtFunctions.binding(()=>{return left.x + this.leftMargin},updateList)
                } else if(leftFloat === AnchorLine.Right){
                    self.x = QtFunctions.binding(()=>{return left.x + left.width + this.leftMargin},updateList)
                } else if(leftFloat === AnchorLine.HorizontalCenter){
                    self.x = QtFunctions.binding(()=>{return left.x + left.width / 2 + this.leftMargin},updateList)
                }
            }

            if(self.parent === right){
                if(rightFloat === AnchorLine.Left){
                    self.width = QtFunctions.binding(()=>{return -this.rightMargin - self.x},updateList)
                } else if(rightFloat === AnchorLine.Right){
                    self.width = QtFunctions.binding(()=>{return right.width - this.rightMargin - self.x},updateList)
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.width = QtFunctions.binding(()=>{return right.width / 2 - this.rightMargin - self.x},updateList)
                }
            } else {
                if(rightFloat === AnchorLine.Left){
                    self.width = QtFunctions.binding(()=>{return right.x - this.rightMargin - self.x},updateList)
                } else if(rightFloat === AnchorLine.Right){
                    self.width = QtFunctions.binding(()=>{return right.x + right.width - this.rightMargin - self.x},updateList)
                } else if(rightFloat === AnchorLine.HorizontalCenter){
                    self.width = QtFunctions.binding(()=>{return right.x + right.width / 2 - this.rightMargin - self.x},updateList)
                }
            }

            

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onTopChanged(){
        let target = this.top.__get()
        let targetFloat = target.__float
        target = target.__parent

        let self = this.__parent

        if(target && this.bottom.__get()) {
            this.onTopBottomChanged()
            return
        }

        if(self && target){
            let updateList = []

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return this.topMargin},updateList)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return target.height + this.topMargin},updateList)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return target.height / 2 + this.topMargin},updateList)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return target.y + this.topMargin},updateList)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return target.y + target.height + this.topMargin},updateList)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 + this.topMargin},updateList)
                }               
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onBottomChanged(){
        let target = this.bottom.__get()
        let targetFloat = target.__float
        target = target.__parent

        let self = this.__parent

        if(target && this.top.__get()) {
            this.onTopBottomChanged()
            return
        }

        if(self && target){
            let updateList = []

            if(self.parent === target){
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return -self.height - this.bottomMargin},updateList)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return target.height - self.height - this.bottomMargin},updateList)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return target.height / 2 - self.height - this.bottomMargin},updateList)
                }
            } else {
                if(targetFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return target.y - self.height - this.bottomMargin},updateList)
                } else if(targetFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return target.y + target.height - self.height - this.bottomMargin},updateList)
                } else if(targetFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return target.y + target.height / 2 - self.height - this.bottomMargin},updateList)
                }               
            }

            for(let property of updateList){
                property.__update()
            }
        }
    }

    onTopBottomChanged(){
        let top = this.top.__get()
        let topFloat = top.__float
        top = top.__parent

        let bottom = this.bottom.__get()
        let bottomFloat = bottom.__float
        bottom = bottom.__parent

        let self = this.__parent

        if(self && top && bottom){
            let updateList = []

            if(self.parent === top){
                if(topFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return this.topMargin},updateList)
                } else if(topFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return top.height + this.topMargin},updateList)
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return top.height / 2 + this.topMargin},updateList)
                }
            } else {
                if(topFloat === AnchorLine.Top){
                    self.y = QtFunctions.binding(()=>{return top.y + this.topMargin},updateList)
                } else if(topFloat === AnchorLine.Bottom){
                    self.y = QtFunctions.binding(()=>{return top.y + top.height + this.topMargin},updateList)
                } else if(topFloat === AnchorLine.VerticalCenter){
                    self.y = QtFunctions.binding(()=>{return top.y + top.height / 2 + this.topMargin},updateList)
                }
            }

            if(self.parent === bottom){
                if(bottomFloat === AnchorLine.Top){
                    self.height = QtFunctions.binding(()=>{return -this.bottomMargin - self.y},updateList)
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.height = QtFunctions.binding(()=>{return bottom.height - this.bottomMargin - self.y},updateList)
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.height = QtFunctions.binding(()=>{return bottom.height / 2 - this.bottomMargin - self.y},updateList)
                }
            } else {
                if(bottomFloat === AnchorLine.Top){
                    self.height = QtFunctions.binding(()=>{return bottom.y - this.bottomMargin - self.y},updateList)
                } else if(bottomFloat === AnchorLine.Bottom){
                    self.height = QtFunctions.binding(()=>{return bottom.y + bottom.height - this.bottomMargin - self.y},updateList)
                } else if(bottomFloat === AnchorLine.VerticalCenter){
                    self.height = QtFunctions.binding(()=>{return bottom.y + bottom.height / 2 - this.bottomMargin - self.y},updateList)
                }
            }

            

            for(let property of updateList){
                property.__update()
            }
        }
    }
}

module.exports = Anchors