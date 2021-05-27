const mysql = require('mysql2');

//to acess require this module and after con.getConnection...

// when app entry to tests		
 module.exports = {
	con: mysql.createPool({
		host     : process.env.HOST,
		user     : process.env.USER_LOCAL,
		password : process.env.PASSWORD,
		database : process.env.DATABASE
	})
}; 