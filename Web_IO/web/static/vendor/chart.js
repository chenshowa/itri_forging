    // am4core.ready(function() {

    // // Themes begin
    // am4core.useTheme(am4themes_animated);
    // // Themes end

    // var chart = am4core.create("chartdiv", am4charts.XYChart);
    // chart.paddingRight = 20;

    // var data = [];
    // var visits = 10;
    // for (var i = 1; i < 200; i++) {
    //   // visits += Math.round((Math.random() < 0.5 ? 1 : -1) * Math.random() * 10);
    //   visits += 0.001;
    //   data.push({ date: new Date(2018, 0, i), value: visits });
    // }

    // chart.data = data;

    // // Create Xaxes
    // var dateAxis = chart.xAxes.push(new am4charts.DateAxis());
    // dateAxis.renderer.grid.template.location = 0;
    // dateAxis.minZoomCount = 5;

    // // this makes the data to be grouped
    // dateAxis.groupData = true;
    // dateAxis.groupCount = 500;

    // // Create Yaxes
    // var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());

    // // Create series
    // var series = chart.series.push(new am4charts.LineSeries());
    // series.dataFields.dateX = "date";
    // series.dataFields.valueY = "value";
    // series.tooltipText = "{valueY}";
    // series.tooltip.pointerOrientation = "vertical";
    // series.tooltip.background.fillOpacity = 0.5;

    // chart.cursor = new am4charts.XYCursor();
    // chart.cursor.xAxis = dateAxis;

    // var scrollbarX = new am4core.Scrollbar();
    // scrollbarX.marginBottom = 20;
    // chart.scrollbarX = scrollbarX;

    // }); // end am4core.ready()

    // 之前的程式，現在用不到了
    // https://www.amcharts.com/docs/v4/concepts/axes/

    // 建立socket連接，等待server“傳送”數據，呼叫callback函数更新圖表


      // 填入數據
     

    $(document).ready(function() {
    
      console.log("connect socket ready ! ")
      namespace = '/test';
      var socket = io.connect(location.protocol + '//' + document.domain + ':' + location.port + namespace);
    
    
      console.log("connect socket succes ! ")
      socket.on('server_response', function (res) {
        update_mychart(res);
      });
    
    });

    var itridata = [];
    var update_mychart = function (res) {

      console.log("connect update_mychart succes ! ")
      
  
      // 準備數據    
      console.log(res)
      itridata.push({
        scales : res.scales
        // value : res.value,
        // value2 : res.value2
      })
      // 插入數據，在超出範圍同時刪除數據，並透過itridata判斷長度
      if (itridata.length <200){
      chart.addData( {scales : res.scales, value : res.value, value2 : res.value2}, 0);
        }
        else if (itridata.length >= 200) 
            {chart.addData( {scales : res.scales, value : res.value, value2 : res.value2}, 1);
            itridata.shift();
        }
        
     
    //   if (itridata.length <=200){
        // chart.addData( {scales : itridata.scales}, );
    //   }
        // else if (itridata.length >200) {
        // itridata.shift();
        // 
    //   }
    //   chart.data = itridata;
      console.log(itridata)
      return itridata
    };

    
    // chart.addData({scales : itridata.scales,
    // value : itridata.value, value2 : itridata.value2}, 1);
    console.log(update_mychart)

    
    var chart = am4core.create("chartdiv", am4charts.XYChart);

    am4core.ready(function() {

      console.log("load charts succes ! ")
      // Themes begin
      am4core.useTheme(am4themes_animated);
      // Themes end7
      chart.hiddenState.properties.opacity = 0;
      chart.padding(0, 0, 0, 0);
    //   chart.paddingRight = 0;

    
      // for (var i = 1; i < 200; i++) {
        // visits += 0.00001;
        // irtidata.push({ scales : i, value2 : visits });
        // irtidata.push({ scales : i, value : visits });
      // }

    
      // chart.data2 = data2;



    // Create Xaxes
    var scalesAxis = chart.xAxes.push(new am4charts.ValueAxis());
    scalesAxis.dataFields.scales = "scales";
    scalesAxis.renderer.grid.template.location = 0.0001;
    scalesAxis.startLocation = 0.5;
    scalesAxis.endLocation = 0.5;
 

    // Create Yaxes
    var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());
    // valueAxis.min = -2.5;
    // valueAxis.max = 2.5;
    // valueAxis.strictMinMax = true;

    // Create series
    var series = chart.series.push(new am4charts.LineSeries());
    series.name = "channel_1";
    series.dataFields.valueX = "scales";
    series.dataFields.valueY = "value";
    series.tooltipText = "{valueY}";
    series.tooltip.pointerOrientation = "vertical";
    series.tooltip.background.fillOpacity = 0.5;

    var series2 = chart.series.push(new am4charts.LineSeries());
    series2.name = "channel_2";
    series2.dataFields.valueX = "scales";
    series2.dataFields.valueY = "value2";
    series2.tooltipText = "{valueY2}";
    series2.tooltip.pointerOrientation = "vertical";
    series2.tooltip.background.fillOpacity = 0.5;


    chart.cursor = new am4charts.XYCursor();
    // chart.cursor.xAxis = dateAxis;

    var scrollbarX = new am4core.Scrollbar();
    scrollbarX.marginBottom = 0.5;
    chart.scrollbarX = scrollbarX;

    // Add legend
    chart.legend = new am4charts.Legend();
  });

   // end am4core.ready()













////////////////////////////////////////////////////////////////////////////////////////////

// 之前的程式，現在用不到了

// chart.data = [{
//   "scales": 1,
//   "value": 5
// }, {
//   "scales": 2,
//   "value": 4
// }, {
//   "scales": 3,
//   "value": 6
// }];


// // Create Xaxes
// var scalesAxis = chart.xAxes.push(new am4charts.scalesAxis());
// scalesAxis.dataFields.scales = "scales";

// // Create Yaxes
// // var valueAxis = chart.yAxes.push(new am4charts.ValueAxis());
// // valueAxis.renderer.grid.template.strokeOpacity = 1;
// // valueAxis.renderer.grid.template.stroke = am4core.color("#A0CA92");
// // valueAxis.renderer.grid.template.strokeWidth = 2;

// // Create series
// var series = chart.series.push(new am4charts.ColumnSeries());
// series.dataFields.valueY = "value";
// series.dataFields.scalesX = "scales";
// series.name = "Sales";

// // Add data


