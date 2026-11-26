// Run: node --test tests/unit
// TextEdit with RichText as QQuickTextEdit with a QTextDocument (qtbase / qtdeclarative 6.12): the HTML read into a
// document and written back (QTextHtmlParser, QTextHtmlImporter, QTextHtmlExporter), the plain text and positions of
// the document (block and line separators, objects, table cells), fragments (getFormattedText), the edits of the
// methods and of the keys, undo and redo, textFormat and the layout's lines and positions.
// The expected values are what Qt 6.12 gives for the same calls (qml -platform offscreen, TextEdit with font.family
// Arial and font.pixelSize 12). Without a DOM the text is measured at half the pixel size per character.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')
const RT = require('../../Runtime/QtQuick/TextDocument/RichText')
const F = require('../../Runtime/QtQuick/TextDocument/Formats')
const L = require('../../Runtime/QtQuick/TextDocument/Layout')

const FONT = { family: 'Arial', pixelSize: 12, pointSize: 9, sizeUnit: 'px', weight: 400 }
const HEAD = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">\n<html><head><meta name="qrichtext" content="1" /><meta charset="utf-8" /><style type="text/css">\np, li { white-space: pre-wrap; }\nhr { height: 1px; border-width: 0; }\nli.unchecked::marker { content: "\\2610"; }\nli.checked::marker { content: "\\2612"; }\n</style></head>'

const body = (html) => html.slice(html.indexOf('<body'))

