
var app = angular.module('myShopper', ['ui.router']);

app.config(function($stateProvider, $urlRouterProvider) {
	
	 $urlRouterProvider.otherwise('/login');
	
	$stateProvider
	
	.state('login',{
		url:"/login",
		templateUrl: "templates/login.html",
		controller: "myLoginCtrl"
		
	})
	.state('cart',{
		url:"/cart",
		templateUrl: "templates/cart.html",
		controller: "myCartCtrl"
		
	})
	.state('cat',{
		url:"/cat",
		templateUrl: "templates/cat.html",
		controller: "myCatCtrl"
		
	})
	
	
});


app.controller('myCatCtrl', function($scope,$http,$state) {

	
	$http.post("status.php").success(function(res){
		console.log(res);
	
		if(res.session != 'true'){
			$state.go("login");
		}
	
	
	}).error(function(err){
		console.log(err);
	});	
	
	$scope.logout = function () {
		
		$http.post("logout.php").success(function(res){
			console.log(res);
			$state.go("login");
		}).error(function(err){
			console.log(err);
		});	
		
		
	}
	
	/*
	$scope.showMe = false;
	$scope.dysItems = function() {
		$scope.showMe = !$scope.showMe;
	}*/

	
});

app.controller('myCartCtrl', function($scope,$http,$state) {

		$http.post("status.php").success(function(res){
				console.log(res);
			
				if(res.session != 'true'){
					$state.go("login");
				}
			
			
			}).error(function(err){
				console.log(err);
			});	
		
		
		$scope.logout = function () {
			
			$http.post("logout.php").success(function(res){
				console.log(res);
				$state.go("login");
			}).error(function(err){
				console.log(err);
			});	
			
			
		}
		
});

app.controller('myLoginCtrl', function($scope,$http,$state) {
	
	$scope.loginDet = {
		username : undefined,
		password : undefined
	}
	
	$scope.registerDet = {
			
		username : undefined,
		password : undefined	

	}
	
	
	$scope.login = function () { 
		
		var info = {
				
				user : $scope.loginDet.username,
				pass : $scope.loginDet.password
				
				
		}
		
		$http.post("login.php", info).success(function(res){
			console.log(res);
			$state.go("cart");
		}).error(function(err){
			console.log(err);
		});	
		
		
		
	}
	

	$scope.register = function(){
		
		var info = {
				
				user : $scope.registerDet.username,
				pass : $scope.registerDet.password	
		}
	
		$http.post("register.php", info).success(function(res){
			console.log(res);
		}).error(function(err){
			console.log(err);
		});	
		
		
		
	}
	
});
