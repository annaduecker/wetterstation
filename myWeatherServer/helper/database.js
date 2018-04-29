

var mysql = require('mysql');
var config={
    host: "52.136.229.37",
    user: "root",
    password: "my-secret-pw" ,
    database: "iot",
    connectionLimit: 15,
    supportBigNumbers: true
  };
 
  


class Database {
    constructor() {
        this.pool = mysql.createPool( config );

        
    }
    query( sql, args ) {
        return new Promise( ( resolve, reject ) => {
            this.pool.getConnection(function(err, connection) {
            connection.query( sql, args, ( err, rows ) => {
                if ( err )
                    return reject( err );
                    connection.release()
                    resolve( rows );
            } );
        });
        } );
    }
    close(connection) {
        return new Promise( ( resolve, reject ) => {
            connection.release( err => {
                if ( err )
                    return reject( err );
                resolve();
            } );
        } );
    }
}

module.exports= Database;

/*
exports.getSensorDataById = function(sensorId,fromDate) {
    // get a connection from the pool
    
    
    
  };

exports.insertSensorData = function(sensorId,sensorValue, callback) {
  var sql = "INSERT INTO `sensor_daten`(`sensorid`, `sensorwert`, `datum`) VALUES (?,?,now())";
  // get a connection from the pool
  sensorId,sensorValue
}:
*/