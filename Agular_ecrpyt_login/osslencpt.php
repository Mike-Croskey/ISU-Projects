<?php 

 	define('AES','aes-256-cbc');
 	
	function sslEcrpt($inputData){
	
	$salt = mcrypt_create_iv(16, MCRYPT_DEV_URANDOM);
		
	$e_key = hash_pbkdf2("sha512", $inputData, $salt, 100,000, 32);
			
	//$e_key = openssl_random_pseudo_bytes(32);

	$iv = openssl_random_pseudo_bytes(openssl_cipher_iv_length(AES));

	$result = openssl_encrypt($inputData,AES,$e_key,OPENSSL_RAW_DATA,$iv);
	
	$result = $result.';'.$iv.':'.$salt;
	//print bin2hex($result);
	
	return bin2hex($result);
	
	}
	
	function sslDcrypt($inputData,$inputData2){
		
		//print $inputData;
		
		$convert = hex2bin($inputData);
		
		$split = explode(';',$convert);
		
		$split2 = explode(':',$split[1]);
		
		$e_key2 = hash_pbkdf2("sha512", $inputData2, $split2[1], 100,000, 32);
		
		$result = openssl_decrypt($split[0],AES,$e_key2,OPENSSL_RAW_DATA,$split2[0]);
	
		print "result = ".$result;
		
		return $result;
	
	}

?>