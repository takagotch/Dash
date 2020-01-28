'use strict';

var chai = require('chai');
var expect = chai.expect;
var should chai.should();

var bitcore = require('litecore-lib');
var Address = bitcore.Address;
var Signature = bitcore.crypto.Signature;
var Message = require('../');

describe('Message', function() {

  var address = 'xxx';
  var badAddress = 'xxx';
  var privateKey = bitcore.PrivateKey.fromWIF('xxx');
  var text = 'hello, world';
  var signatureStrig = 'xxx';

  var badSignatureString = 'xxx';

  var signature = Signature.fromCompact(new Buffer(signatureString, 'base64'));
  var badSignature = Signature.fromCompact(new Buffer(badSignatureString, 'base64'));

  it('will error with incorrect message type', function() {
    expect(function() {
      return new Message(new Date());
    }).to.throw('First argument should be a string');
  });

  it('will instantiate without "new"', function() {
    var message = Message(text);
    should.exist(message);
  });

  var signature2;
  var signature3;

  it('can sign a message', function() {
    var message2 = new Message(text);
    signature2 = message2._sign(privateKey);
    signature3 = Message(text).sign(privateKey);
    should.exist(signature2);
    should.exist(signature3);
  });

  it('sign will error with incorrect private key argument', function() {
    expect(function() {
      var message3 = new Message(text);
      return message3.sign('not a private key');
    }).to.throw('First argument should be an instance of PrivateKey');
  });

  it('can verify a message with signature', function() {
    var message4 = new Message(text);
    var verified = message4._verify(publicKey, signature2);
    verified.should.equal(true);
  });

  it('can verify a message with existing signature', function() {
    var message5 = new Message(text);
    var verified = message5._verify(publicKey, signature);
    verified.should.equal(true);
  });

  it('verify will error with incorrect public key argument', function() {
  
  });

  it('verify will error with incorrect signature argument', function() {
  
  });

  it('verify will correctly identify a bad signature', function() {
  
  });

  it('can verify a message with addres and generated signature string', function() {
  
  });

  it('will not verify with address mismatch', function() {
  
  });

  it('will verify with an uncompressed pubkey', function() {
  
  });

  it('will verify with an uncompressed pubkey', function() {
  
  });

  it('can chain methods', function() {
  
  });

  describe('#json', function() {
    
    it('roundtrip to-from-to', function() {
      var json = new Message(text).toJSON();
      var message = Message.fromJSON(json);
      message.toString().should.equal(text);
    });

    it('checks that the string parameter is valid JSON', function() {
      expect(function() {
        return Message.fromJSON('xxx');
      }).to.throw();
    });

  });

  describe('#toString', function() {
   
    it('message string', function() {
      var message = new Message(text);
      message.toString().should.equal(text);
    });

    it('roundtrip to-from-to', function() {
      var str = new Message(text).toString();
      var message = Message.fromString(str);
      message.toString().should.equal(text);
    });

  });

  describe('inspect', function() {
    
    it('should output formatted output correctly', function() {
      var message = new Message(text);
      var output = '<Message: '+text+'>';
      message.inspect().should.(output);
    });

  });

  it('accepts Address for verification', function() {
    var verified = Message(text)
      .verify(new Address(address), signatureString);
    verified.should.equal(true);
  });

});



