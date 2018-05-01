var express = require('express');
var router = express.Router();
var Database = require('../helper/database.js');
var debugHelper = require('../helper/debugHelper.js');
var database = new Database();
var weatherStationHelper = require('../helper/weatherstation.js');


/* POST wetterstation sensorData listing. */
router.post('/sensordata/add', function (req, res, next) {
    let sensorData = req.body.data;
    
    var promises = [];
    promises.push(debugHelper.logger(req));
    promises.push(debugHelper.loggerDatalogger(sensorData));

     Promise.all(promises);
    weatherStationHelper.insertSensorData(sensorData)
        .then(function (data) {
            return new Promise((resolve, reject) => {
                res.setHeader('Content-Type', 'application/json');
                res.status(201);
                res.end();
                resolve(data);
              })
        })
        .catch(function (e) {
            res.status(500, {
                error: e
            });
            res.end(e.endUserMessage);
        });
});

router.get('/sensor/:sensorid', function (req, res, next) {
    let sensorId = req.params.sensorid;
    let fromDate = req.headers.fromdate;
    let toDate = req.headers.todate;

    weatherStationHelper.getSensorDataById(sensorId, fromDate, toDate)
        .then(function (data) {
            return new Promise((resolve, reject) => {
                res.setHeader('Content-Type', 'application/json');
                res.end(JSON.stringify(data));
                resolve(data);
              })
        })
        .then(function(data){
            debugHelper.logger(req);
            debugHelper.loggerDatalogger(data);
        })
        .catch(function (e) {
            res.status(500, {
                error: e
            });
            res.end(e.endUserMessage);
        });
    // Respond with results as JSON

});

router.get('/sensor/', function (req, res, next) {
    let fromDate = req.headers.fromdate;
    let toDate = req.headers.todate;
    weatherStationHelper.getAllSensors(fromDate, toDate)
    .then(function (data) {
        return new Promise((resolve, reject) => {
            res.setHeader('Content-Type', 'application/json');
            res.end(JSON.stringify(data));
            resolve(data);
          })
    })
    .then(function(data){
        debugHelper.logger(req);
        debugHelper.loggerDatalogger(data);
    })
        .catch(function (e) {
            res.status(500, {
                error: e
            });
            res.end(e.endUserMessage);
        });
    // Respond with results as JSON

});

router.get('/sensor/:sensorid/calculated/avg', function (req, res, next) {
    let sensorId = req.params.sensorid;
    weatherStationHelper.getSensorByIdCalculatedAvg(sensorId)
    .then(function (data) {
        return new Promise((resolve, reject) => {
            res.setHeader('Content-Type', 'application/json');
            res.end(JSON.stringify(data));
            resolve(data);
          })
    })
    .then(function(data){
        debugHelper.logger(req);
        debugHelper.loggerDatalogger(data);
    })
        .catch(function (e) {
            res.status(500, {
                error: e
            });
            res.end(e.endUserMessage);
        });
    // Respond with results as JSON

});

router.get('/sensor/calculated/avg', function (req, res, next) {
    weatherStationHelper.getSensorCalculatedAvg()
    .then(function (data) {
        return new Promise((resolve, reject) => {
            res.setHeader('Content-Type', 'application/json');
            res.end(JSON.stringify(data));
            resolve(data);
          })
    })
    .then(function(data){
        debugHelper.logger(req);
        debugHelper.loggerDatalogger(data);
    })
        .catch(function (e) {
            res.status(500, {
                error: e
            });
            res.end(e.endUserMessage);
        });
    // Respond with results as JSON

});


module.exports = router;