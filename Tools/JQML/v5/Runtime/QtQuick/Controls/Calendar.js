// The calendar of QtQuick.Controls (Qt 6.12, since 6.3): MonthGrid (T.AbstractMonthGrid), DayOfWeekRow
// (T.AbstractDayOfWeekRow), WeekNumberColumn (T.AbstractWeekNumberColumn), CalendarModel and the Calendar singleton of the
// months. Checked against Qt 6.12 (tests/diff/cases/calendar.qml):
//  - MonthGrid.source: the 42 days of six weeks around month / year, from the first day of the week of the locale, each
//    with date, day, today, weekNumber, month, year; title: the month and the year; pressed / pressAndHold (date) of a
//    press on a day, released / clicked of that day on the release, wherever it is;
//  - DayOfWeekRow.source: the 7 days from the first one of the locale (day, longName, shortName, narrowName);
//  - WeekNumberColumn.source: the 6 ISO week numbers of the rows;
//  - CalendarModel: a month for each month from from to to (month, year), monthAt / yearAt / indexOf.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { QtObject } = require('../../Core/QtObject')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { ListModel } = require('../../QtQml/Models/ListModel')

const localeOf = (control) => control.$v[Control.LOCALE] || require('../../Qt/Locale').locale()
// ISO 8601 week number (QDate::weekNumber)
function weekNumber(date){
    let d = new Date(Date.UTC(date.getFullYear(), date.getMonth(), date.getDate()))
    let day = d.getUTCDay() || 7
    d.setUTCDate(d.getUTCDate() + 4 - day)
    let yearStart = new Date(Date.UTC(d.getUTCFullYear(), 0, 1))
    return Math.ceil(((d - yearStart) / 86400000 + 1) / 7)
}
// the first day shown for a month: the first day of the week before its 1st, never the 1st itself (QQuickMonthModelPrivate::
// populate: some days of the month before are always shown)
function firstShown(year, month, firstDay){
    let first = new Date(year, month, 1)
    let weekday = first.getDay() || 7       // Qt: Monday 1 .. Sunday 7
    let back = (weekday - firstDay + 7) % 7 || 7
    return new Date(year, month, 1 - back)
}
// the positioner of the content lays its new delegates out at once (the Grid of Qt is complete after its Repeater)
function layoutContent(control){
    let content = control.$v[Control.CONTENTITEM]
    if(content && typeof content.forceLayout === 'function') content.forceLayout()
}
// a model of rows (a ListModel the delegates read roles from)
function rowsModel(owner, rows){
    let model = owner.$sourceModel
    if(!model){
        model = owner.$sourceModel = new ListModel(null)
    }
    model.clear()
    for(let row of rows) model.append(row)
    return model
}

