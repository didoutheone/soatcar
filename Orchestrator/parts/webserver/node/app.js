/*
    1 - Installer NODE-JS selon l'environnment du serveur (Mac, Windows, RaspberryPi...)
    1.1 -- S'assurer que le fichier "package.json" est bien dans le répertoire du projet et qu'on est positionné sur ce répertoire.
    2 - Installer le module SOCKET.IO (commande sur le serveur : npm install socket.io)

*/

// https://openclassrooms.com/fr/courses/1056721-des-applications-ultra-rapides-avec-node-js/1057825-socket-io-passez-au-temps-reel
// Lancement (terminal) : node app.js
// Puis depuis un navigateur : http://localhost:8080

var http = require('http');
var fs = require('fs');
var mmap = require('./mmap-io/mmap-io');

var cnt = 0;
var buffer;
var state = {
	outputSteering: 0,
	mode: "AUTO",
	outputThrottle: 0,
	Id: 0
};
var previousStateId = 0;


var initMMap = function() {
	var fd = fs.openSync("/var/tmp/soatcarmmf.tmp", "r+");
	var size = fs.statSync("/var/tmp/soatcarmmf.tmp").size;
	buffer = mmap.map(size, mmap.PROT_WRITE, mmap.MAP_SHARED, fd);
	fs.closeSync(fd);
	return 0;
}

var getInt = function(offset) {
	return (buffer[offset + 3] << 24) | (buffer[offset + 2] << 16) | (buffer[offset + 1] << 8) | buffer[offset + 0];
};


initMMap();



// Chargement du fichier index.html affiché au client
var server = http.createServer(function(req, res) {
    fs.readFile('./index.html', 'utf-8', function(error, content) {
        res.writeHead(200, {"Content-Type": "text/html"});
        res.end(content);
    });
});

// Chargement de socket.io
var io = require('socket.io').listen(server);

// Quand un client se connecte, on le note dans la console
io.sockets.on('connection', function (socket) {
    console.log('Un client est connecté ! '+socket.id);
	
	socket.emit(
		'message', 
		{
			content: 'Vous êtes (vraiment) bien connecté !', 
			importance: '14'
		}
	);
	
	// Envoyer un message à tout le monde, sauf celui qui le génère
	socket.broadcast.emit(
		'message',
		{
			content : 'Message à tous : Nouvelle connexion ('+socket.id+')',
			importance: '1'
		}
	);
	
	// Quand le serveur reçoit un signal de type "UPDATE_STATUS" du client    
    socket.on('update_status', function (message) {
		console.log(socket.pseudo+' met à jour mon status : ' + message);
		socket.broadcast.emit(
		    'update_status',
            {
				author : socket.pseudo,
				status : message,
                importance : '2'
            }
	    );
		
	});	
	
	// Quand le serveur reçoit un signal de type "message" du client    
    socket.on('message', function (message) {
        console.log(socket.pseudo+' me parle ! Il me dit : ' + message);
		socket.broadcast.emit(
			'message',
			{
				content : socket.pseudo+' dit : ' + message,
				importance: '3'
			}
		);
    });	
	
	// Stocker des information dans le socket (sorte d'infos de sessions simplifiées - voir session.socket.io pour faire propre : https://www.npmjs.com/package/session.socket.io )
	socket.on('petit_nouveau', function(pseudo){
		socket.pseudo = pseudo;
        console.log('Nouveau nom : '+socket.pseudo);
        
        // Envoyer un message à tout le monde, sauf celui qui le génère
	    socket.broadcast.emit(
		    'message',
            {
                content : 'Message à tous : '+socket.pseudo+' vient d\'arriver',
                importance: '2'
            }
	    );
	});
	
	// Messages volatiles : peuvent etre perdus
	var randomJauge = setInterval(function () {
		socket.volatile.emit('volatile-jauge', Math.floor(Math.random() * 100));
	}, 10000);


	var checkState = setInterval(function () {
		var inputsNo = getInt(30);
		if(inputsNo != previousStateId) {
			state.outputSteering = getInt(0);
			state.mode = "AUTO";
			state.outputThrottle = getInt(10);
			state.Id = cnt;
			console.log('will emit ' + JSON.stringify(state));
			socket.emit(
				'update_status',
				{
					author : socket.pseudo,
					status : JSON.stringify(state),
					importance : '2'
				}
			);
			cnt++;
		}
		previousStateId = inputsNo;
	}, 10);


	socket.on('disconnect', function () {
		console.log('client ('+socket.pseudo+') ['+socket.id+'] déconnecté.');
		clearInterval(randomJauge);
		clearInterval(checkState);
	});
});

console.log('Server start');
server.listen(8080);
console.log('Server started \n (Press CTRL+C to quit) \n Running on http://localhost:8080 ');