const QT_CASES = [
    { html: "<b>b</b><i>i</i><u>u</u><s>s</s>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">b</span><span style=\" font-style:italic;\">i</span><span style=\" text-decoration: underline;\">u</span><span style=\" text-decoration: line-through;\">s</span></p></body></html>",
      raw: "bius",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-weight:700;\">b</span><span style=\" font-style:italic;\">i</span><span style=\" text-decoration: underline;\">u</span><span style=\" text-decoration: line-through;\">s</span><!--EndFragment--></p></body></html>" },
    { html: "<font color=\"red\" size=\"5\" face=\"Courier\">f</font>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Courier'; font-size:x-large; color:#ff0000;\">f</span></p></body></html>",
      raw: "f",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-family:'Courier'; font-size:x-large; color:#ff0000;\">f</span><!--EndFragment--></p></body></html>" },
    { html: "<span style=\"color:#123456; background-color:yellow; font-size:20px; font-family:Times\">x</span>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Times'; font-size:20px; color:#123456; background-color:#ffff00;\">x</span></p></body></html>",
      raw: "x",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-family:'Times'; font-size:20px; color:#123456; background-color:#ffff00;\">x</span><!--EndFragment--></p></body></html>" },
    { html: "<a href=\"http://x.y\">link</a>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://x.y\"><span style=\" text-decoration: underline; color:#0000ff;\">link</span></a></p></body></html>",
      raw: "link",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><a href=\"http://x.y\"><span style=\" text-decoration: underline; color:#0000ff;\">link</span></a><!--EndFragment--></p></body></html>" },
    { html: "a<sub>2</sub>b<sup>3</sup>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a<span style=\" vertical-align:sub;\">2</span>b<span style=\" vertical-align:super;\">3</span></p></body></html>",
      raw: "a2b3",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->a<span style=\" vertical-align:sub;\">2</span>b<span style=\" vertical-align:super;\">3</span><!--EndFragment--></p></body></html>" },
    { html: "<h1>H1</h1><h2>H2</h2><h3>H3</h3>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">H1</span></h1>\n<h2 style=\" margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:700;\">H2</span></h2>\n<h3 style=\" margin-top:14px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:large; font-weight:700;\">H3</span></h3></body></html>",
      raw: "H1\u2029H2\u2029H3",
      fragment: "<body>\n<h1 style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-size:xx-large; font-weight:700;\">H1</span></h1>\n<h2 style=\" margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:700;\">H2</span></h2>\n<h3 style=\" margin-top:14px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:large; font-weight:700;\">H3</span><!--EndFragment--></h3></body></html>" },
    { html: "<ul><li>one</li><li>two</li></ul>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one</li>\n<li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two</li></ul></body></html>",
      raw: "one\u2029two",
      fragment: "<body>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->one</li>\n<li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two<!--EndFragment--></li></ul></body></html>" },
    { html: "<pre>  a  b\nc</pre>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<pre style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">  a  b</span></pre>\n<pre style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">c</span></pre></body></html>",
      raw: "  a  b\u2029c",
      fragment: "<body>\n<pre style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-family:'monospace';\">  a  b</span></pre>\n<pre style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">c</span><!--EndFragment--></pre></body></html>" },
    { html: "<p align=\"center\">c</p><p style=\"text-align:right\">r</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c</p>\n<p align=\"right\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">r</p></body></html>",
      raw: "c\u2029r",
      fragment: "<body>\n<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->c</p>\n<p align=\"right\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">r<!--EndFragment--></p></body></html>" },
    { html: "<blockquote>q</blockquote>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">q</p></body></html>",
      raw: "q",
      fragment: "<body>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->q<!--EndFragment--></p></body></html>" },
    { html: "<p>a</p><hr/><p>b</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</p>\n<hr />\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</p></body></html>",
      raw: "a\u2029\u2029b",
      fragment: "<body>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->a</p>\n<hr />\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b<!--EndFragment--></p></body></html>" },
    { html: "x<img src=\"a.png\" width=\"10\" height=\"10\">y",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x<img src=\"a.png\" width=\"10\" height=\"10\" />y</p></body></html>",
      raw: "x\ufffcy",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->x<img src=\"a.png\" width=\"10\" height=\"10\" />y<!--EndFragment--></p></body></html>" },
    { html: "a&nbsp;b &lt;i&gt; &#65; &#x42; &quot;q&quot;",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a\u00a0b &lt;i&gt; A B &quot;q&quot;</p></body></html>",
      raw: "a\u00a0b <i> A B \"q\"",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->a\u00a0b &lt;i&gt; A B &quot;q&quot;<!--EndFragment--></p></body></html>" },
    { html: "  lead  and  trail  ",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">lead and trail </p></body></html>",
      raw: "lead and trail ",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->lead and trail <!--EndFragment--></p></body></html>" },
    { html: "<p>a<br>b</p><p>c</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a<br />b</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c</p></body></html>",
      raw: "a\u2028b\u2029c",
      fragment: "<body>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->a<br />b</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c<!--EndFragment--></p></body></html>" },
    { html: "<ul><li>one<ul><li>nested</li></ul></li><li>two</li></ul>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 2;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">nested</li></ul></li>\n<li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two</li></ul></body></html>",
      raw: "one\u2029nested\u2029two",
      fragment: "<body>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->one\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 2;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">nested</li></ul></li>\n<li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two<!--EndFragment--></li></ul></body></html>" },
    { html: "<b>Hi</b> <span style=\"background-color:#ff0000\">a b</span> c<br/>d &amp; <i>e</i>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">Hi</span> <span style=\" background-color:#ff0000;\">a b</span> c<br />d &amp; <span style=\" font-style:italic;\">e</span></p></body></html>",
      raw: "Hi a b c\u2028d & e",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-weight:700;\">Hi</span> <span style=\" background-color:#ff0000;\">a b</span> c<br />d &amp; <span style=\" font-style:italic;\">e</span><!--EndFragment--></p></body></html>" },
    { html: "<style>.red{color:red} p b{font-size:30px}</style><p class=\"red\">r <b>big</b></p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#ff0000;\">r </span><span style=\" font-size:30px; font-weight:700; color:#ff0000;\">big</span></p></body></html>",
      raw: "r big",
      fragment: "<body>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" color:#ff0000;\">r </span><span style=\" font-size:30px; font-weight:700; color:#ff0000;\">big</span><!--EndFragment--></p></body></html>" },
    { html: "<span style=\"color:rgba(10,20,30,0.5)\">t</span><span style=\"background:transparent\">u</span>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:rgba(10,20,30,0.498039);\">t</span><span style=\" background-color:transparent;\">u</span></p></body></html>",
      raw: "tu",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" color:rgba(10,20,30,0.498039);\">t</span><span style=\" background-color:transparent;\">u</span><!--EndFragment--></p></body></html>" },
    { html: "<p style=\"margin:5px 6px; text-indent:3px; line-height:150%\">m</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:5px; margin-bottom:5px; margin-left:6px; margin-right:6px; -qt-block-indent:0; text-indent:3px; line-height:150%;\">m</p></body></html>",
      raw: "m",
      fragment: "<body>\n<p style=\" margin-top:5px; margin-bottom:5px; margin-left:6px; margin-right:6px; -qt-block-indent:0; text-indent:3px; line-height:150%;\"><!--StartFragment-->m<!--EndFragment--></p></body></html>" },
    { html: "<font size=\"+1\">p1</font><big>big</big><small>small</small>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:large;\">p1big</span><span style=\" font-size:small;\">small</span></p></body></html>",
      raw: "p1bigsmall",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-size:large;\">p1big</span><span style=\" font-size:small;\">small</span><!--EndFragment--></p></body></html>" },
    { html: "<span title=\"tip\">t</span><a name=\"n\">x</a>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span title=\"tip\">t</span><a name=\"n\"></a>x</p></body></html>",
      raw: "tx",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span title=\"tip\">t</span><a name=\"n\"></a>x<!--EndFragment--></p></body></html>" },
    { html: "<ol type=\"a\" start=\"3\"><li>x</li></ol><ul type=\"square\"><li>y</li></ul>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol type=\"a\" start=\"3\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</li></ol>\n<ul type=\"square\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">y</li></ul></body></html>",
      raw: "x\u2029y",
      fragment: "<body>\n<ol type=\"a\" start=\"3\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->x</li></ol>\n<ul type=\"square\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">y<!--EndFragment--></li></ul></body></html>" },
    { html: "<span style=\"font: italic bold 14px Arial, Helvetica\">f</span>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Arial','Helvetica'; font-size:14px; font-weight:700; font-style:italic;\">f</span></p></body></html>",
      raw: "f",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment--><span style=\" font-family:'Arial','Helvetica'; font-size:14px; font-weight:700; font-style:italic;\">f</span><!--EndFragment--></p></body></html>" },
    { html: "<p style=\"-qt-paragraph-type:empty\"></p><p>x</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</p></body></html>",
      raw: "\u2029x",
      fragment: "<body>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x<!--EndFragment--></p></body></html>" },
    { html: "<p style=\"background-color:#00ff00\">bg</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#00ff00;\"><span style=\" background-color:#00ff00;\">bg</span></p></body></html>",
      raw: "bg",
      fragment: "<body>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background-color:#00ff00;\"><!--StartFragment--><span style=\" background-color:#00ff00;\">bg</span><!--EndFragment--></p></body></html>" },
    { html: "<table><tr><td>a</td><td>b</td></tr></table>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</p></td></tr></table></body></html>",
      raw: "\ufdd0a\ufdd0b\ufdd1",
      fragment: "<body>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</p></td></tr></table></body></html>" },
    { html: "x<table border=\"1\" cellspacing=\"0\" cellpadding=\"3\" width=\"80%\"><tr><td colspan=\"2\">wide</td></tr><tr><td bgcolor=\"yellow\">c</td><td>d</td></tr></table>y",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</p>\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" width=\"80%\" cellspacing=\"0\" cellpadding=\"3\">\n<tr>\n<td colspan=\"2\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">wide</p></td></tr>\n<tr>\n<td bgcolor=\"#ffff00\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" background-color:#ffff00;\">c</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">d</p></td></tr></table>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">y</p></body></html>",
      raw: "x\ufdd0wide\ufdd0c\ufdd0d\ufdd1y",
      fragment: "<body>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->x</p>\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" width=\"80%\" cellspacing=\"0\" cellpadding=\"3\">\n<tr>\n<td colspan=\"2\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">wide</p></td></tr>\n<tr>\n<td bgcolor=\"#ffff00\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" background-color:#ffff00;\">c</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">d</p></td></tr></table>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">y<!--EndFragment--></p></body></html>" },
    { html: "<table><tr><td rowspan=\"2\">r</td><td>1</td></tr><tr><td>2</td></tr></table>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td rowspan=\"2\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">r</p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1</p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2</p></td></tr></table></body></html>",
      raw: "\ufdd0r\ufdd01\ufdd02\ufdd1",
      fragment: "<body>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td rowspan=\"2\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">r</p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1</p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2</p></td></tr></table></body></html>" },
    { html: "<p>before</p><table><tr><td><p>p1</p><p>p2</p></td><td></td></tr></table><p>after</p>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">before</p>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">p1</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">p2</p></td>\n<td></td></tr></table>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">after</p></body></html>",
      raw: "before\ufdd0p1\u2029p2\ufdd0\ufdd1after",
      fragment: "<body>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><!--StartFragment-->before</p>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">p1</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">p2</p></td>\n<td></td></tr></table>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">after<!--EndFragment--></p></body></html>" },
    { html: "<table><tr><td><table><tr><td>in</td></tr></table></td></tr></table>",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">in</p></td></tr></table></td></tr></table></body></html>",
      raw: "\ufdd0\ufdd0in\ufdd1\ufdd1",
      fragment: "<body>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">in</p></td></tr></table></td></tr></table></body></html>" },
]

const QT_EDITS = [
    { html: "<b>bold</b> plain", steps: [
        { op: ["insert",2,"X"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">bo</span>X<span style=\" font-weight:700;\">ld</span> plain</p></body></html>", raw: "boXld plain", cursor: 0 },
        { op: ["insert",0,"<i>I</i>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">I</span><span style=\" font-weight:700;\">bo</span>X<span style=\" font-weight:700;\">ld</span> plain</p></body></html>", raw: "IboXld plain", cursor: 1 },
        { op: ["insert",99,"Z"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">I</span><span style=\" font-weight:700;\">bo</span>X<span style=\" font-weight:700;\">ld</span> plain</p></body></html>", raw: "IboXld plain", cursor: 1 },
        { op: ["insert",5,"Y<br>W"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">I</span><span style=\" font-weight:700;\">bo</span>X<span style=\" font-weight:700;\">l</span>Y<br />W<span style=\" font-weight:700;\">d</span> plain</p></body></html>", raw: "IboXlY\u2028Wd plain", cursor: 1 },
    ] },
    { html: "<p align=\"center\">aa</p><p align=\"right\">bb</p>", steps: [
        { op: ["remove",1,4], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">ab</p></body></html>", raw: "ab", cursor: 0 },
    ] },
    { html: "<p align=\"center\">aa</p><p align=\"right\">bb</p>", steps: [
        { op: ["remove",2,3], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aabb</p></body></html>", raw: "aabb", cursor: 0 },
    ] },
    { html: "<p align=\"center\"></p><p align=\"right\">bb</p>", steps: [
        { op: ["remove",0,1], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p align=\"right\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</p></body></html>", raw: "b", cursor: 0 },
    ] },
    { html: "<p>one</p>", steps: [
        { op: ["append","<i>two</i>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">two</span></p></body></html>", raw: "one\u2029two", cursor: 0 },
        { op: ["append","three"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">two</span></p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">three</p></body></html>", raw: "one\u2029two\u2029three", cursor: 0 },
        { op: ["append","<ul><li>x</li></ul>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one</p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">two</span></p>\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">three</p>\n<p style=\"-qt-paragraph-type:empty; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</li></ul></body></html>", raw: "one\u2029two\u2029three\u2029\u2029x", cursor: 0 },
    ] },
    { html: "<ul><li>a</li><li>b</li></ul>", steps: [
        { op: ["insert",1,"<p>P</p>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">aP</li>\n<li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul></body></html>", raw: "aP\u2029b", cursor: 0 },
        { op: ["remove",0,2], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\"-qt-paragraph-type:empty; margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></li>\n<li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul></body></html>", raw: "\u2029b", cursor: 0 },
    ] },
    { html: "<b>ab</b>cd", steps: [
        { op: ["remove",1,3], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">a</span>d</p></body></html>", raw: "ad", cursor: 0 },
        { op: ["insert",1,"q"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">a</span>qd</p></body></html>", raw: "aqd", cursor: 0 },
    ] },
    { html: "", steps: [
        { op: ["insert",0,"<span style=\"color:red\">r</span>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#ff0000;\">r</span></p></body></html>", raw: "r", cursor: 1 },
        { op: ["insert",1,"s"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#ff0000;\">r</span>s</p></body></html>", raw: "rs", cursor: 2 },
    ] },
    { html: "<h1>Head</h1><p>para</p>", steps: [
        { op: ["remove",4,5], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">Head</span>para</h1></body></html>", raw: "Headpara", cursor: 0 },
        { op: ["insert",2,"<b>B</b>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">He</span><span style=\" font-weight:700;\">B</span><span style=\" font-size:xx-large; font-weight:700;\">ad</span>para</h1></body></html>", raw: "HeBadpara", cursor: 0 },
    ] },
    { html: "<p>a</p><p>b</p><p>c</p>", steps: [
        { op: ["remove",1,4], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">ac</p></body></html>", raw: "ac", cursor: 0 },
    ] },
    { html: "plain", steps: [
        { op: ["clear"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", raw: "", cursor: 0 },
        { op: ["insert",0,"x"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</p></body></html>", raw: "x", cursor: 1 },
    ] },
    { html: "<a href=\"u\">link</a> after", steps: [
        { op: ["insert",4,"Q"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"u\"><span style=\" text-decoration: underline; color:#0000ff;\">link</span></a>Q after</p></body></html>", raw: "linkQ after", cursor: 0 },
        { op: ["insert",2,"<a href=\"v\">L</a>"], body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"u\"><span style=\" text-decoration: underline; color:#0000ff;\">li</span></a><a href=\"v\"><span style=\" text-decoration: underline; color:#0000ff;\">L</span></a><a href=\"u\"><span style=\" text-decoration: underline; color:#0000ff;\">nk</span></a>Q after</p></body></html>", raw: "liLnkQ after", cursor: 0 },
    ] },
]

// ---- the document ----

test('HTML into a document and back as Qt writes it (toHtml)', () => {
    for(let c of QT_CASES){
        let doc = RT.createDocument()
        RT.setDocumentHtml(doc, c.html)
        let html = RT.toHtml(doc, F.fontFormat(FONT))
        assert.ok(html.startsWith(HEAD), c.html)
        assert.equal(body(html), c.body, c.html)
    }
})

test('the plain text of a document: block and line separators, no-break spaces, objects, table cells', () => {
    for(let c of QT_CASES){
        let doc = RT.createDocument()
        RT.setDocumentHtml(doc, c.html)
        assert.equal(doc.rawText(), c.raw, c.html)
        assert.equal(doc.length(), c.raw.length, c.html)
    }
})

test('getFormattedText of the whole document: a fragment with its markers (QTextDocumentFragment::toHtml)', () => {
    for(let c of QT_CASES){
        let doc = RT.createDocument()
        RT.setDocumentHtml(doc, c.html)
        assert.equal(body(RT.formattedText(doc, 0, doc.length())), c.fragment, c.html)
    }
})

test('insert of HTML, remove, append and clear as QQuickTextEdit edits the document', () => {
    for(let c of QT_EDITS){
        let doc = RT.createDocument()
        RT.setDocumentHtml(doc, c.html)
        let cursor = 0
        for(let step of c.steps){
            let [op, a, b] = step.op
            let before = doc.length()
            if(op === 'insert'){
                if(a >= 0 && a < doc.characterCount()){
                    RT.insertHtml(doc, a, b)
                    if(cursor >= a) cursor += doc.length() - before
                }
            } else if(op === 'remove'){
                let removed = RT.removeRange(doc, a, b)
                if(cursor > removed.start) cursor = cursor <= removed.end ? removed.start : cursor - (removed.end - removed.start)
            } else if(op === 'append') RT.append(doc, a, true)
            else if(op === 'clear'){
                RT.removeRange(doc, 0, doc.length())
                cursor = 0
            }
            assert.equal(body(RT.toHtml(doc, F.fontFormat(FONT))), step.body, c.html + ' ' + JSON.stringify(step.op))
            assert.equal(doc.rawText(), step.raw, c.html + ' ' + JSON.stringify(step.op))
            assert.equal(cursor, step.cursor, c.html + ' ' + JSON.stringify(step.op))
        }
    }
})

test('mightBeRichText: the first tag of the first line is an element Qt knows', () => {
    assert.equal(RT.mightBeRichText('<b>x</b>'), true)
    assert.equal(RT.mightBeRichText('  <!DOCTYPE html>'), true)
    assert.equal(RT.mightBeRichText('a &lt; b'), true)
    assert.equal(RT.mightBeRichText('<no name>'), false)
    assert.equal(RT.mightBeRichText('text\n<b>x</b>'), false)
    assert.equal(RT.mightBeRichText('plain'), false)
})

test('the layout: lines broken at the width, a line separator ends a line, positions and x', () => {
    let { measure } = require('../../Runtime/QtQuick/TextLayout')
    let doc = RT.createDocument()
    RT.setDocumentHtml(doc, 'aaaa bbbb cccc<br/>dd')
    let options = { font: FONT, width: 60, wrapMode: L.WRAP, alignment: 1, margin: 0, tabStop: 80, measure }
    let layouter = new L.Layouter(doc, options)
    let layout = layouter.layout()
    // 6 px a character: "aaaa bbbb " fits 60, "cccc" goes on, the break ends the line
    assert.deepEqual(layout.blocks[0].lines.map(l => [l.start, l.end]), [[0, 10], [10, 15], [15, 17]])
    assert.equal(layout.lineCount, 3)
    let { line } = L.lineOf(layout, 12)
    assert.equal(line.start, 10)
    assert.equal(L.cursorToX(layouter, line, 12), 12)
    assert.equal(L.xToCursor(layouter, line, 13), 12)
    assert.equal(L.xToCursor(layouter, line, 16), 13)
})

test('the layout of a list item and a table', () => {
    let { measure } = require('../../Runtime/QtQuick/TextLayout')
    let doc = RT.createDocument()
    RT.setDocumentHtml(doc, '<ol><li>one</li></ol><table cellspacing="2"><tr><td>a</td><td>bb</td></tr></table>')
    let layout = new L.Layouter(doc, { font: FONT, width: Infinity, wrapMode: 0, alignment: 1, margin: 0, tabStop: 80, measure }).layout()
    let item = layout.blocks[0]
    // indented by the list (40 px), its number left of the text
    assert.equal(item.left, 40)
    assert.equal(item.marker.text, '1.')
    let table = layout.tables[0]
    assert.equal(table.cells.length, 2)
    // spacing 2 around cells as wide as their text
    assert.deepEqual(table.cells.map(c => [c.x, c.width]), [[2, 6], [10, 12]])
    assert.equal(table.width, 24)
})

// ---- the TextEdit ----

function cases(name, runtime){
    const { engine, TextEdit } = runtime

    function make(setup){
        let item
        engine.batch(() => {
            item = new TextEdit(null)
            item.font.family = 'Arial'
            item.font.pixelSize = 12
            if(setup) setup(item)
        })
        return item
    }

    const key = (item, k, modifiers = 0, text = '') => engine.batch(() => item.$keyEvent({ key: k, modifiers, text }, 'pressed'))
    const raw = (item) => item.getText(0, item.length)

    test(name + ': text is the HTML of the document, getText and length its plain text', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<b>Hi</b> there<br/>x'
        })
        assert.equal(body(edit.text), '<body style=" font-family:\'Arial\'; font-size:12px; font-weight:400; font-style:normal;">\n<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><span style=" font-weight:700;">Hi</span> there<br />x</p></body></html>')
        assert.equal(raw(edit), 'Hi there\u2028x')
        assert.equal(edit.length, 10)
        assert.equal(edit.lineCount, 2)
        assert.equal(body(edit.getFormattedText(1, 4)), '<body>\n<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><!--StartFragment--><span style=" font-weight:700;">i</span> t<!--EndFragment--></p></body></html>')
    })

    test(name + ': typing takes the format of the character before the cursor; Return a block, Shift+Return a line separator', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<b>Hi</b> there'
        })
        engine.batch(() => { edit.cursorPosition = 1 })
        key(edit, 0x58, 0, 'x')
        assert.ok(body(edit.text).includes('<span style=" font-weight:700;">Hxi</span> there'))
        engine.batch(() => { edit.cursorPosition = edit.length })
        key(edit, 0x01000004)
        key(edit, 0x5a, 0, 'z')
        key(edit, 0x01000004, 0x02000000)
        key(edit, 0x57, 0, 'w')
        assert.equal(raw(edit), 'Hxi there\u2029z\u2028w')
        assert.equal(edit.cursorPosition, 13)
        assert.equal(edit.lineCount, 3)
    })

    test(name + ': undo and redo restore the document, the cursor at the change', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<i>ab</i>'
        })
        engine.batch(() => { edit.cursorPosition = 2 })
        key(edit, 0x43, 0, 'c')
        key(edit, 0x01000003)
        key(edit, 0x01000003)
        assert.equal(raw(edit), 'a')
        engine.batch(() => edit.undo())
        assert.equal(raw(edit), 'abc')
        assert.equal(edit.cursorPosition, 3)
        engine.batch(() => edit.redo())
        assert.equal(raw(edit), 'a')
        assert.equal(edit.cursorPosition, 1)
    })

    test(name + ': Backspace at the start of a list item takes it out of the list, of an indented block one indent', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<ul><li>a</li><li>b</li></ul><p style="-qt-block-indent:1">c</p>'
        })
        engine.batch(() => { edit.cursorPosition = 2 })
        key(edit, 0x01000003)
        assert.equal(raw(edit), 'a\u2029b\u2029c')
        assert.ok(body(edit.text).includes('<li style=" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">a</li></ul>'))
        assert.ok(body(edit.text).includes('-qt-block-indent:1; text-indent:0px;">b</p>'))
        engine.batch(() => { edit.cursorPosition = 4 })
        key(edit, 0x01000003)
        assert.ok(body(edit.text).includes('-qt-block-indent:0; text-indent:0px;">c</p>'))
    })

    test(name + ': a table: its cells in the plain text, Backspace keeps a cell, a selection over cells clears them', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<table><tr><td>a</td><td>b</td></tr></table>'
        })
        assert.equal(raw(edit), '\ufdd0a\ufdd0b\ufdd1')
        engine.batch(() => { edit.cursorPosition = 3 })
        key(edit, 0x01000003)
        assert.equal(raw(edit), '\ufdd0a\ufdd0b\ufdd1')
        engine.batch(() => edit.select(1, 4))
        assert.equal(edit.selectedText, 'ab')
        key(edit, 0x51, 0, 'q')
        assert.equal(raw(edit), '\ufdd0\ufdd0q\ufdd1')
    })

    test(name + ': textFormat: a rich text as plain text shows its HTML; AutoText reads HTML as rich text', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<b>b</b>'
        })
        let html = edit.text
        engine.batch(() => { edit.textFormat = TextEdit.PlainText })
        assert.equal(edit.text, html)
        assert.equal(edit.length, html.length)
        engine.batch(() => {
            edit.textFormat = TextEdit.AutoText
            edit.text = '<i>i</i>'
        })
        assert.equal(raw(edit), 'i')
        engine.batch(() => { edit.text = 'no tags' })
        assert.equal(edit.text, 'no tags')
        engine.batch(() => edit.insert(0, '<b>B</b>'))
        assert.equal(raw(edit), 'Bno tags')
        assert.ok(body(edit.text).includes('<span style=" font-weight:700;">B</span>no tags'))
    })

    test(name + ': positions from the layout: cursorRectangle, positionAt, positionToRectangle', () => {
        let edit = make((e) => {
            e.textFormat = TextEdit.RichText
            e.text = '<p>ab</p><p>cd</p>'
        })
        let second = edit.positionToRectangle(4)
        let first = edit.positionToRectangle(1)
        assert.equal(first.x, 6)
        assert.ok(second.y > first.y)
        assert.equal(edit.positionAt(7, second.y + 1), 4)
        assert.equal(edit.positionAt(100, 0), 2)
    })
}

for(let [name, runtime] of runtimes) cases(name, runtime())
