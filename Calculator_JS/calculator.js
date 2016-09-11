//CALCULATOR.JS





var Calc = {

		Model : {

			keyFlag : 0,
			resultFlag : 0,
			oldValue : undefined,
			oppValue : -1,
			newValue : undefined,
			resValue : undefined,
			memValue : 0
			
			
		},


		View : {
			
			textRow : {id: "textRow", type: "text", value: "", onclick:""},
			memRow : {id: "memRow", type: "text", value: "", onclick:""},
			
			//

			btnMMr : {id: "btnMMr", type: "button", value: "MR", onclick:""},
			btnMMn : {id: "btnMMn", type: "button", value: "M-", onclick:""},			
			btnMPl : {id: "btnMPl", type: "button", value: "M+", onclick:""},
			btnMCr : {id: "btnMCr", type: "button", value: "MC", onclick:""},
			btnClr : {id: "btnClr", type: "button", value: "C", onclick:""},
			
			
			
			//math fuction buttons
			
			btnEql : {id: "btnEql", type: "button", value: "=", onclick:""},			
			btnDec : {id: "btnDec", type: "button", value: ".", onclick:""},
			btnAdd : {id: "btnAdd", type: "button", value: "+", onclick:""},
			btnSub : {id: "btnSub", type: "button", value: "-", onclick:""},
			btnMul : {id: "btnMul", type: "button", value: "*", onclick:""},
			btnDiv : {id: "btnDiv", type: "button", value: "/", onclick:""},
			
			//number buttons
			
			btn9 : {id: "btn9", type: "button", value: 9, onclick:""},
			btn8 : {id: "btn8", type: "button", value: 8, onclick:""},
			btn7 : {id: "btn7", type: "button", value: 7, onclick:""},
			btn6 : {id: "btn6", type: "button", value: 6, onclick:""},
			btn5 : {id: "btn5", type: "button", value: 5, onclick:""},
			btn4 : {id: "btn4", type: "button", value: 4, onclick:""},
			btn3 : {id: "btn3", type: "button", value: 3, onclick:""},
			btn2 : {id: "btn2", type: "button", value: 2, onclick:""},
			btn1 : {id: "btn1", type: "button", value: 1, onclick:""},
			btn0 : {id: "btn0", type: "button", value: 0, onclick:""}
		},

		run : function() {
			Calc.attachHandlers();
			console.log(Calc.display());
			return Calc.display();
		},


		displayElement : function (element) {
			var s = "<input ";
			s += " id=\"" + element.id + "\"";
			s += " type=\"" + element.type + "\"";
			s += " value= \"" + element.value + "\"";
			s += " onclick= \"" + element.onclick + "\"";
			s += ">";
			return s;

		},

		display : function() {
			var s;
			s = "<table id=\"myTable\" border=3>"
				s += "<tr><td>" + Calc.displayElement(Calc.View.textRow) + "</td></tr>";
				s += "<tr><td>" + Calc.displayElement(Calc.View.memRow) + "</td></tr>";

			s += "<tr><td>";
			s += Calc.displayElement(Calc.View.btn7);
			s += Calc.displayElement(Calc.View.btn8);
			s += Calc.displayElement(Calc.View.btn9);
			s += Calc.displayElement(Calc.View.btnAdd);
			s += "<tr><td>";
			s += Calc.displayElement(Calc.View.btn4);
			s += Calc.displayElement(Calc.View.btn5);
			s += Calc.displayElement(Calc.View.btn6);
			s += Calc.displayElement(Calc.View.btnSub);
			s += "<tr><td>";
			s += Calc.displayElement(Calc.View.btn1);
			s += Calc.displayElement(Calc.View.btn2);
			s += Calc.displayElement(Calc.View.btn3);
			s += Calc.displayElement(Calc.View.btnMul);
			s += "<tr><td>";
			s += Calc.displayElement(Calc.View.btn0);
			s += Calc.displayElement(Calc.View.btnDec);
			s += Calc.displayElement(Calc.View.btnEql);
			s += Calc.displayElement(Calc.View.btnDiv);
			s += "<tr><td>";
			s += Calc.displayElement(Calc.View.btnClr);
			s += Calc.displayElement(Calc.View.btnMCr);
			s += Calc.displayElement(Calc.View.btnMMn);
			s += Calc.displayElement(Calc.View.btnMPl);
			s += Calc.displayElement(Calc.View.btnMMr);
			s += "</tr></td></table>";
			return s;
		},

		attachHandlers : function() {

			Calc.View.btnMMr.onclick = "Calc.btnMMrHdlr()";
			Calc.View.btnClr.onclick = "Calc.btnClrHdlr()"; 
			Calc.View.btnMCr.onclick = "Calc.btnMCrHdlr()"; 
			Calc.View.btnMMn.onclick = "Calc.btnMMnHdlr()"; 
			Calc.View.btnMPl.onclick = "Calc.btnMPlHdlr()"; 
			
			
			Calc.View.btnAdd.onclick = "Calc.btnAddHdlr()"; 
			Calc.View.btnSub.onclick = "Calc.btnSubHdlr()"; 
			Calc.View.btnMul.onclick = "Calc.btnMulHdlr()"; 
			Calc.View.btnDiv.onclick = "Calc.btnDivHdlr()"; 
			Calc.View.btnEql.onclick = "Calc.btnEqlHdlr()"; 
			Calc.View.btnDec.onclick = "Calc.btnDecHdlr()"; 
			
			
			for(var b=0;b<10;b++){
				
				Calc.View["btn"+b].onclick = "Calc.btn"+b+"Hdlr()";
				
			}
			


		},
		
		
		Controller : {
			result : 0,
			add : function() {
				//alert((Calc.Model.oldValue)+""+(Calc.Model.newValue));
				if(Calc.Model.resultFlag==0){
					return parseFloat(Calc.Model.oldValue)+parseFloat(Calc.Model.newValue);
				}else{
					return parseFloat(Calc.Model.resValue)+parseFloat(Calc.Model.newValue);

				}
			},
			sub : function() {
				//alert((Calc.Model.oldValue)+""+(Calc.Model.newValue));	
				if(Calc.Model.resultFlag==0){
					return parseFloat(Calc.Model.oldValue)-parseFloat(Calc.Model.newValue);
				}else{
					return parseFloat(Calc.Model.resValue)-parseFloat(Calc.Model.newValue);

				}
			},
			mul : function() {
				if(Calc.Model.resultFlag==0){
					return parseFloat(Calc.Model.oldValue)*parseFloat(Calc.Model.newValue);
				}else{
					return parseFloat(Calc.Model.resValue)*parseFloat(Calc.Model.newValue);

				}
			},
			div : function() {
				
				if(Calc.Model.newValue == 0){
					alert("Error :: Division by 0");
				}else{				
					if(Calc.Model.resultFlag==0){
						return parseFloat(Calc.Model.oldValue)/parseFloat(Calc.Model.newValue);
					}else{
						return parseFloat(Calc.Model.resValue)/parseFloat(Calc.Model.newValue);

					}
				}
			}

			
		},
		
		
		btnClrHdlr : function() {
			//alert("button Clr");
			document.getElementById("textRow").value = "";
			Calc.Model.resultFlag=1;
			Calc.Model.keyFlag=0;
			Calc.Model.oldValue=0;
			Calc.Model.newValue=0;

			
			
		},
		btnMMrHdlr : function() {
			//alert("button Memory read");
			document.getElementById("memRow").value = Calc.Model.memValue;
		},
		btnMCrHdlr : function() {
			//alert("button Memory Clr");
			Calc.Model.memValue = 0;
			document.getElementById("memRow").value = "";
		},
		btnMPlHdlr : function() {
			//alert("button Memory +");
			Calc.Model.memValue = parseFloat(Calc.Model.memValue)+parseFloat(document.getElementById("textRow").value);
		},
		btnMMnHdlr : function() {
			//alert("button Memory -");
			Calc.Model.memValue = parseFloat(Calc.Model.memValue)-parseFloat(document.getElementById("textRow").value);

		},
		btnAddHdlr : function() {
			//alert("button add");
			if(Calc.Model.keyFlag==1){
				alert("Expected [0-9] [. = C] [MR M- M+]");
			}else{
				document.getElementById("textRow").value += "+";
				Calc.Model.oppValue = "+";
				Calc.Model.keyFlag=1;
			}
		},
		btnSubHdlr : function() {
			//alert("button subtact");
			if(Calc.Model.keyFlag==1){
				alert("Expected [0-9] [. = C] [MR M- M+]");
			}else{
				document.getElementById("textRow").value += "-";
				Calc.Model.oppValue = "-";
				Calc.Model.keyFlag=1;
			}
		},
		btnMulHdlr : function() {
			//alert("button multiply");
			if(Calc.Model.keyFlag==1){
				alert("Expected [0-9] [. = C] [MR M- M+]");
			}else{
				document.getElementById("textRow").value += "*";
				Calc.Model.oppValue = "*";
				Calc.Model.keyFlag=1;
			}
		},
		btnDivHdlr : function() {
			//alert("button divide");
			if(Calc.Model.keyFlag==1){
				alert("Expected [0-9] [. = C] [MR M- M+]");
			}else{
				document.getElementById("textRow").value += "/";
				Calc.Model.oppValue = "/";
				Calc.Model.keyFlag=1;
			}
		},
		btnEqlHdlr : function() {
			//alert("button equals");
			
			if(Calc.Model.oppValue==="+"){
				document.getElementById("textRow").value = Calc.Controller.add();
				Calc.Model.resValue = document.getElementById("textRow").value;
				Calc.Model.resultFlag=1;
				Calc.Model.keyFlag=0;
			}else if(Calc.Model.oppValue==="-"){
				document.getElementById("textRow").value = Calc.Controller.sub();
				Calc.Model.resValue = document.getElementById("textRow").value;
				Calc.Model.resultFlag=1;
				Calc.Model.keyFlag=0;
			}else if(Calc.Model.oppValue==="*"){
				document.getElementById("textRow").value = Calc.Controller.mul();
				Calc.Model.resValue = document.getElementById("textRow").value;
				Calc.Model.resultFlag=1;
				Calc.Model.keyFlag=0;
			}else if(Calc.Model.oppValue=="/"){
				document.getElementById("textRow").value = Calc.Controller.div();
				Calc.Model.resValue = document.getElementById("textRow").value;
				Calc.Model.resultFlag=1;
				Calc.Model.keyFlag=0;
			}else{
				alert("expexted expresion in format [val1] <opperand> [val2]");
			}
			
		},
		
		
		btnDecHdlr : function() {
		//	alert("button decimal");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = ".";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += ".";
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}

		},
		btn9Hdlr : function() {
			//alert("button 9");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "9";
				Calc.Model.resultFlag=0;

			}else{
			document.getElementById("textRow").value += "9";
			}
			
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		},

		btn8Hdlr : function() {
			//alert("button 8");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "8";
				Calc.Model.resultFlag=0;

			}else{
			document.getElementById("textRow").value += "8";
			}
			
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		},

		btn7Hdlr : function() {
			//alert("button 7");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "7";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "7";			
			}
			
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}

		},

		btn6Hdlr : function() {
			//alert("button 6");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "6";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "6";			
			}
			
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}

		},

		btn5Hdlr : function() {
			//alert("button 5");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "5";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "5";			
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		},

		btn4Hdlr : function() {
			//alert("button 4");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "4";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "4";
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		},

		btn3Hdlr : function() {
			//alert("button 3");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "3";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "3";			
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}

		},

		btn2Hdlr : function() {
			//alert("button 2");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "2";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "2";			
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		},

		btn1Hdlr : function() {
			//alert("button 1");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "1";
				Calc.Model.resultFlag=0;

			}else{
				document.getElementById("textRow").value += "1";
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		},
		btn0Hdlr : function() {
			//alert("button 0");
			if(Calc.Model.resultFlag==1){
				document.getElementById("textRow").value = "0";
				Calc.Model.resultFlag=0;
			}else{
				document.getElementById("textRow").value += "0";
			}
			if(Calc.Model.keyFlag==0){
				Calc.Model.oldValue = document.getElementById("textRow").value;
	
			}else{
				var val = document.getElementById("textRow").value;
				var i = Calc.Model.oldValue.length;
				var splitVal = val.substring(i+1,val.length);
				Calc.Model.newValue = splitVal;
			}
		}

} // end of Calc;
