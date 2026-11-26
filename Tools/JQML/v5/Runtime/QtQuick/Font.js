const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// the text of the font is laid out again: its owner's, and of the texts that show it (`font: control.font` gives a Text the
// font of the control itself)
const ownerText = (group) => {
    let owner = group.$owner
    if(owner && owner.$updateImplicitSize) owner.$updateImplicitSize()
    if(group.$users) for(let user of group.$users) if(user !== owner && user.$updateImplicitSize) user.$updateImplicitSize()
}

const POINTS_PER_PIXEL = 72 / 96

// QQuickFontValueType: pixelSize and pointSize both read as a size, from whichever was set last
class Font extends GroupObject {
    $info(){
        let v = this.$v
        let weight = v[Font.WEIGHT]
        if(v[Font.BOLD] && weight < 700) weight = 700
        return {
            pixelSize: v[Font.PIXELSIZE],
            family: v[Font.FAMILY],
            weight,
            italic: v[Font.ITALIC],
            underline: v[Font.UNDERLINE],
            strikeout: v[Font.STRIKEOUT],
            capitalization: v[Font.CAPITALIZATION],
            letterSpacing: v[Font.LETTERSPACING],
            wordSpacing: v[Font.WORDSPACING],
            // the size the font was given in (QFont keeps a point size or a pixel size, the one set last)
            pointSize: v[Font.POINTSIZE],
            sizeUnit: this.$sizeUnit || 'pt',
        }
    }

    $write(id, value){
        if(id === Font.PIXELSIZE && value > 0) this.$sizeUnit = 'px'
        else if(id === Font.POINTSIZE && value > 0) this.$sizeUnit = 'pt'
        if(value > 0){
            if(id === Font.PIXELSIZE) this.$v[Font.POINTSIZE] = value * POINTS_PER_PIXEL
            if(id === Font.POINTSIZE) this.$v[Font.PIXELSIZE] = Math.round(value / POINTS_PER_PIXEL)
        }
        super.$write(id, value)
        // QFont::setBold: the weight Bold or Normal; QFont::bold: a weight above Medium
        if(id === Font.BOLD){
            let weight = this.$v[Font.WEIGHT]
            if(value ? weight <= 500 : weight > 500) this.$write(Font.WEIGHT, value ? 700 : 400)
        } else if(id === Font.WEIGHT){
            if(this.$v[Font.BOLD] !== value > 500) this.$write(Font.BOLD, value > 500)
        }
    }
}
defineType(Font, {
    props: {
        pixelSize: { kind: 'int', initial: 12, effect: ownerText },
        pointSize: { kind: 'real', initial: 9, effect: ownerText },
        family: { kind: 'string', initial: 'Segoe UI', effect: ownerText },
        bold: { kind: 'bool', effect: ownerText },
        weight: { kind: 'int', initial: 400, effect: ownerText },
        italic: { kind: 'bool', effect: ownerText },
        underline: { kind: 'bool', effect: ownerText },
        strikeout: { kind: 'bool', effect: ownerText },
        capitalization: { kind: 'int', effect: ownerText },
        letterSpacing: { kind: 'real', effect: ownerText },
        wordSpacing: { kind: 'real', effect: ownerText },
        hintingPreference: 'int',
        kerning: { kind: 'bool', initial: true },
        preferShaping: { kind: 'bool', initial: true },
        styleName: 'string',
    },
})
Object.assign(Font, {
    MixedCase: 0, AllUppercase: 1, AllLowercase: 2, SmallCaps: 3, Capitalize: 4,
    Thin: 100, ExtraLight: 200, Light: 300, Normal: 400, Medium: 500, DemiBold: 600, Bold: 700, ExtraBold: 800, Black: 900,
})

module.exports = { Font }
