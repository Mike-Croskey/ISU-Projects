<?php 
	include "dbConnecter.php";
	include "osslencpt.php";
	
	$fetch = file_get_contents("php://input");
	
	$object = json_decode($fetch);
	
	$user = $object->user;
	$pass = $object->pass;
	
	$pass = sslEcrpt($pass);
	
	$sql = $sqlCon->prepare("INSERT INTO user (user,pass) VALUES (?,?)");
	$sql->bind_param('ss', $user , $pass);
	
	$result = $sql->execute();
	
	echo $fetch;

?>