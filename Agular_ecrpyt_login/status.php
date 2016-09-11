<?php 
    session_start();

    $res = array();
    
	$s = 'invalid';
	if(isset($_SESSION['auth'])){
		
		$s = $_SESSION['auth'];
		
	}

	if($s==='valid42'){
		$res['session'] = 'true';
	}else{
		$res['session'] = 'false';
	}
	
	echo json_encode($res);

?>