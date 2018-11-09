// https://openclassrooms.com/fr/courses/1056721-des-applications-ultra-rapides-avec-node-js/1057825-socket-io-passez-au-temps-reel
// Lancement (terminal) : node app.js
// Puis depuis un navigateur : http://localhost:8080

var http = require('http');
var fs = require('fs');

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
			content : 'Message à tous : '+socket.pseudo+' vient d\'arriver',
			importance: '2'
		}
	);
	
	// Quand le serveur reçoit un signal de type "message" du client    
    socket.on('message', function (message) {
        console.log(socket.pseudo+' me parle ! Il me dit : ' + message);
    });	
	
	// Stocker des information dans le socket (sorte d'infos de sessions simplifiées - voir session.socket.io pour faire propre : https://www.npmjs.com/package/session.socket.io )
	socket.on('petit_nouveau', function(pseudo){
		socket.pseudo = pseudo;
		console.log('Nouveau nom : '+socket.pseudo);
	});
	
	// Messages volatiles
	//var tweets = setInterval(function () {
		//console.log('Envoi message volatile à ('+socket.pseudo+')');
		//socket.volatile.emit('volatile-message', new Date().toLocaleString());
	//}, 4000);
	
	var randomJauge = setInterval(function () {
		//console.log('Envoi jauge voltile à ('+socket.pseudo+')');
		socket.volatile.emit('volatile-jauge', Math.floor(Math.random() * 100));
	}, 100);
	

	socket.on('disconnect', function () {
		console.log('client ('+socket.pseudo+') ['+socket.id+'] déconnecté.');
		//clearInterval(tweets);
		clearInterval(randomJauge);
	});
});

console.log('Server start');
server.listen(8080);
console.log('Server started');