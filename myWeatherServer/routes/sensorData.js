var express = require('express');
var router = express.Router();
var db = require('../helper/database');

/* POST wetterstation sensorData listing. */
router.post('/addSensorData', function(req, res, next) {
  var sensorData=req.body.data;

  sensorData.forEach(element => {
    db.insertSensorData(element.sensorId,element.sensorValue, function(err, results) {
      if(err) { res.send(500,"Server Error"); return;
    }}
    );
  })
 
     // Respond with results as JSON
  res.send('data inserted successful:'+ JSON.stringify(sensorData));
});

module.exports = router;