// ---- MonthGrid (QQuickMonthGrid) ----
class AbstractMonthGrid extends Control {
    constructor(parent = null){
        super(parent)
        let now = new Date()
        this.$plain(AbstractMonthGrid.MONTH, now.getMonth())
        this.$plain(AbstractMonthGrid.YEAR, now.getFullYear())
    }
    $days(){
        let month = this.$v[AbstractMonthGrid.MONTH], year = this.$v[AbstractMonthGrid.YEAR]
        let locale = localeOf(this)
        let start = firstShown(year, month, locale.firstDayOfWeek)
        let today = new Date()
        let rows = []
        for(let i = 0; i < 42; i++){
            let date = new Date(start.getFullYear(), start.getMonth(), start.getDate() + i)
            rows.push({ date, day: date.getDate(), today: date.toDateString() === today.toDateString(), weekNumber: weekNumber(date),
                month: date.getMonth(), year: date.getFullYear() })
        }
        return rows
    }
    $update(){
        let month = this.$v[AbstractMonthGrid.MONTH], year = this.$v[AbstractMonthGrid.YEAR]
        this.$plain(AbstractMonthGrid.SOURCE, rowsModel(this, this.$days()))
        let locale = localeOf(this)
        let title = this.$titleSet ? this.$v[AbstractMonthGrid.TITLE] : locale.standaloneMonthName(month + 1) + ' ' + year
        this.$plain(AbstractMonthGrid.TITLE, title)
    }
    $componentComplete(){
        super.$componentComplete()
        this.$update()
        layoutContent(this)
    }
    // the day under a point: the delegate of the content (a Grid of 42)
    $dateAt(x, y){
        let content = this.$v[Control.CONTENTITEM]
        if(!content) return null
        let { Repeater } = require('../Repeater')
        let days = (content.$children || []).filter(c => c.$isItem && !(c instanceof Repeater))
        let rows = this.$days()
        for(let i = 0; i < days.length && i < rows.length; i++){
            let d = days[i]
            let px = content.$v[Item.X] + d.$v[Item.X], py = content.$v[Item.Y] + d.$v[Item.Y]
            if(x >= px && y >= py && x < px + d.$v[Item.WIDTH] && y < py + d.$v[Item.HEIGHT]) return rows[i].date
        }
        return null
    }
    $mousePress(x, y, button){
        if(button !== 1) return false
        let date = this.$dateAt(x, y)
        if(!date) return false
        this.$pressedDate = date
        engine.emitArgs(this, AbstractMonthGrid.SIGNAL_pressed, [date])
        this.$hold = setTimeout(() => { if(this.$pressedDate) engine.batch(() => engine.emitArgs(this, AbstractMonthGrid.SIGNAL_pressAndHold, [date])) }, 800)
        return true
    }
    // QQuickMonthGridPrivate::handleRelease: released and clicked of the pressed day, wherever the release is (checked)
    $mouseRelease(){
        clearTimeout(this.$hold)
        let date = this.$pressedDate
        this.$pressedDate = null
        if(!date) return
        engine.emitArgs(this, AbstractMonthGrid.SIGNAL_released, [date])
        engine.emitArgs(this, AbstractMonthGrid.SIGNAL_clicked, [date])
    }
    $mouseMove(){}
    $ungrabMouse(){ clearTimeout(this.$hold); this.$pressedDate = null }
}
defineType(AbstractMonthGrid, {
    props: { month: 'int', year: 'int', source: { kind: 'var', initial: null }, title: 'string', delegate: { kind: 'component', initial: null } },
    signals: ['pressed(date)', 'released(date)', 'clicked(date)', 'pressAndHold(date)'],
})
AbstractMonthGrid.$setters = withSetters(Control, [
    [AbstractMonthGrid.MONTH, (g, v) => { g.$plain(AbstractMonthGrid.MONTH, v); if(g.$f & engine.COMPLETED) g.$update() }],
    [AbstractMonthGrid.YEAR, (g, v) => { g.$plain(AbstractMonthGrid.YEAR, v); if(g.$f & engine.COMPLETED) g.$update() }],
    [AbstractMonthGrid.TITLE, (g, v) => { g.$titleSet = v !== undefined; if(v !== undefined) g.$plain(AbstractMonthGrid.TITLE, v); else g.$update() }],
    [Control.LOCALE, (g, v) => { g.$plain(Control.LOCALE, v); if(g.$f & engine.COMPLETED) g.$update() }],
])

// ---- DayOfWeekRow (QQuickDayOfWeekRow) ----
class AbstractDayOfWeekRow extends Control {
    constructor(parent = null){
        super(parent)
    }
    $update(){
        let locale = localeOf(this)
        let first = locale.firstDayOfWeek
        let rows = []
        for(let i = 0; i < 7; i++){
            let day = ((first - 1 + i) % 7) + 1
            rows.push({ day, longName: locale.standaloneDayName(day, 0), shortName: locale.standaloneDayName(day, 1), narrowName: locale.standaloneDayName(day, 2) })
        }
        this.$plain(AbstractDayOfWeekRow.SOURCE, rowsModel(this, rows))
    }
    $componentComplete(){
        super.$componentComplete()
        this.$update()
        layoutContent(this)
    }
}
defineType(AbstractDayOfWeekRow, { props: { source: { kind: 'var', initial: null }, delegate: { kind: 'component', initial: null } } })
AbstractDayOfWeekRow.$setters = withSetters(Control, [[Control.LOCALE, (g, v) => { g.$plain(Control.LOCALE, v); if(g.$f & engine.COMPLETED) g.$update() }]])

