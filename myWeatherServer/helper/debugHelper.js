const winston = require('winston');
winston.level = process.env.LOG_LEVEL || "silly";
module.exports = {
  logger: function (req) {
    return new Promise((resolve, reject) => {
        winston.log('debug',req.method, "URL:"+req.originalUrl+"  " +new Date().toISOString());
      resolve(req);
    })
  },
  loggerDatalogger:function(data){
    return new Promise((resolve, reject) => {
        winston.log('silly',"Data:", JSON.stringify(data)+ "  " +new Date().toISOString());
      resolve(data);
    })
  }
}

