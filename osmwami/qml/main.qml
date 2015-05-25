import QtQuick 2.3

import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.0

import QtPositioning 5.3

import net.sf.libosmscout.map 1.0

import "custom"

Window {
    id: mainWindow
    objectName: "main"
    title: "OSMScout"
    visible: true
    width: 1600
    height: 900

    property double myPlaceX: 55.74352
    property double myPlaceY: 37.60600

    function openAboutDialog() {
        var component = Qt.createComponent("AboutDialog.qml")
        var dialog = component.createObject(mainWindow, {})

        dialog.opened.connect(onDialogOpened)
        dialog.closed.connect(onDialogClosed)
        dialog.open()
    }

    function showLocation(location) {
        map.showLocation(location)
    }

    function onDialogOpened() {
        info.visible = false
        navigation.visible = false
    }

    function onDialogClosed() {
        info.visible = true
        navigation.visible = true

        map.focus = true
    }

    PositionSource {
        id: positionSource

        active: true

        onValidChanged: {
            console.log("Positioning is " + valid)
            console.log("Last error " + sourceError)

            for (var m in supportedPositioningMethods) {
                console.log("Method " + m)
            }
        }

        onPositionChanged: {
            console.log("Position changed:")

            if (position.latitudeValid) {
                console.log("  latitude: " + position.coordinate.latitude)
            }

            if (position.longitudeValid) {
                console.log("  longitude: " + position.coordinate.longitude)
            }

            if (position.altitudeValid) {
                console.log("  altitude: " + position.coordinate.altitude)
            }

            if (position.speedValid) {
                console.log("  speed: " + position.speed)
            }

            if (position.horizontalAccuracyValid) {
                console.log("  horizontal accuracy: " + position.horizontalAccuracy)
            }

            if (position.verticalAccuracyValid) {
                console.log("  vertical accuracy: " + position.verticalAccuracy)
            }
        }
    }

    GridLayout {
        id: content
        anchors.fill: parent

        Map {
            id: map
            Layout.fillWidth: true
            Layout.fillHeight: true
            focus: true


            function getFreeRect() {
                return Qt.rect(Theme.horizSpace,
                               Theme.vertSpace+searchDialog.height+Theme.vertSpace,
                               map.width-2*Theme.horizSpace,
                               map.height-searchDialog.y-searchDialog.height-3*Theme.vertSpace)
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Plus) {
                    map.zoomIn(2.0)
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Minus) {
                    map.zoomOut(2.0)
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Up) {
                    map.up()
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Down) {
                    map.down()
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Left) {
                    if (event.modifiers & Qt.ShiftModifier) {
                        map.rotateLeft();
                    }
                    else {
                        map.left();
                    }
                    event.accepted = true
                }
                else if (event.key === Qt.Key_Right) {
                    if (event.modifiers & Qt.ShiftModifier) {
                        map.rotateRight();
                    }
                    else {
                        map.right();
                    }
                    event.accepted = true
                }
                else if (event.modifiers===Qt.ControlModifier &&
                         event.key === Qt.Key_F) {
                    searchDialog.focus = true
                    event.accepted = true
                }
            }

            // Use PinchArea for multipoint zoom in/out?

            SearchDialog {
                id: searchDialog

                y: Theme.vertSpace
                visible: true

                anchors.horizontalCenter: parent.horizontalCenter

                desktop: map

                onShowLocation: {
                    map.showLocation(location)
                }

                onStateChanged: {
                    if (state==="NORMAL") {
                        onDialogClosed()
                    }
                    else {
                        onDialogOpened()
                    }
                }
            }

            Rectangle {
                id: meItem

                x: 400
                y: 400
                Image {
                    source: "qrc:/pics/pin.png"
                }
            }

            Rectangle {
                id: buttonWhereami
                //text: "Where am I?"
                x: Theme.horizSpace + 10
                //y: parent.height-height-Theme.vertSpace
                y: Theme.vertSpace + 10
                color: "transparent"
                radius: 40

                width: 220; height: 105

                visible: true
                signal clicked

                Image {
                    id: buttonWhereamiButtonImageStyle
                    source: "qrc:/pics/logo.png"
                    //opacity: buttonWhereami.pressed ? 0.5 : 1.0
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: buttonWhereami
                    onClicked:
                    {
                        buttonWhereami.clicked()
                        console.log(buttonWhereami + " clicked" )

                        map.showCoordinates(myPlaceX, myPlaceY)
                    }
                }
            }

            Rectangle {
                id: buttonSearch
                x: Theme.horizSpace
                y: parent.height-height-Theme.vertSpace
                color: "transparent"
                radius: 40

                width: 520; height: 88

                visible: true
                signal clicked

                Image {
                    id: buttonSearchButtonImageStyle
                    source: "qrc:/pics/Search.png"
                    //opacity: buttonWhereami.pressed ? 0.5 : 1.0
                    anchors.centerIn: parent
                }



                MouseArea {
                    anchors.fill: buttonSearch
                    onClicked:
                    {
                        buttonSearch.clicked()
                        console.log(buttonSearch + " clicked" )

                        //nextPage(buttonName)
                    }
                }
            }

            Button {
                id: testRoute1
                text: "TestRoute1"
                x: Theme.horizSpace
                y: 500

                onClicked: {
                    var startLo = Qt.createComponent()
                    //routingModel.setStartAndTarget(startLocation,
                    //                               destinationLocation)
                }
            }
/*
            Button {
                id: buttonNameForSearch
                //text: "Where am I?"
                //x: Theme.horizSpace
                x: parent.width-width-Theme.horizSpace
                y: parent.height-height-Theme.vertSpace

                //y: parent.height-height-Theme.vertSpace
                //y: Theme.vertSpace
                visible: true

                style: ButtonStyle {
                    id: buttonNameForSearchButtonStyle

                    Image {
                        id: buttonNameForSearchButtonImageStyle
                        source: "qrc:/pics/Search.png"
                        opacity: basicButton.pressed ? 0.5 : 1.0
                        anchors.centerIn: parent
                    }

                }
                onClicked:
                {
                    console.log(buttonWhereami + " clicked" )
                    //nextPage(buttonName)
                }
            }
*/

/*
            Button
            {
                buttonWhereami: "Where am I?"
                id:spb1; text: qsTr(buttonWhereami);
                onClicked:
                {
                    console.log(buttonWhereami + " clicked" )
                    //nextPage(buttonName)
                }
                Image {

                    source: "qrc:/pics/logo.png"
                    anchors.centerIn: parent
                }
            }

            ColumnLayout {
                id: whereami

                x: Theme.horizSpace

                //y: parent.height-height-Theme.vertSpace
                y: Theme.vertSpace
                visible: true

                //spacing: Theme.mapButtonSpace

                MapButton {
                    id: iAmButton
                    label: "Where am I?"

                    onClicked: {
                        openAboutDialog()
                    }
                }
            }
*/

/*
            // Bottom left column
            ColumnLayout {
                id: info

                x: Theme.horizSpace
                y: parent.height-height-Theme.vertSpace
                visible: true

                spacing: Theme.mapButtonSpace

                MapButton {
                    id: about
                    label: "?"

                    onClicked: {
                        openAboutDialog()
                    }
                }
            }
            */

            // Bottom right column
            ColumnLayout {
                id: navigation

                x: parent.width-width-Theme.horizSpace
                y: parent.height-height-Theme.vertSpace

                spacing: Theme.mapButtonSpace

                MapButton {
                    id: zoomIn
                    label: "+"

                    onClicked: {
                        map.zoomIn(2.0)
                    }
                }

                MapButton {
                    id: zoomOut
                    label: "-"

                    onClicked: {
                        map.zoomOut(2.0)
                    }
                }
            }
        }
    }
}
