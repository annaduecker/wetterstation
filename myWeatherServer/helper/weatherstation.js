let errorMessageStatement = 'No valid parameter';
let Database = require('../helper/database.js');
let database = new Database();

module.exports = {
  getSensorCalculatedAvg: function () {
    return new Promise((resolve, reject) => {
      let query = 'SELECT * FROM `v_sensor_durchschnitt`';
      resolve(database.query(query));
    })
  },
  getAllSensors: function (fromDate, toDate) {
    return new Promise((resolve, reject) => {
      let query;
      if (fromDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE `datum` >= ?';
      }
      else if (fromDate && toDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE  `datum` >= ?  AND `datum` <= ?';
      }
      else if (toDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE  `datum` <= ?';
      }
      else {
        query = 'SELECT * FROM `sensor_daten`';
      }
      resolve(database.query(query, [fromDate, toDate]));
    })
  },
  getSensorDataById: function (sensorId, fromDate, toDate) {
    return new Promise((resolve, reject) => {
      let query;
      if (sensorId && fromDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ? AND `datum` >= ?';
      }
      else if (sensorId && fromDate && toDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ? AND `datum` >= ?  AND `datum` <= ?';
      }
      else if (!isNaN(sensorId)) {
        query = 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ?';
      }else {
        reject(errorMessageStatement);
        return;
      }

      resolve(database.query(query, [sensorId, fromDate, toDate]))
    })
  },
  getSensorByIdCalculatedAvg: function (sensorId) {
    return new Promise((resolve, reject) => {
      if (sensorId && isNaN(sensorId)) {
        reject(errorMessageStatement);
        return;
      }
      let query = 'SELECT * FROM `v_sensor_durchschnitt` WHERE sensorid=?';
      resolve(database.query(query, [sensorId]));
    })
  },
  getSensorDataById: function (sensorId, fromDate, toDate) {
    return new Promise((resolve, reject) => {
      let query;
      if (sensorId && fromDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ? AND `datum` >= ?';
      }
      else if (sensorId && fromDate && toDate) {
        query = 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ? AND `datum` >= ?  AND `datum` <= ?';
      }
      else if (!isNaN(sensorId)) {
        query = 'SELECT * FROM `sensor_daten` WHERE `sensorid`= ? ORDER BY  datum DESC LIMIT 3';
      }else {
        reject(errorMessageStatement);
        return;
      }

      resolve(database.query(query, [sensorId, fromDate, toDate]));
    })
  },
  insertSensorData: function (sensorData) {
    var promises = sensorData.map(element => {
      return database.query('INSERT INTO `sensor_daten`(`sensorid`, `sensorwert`, `datum`) VALUES (?,?,now())', [element.sensorId, element.sensorValue])
        .catch(function (e) {
          res.status(500, {
            error: e
          })
        })
    })
    return new Promise((resolve, reject) => {
      resolve(Promise.all(promises));
    })
  }
}

/*
exports.getSensorDataById = function(sensorId,fromDate) {
    // get a connection from the pool
    
    
    
  }

exports.insertSensorData = function(sensorId,sensorValue, callback) {
  var sql = "INSERT INTO `sensor_daten`(`sensorid`, `sensorwert`, `datum`) VALUES (?,?,now())"
  // get a connection from the pool
  sensorId,sensorValue
}:
*/
