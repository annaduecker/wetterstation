var mysql = require('mysql');
var config = {
  host: '172.17.0.2',
  user: 'root',
  password: 'my-secret-pw',
  database: 'iot',
  connectionLimit: 15,
  supportBigNumbers: true
};
let CONNECTIONERROR="No data found. Please contact administrator";


class Database {
  constructor () {
    this.pool = mysql.createPool(config);
  }
  query (sql, args) {
    return new Promise((resolve, reject) => {
      this.pool.getConnection(function (err, connection) {
        if (err){
          err.endUserMessage=CONNECTIONERROR;
         reject(err);
         return;
        }
        connection.query(sql, args, (err, rows) => {
          if (err){
            err.endUserMessage=CONNECTIONERROR;
            reject(err);
          }
          connection.release();
          resolve(rows);
        })
      })
    })
  }
  close (connection) {
    return new Promise((resolve, reject) => {
      connection.release(err => {
        if (err)
          return reject(err);
        resolve();
      })
    })
  }
}

module.exports = Database;