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
	if (req.url == '/') {
		fs.readFile('./index.html', 'utf-8', function(error, content) {
			res.writeHead(200, {"Content-Type": "text/html"});
			res.end(content);
		});
	} else {
		fs.readFile('./'+req.url, function(error, content) {
			res.writeHead(200, {"Content-Type": "image/png"});
			res.end(content);
		});
	}
});

// Chargement de socket.io
var io = require('socket.io').listen(server);

// Quand un client se connecte, on le note dans la console
io.sockets.on('connection', function (socket) {
    console.log('[MONITORING SERVER] client connecté - '+socket.id);
	
	var checkState = setInterval(function () {
		var currentStateId = getInt(30);
		if(currentStateId != previousStateId) {
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
			cnt++;		// Demander à Didier pourquoi ce compteur ?
		}
		previousStateId = currentStateId;
	}, 10);		// Execution toutes les 10ms

	var fluxCamera = setInterval(function () {
		fs.readFile('./camera.jpeg', function(error, content) {
			socket.volatile.emit('fluxCamera', {image:true, buffer:content.toString('base64')});
		});
	}, 100);	// Execution toutes les 100ms

	socket.on('disconnect', function () {
		console.log('client ('+socket.pseudo+') ['+socket.id+'] déconnecté.');
		clearInterval(checkState);
	});
});

console.log('Server starting...');
server.listen(8080);
console.log('Server started - Running on http://localhost:8080 ');
