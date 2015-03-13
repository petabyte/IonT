// Load the necessary servers.
var mqtt = require("mqtt");
var elasticsearch = require('elasticsearch');
 
//Create our elasticsearch Client
var elasticSearchClient = new elasticsearch.Client({
  host: 'localhost:9200',
  log: 'trace'
});

// Create our MQ Client.
var clientIdVar = "myclientid_" + parseInt(Math.random() * 100, 10);
var mqClient = mqtt.connect({host:'localhost', port:1883, clientId: clientIdVar});
mqClient.subscribe("events_messages");
//
mqClient.on('connect',  function () {
    console.log("Connected to mq!");
});
 //Gets called whenever you receive a message for your subscriptions
mqClient.on('message',  function (topic, queueMessage) {  
 	 //Log the message
     console.log("This is the topic " + topic);
 	 console.log("This is the queueMessage " + queueMessage);
     queueMessage = JSON.parse(queueMessage);
     //Do something with the push message you received
     elasticSearchClient.create({
     	"index": "events",
     	"type":"events_messages",
     	 "body":{
     	 	"event_id": clientIdVar,
     	 	"message_type": queueMessage.type,
     	 	"message": queueMessage.message,
     	 	"product": queueMessage.product
     	 }
     	}, function (error, response){
     	 	console.log("This is the response " + error + " " +response);
     	});
 });

// For logging purpose....
console.log( "Server is running....." );