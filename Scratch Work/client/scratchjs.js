var ID = prompt("Enter your name (Max 8 characters)", "New001").substr(0, 8);

var FancyWebSocket = function(url)
{
	var callbacks = {};
	var ws_url = url;
	var conn;

	this.bind = function(event_name, callback){
		callbacks[event_name] = callbacks[event_name] || [];
		callbacks[event_name].push(callback);
		return this;// chainable
	};

	this.send = function(event_name, event_data){
		this.conn.send( event_data );
		return this;
	};

	this.connect = function() {
		if ( typeof(MozWebSocket) == 'function' )
			this.conn = new MozWebSocket(url);
		else
			this.conn = new WebSocket(url);

		// dispatch to the right handlers
		this.conn.onmessage = function(evt){
			dispatch('message', evt.data);
		};

		this.conn.onclose = function(){dispatch('close',null)}
		this.conn.onopen = function(){dispatch('open',null)}
	};

	this.disconnect = function() {
		this.conn.close();
	};

	var dispatch = function(event_name, message){
		var chain = callbacks[event_name];
		if(typeof chain == 'undefined') return; // no callbacks for this event
		for(var i = 0; i < chain.length; i++){
			chain[i]( message )
		}
	}
};

var Server;



		function log( text ) {
					$log = $('#log');
					//Add text to log
					$log.append(($log.val()?"\n":'')+text);
					//Autoscroll
					$log[0].scrollTop = $log[0].scrollHeight - $log[0].clientHeight;
		}

		function send( text ) {
			Server.send( 'message', text );
		}

        function connect(){
            log('Connecting...');
			Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);

			$('#message').keypress(function(e) {
				if ( e.keyCode == 13 && this.value ) {
					log( 'You: ' + this.value );
					send( this.value );

					$(this).val('');
				}
			});

			//Let the user know we're connected
			Server.bind('open', function() {
                document.getElementById("cntBtn").disabled = true;
				log( "Connected." );
				send("ID:" + ID);
			});

			//OH NOES! Disconnection occurred.
			Server.bind('close', function( data ) {
                document.getElementById("cntBtn").disabled = false;
				log( "Disconnected." );
			});

			//Log any messages sent from server
			Server.bind('message', function( payload ) {
				//log( payload );
				try{
					var split = payload.split("|");
					for (i = 0; i < split.length; i++){
						var split2 = split[i].split(":");
						if (split2[0][0] == "p"){
							var split3 = split2[1].split(",");
							player.paddle.x = Number(split3[0]);
							player.paddle.y = Number(split3[1]);
						}
						else if (split2[0][0] == "s"){
							Score = Number(split2[1]);
							document.getElementById('score').innerHTML = "Score: " + Score;
						}
						else if (split2[0] == "bp"){
							var split3 = split2[1].split(",");
							ball.x = Number(split3[0]);
							ball.y = Number(split3[1]);
						}
						else{
							//log("THIS IS NOT PROPER PROTOCOL/ ITS THE WELCOME HANDSHAKE");
						}
					}
				} catch(e){//log("ERROR");
				}
			});

			Server.connect();
        }
		



var animate = window.requestAnimationFrame ||
  window.webkitRequestAnimationFrame ||
  window.mozRequestAnimationFrame ||
  function(callback) { window.setTimeout(callback, 1000/60) };
var canvas = document.createElement('canvas');
var width = 600;
var height = 600;
canvas.width = width;
canvas.height = height;
var context = canvas.getContext('2d');
window.onload = function() {
  document.body.appendChild(canvas);
  animate(step);
};

var step = function() {
  update();
  render();
  animate(step);
};

function Paddle(x, y, width, height) {
  this.x = x;
  this.y = y;
  this.width = width;
  this.height = height;
  this.x_speed = 0;
  this.y_speed = 0;
}

Paddle.prototype.render = function() {
  context.fillStyle = "#0000FF";
  context.fillRect(this.x, this.y, this.width, this.height);
};

function Player() {
   this.paddle = new Paddle(width/2 - 25, height - 20, 50, 10);
}

/*function Computer() {
  this.paddle = new Paddle(width/2 - 25, 10, 50, 10);
}*/

