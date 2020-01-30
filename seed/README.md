
```sh
./mnemonic < wallet.seed > 12xxx
cat 12xxx

./mnemonic 12xxx wallet.seed
cat wallet.seed

./newseed > wallet.seed
cat wallet.seed

./genpriv 0 < wallet.seed
./genpub 0 < wallet.seed

cat master_public_key
./genpub 0 < master_public_key

./newkey > private.key
cat private.key

./addr < private

```
