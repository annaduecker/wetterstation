var express = require('express');
var router = express.Router();
var Database = require('../helper/database.js');
var database=new Database();
var weatherStationHelper = require('../helper/weatherstation.js');

/* POST wetterstation sensorData listing. */
router.post('/sensordata/add', function(req, res, next) {
  let sensorData=req.body.data;
  weatherStationHelper.insertSensorData(sensorData)
  .then(function (data) {
    res.setHeader('Content-Type', 'application/json');
    res.send("Data inserted successful: "+JSON.stringify(sensorData));
  })
});

router.get('/sensor/:sensorid', function(req, res, next) {
  let sensorId=req.params.sensorid;
  let fromDate= req.headers.fromdate;
  let toDate= req.headers.todate;
  weatherStationHelper.getSensorDataById(sensorId,fromDate,toDate)
  .then(function (data) {
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify(data));
  })
  .catch(function (e) {
      res.status(500, {
          error: e
      });
      res.end(e);
  });
     // Respond with results as JSON

});

router.get('/sensor/:sensorid/calculated/avg', function(req, res, next) {
  let sensorId=req.params.sensorid;
  weatherStationHelper.getSensorByIdCalculatedAvg(sensorId)
  .then(function (data) {
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify(data));
  })
  .catch(function (e) {
      res.status(500, {
          error: e
      });
      res.end(e);
  });
     // Respond with results as JSON

});

router.get('/sensor/calculated/avg', function(req, res, next) {
  weatherStationHelper.getSensorCalculatedAvg()
  .then(function (data) {
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify(data));
  })
  .catch(function (e) {
      res.status(500, {
          error: e
      });
      res.end(e);
  });
     // Respond with results as JSON

});


module.exports = router;
