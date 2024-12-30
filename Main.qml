import QtQuick
import QtQuick.Controls
import QtMultimedia
import Qt.labs.platform
import Qt5Compat.GraphicalEffects
import My.SongHelper 1.0
import My.BarsVisualizer 1.0
pragma ComponentBehavior: Bound
MyWindow{
    enum PlayCycle{SingleCycle=0,ListCycle,Random}

    signal removeSongItem(int index)
    signal downloadSongItem(int index)

    readonly property int iconSize:28
    property color mainColor:Qt.color("#323232")
    property color mainColorInverse:Qt.rgba(1-mainColor.r,1-mainColor.g,1-mainColor.b,mainColor.a)

    property list<string> songs
    property list<string> searchResults
    property bool showSearchResult:false
    property int curSongIndex:-1
    property int songNums:showSearchResult?searchResults.length:songs.length

    id:root
    width:rect.width
    height:rect.height+visualizer.height

    FileDialog{
        id:fileDialog
        fileMode:FileDialog.OpenFiles
        nameFilters:["music files (*.mp3 *.wav *.m4a)"]
        onFilesChanged: {
            SongHelper.generateSongItems(files)
        }
    }

    Connections{
        target:SongHelper
        function onNewSongNamesReady(names){
            root.songs = root.songs.concat(names)
        }
        function onLastSongKeysPressed(){
            root.lastSong()
        }
        function onNextSongKeysPressed(){
            root.nextSong()
        }
        function onPlayingStateSwitchKeysPressed(){
            if(playMusic.playing){
                playMusic.pause()
                barsVisualizer.pause()
            }
            else
                playMusic.play()
        }
        function onMainColorChanged(color){
            console.log("recieve new color ",color)
            root.mainColor = color
        }
        function onSearchResultsReady(results){
            root.searchResults = results
            root.showSearchResult = true
        }
    }
    Connections{
        target:root
        function onRemoveSongItem(index){
            SongHelper.removeSongItem(index)
        }
        function onDownloadSongItem(index){
            SongHelper.downloadSongItem(index)
        }
    }


    MediaPlayer{
        id:playMusic

        audioOutput:AudioOutput{}
        audioBufferOutput:SongHelper.getAudioBufferOutput()
        onMediaStatusChanged: {
            switch(playMusic.mediaStatus){
            case MediaPlayer.LoadedMedia:
                playMusic.play()
                return
            case MediaPlayer.EndOfMedia:
                if(playCycle.curMode==Main.SingleCycle)
                    playMusic.play()
                else
                    root.nextSong()
                return
            }
        }
    }
    Rectangle{
        id:visualizer
        width:rect.width
        height:20
        color:"transparent"
        BarsVisualizer{
            id:barsVisualizer
            anchors.fill:parent
        }
    }

    Rectangle{
        id:rect
        anchors.top:visualizer.bottom
        width:container.width
        height:container.height
        radius:7
        color:root.mainColor
        FastBlur{
            anchors.fill:parent
            source:attachedPic
            radius:32
        }
        Row{
            id:container
            Item{
                height:containerCol.height
                width:height
                id:imgContainer
                property bool diskMode:list.listOpen
                Image{
                    id:mask
                    source:"image://songpic/disk"
                    visible:false
                }
                Image{
                    id:attachedPic
                    cache:false
                    anchors.fill:parent
                    asynchronous:true
                    retainWhileLoading:true
                    source:"image://songpic/"+root.curSongIndex
                    fillMode:Image.PreserveAspectFit
                    visible:!imgContainer.diskMode
                }
                OpacityMask{
                    anchors.fill:imgContainer
                    source:attachedPic
                    maskSource:mask
                    visible:imgContainer.diskMode
                    rotation:360*slider.value
                }

                property int dragX: 0
                property int dragY: 0
                property bool dragging: false
                MouseArea{
                    id:msarea
                    anchors.fill: parent

                    onPressed:e=>{
                        imgContainer.dragX = mouseX
                        imgContainer.dragY = mouseY
                        imgContainer.dragging = true

                    }
                    onReleased:e=>{
                        imgContainer.dragging = false

                    }
                    onPositionChanged:e=>{
                        if(imgContainer.dragging)
                        {
                            root.x += mouseX - imgContainer.dragX
                            root.y += mouseY - imgContainer.dragY
                        }
                    }
                }
            }

            Column{
                id:containerCol
                Row{
                    z:1
                    id:mainRow
                    anchors.horizontalCenter:parent.horizontalCenter
                    rightPadding:5
                    MyImage{
                        id:pulldown1
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/pull_down.svg")
                        svgColor:root.mainColorInverse
                        property bool functionsRowOpen:false
                        rotation:functionsRowOpen?180:0
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                pulldown1.functionsRowOpen=!pulldown1.functionsRowOpen
                            }
                        }

                    }
                    Slider{
                        id:slider
                        width:170
                        from:0
                        to:1.
                        value:playMusic.position/playMusic.duration
                        anchors.verticalCenter:parent.verticalCenter
                        background: Rectangle {
                            x: slider.leftPadding
                            y: slider.topPadding + slider.availableHeight / 2 - height / 2
                            implicitWidth: 200
                            implicitHeight: 4
                            width: slider.availableWidth
                            height: implicitHeight
                            radius: 2
                            color: "transparent"

                            Rectangle {
                                id:leftPart
                                width: slider.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                gradient:Gradient{
                                    orientation:Gradient.Horizontal
                                    GradientStop{position:0.0;color:root.mainColorInverse}
                                    GradientStop{position:1.0;color:root.mainColor}
                                }
                            }
                            Rectangle {
                                anchors.left:leftPart.right
                                width:slider.width-leftPart.width
                                height:parent.height
                                radius:2
                                color:"transparent"
                                border.color:
                                    Qt.rgba(root.mainColorInverse.r,root.mainColorInverse.g,root.mainColorInverse.b,(1-slider.visualPosition)*0.7)
                            }
                        }
                        handle: Rectangle {
                            y: slider.topPadding + slider.availableHeight / 2 - height / 2
                            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                            implicitWidth: root.iconSize/2
                            implicitHeight: root.iconSize/2
                            radius: root.iconSize/4
                            color: slider.pressed ? Qt.rgba(root.mainColor.r,root.mainColor.g,root.mainColor.b,0.7)
                                                  : root.mainColor
                            border.color: root.mainColorInverse
                        }
                        onMoved:{
                            playMusic.position = slider.value*playMusic.duration
                        }
                    }
                    MyImage{
                        id:netEaseCloud
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/net_ease_cloud.svg")

                    }

                }
                Row{
                    z:1
                    anchors.horizontalCenter:parent.horizontalCenter
                    id:functionsRow
                    visible:pulldown1.functionsRowOpen

                    MyImage{
                        id:playCycle
                        width:root.iconSize
                        height:root.iconSize
                        property int curMode:Main.ListCycle
                        svgColor:root.mainColorInverse
                        source:{
                            var cycle='a'
                            switch(curMode){
                            case Main.SingleCycle:
                                return Qt.resolvedUrl("assets/icon/single_cycle.svg")
                            case Main.ListCycle:
                                return Qt.resolvedUrl("assets/icon/list_cycle.svg")
                            case Main.Random:
                                return Qt.resolvedUrl("assets/icon/random.svg")
                            }
                        }

                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                playCycle.curMode = (playCycle.curMode+1)%3

                            }
                        }
                    }
                    MyImage{
                        id:left
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/left.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:root.lastSong()
                        }
                    }
                    MyImage{
                        id:pausePlay
                        width:root.iconSize
                        height:root.iconSize
                        source:playMusic.playing?Qt.resolvedUrl("assets/icon/pause.svg"):
                                                  Qt.resolvedUrl("assets/icon/play.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                console.log("seekable:",playMusic.seekable)
                                if(playMusic.playing){
                                    playMusic.pause()
                                    barsVisualizer.pause()
                                }
                                else
                                    playMusic.play()
                            }
                        }
                    }
                    MyImage{
                        id:right
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/right.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:root.nextSong()
                        }
                    }
                    MyImage{
                        id:folderOpen
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/folder_open.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                fileDialog.open()
                            }
                        }
                    }
                    MyImage{
                        id:list
                        width:root.iconSize
                        height:root.iconSize
                        property bool listOpen:false
                        source:Qt.resolvedUrl("assets/icon/list.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                list.listOpen=!list.listOpen
                            }
                        }
                    }
                    MyImage{
                        id:poweroff
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/poweroff.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                root.close()
                            }
                        }
                    }

                }
                Row{
                    visible:list.listOpen
                    id:searchRow
                    MyImage{
                        id:search
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/search.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                // console.log(searchInput.text)
                                if(searchInput.text)
                                    SongHelper.searchSongs(searchInput.text)
                            }
                        }
                    }
                    TextField{
                        id:searchInput
                        anchors.verticalCenter:searchRow.verticalCenter
                        placeholderText: qsTr("search online")
                        background: Rectangle {
                            radius:8
                            implicitWidth: mainRow.width-2*root.iconSize
                            implicitHeight:root.iconSize/2
                            color: searchInput.enabled?Qt.rgba(1,1,1,0.4):Qt.rgba(1,1,1,0.2)
                        }
                    }
                    MyImage{
                        id:clear
                        width:root.iconSize
                        height:root.iconSize
                        source:Qt.resolvedUrl("assets/icon/clear.svg")
                        svgColor:root.mainColorInverse
                        MouseArea{
                            anchors.fill:parent
                            onClicked:{
                                root.showSearchResult=false
                                SongHelper.quitSearchResults()
                                searchInput.text=""
                            }
                        }
                    }
                }

                Row{
                    visible:list.listOpen
                    ListView{
                        width:mainRow.width
                        height:400

                        model:root.showSearchResult?root.searchResults:root.songs
                        delegate:Rectangle{
                            id:songItem
                            width:mainRow.width
                            height:root.iconSize/2
                            required property string modelData
                            required property int index
                            color{
                                r:rect.color.r
                                g:rect.color.g
                                b:rect.color.b
                                a:root.curSongIndex==index?0.7:msa.containsMouse?0.4:0
                            }
                            MouseArea{
                                id:msa
                                anchors.fill:parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton
                                onClicked:e=>{
                                    if(e.button==Qt.LeftButton){
                                        if(root.curSongIndex == songItem.index){
                                            root.curSongIndexChanged()
                                        }else{
                                            root.curSongIndex = songItem.index
                                        }

                                        playMusic.setSource(SongHelper.getSongUrl(songItem.index))
                                    }else{
                                        console.log("right clicked")
                                        if(root.showSearchResult){
                                            //尝试将其download

                                        }else{
                                            root.songs.splice(songItem.index,1)
                                            root.removeSongItem(songItem.index)
                                            if(songItem.index<root.curSongIndex){
                                                curSongIndex--
                                            }
                                        }
                                    }
                                }
                                hoverEnabled:true
                                ToolTip{
                                    id:tip
                                    visible:msa.containsMouse
                                    delay:800
                                    text:root.showSearchResult?qsTr("right click to download"):qsTr("right click to remove from the list")
                                    timeout:4000
                                    contentItem: Text {
                                        text: tip.text
                                        font: tip.font
                                        color:root.mainColorInverse
                                        style:Text.Outline
                                        styleColor:root.mainColor
                                    }
                                    background:Rectangle{
                                        color:rect.color
                                    }

                                }
                            }
                            Text{
                                id:songItemText
                                width:parent.width
                                text:songItem.modelData
                                elide:Text.ElideLeft
                                color:root.mainColorInverse
                                style:Text.Outline
                                styleColor:root.mainColor
                            }


                        }
                    }

                }

                add:positionerTrans
            }
        }
    }

    Transition{
        id:positionerTrans
        NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
    }


    function nextSong(){
        if(root.curSongIndex==-1)
            return
        if(playCycle.curMode==Main.ListCycle){
            root.curSongIndex = (root.curSongIndex+1)%root.songNums
            playMusic.setSource(SongHelper.getSongUrl(root.curSongIndex))
        }else if(playCycle.curMode==Main.Random){
            root.curSongIndex = Math.round(Math.random()*(root.songNums-1))
            playMusic.setSource(SongHelper.getSongUrl(root.curSongIndex))
        }
    }
    function lastSong(){
        if(root.curSongIndex==-1)
            return
        root.curSongIndex = (root.curSongIndex-1+root.songNums)%root.songNums
        playMusic.setSource(SongHelper.getSongUrl(root.curSongIndex))
    }

}
