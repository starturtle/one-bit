import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Pane {
  Layout.alignment: Qt.AlignTop | Qt.AlignLeft
  Layout.margins: 0
  GridLayout {
    columns: 8
    Button {
      id: add
      text: "Add"
      Layout.columnSpan: 4
      onClicked: {
        if (! cols3.visible)
        {
          cols3.visible = true
          remove.enabled = true
        }
        else if (! cols4.visible)
        {
          cols4.visible = true
        }
        else if (! cols5.visible)
        {
          cols5.visible = true
        }
        else if (! cols6.visible)
        {
          cols6.visible = true
        }
        else if (! cols7.visible)
        {
          cols7.visible = true
        }
        else if (! cols8.visible)
        {
          cols8.visible = true
        }
        else if (! cols9.visible)
        {
          cols9.visible = true
        }
        else if (! cols10.visible)
        {
          cols10.visible = true
        }
        else if (! cols11.visible)
        {
          cols11.visible = true
        }
        else if (! cols12.visible)
        {
          cols12.visible = true
        }
        else if (! cols13.visible)
        {
          cols13.visible = true
        }
        else if (! cols14.visible)
        {
          cols14.visible = true
        }
        else if (! cols15.visible)
        {
          cols15.visible = true
        }
        else if (! cols16.visible)
        {
          cols16.visible = true
        }
        else if (! cols17.visible)
        {
          cols17.visible = true
        }
        else if (! cols18.visible)
        {
          cols18.visible = true
        }
        else if (! cols19.visible)
        {
          cols19.visible = true
        }
        else if (! cols20.visible)
        {
          cols20.visible = true
        }
        else if (! cols21.visible)
        {
          cols21.visible = true
        }
        else if (! cols22.visible)
        {
          cols22.visible = true
        }
        else if (! cols23.visible)
        {
          cols23.visible = true
        }
        else if (! cols24.visible)
        {
          cols24.visible = true
          enabled = false
        }
      }
    }
    Button {
      id: remove
      text: "Remove"
      Layout.columnSpan: 4
      onClicked: {
        if (cols24.visible)
        {
          cols24.visible = false
          add.enabled = true
        }
        else if (cols23.visible)
        {
          cols23.visible = false
        }
        else if (cols22.visible)
        {
          cols22.visible = false
        }
        else if (cols21.visible)
        {
          cols21.visible = false
        }
        else if (cols20.visible)
        {
          cols20.visible = false
        }
        else if (cols19.visible)
        {
          cols19.visible = false
        }
        else if (cols18.visible)
        {
          cols18.visible = false
        }
        else if (cols17.visible)
        {
          cols17.visible = false
        }
        else if (cols16.visible)
        {
          cols16.visible = false
        }
        else if (cols15.visible)
        {
          cols15.visible = false
        }
        else if (cols14.visible)
        {
          cols14.visible = false
        }
        else if (cols13.visible)
        {
          cols13.visible = false
        }
        else if (cols12.visible)
        {
          cols12.visible = false
        }
        else if (cols11.visible)
        {
          cols11.visible = false
        }
        else if (cols10.visible)
        {
          cols10.visible = false
        }
        else if (cols9.visible)
        {
          cols9.visible = false
        }
        else if (cols8.visible)
        {
          cols8.visible = false
        }
        else if (cols7.visible)
        {
          cols7.visible = false
        }
        else if (cols6.visible)
        {
          cols6.visible = false
        }
        else if (cols5.visible)
        {
          cols5.visible = false
        }
        else if (cols4.visible)
        {
          cols4.visible = false
        }
        else if (cols3.visible)
        {
          cols3.visible = false
          enabled = false
        }
      }
    }
    PixelColorSettings {
      id: cols1
      pixelColor: "black"
    }
    PixelColorSettings {
      id: cols2
      pixelColor: "white"
    }
    PixelColorSettings {
      id: cols3
      visible: false
      pixelColor: "red"
    }
    PixelColorSettings {
      id: cols4
      visible: false
      pixelColor: "lime"
    }
    PixelColorSettings {
      id: cols5
      visible: false
      pixelColor: "blue"
    }
    PixelColorSettings {
      id: cols6
      visible: false
      pixelColor: "cyan"
    }
    PixelColorSettings {
      id: cols7
      visible: false
      pixelColor: "magenta"
    }
    PixelColorSettings {
      id: cols8
      visible: false
      pixelColor: "yellow"
    }
    PixelColorSettings {
      id: cols9
      visible: false
      pixelColor: "orange"
    }
    PixelColorSettings {
      id: cols10
      visible: false
      pixelColor: "darkgreen"
    }
    PixelColorSettings {
      id: cols11
      visible: false
      pixelColor: "brown"
    }
    PixelColorSettings {
      id: cols12
      visible: false
      pixelColor: "pink"
    }
    PixelColorSettings {
      id: cols13
      visible: false
      pixelColor: "purple"
    }
    PixelColorSettings {
      id: cols14
      visible: false
      pixelColor: "green"
    }
    PixelColorSettings {
      id: cols15
      visible: false
      pixelColor: "moccasin"
    }
    PixelColorSettings {
      id: cols16
      visible: false
      pixelColor: "peru"
    }
    PixelColorSettings {
      id: cols17
      visible: false
      pixelColor: "indigo"
    }
    PixelColorSettings {
      id: cols18
      visible: false
      pixelColor: "lightseagreen"
    }
    PixelColorSettings {
      id: cols19
      visible: false
      pixelColor: "lightgray"
    }
    PixelColorSettings {
      id: cols20
      visible: false
      pixelColor: "darkgray"
    }
    PixelColorSettings {
      id: cols21
      visible: false
      pixelColor: "springgreen"
    }
    PixelColorSettings {
      id: cols22
      visible: false
      pixelColor: "olive"
    }
    PixelColorSettings {
      id: cols23
      visible: false
      pixelColor: "navy"
    }
    PixelColorSettings {
      id: cols24
      visible: false
      pixelColor: "firebrick"
    }
  }
  property variant colors: {
    if (cols24.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor, cols19.pixelColor, cols20.pixelColor, cols21.pixelColor, cols22.pixelColor, cols23.pixelColor, cols24.pixelColor]
    }
    else if (cols23.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor, cols19.pixelColor, cols20.pixelColor, cols21.pixelColor, cols22.pixelColor, cols23.pixelColor]
    }
    else if (cols22.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor, cols19.pixelColor, cols20.pixelColor, cols21.pixelColor, cols22.pixelColor]
    }
    else if (cols21.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor, cols19.pixelColor, cols20.pixelColor, cols21.pixelColor]
    }
    else if (cols20.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor, cols19.pixelColor, cols20.pixelColor]
    }
    else if (cols19.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor, cols19.pixelColor]
    }
    else if (cols18.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor, cols18.pixelColor]
    }
    else if (cols17.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor, cols17.pixelColor]
    }
    else if (cols16.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor, cols16.pixelColor]
    }
    else if (cols15.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor, cols15.pixelColor]
    }
    else if (cols14.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor, cols14.pixelColor]
    }
    else if (cols13.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor, cols13.pixelColor]
    }
    else if (cols12.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor, cols12.pixelColor]
    }
    else if (cols11.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor, cols11.pixelColor]
    }
    else if (cols10.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor, cols10.pixelColor]
    }
    else if (cols9.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor, cols9.pixelColor]
    }
    else if (cols8.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor, cols8.pixelColor]
    }
    else if (cols7.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor, cols7.pixelColor]
    }
    else if (cols6.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor, cols6.pixelColor]
    }
    else if (cols5.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor, cols5.pixelColor]
    }
    else if (cols4.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor, cols4.pixelColor]
    }
    else if (cols3.visible) {
      return [cols1.pixelColor, cols2.pixelColor, cols3.pixelColor]
    }
    else {
      return [cols1.pixelColor, cols2.pixelColor]
    }
  }
}