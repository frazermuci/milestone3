var clientAddress = "127.0.0.1";
var clientPort = "21234";

function Socket(model){
	console.log("Socket.hello");
	this.model = model;
	this.connection = new WebSocket('ws://'+clientAddress+':'+clientPort, ['soap', 'xmpp']);
	


// Log errors
	//this.scoreArray = [0,0];
	//this.count = 1;
	this.connection.onerror = function (error) {
		console.log('WebSocket Error ' + error);
	};
	
	this.sendMessage = function(inc)
	{
		console.log('Socket.sendMessage [' + inc + ']');
		this.connection.send(inc);
	}
// Log messages from the server
	this.connection.onmessage = (e)=> {
		console.log('Socket.onmessage [' + e.data + ']');
		//this is in scope?
		var array = e.data.split(":");
/* 		if(this.count)
		{
		//assumes client 0 based ids
			var ids = this.model.getIds();
			
			console.log("init:"+ids[0]+":"+ids[1]);
			this.connection.send("init:"+ids[0]+":"+ids[1]);
		} */
		if (array[0] == "init")
		{
			console.log("Socket.onmessage.init");
			//array[1] = model.snakeID;
			this.sendMessage("init:" + model.snakeID);
		}
		else if(array[0] == "start")
		{
			console.log("Socket.onmessage.start");
			parseInt(array[1]);
			getModel().snakeIndex = parseInt(array[2]);
			window.setTimeout(ControllerTick, 50);
		}
		else 
		{
      console.log("Socket.onmessage.else");
			this.deserialize(array[0]);
			ViewRefresh();
			window.setTimeout(ControllerTick, 750);
		}
		//console.log(e.data)
		//{
			//deserialize(array);
			//sendMessage(serialize(model));
		//}
		//this.scoreArray[0] = array[0];
		//this.scoreArray[1] = array[1];
		//this.model.setScore(this.scoreArray);
		//this.count =0;
		ViewRefresh();
	}
	
	this.deserialize = function(s)
	{
    var a = s[0];
    var s1Dir = new Vector(1,0);
    var s1Bonus = false;
    var s1Loss = false;
    var s2Dir = new Vector(1,0);
    var s2Bonus=false;
    var s2Loss=false;
    var s1BonusX = 0;
    var s1BonusY = 0;
    var s2BonusX = 0;
    var s2BonusY = 0;
    console.log("client.deserialize.in [" + a + "]");
    
    // s1Dir s1Loss s1Bonus s2Dir s2Loss s2Bonus
    if(a == '0') // Right
    {
      console.log("client.deserialize.RIGHT 1");
      s1Dir.setX(1);
      s1Dir.setY(0);
    }
    else if(a == '1') // Up
    {
      console.log("client.deserialize.UP 1");
      s1Dir.setX(0);
      s1Dir.setY(-1);
    }
    else if (a == '2') // Left
    {
      console.log("client.deserialize.LEFT 1");
      s1Dir.setX(-1);
      s1Dir.setY(0);
    }
    else if (a == '3')// Down
    {
      console.log("client.deserialize.DOWN 1");
      s1Dir.setX(0);
      s1Dir.setY(1);
    }
    else
    {
      console.log("ERROR direction 1 coding");
      s1Dir.setX(0);
      s1Dir.setY(1);
    }
    
    a = s[1];
    if(a == '1')
    {
      console.log("client.deserialize.LOSS 1");
      s1Loss = true;
    }
    
    if(s[2] != 'F')
    {
      s1Bonus = true;
      s1BonusX = s[2] - '0';
      s1BonusY = s[3] - '0'; 
      console.log("client.deserialize.BONUS 1 [" + s1BonusX + "]" );
    }
    
    a = s[4];
    if(a == '0') // Right
    {
      console.log("client.deserialize.RIGHT 2");
      s2Dir.setX(1);
      s2Dir.setY(0);
    }
    else if(a == '1') // Up
    {
      console.log("client.deserialize.UP 2");
      s2Dir.setX(0);
      s2Dir.setY(-1);
    }
    else if (a == '2') // Left
    {
      console.log("client.deserialize.LEFT 2");
      s2Dir.setX(-1);
      s2Dir.setY(0);
    }
    else if (a == '3')// Down
    {
      console.log("client.deserialize.DOWN 2");
      s2Dir.setX(0);
      s2Dir.setY(1);
    }
    else
    {
      console.log("ERROR direction 2 coding");
      s2Dir.setX(0);
      s2Dir.setY(1);
    }
    
    a = s[5];
    if(a == '1')
    {
      console.log("client.deserialize.LOSS 2");
      s2Loss = true;
    }
    
    if(s[2] != 'F')
    {
      s2Bonus = true;
      s2BonusX = s[2] - '0';
      s2BonusY = s[3] - '0'; 
      console.log("client.deserialize.BONUS 2 [" + s2BonusX + "]" );
    }
    
    console.log("client.deserialize.changeDirection [" + s1Dir.getX() + "]");
    
    // SET TO MODEL
    getModel().changeDirection(0, s1Dir);
    getModel().changeDirection(1, s2Dir);
    console.log("client.deserialize.growSnake");
    getModel().growSnake(0);
    getModel().growSnake(1);
    if(s1Bonus)
    {
        console.log("client.deserialize.BONUS 1set");
        getModel().getSnake(0).eatBonus();
        
        var bonusToChange = 1;
        var newBonusPos = new Vector(s1BonusX, s1BonusY);
        var snake1Head = getModel().getSnake(0).getHead();
        
        if(snake1Head.equals(getModel().getBonuses()[0]))
            bonusToChange = 0;
        
        getModel().getBonuses()[bonusToChange] = newBonusPos;
    }
    if(s2Bonus)
    {
        console.log("client.deserialize.BONUS 2set");
        getModel().getSnake(1).eatBonus();
        
        var bonusToChange = 1;
        var newBonusPos = new Vector(s2BonusX, s2BonusY);
        var snake1Head = getModel().getSnake(0).getHead();
        
        if(snake1Head.equals(getModel().getBonuses()[0]))
            bonusToChange = 0;
        
        
        getModel().getBonuses()[bonusToChange] = newBonusPos;
    }
    
    console.log("client.deserialize.fin test");
    if(s1Loss && s2Loss)
        ControllerTie();
    else if(s1Loss)
        ControllerWin(2);
    else if(s2Loss)
        ControllerWin(1);
    console.log("client.deserialize.end");
	}
	
	this.serialize = function(model)
	{
		
    var snake = getModel().getSnake(model.snakeIndex); // TODO NEEDS TO KNOW WHAT SNAKE
    var dir = snake.direction;
    
    if(dir.equals(new Vector(1,0))) // Right
        return 64+16+4+1; // 01 01 01 01
    if(dir.equals(new Vector(0,-1))) // Up
        return 128+64+32+16+8+4+2+1; // 11 11 11 11
    if(dir.equals(new Vector(-1,0))) // Left
        return 0; // 00 00 00 00
    //if(dir.equals(new Vector(0,1))) // Down
        return 128+32+8+2; // 10 10 10 10
}


//	this.done = ()=>{this.connection.send("DONE")}
	this.done = ()=>{this.sendMessage("DONE")}
};