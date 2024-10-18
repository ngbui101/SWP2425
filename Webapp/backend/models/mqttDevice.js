const awsIot = require('aws-iot-device-sdk');

const device = awsIot.device({
  clientId: 'WebApp',
  host: 'a336z3b6pu6hdu-ats.iot.us-east-1.amazonaws.com',
  port: 8883,
  keyPath: './AWS_Cert/private.pem.key',
  certPath: './AWS_Cert/certificate.pem.crt',
  caPath: './AWS_Cert/AmazonRootCA1.pem',
});

device.on('connect', function () {
  console.log('Connected to AWS IoT Core');
});

module.exports = device;
