<?php 

//sets my sql
$username = "user";
$password = "password";
$dbServer = "localhost";
$dbName   = "coms_shop";

//creates a connection with mysql server
$sqlCon = new mysqli($dbServer, $username, $password, $dbName);

// error check for connection
if ($sqlCon->connect_error) {
	die("Connection failed: " . $sqlCon->connect_error);
	//echo "failed";
} else {
	//echo("Connected successfully<br>");
}



?>