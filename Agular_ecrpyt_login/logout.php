<?php 

session_start();

$_SESSION['uNme'] = null;
$_SESSION['auth'] = 'invalid';
		
session_destroy();		


?>