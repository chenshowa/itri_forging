    // 建立socket連接，等待server“傳送”數據，呼叫callback函数更新圖表
      // 填入數據

    $(document).ready(function() {

      console.log("connect socket ready ! ")
      namespace = '/test';
      var socket = io.connect(location.protocol + '//' + document.domain + ':' + location.port + namespace);


      console.log("connect socket succes ! ")
      socket.on('server_response', function (res) {
        update_mychart(res);
        ShowValue();
      });

    });

    var itridata = [];
    var EigenValue = [];
    var EigenValue2 = [];
    var update_mychart = function (res) {

      console.log("connect update_mychart succes ! ")
      var itridata = [];


      // 準備數據

      console.log(res)
      window.itridata.push({
        // 插入數據時用來判斷資料儲存長度的工具
        scales : res.TimeStamp
      })


      // 插入數據，在超出範圍同時刪除數據，並透過itridata判斷長度

        console.log(res.TimeStamp[0].length)
        if (window.itridata.length <6){
            window.EigenValue.push( {shotMax : res.shotMax[0], shotMin : res.shotMin[0], shotAvg : res.shotAvg[0]})
            window.EigenValue2.push( {shotMax : res.shotMax[1], shotMin : res.shotMin[1], shotAvg : res.shotAvg[1]})
            for (i = 0; i<res.TimeStamp[0].length; i++){
             console.log(window.itridata.length);
              chart.addData( {x : res.TimeStamp[0][i], y1 : res.value[0][i], y2 : res.value[1][i]}, 0);
              // chart.addData( {x : res.TimeStamp[0][i], y1 : res.value[0][i]}, 0);
            }
        }
          else if (window.itridata.length >= 6) {
            window.EigenValue.push( {shotMax : res.shotMax[0], shotMin : res.shotMin[0], shotAvg : res.shotAvg[0]})
            window.EigenValue2.push( {shotMax : res.shotMax[1], shotMin : res.shotMin[1], shotAvg : res.shotAvg[1]})
            for (i = 0; i<res.TimeStamp[0].length; i++){
              console.log(window.itridata.length)
              chart.addData( {x : res.TimeStamp[0][i], y1 : res.value[0][i], y2 : res.value[1][i]}, 1);
              // chart.addData( {x : res.TimeStamp[0][i], y1 : res.value[0][i]}, 1);
            }
            window.itridata.shift();
            window.EigenValue.shift();
            window.EigenValue2.shift();
        }

      // console.log(window.itridata)
      // console.log(window.EigenValue)
      console.log(chart.data)
      return itridata
    };
    console.log(window.EigenValue)
    console.log(chart)
    var count = 0;
    function ShowValue(){
      // console.log(window.EigenValue)
      // console.log(window.EigenValue[0])
      temp = window.EigenValue[0]
      console.log(temp)
      console.log(typeof(temp))

        document.getElementById("test0").innerHTML = window.EigenValue[0]['shotMax'][0];
        document.getElementById("test1").innerHTML = window.EigenValue[0]['shotMin'][0];
        document.getElementById("test2").innerHTML = window.EigenValue[0]['shotAvg'][0];

        document.getElementById("test3").innerHTML = window.EigenValue[0]['shotMax'][1];
        document.getElementById("test4").innerHTML = window.EigenValue[0]['shotMin'][1];
        document.getElementById("test5").innerHTML = window.EigenValue[0]['shotAvg'][1];

        // document.getElementById("test6").innerHTML = window.EigenValue[0]['shotMax'][0];
        // document.getElementById("test7").innerHTML = window.EigenValue[0]['shotMin'][0];
        // document.getElementById("test8").innerHTML = window.EigenValue[0]['shotAvg'][0];
//
        // document.getElementById("test9").innerHTML = window.EigenValue[0]['shotMax'][0];
        // document.getElementById("test10").innerHTML = window.EigenValue[0]['shotMin'][0];
        // document.getElementById("test11").innerHTML = window.EigenValue[0]['shotAvg'][0];
//
        // document.getElementById("test12").innerHTML = window.EigenValue[0]['shotMax'][0];
        // document.getElementById("test13").innerHTML = window.EigenValue[0]['shotMin'][0];
        // document.getElementById("test14").innerHTML = window.EigenValue[0]['shotAvg'][0];


        document.getElementById("2test0").innerHTML = window.EigenValue2[0]['shotMax'][0];
        document.getElementById("2test1").innerHTML = window.EigenValue2[0]['shotMin'][0];
        document.getElementById("2test2").innerHTML = window.EigenValue2[0]['shotAvg'][0];
        //
        document.getElementById("2test3").innerHTML = window.EigenValue2[0]['shotMax'][1];
        document.getElementById("2test4").innerHTML = window.EigenValue2[0]['shotMin'][1];
        document.getElementById("2test5").innerHTML = window.EigenValue2[0]['shotAvg'][1];
        // document.getElementById("2test6").innerHTML = window.EigenValue2[0]['shotMax'][0];

        // document.getElementById("2test7").innerHTML = window.EigenValue2[0]['shotMin'][0];
        // document.getElementById("2test8").innerHTML = window.EigenValue2[0]['shotAvg'][0];
// 2
        // document.getElementById("2test9").innerHTML = window.EigenValue2[0]['shotMax'][0];
        // document.getElementById("2test10").innerHTML = window.EigenValue2[0]['shotMin'][0];
        // document.getElementById("2test11").innerHTML = window.EigenValue2[0]['shotAvg'][0];
// 22
        // document.getElementById("2test12").innerHTML = window.EigenValue2[0]['shotMax'][0];
        // document.getElementById("2test13").innerHTML = window.EigenValue2[0]['shotMin'][0];
        // document.getElementById("2test14").innerHTML = window.EigenValue2[0]['shotAvg'][0];

      console.log(count)
      console.log("show value")
    }

    var chart = am4core.create("chartdiv", am4charts.XYChart);

    am4core.ready(function() {

      console.log("load charts succes ! ")
      // Themes begin
      am4core.useTheme(am4themes_animated);
      // Themes end7
      chart.hiddenState.properties.opacity = 0;
      chart.padding(0, 0, 0, 0);

    chart.dateFormatter.inputDateFormat = "i";

    // Create Xaxes
    var scalesAxis = chart.xAxes.push(new am4charts.DateAxis());

    scalesAxis.startLocation = 0.5;
    scalesAxis.endLocation = 0.5;
    // scalesAxis.min = (new Date(2019, 12, 6, 0).getTime());
    // scalesAxis.max = (new Date(2019, 12, 6, 6).getTime());
    // scalesAxis.renderer.labels.template.disabled = true;

    // Create Yaxes
    var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());
    // valueAxis.min = -2.5;
    // valueAxis.max = 2.5;
    // valueAxis.strictMinMax = true;

    // Create series
    var series = chart.series.push(new am4charts.LineSeries());

    series.name = "channel_1";
    series.dataFields.dateX = "x";
    series.dataFields.valueY = "y1";
    series.tooltipText = "{valueY}";
    series.tooltip.pointerOrientation = "vertical";
    series.tooltip.background.fillOpacity = 0.5;

    var series2 = chart.series.push(new am4charts.LineSeries());
    series2.name = "channel_2";
    series2.dataFields.dateX = "x";
    series2.dataFields.valueY = "y2";
    series2.tooltipText = "{valueY2}";
    series2.tooltip.pointerOrientation = "vertical";
    series2.tooltip.background.fillOpacity = 0.5;


    chart.cursor = new am4charts.XYCursor();
    // chart.cursor.xAxis = dateAxis;

    // var scrollbarX = new am4core.Scrollbar();
    // scrollbarX.marginBottom = 0.5;
    // chart.scrollbarX = scrollbarX;

    // Add legend
    chart.legend = new am4charts.Legend();
  });

   // end am4core.ready()
