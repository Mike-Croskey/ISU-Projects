<?php 

session_start();
session_regenerate_id(true);

include "dbConnecter.php";
include "osslencpt.php";

$fetch = file_get_contents("php://input");

$object = json_decode($fetch);

$user = $object->user;
$pass = $object->pass;

$sqlQuery = $sqlCon->prepare("SELECT pass FROM user WHERE user = ?");
$sqlQuery->bind_param('s', $user);

$sqlErr = $sqlQuery->execute();

$sqlQuery->bind_result($dbPass);

$sqlQuery->fetch();

$chk;


if(sslDcrypt($dbPass,$pass) === $pass){
	
	$chk['result']['pass'] = 'valid';
	$_SESSION['uNme'] = $user;
	$_SESSION['auth'] = 'valid42';
	echo json_encode($chk);

}


/* what i was doing...
if ($result->num_rows > 0) {

	//checks rows of checkout history to see if book is availible for return
	while($row = $result->fetch_assoc()) {

		if($row["usrname"] == $usrName){
		
			$chk['result']['user'] = 'valid';
				
				
			if(md5($usrPass)==$row["password"]){
		
				$chk['result']['pass'] = 'valid';
				$_SESSION['uNme'] = $usrName;
		
		
				$sql = "SELECT libr from users WHERE usrname='$usrName'";
		
				$result = $sqlCon->query($sql);
					
				if ($result->num_rows > 0) {
					// output data of each row
					while($row = $result->fetch_assoc()) {
						if($row["libr"] === "on"){
							$chk['result']['libr']='true';
							return $chk;
						}else{
							$chk['result']['libr']='false';
							return $chk;
						}
					}
				}
		
			}else{
		
				$chk['result']['pass'] = 'invalid';
				return $chk;
			}
		
		}else{
		
			$chk['result']['user'] = 'invalid';
		
		}
		
	}
}else{
		
	$chk['result']['check'] = "invalid";
}

*/


?>