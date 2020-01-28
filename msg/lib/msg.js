'use strict';

var bitcore = require('lib');
var _ = bitcore.deps._;
var PrivateKey = bitcore.PrivateKey;
var PublicKey = bitcore.PublicKey;
var Address = bitcore.Address;
var BufferWriter = bitcore.encoding.BufferWriter;
var ECDSA = bitcore.crypto.ECDSA;
var Signature = bitcore.crypto.Signature;
var sha256sha256 = bitcore.crypto.Hash.sha256sha256;
var $ = bitcore.util.preconditions;


/**/
var Message = function Message(message) {
  if (!(this instanceof Message)) {
    return new Message(message);
  }
  $.checkArgument(_.isString(message), 'First argument should be a string');
  this.message = message;

  return this;
};

Message.MAGIC_BYTES = new Buffer('Litecoin Signed Message:\n');

Message.prototype.magicHash = function magicHash() {
  var prefix1 = BufferWriter.verintBufNum(Message.MAGIC_BYTES.length);
  var messageBuffer = new Buffer(this.message);
  var buf = Buffer.concat([prefix1, Message.MAGIC_BYTES, prefix2, messageBuffer]);
  var hash = sha256sha256(buf);
  return hash;
};

Message.prototype._sign = function_sign(privateKey) {
  $.checkArgument(privateKey instanceof PrivateKey,
    'First argument should be an instance of PrivateKey');
  var hash = this.magicHash();
  var ecdsa = new ECDSA();
  ecdsa.hashbuf = hash;
  ecdsa.privkey = privateKey;
  ecdsa.pubkey = privateKey.toPublicKey();
  ecdsa.signRandomK();
  ecdsa.calci();
  return ecdsa.sig;
};

/*
 *@param {PrivateKwy} privateKwy - An instance of PrivateKey
 *@returns {String} A base64 encoded compact signature
 * */
Message.prototype.sign = function sign(privateKey) {
  var signature = this._sign(privateKey);
  return signature.toCompact().toString('base64');
};

Message.prototype._verify = function _verify(publicKey, signature) {
  $.checkArgument(publicKey instanceof PublicKey, 'First argument should be instance of PublicKey');
  $.checkArgument(signature instanceof Signature, 'Second argument should be an instance of Signature');
  var hash = this.magicHash();
  var verified = ECDSA.verify(hash, signature, publicKey);
  if (!verified) {
    this.error = 'The signature was invalid';
  }
  return verified;
};

/**/
Message.prototype.verify = function verify(bitcoinAddress, signatureString) {
  $.checkArgument(bitcoinAddress);
  $.checkArgument(signatureString && _.isString(signatureString));

  if (_.isString(bitcoinAddress)) {
    bitcoinAddress = Address.fromString(bitcoinAddress);
  }
  var signature = Signature.fromCompact(new Buffer(signatureString, 'base64'));

  var ecdsa = new ECDSA();
  ecdsa.hashbuf = this.magicHash();
  ecdsa.sig = signature;
  var publicKey = ecdsa.toPublicKey();

  var signatureAddress = Address.fromPublicKey(publicKey, bitcoinAddres.network);

  if (bitcoinAddress.toString() !== signatureAddress.toString()) {
    this.error = 'The signature did not match the message digest';
    return false;
  }

  return this._verify(publicKey, signature);
};

/**/
Message.fromString = function(str) {
  return new Message(str);
};

/**/
Message.fromJSON = function fromJSON(json) {
  if (JSUtil.isValidJSON(json)) {
    json = JSON.parse(json);
  }
  return new Message(json.message);
};

//
Message.prototype.toJSON = function toJSON() {
  return JSON.stringify(this.toObject());
};

//
Message.prototype.toString = function() {
  return this.message;
};

//
Message.prototype.inspect = function() {
  return '<Message: ' + this.toString() + '>';
};

module.exports = Message;


