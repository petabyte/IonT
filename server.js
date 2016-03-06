// Load the necessary servers.
var mqtt = require('mqtt');
var moment = require('moment');
var elasticsearch = require('elasticsearch');
var schedule = require('node-schedule');
 
//Create our elasticsearch Client
var elasticSearchClient = new elasticsearch.Client({
  host: '172.31.19.77:9200',
  log: 'trace'
});

// Create our MQ Client.
var clientIdVar = "myclientid_" + parseInt(Math.random() * 100, 10);
var mqClient = mqtt.connect("mqtt://172.31.19.77:1883",{protocolId: 'MQIsdp',protocolVersion: 3});
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
     var now = moment().format();
     if(queueMessage.value > 0){
         elasticSearchClient.create({
        "index": "iot_events",
        "type":"events_messages",
         "body":{
          "event_id": clientIdVar,
          "message_type": queueMessage.type,
          "message": queueMessage.message,
          "product": queueMessage.product,
          "value": queueMessage.value,
          "postDate":now
         }
        }, function (error, response){
          console.log("This is the response " + error + " " +response);
        });
     }
 });


//Scheduled send a message back to the device

var indexVar = "iot_events";
var typeVar = "events_messages";
var queryObjVar = {"size":10000,"query": {"range":{"postDate":{"gt":"now-1d/d","lt":"now"}}}};
var jobSchedule = '/20 * * * * *';

var processHits = function(body){
   var total = 0;
   var hits = body.hits.hits;
   for (var index in hits) {
       var floatValue = parseFloat(hits[index]._source.value)
       total += floatValue;
   }
   //10 minutes practice should be good
   var totalMinutes = ((total / 1000000)/60);
   console.log("Total " + totalMinutes);
   if(totalMinutes >= 10){
      console.log("Good!");
      mqClient.publish('practice_status', 'green');
   } else if (totalMinutes >= 5 && totalMinutes <= 9){
    console.log("Not so bad!");
      mqClient.publish('practice_status', 'yellow');
   } else {
    console.log("Bad!");
      mqClient.publish('practice_status', 'red');
   }
};

var processError = function (error) {
     console.trace(error.message);
};

//Schedule the job to query elasticsearch and publish an mqtt message
var job = schedule.scheduleJob(jobSchedule, function(){
    console.log("Running Job");
    elasticSearchClient.search({index:indexVar, type:typeVar, body:queryObjVar})
    .then(processHits, processError);
});
 

// For logging purpose....
console.log( "Server is running....." );
