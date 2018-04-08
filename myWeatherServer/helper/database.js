var mysql = require('mysql');

var pool = mysql.createPool({
  host: "51.136.48.223",
  user: "root",
  password: "my-secret-pw" ,
  database: "iot",
  connectionLimit: 10,
  supportBigNumbers: true
});

// Get records from a city
exports.insertSensorData = function(sensorId,sensorValue, callback) {
  var sql = "INSERT INTO `sensor_daten`(`sensorid`, `sensorwert`, `datum`) VALUES (?,?,now())";
  // get a connection from the pool

  pool.getConnection(function(err, connection) {
    if(err) { console.log(err); callback(true); return; }
    // make the query
    connection.query(sql, [sensorId,sensorValue], function(err, results) {
      connection.release();
      if(err) { console.log(err); callback(true); return; }
      callback(false, results);
    });
  });
};