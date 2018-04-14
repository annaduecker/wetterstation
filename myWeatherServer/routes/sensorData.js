var express = require('express');
var router = express.Router();
var Database = require('../helper/database.js');
var database=new Database();

/* POST wetterstation sensorData listing. */
router.post('/sensordata/add', function(req, res, next) {

  
  var sensorData=req.body.data;
  var promises=  sensorData.map(element => {
    return database.query( 'INSERT INTO `sensor_daten`(`sensorid`, `sensorwert`, `datum`) VALUES (?,?,now())',[element.sensorId,element.sensorValue])
  .catch(function (e) {
      res.status(500, {
          error: e
      });
  })
  })
  console.log(promises)
  Promise.all(promises).then(function (data) {
    res.setHeader('Content-Type', 'application/json');
    res.send(JSON.stringify(sensorData));
})
});

router.get('/sensor/:sensorid', function(req, res, next) {
  var sensorId=req.params.sensorid;
  var fromDate= req.headers.fromdate;
  var toDate= req.headers.todate;

  database.query( 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ? AND `datum` >= ?',[sensorId,fromDate])
  .then(function (data) {
      res.setHeader('Content-Type', 'application/json');
      res.end(JSON.stringify(data));
  })
  .catch(function (e) {
      res.status(500, {
          error: e
      });
  });
     // Respond with results as JSON

});

module.exports = router;