Player.prototype.render = function() {
  this.paddle.render();
};

/*Computer.prototype.render = function() {
  this.paddle.render();
};*/

function Ball(x, y) {
  this.x = x;
  this.y = y;
  this.x_speed = 0;
  this.y_speed = 3;
  this.radius = 5;
}

Ball.prototype.render = function() {
  context.beginPath();
  context.arc(this.x, this.y, this.radius, 2 * Math.PI, false);
  context.fillStyle = "#000000";
  context.fill();
};

var player = new Player();
var Score = 0;
//var computer = new Computer();
var ball = new Ball(width/2, height/2);

var render = function() {
  context.fillStyle = "#AAAAAA";
  context.fillRect(0, 0, width, height);
  player.render();
  //computer.render();
  ball.render();
};

Ball.prototype.update = function(paddle1, paddle2) {
  /*this.x += this.x_speed;
  this.y += this.y_speed;
  var top_x = this.x - this.radius;
  var top_y = this.y - this.radius;
  var bottom_x = this.x + this.radius;
  var bottom_y = this.y + this.radius;

  if(this.x - this.radius < 0) { // hitting the left wall
    this.x = this.radius;
    this.x_speed = -this.x_speed;
  } else if(this.x + this.radius > width) { // hitting the right wall
    this.x = width - this.radius;
    this.x_speed = -this.x_speed;
  }

  if(this.y < 0 || this.y > height) { // a point was scored
    this.x_speed = 0;
    this.y_speed = 3;
    this.x = width/2;
    this.y = height/2;
  }

  if(top_y > height/2) {
    if(top_y < (paddle1.y + paddle1.height) && bottom_y > paddle1.y && top_x < (paddle1.x + paddle1.width) && bottom_x > paddle1.x) {
      // hit the player's paddle
      this.y_speed = -3;
      this.x_speed += (paddle1.x_speed / 2);
      this.y += this.y_speed;
    }
  } else {
    if(top_y < (paddle2.y + paddle2.height) && bottom_y > paddle2.y && top_x < (paddle2.x + paddle2.width) && bottom_x > paddle2.x) {
      // hit the computer's paddle
      this.y_speed = 3;
      this.x_speed += (paddle2.x_speed / 2);
      this.y += this.y_speed;
    }
  }*/
};

var keysDown = {};
var leftdown = false;
var rightdown = false;

window.addEventListener("keydown", function(event) {
  keysDown[event.keyCode] = true;
});

window.addEventListener("keyup", function(event) {
	var value = event.keyCode;   
  delete keysDown[event.keyCode];
  if (value == 37) { // Left arrow
  leftdown = false;
  send('LU');} 
  else if (value == 39) { // right arrow
      rightdown = false;
	  send('RU');}
});

Player.prototype.update = function() {
  for(var key in keysDown) {
    var value = Number(key);
    if(value == 37 && !leftdown) { // left arrow
	  send('LD');
	  leftdown = true;
    } else if (value == 39 && !rightdown) { // right arrow
	  send('RD');
	  rightdown = true;
    }
  }
};

Paddle.prototype.move = function(x, y) {
  this.x += x;
  this.y += y;
  this.x_speed = x;
  this.y_speed = y;
  if(this.x < 0) { // all the way to the left
    this.x = 0;
    this.x_speed = 0;
  } else if (this.x + this.width > width) { // all the way to the right
    this.x = width - this.width;
    this.x_speed = 0;
  }
}

var update = function() {
  player.update();
  //computer.update(ball);
  //ball.update(player.paddle, computer.paddle);
};

/*Computer.prototype.update = function(ball) {
  var x_pos = ball.x;
  var diff = -((this.paddle.x + (this.paddle.width / 2)) - x_pos);
  if(diff < 0 && diff < -4) { // max speed left
    diff = -5;
  } else if(diff > 0 && diff > 4) { // max speed right
    diff = 5;
  }
  this.paddle.move(diff, 0);
  if(this.paddle.x < 0) {
    this.paddle.x = 0;
  } else if (this.paddle.x + this.paddle.width > width) {
    this.paddle.x = width - this.paddle.width;
  }
};*/