// ---- WeekNumberColumn (QQuickWeekNumberColumn) ----
class AbstractWeekNumberColumn extends Control {
    constructor(parent = null){
        super(parent)
        let now = new Date()
        this.$plain(AbstractWeekNumberColumn.MONTH, now.getMonth())
        this.$plain(AbstractWeekNumberColumn.YEAR, now.getFullYear())
    }
    $update(){
        let month = this.$v[AbstractWeekNumberColumn.MONTH], year = this.$v[AbstractWeekNumberColumn.YEAR]
        let start = firstShown(year, month, localeOf(this).firstDayOfWeek)
        let rows = []
        for(let w = 0; w < 6; w++) rows.push({ weekNumber: weekNumber(new Date(start.getFullYear(), start.getMonth(), start.getDate() + w * 7)) })
        this.$plain(AbstractWeekNumberColumn.SOURCE, rowsModel(this, rows))
    }
    $componentComplete(){
        super.$componentComplete()
        this.$update()
        layoutContent(this)
    }
}
defineType(AbstractWeekNumberColumn, { props: { month: 'int', year: 'int', source: { kind: 'var', initial: null }, delegate: { kind: 'component', initial: null } } })
AbstractWeekNumberColumn.$setters = withSetters(Control, [
    [AbstractWeekNumberColumn.MONTH, (g, v) => { g.$plain(AbstractWeekNumberColumn.MONTH, v); if(g.$f & engine.COMPLETED) g.$update() }],
    [AbstractWeekNumberColumn.YEAR, (g, v) => { g.$plain(AbstractWeekNumberColumn.YEAR, v); if(g.$f & engine.COMPLETED) g.$update() }],
    [Control.LOCALE, (g, v) => { g.$plain(Control.LOCALE, v); if(g.$f & engine.COMPLETED) g.$update() }],
])

// ---- CalendarModel (QQuickCalendarModel): a month for each month from from to to ----
class CalendarModel extends ListModel {
    constructor(parent = null){
        super(parent)
        QtObject.prototype.$write.call(this, CalendarModel.FROM, new Date(1, 0, 1))
        QtObject.prototype.$write.call(this, CalendarModel.TO, new Date(275759, 9, 25))
        engine.scheduleComponentComplete(this)
    }
    $componentComplete(){
        super.$componentComplete && super.$componentComplete()
        this.$fill()
    }
    $range(){
        let from = this.$v[CalendarModel.FROM], to = this.$v[CalendarModel.TO]
        return { y0: from.getFullYear(), m0: from.getMonth(), count: Math.max(0, (to.getFullYear() - from.getFullYear()) * 12 + to.getMonth() - from.getMonth() + 1) }
    }
    // the months as rows (made on demand: a model of 3 million months stays small)
    $fill(){
        let { count } = this.$range()
        this.$rows = new Proxy([], { get: (target, key) => {
            if(key === 'length') return count
            let i = Number(key)
            if(Number.isInteger(i) && i >= 0 && i < count) return { month: this.monthAt(i), year: this.yearAt(i) }
            return target[key]
        } })
        this.$write(ListModel.COUNT, count)
    }
    monthAt(index){ let r = this.$range(); return (r.m0 + index) % 12 }
    yearAt(index){ let r = this.$range(); return r.y0 + Math.floor((r.m0 + index) / 12) }
    indexOf(a, b){
        let r = this.$range()
        let year = a instanceof Date ? a.getFullYear() : a, month = a instanceof Date ? a.getMonth() : b
        let index = (year - r.y0) * 12 + month - r.m0
        return index >= 0 && index < r.count ? index : -1
    }
    $write(id, value){
        QtObject.prototype.$write.call(this, id, value)
        if((id === CalendarModel.FROM || id === CalendarModel.TO) && this.$f & engine.COMPLETED) this.$fill()
    }
}
defineType(CalendarModel, { props: { from: 'var', to: 'var' } })

// ---- Calendar: the months (a singleton of enums) ----
class Calendar extends QtObject {}
defineType(Calendar, {})
Calendar.$singleton = true
const MONTHS = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December']
MONTHS.forEach((name, i) => { Calendar[name] = i; Calendar.prototype[name] = i })

module.exports = { AbstractMonthGrid, AbstractDayOfWeekRow, AbstractWeekNumberColumn, CalendarModel, Calendar }
