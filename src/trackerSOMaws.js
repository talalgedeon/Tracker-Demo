var Particle = require('particle-api-js');
var particle = new Particle();


console.log('Loading function');

exports.handler = async (event, context) => {

    var fnPr = particle.callFunction({ deviceId: event.deviceId, name: 'useCaseState', argument: event.argument, auth: event.auth });

    fnPr.then(
    function(data) {
        callback(null, data);
    }, function(err) {
        callback(err, null);
    });

};