import QtQuick 2.3

import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.0

import QtPositioning 5.3

import QtGraphicalEffects 1.0

import net.sf.libosmscout.map 1.0

import "custom"

Window {
    id: mainWindow
    objectName: "main"
    title: "OSMScout"
    visible: true
    width: 1920
    height: 1080

    property int categoryListWidth: 596

    property double deltaLat: 0.00354
    property double deltaLon: 0.00927

    //lat:47.2054 lon38.9429
    property double myPlaceLat: 47.2054
    property double myPlaceLon: 38.9429

    property double myStartPlaceLat: 55.74352
    property double myStartPlaceLon: 37.60600

    //property double myTargetLat: 55.71046
    //property double myTargetLon: 37.47212

    //property double myPlaceLat: 55.74352
    //property double myPlaceLon: 37.60600

    //property double myTargetLat: 55.71046
    //property double myTargetLon: 37.47212

    //INFO LIST root if category list, another value if some places list
    property string curCategoryName: "root"

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

    /*
    function onDialogOpened() {
        info.visible = false
        navigation.visible = false
    }

    function onDialogClosed() {
        info.visible = true
        navigation.visible = true

        map.focus = true
    }*/

    function testFunction() {
        console.log("Test function !!!!!!!!!!!!")
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

        POI {
            id: myPOI
        }

        RoutingListModel {
            id: myRoutingModel
        }

        Map {
            id: map
            //width: 1400
            Layout.fillWidth: true
            //Layout.fillWidth: false
            Layout.fillHeight: true
            //focus: true
            focus: false


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

            /*SearchDialog {
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
            }*/

            /*Rectangle {
                id: meItem

                x: 400
                y: 400
                Image {
                    source: "qrc:/pics/pin.png"
                }
            }*/

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

                        //put my place and show coord
                        myPOI.loadAllPOIs()
                        myRoutingModel.showMe(myStartPlaceLat, myStartPlaceLon)
                        map.showCoordinates(myStartPlaceLat, myStartPlaceLon)
                        //map.showCoordinates(map.myLat, map.myLon)
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
                    console.log("TestRoute: lastLat:"+ map.lastLat+ " lastLon:" + map.lastLon)
                    myRoutingModel.setStartAndTargetByCoord(myStartPlaceLat, myStartPlaceLon, map.lastLat, map.lastLon)
                    //console.log("TestRoute: lastLat:"+ map.lastLat+ " lastLon:" + map.lastLon)
                    //myRoutingModel.setStartAndTargetByCoord(map.myLat, map.myLon, map.lastLat, map.lastLon)
                }
            }


            Button {
                id: loadPOI1
                text: "LoadPOI"
                x: Theme.horizSpace
                y: 600

                onClicked: {
                    myRoutingModel.loadPOI(myPlaceLat, myPlaceLon)
                    //myRoutingModel.setStartAndTargetByCoord(myPlaceLat, myPlaceLon, myTargetLat, myTargetLon)
                    //myRoutingModel.setStartAndTarget(myPOI.startLoc, myPOI.endLoc)
                    //var startLo = Qt.createComponent()
                    //routingModel.setStartAndTarget(startLocation,
                    //                               destinationLocation)
                }
            }


            Button {
                id: switchModelRootButton
                text: "Back"
                x: Theme.horizSpace
                y: 700

                onClicked: {
                    console.log("Back: "+ curCategoryName)
                    curCategoryName = "root"
                    infoListView.delegate = categoriesDelegate
                    //infoListView.model = CategoryModel
                    infoListView.model = categoriesModel
                    infoListWidget.color = "transparent"
                    infoListWidget.opacity = 0.9
                    infoListWidget.radius = 20


                }
            }

            Button {
                id: switchModelButton
                text: "Switch model"
                x: Theme.horizSpace
                y: 800

                onClicked: {
                    console.log("Switch model: "+ curCategoryName)
                    if (curCategoryName == "root1")
                    {
                        infoListView.delegate = categoriesDelegate
                        //infoListView.model = CategoryModel
                        infoListView.model = categoriesModel
                        infoListWidget.color = "transparent"
                        infoListWidget.opacity = 0.9
                        infoListWidget.radius = 20

                    }
                    else if (curCategoryName == "pharmacy")
                    {
                        infoListWidget.color = "white"
                        infoListWidget.opacity = 1.0

                        infoListView.delegate = placesDelegate
                        //infoListView.model = placesModel
                        infoListView.model = pharmacyModel

                        //console.log("Places set section")
                        //infoListView.section.delegate = placesSectionHeadDelegate
                        //infoListView.section.property = curCategoryName
                        //infoListView.section.criteria = ViewSection.FullString
                    } else if (curCategoryName == "hotel")
                    {
                        infoListWidget.color = "white"
                        infoListWidget.opacity = 1.0

                        infoListView.delegate = placesDelegate
                        //infoListView.model = placesModel
                        infoListView.model = hotelModel

                        //console.log("Places set section")
                        //infoListView.section.delegate = placesSectionHeadDelegate
                        //infoListView.section.property = curCategoryName
                        //infoListView.section.criteria = ViewSection.FullString
                    } else if (curCategoryName == "museum")
                    {
                        infoListWidget.color = "white"
                        infoListWidget.opacity = 1.0

                        infoListView.delegate = placesDelegate
                        //infoListView.model = placesModel
                        infoListView.model = museumModel

                        //console.log("Places set section")
                        //infoListView.section.delegate = placesSectionHeadDelegate
                        //infoListView.section.property = curCategoryName
                        //infoListView.section.criteria = ViewSection.FullString
                    }




                    else {
                        infoListWidget.color = "white"
                        infoListWidget.opacity = 1.0

                        infoListView.delegate = placesDelegate
                        infoListView.model = placesModel


                    }

                    //myRoutingModel.loadPOI(myPlaceLat, myPlaceLon)
                    //myRoutingModel.setStartAndTargetByCoord(myPlaceLat, myPlaceLon, myTargetLat, myTargetLon)
                    //myRoutingModel.setStartAndTarget(myPOI.startLoc, myPOI.endLoc)
                    //var startLo = Qt.createComponent()
                    //routingModel.setStartAndTarget(startLocation,
                    //                               destinationLocation)
                }
            }

            // Bottom right column
            ColumnLayout {
                id: navigation

                x: parent.width-categoryListWidth-Theme.horizSpace - 50
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

            /////////////////////////////////////////////////////////
            //categoryMenu
            /////////////////////////////////////////////////////////

            ColumnLayout {
                //x: parent.width-width-Theme.horizSpace
                //y: parent.height-height-Theme.vertSpac
                x: parent.width - width
                y: 0

                Rectangle {
                    id: infoListWidget
                    focus: true
                    width: categoryListWidth; height: 1080
                    //width: 196; height: 500
                    Layout.fillHeight: true
                    //color: "white"
                    color: "transparent"
                    opacity: 0.9

                    radius: 20

                    ListView {
                        id: infoListView
                        anchors.fill: parent
                        //model: CategoryModel {}
                        model: categoriesModel
                        //model: placesModel
                        delegate: categoriesDelegate

                        //delegate: placesDelegate
                        //highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                        focus: true
                        //interactive: false

                        section.criteria: ViewSection.FullString
                        section.delegate: placesSectionHeadDelegate
                        section.property: "category"
                    }

                    //border.color: "grey"
                    //border.width: 1

                    //////////////////////////////////////////////////////////////
                    //Places
                    //////////////////////////////////////////////////////////////

                    ListModel {
                        id: placesModel
                        ListElement {
                            category: "pharmacy"
                            name: "pharmacy 1"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "hotel 2"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "museum 3"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "nightclub 4"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "restaurant 5"
                        }
                    }

                    /*
                    ListModel {
                        id: pharmacyModel
                        ListElement {
                            category: "pharmacy"
                            name: "pharmacy 1"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "hotel 2"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "museum 3"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "nightclub 4"
                        }
                        ListElement {
                            category: "pharmacy"
                            name: "restaurant 5"
                        }
                    }*/

                    Component {
                        id: placesSectionHeadDelegate
                        Rectangle {
                            //width: container.width
                            width: categoryListWidth
                            height: 100
                            color: "lightsteelblue"

                            Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                text: "<- Вернуться к категориям"+ '\n' + section
                                font.bold: true
                                font.pixelSize: 20
                                anchors.centerIn: parent
                                //anchors.fill: parent
                            }

                            MouseArea {
                                anchors.fill: parent
                                //onClicked: fruitModel.setProperty(index, "cost", cost * 2)
                                onClicked: {
                                    console.log("Return to categories clicked")
                                    curCategoryName = "root"

                                }
                            }

                        }
                    }

                    Component {
                        id: placesDelegate
                        Item {
                            width: categoryListWidth
                            height: 100

                            //Column {
                            //Text { text: '<b>Name:</b> ' + name }
                            //    Text { text: '<b>Number:</b> ' + number }
                            //}

                            Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                //text: '<b>Name:</b> ' + name + " index" + index
                                text: modelData
                                anchors.fill: parent
                            }

                            MouseArea {
                                anchors.fill: parent
                                //onClicked: fruitModel.setProperty(index, "cost", cost * 2)
                                onClicked: {
                                    console.log(name + " clicked")
                                    ListView.currentIndex = index
                                    curCategoryName = name
                                    //myPOI.loadPOIPlacesFromDBbyName(name)
                                }
                            }
                        }
                    }

                    //////////////////////////////////////////////////////////////
                    //Categories
                    //////////////////////////////////////////////////////////////

                    /*
                    ListModel {
                        id: categoriesModel
                        ListElement {
                            name: "pharmacy"
                            iconName: "Pharmacies.png"
                        }
                        ListElement {
                            name: "hotel"
                            iconName: "Hotels.png"
                        }
                        ListElement {
                            name: "museum"
                            iconName: "Museums.png"
                        }
                        ListElement {
                            name: "nightclub"
                            iconName: "Nightclubs.png"
                        }
                        ListElement {
                            name: "restaurant"
                            iconName: "Restaurants.png"
                        }
                    }
                    */


                    Component {
                        id: categoriesDelegate
                        Item {
                            width: categoryListWidth; height: 216
                            //Column {
                            Text { text: '<b>Name:</b> ' + name }
                            //Image {
                            //    source: '../pics/'+iconName
                            //}
                            //    Text { text: '<b>Number:</b> ' + number }
                            //}


                            MouseArea {
                                id: catMouseArea
                                anchors.fill: parent
                                //onClicked: fruitModel.setProperty(index, "cost", cost * 2)
                                onClicked: {
                                    console.log(name + " clicked")
                                    curCategoryName = name

                                    //ListView.currentIndex = index
                                    //myPOI.loadPOIPlacesFromDBbyName(name)
                                }
                            }
                        }

                    } //component


                    /*
                DropShadow {
                    fast: true
                    anchors.fill: infoListWidget
                    horizontalOffset: 3
                    verticalOffset: 3
                    radius: 8.0
                    samples: 16
                    color: "#80000000"
                    source: infoListWidget
                }*/
                } //rectangle


            } //columnlayout

        } //map

    }//gridlayout

} //window
