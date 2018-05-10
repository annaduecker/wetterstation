let errorMessageStatement = 'No valid parameter';
let Database = require('../helper/database.js');
let database = new Database();

module.exports = {
  getSensorCalculatedAvg: function () {
    return new Promise((resolve, reject) => {
      let query = 'SELECT * FROM `v_sensor_durchschnitt` WHERE datum=Date(now())';
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

  getSensorCalculatedAvgLast: function () {
    return new Promise((resolve, reject) => {
      let query = 'select m1.sensorid,round(AVG(sensorwert)),datum from sensor_daten as m1  INNER JOIN sensor_configuration as m3 ON m1.sensorid=m3.sensorId where datum = (select max(datum) from sensor_daten as m2 where m1.sensorid = m2.sensorid ) GROUP BY sensorid,datum ORDER BY `m1`.`sensorid` ASC';
      resolve(database.query(query));
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
        query = "SELECT * FROM `v_sensor_daten` WHERE `sensorid`= ? AND cast(datum as date)=cast(now()as date) ORDER BY  datum ";
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
