'use strict';

var bitcore = require('lib');
var _ = bitcore.deps._;



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

};


