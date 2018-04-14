

var mysql = require('mysql');
var config={
    host: "51.136.15.163",
    user: "root",
    password: "my-secret-pw" ,
    database: "iot",
    connectionLimit: 10,
    supportBigNumbers: true
  };
 
  
  
//var pool = mysql.createPool();

class Database {
    constructor() {
        this.connection = mysql.createConnection( config );
    }
    query( sql, args ) {
        return new Promise( ( resolve, reject ) => {
            this.connection.query( sql, args, ( err, rows ) => {
                if ( err )
                    return reject( err );
                resolve( rows );
            } );
        } );
    }
    close() {
        return new Promise( ( resolve, reject ) => {
            this.connection.end( err => {
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