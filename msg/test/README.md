
```
npm install litecore-message
bower install litecore-message
```

```
var litecore = require('litecore-lib');
var Message = require('litecore-message');

var privateKey = litecore.PrivateKey.fromWIF('xxx');
var signature = Message('hello, world').sign(privateKey);


var address = 'xxx';
var signature = 'xxx';
var verified = Message('hello, world').verify(address, signature);

```


