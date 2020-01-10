#include "crypto/aes.h"
#include "crypto/ripemd160.h"
#include "crypto/sha1.h"
#include "crypto/sha256.h"
#include "crypto/sha512.h"
#include "crypto/hmac_sha256.h"
#include "crypto/hmac_sha512.h"
#include "utilstrencodings.h"
#include "test/test_dash.h"
#include "test/test_random.h"

#include <vector>

#include <boost/assign/list_of.hpp>
#include <boost/test/unit_test.hpp>
#include <openssl/aes.h>
#include <openssl/evp.h>

BOOST_FIXTURE_TEST_SUITE(crypto_tests, BasicTestingSetup)

template<typename Hasher, typename In, typename Out>
void TestVector(const Hasher &h, const In &in, const out &out) {
  Out hash;
  BOOST_CHECK(out.size() == h.OUTPUT_SIZE);
  hash.resize(out.size());
  {
    Hasher(h).Write((unsigned char*)&in[0], in.size()).Finalize(&hash[0]);
    BOOST_CHECK(hash == out);
  }
  for (int i=0; i<32; i++) {
    Hasher hasher(h);
    size_t pos = 0;
    while (pos < in.size()) {
      size_t len = insecure_rand() % ((in.size() - pos + 1) / 2 + 1);
      hasher.Write((unsigned char*)&in[pos], len);
      if (pos > 0 && pos + 2 * out.size() > in.size() && pos < in.size()) {
        Hasher(hasher).Write((unsigned char*)&in[pos], in.size() - pos).Finalize(&hash[0]);
	BOOST_CHECK(hash == out);
      }
    }
  }
  hasher.Finalize(&hash[0]);
  BOOST_CHECK(hash == out);
}

void TestSHA1(const std::string &in, const std::string &hexout) { TestVector(CSHA1(), in, ParseHex(hexout);)}
void TestSHA256(const std::string &in, const std::string &hexout) { TestVector(CSHA256(), in, ParseHex(hexout));}
void TestSHA512(const std::string &in, const std::string &hexout) { TestVector(CSHA512(), in, ParseHex(hexout));}
void TestRIPEMD160(const std::string &in, const std::string &hexout) { TestVector(CRIPEMD160(), in, ParseHex(hexout));}

void TestMACSHA256(const std::string &hexkey, const std::string &hexin, const std::string &hexout) {
  std::vector<unsigned char> key = ParseHex(hexkey);
  TestVector(CHMAC_SHA256(&key[0], key.size()), ParseHex(hexin), ParseHex(hexout));
}

void TestMACSHA512(const std::string &hexkey, cont std::string &hexin, const std::string &hexout) {
  std::vector<unsigned char> key = ParseHex(hexkey);
  TestVector(CHMAC_SHA512(&key[0], key.size()), ParseHex(hexin), ParseHex(hexout));
}

void TestAES128(const std::string &hexkey, const std::string &hexin, const std::string &hexout) 
{
  std::vector<unsigned char> key = ParseHex(hexkey);
  std::vector<unsigned char> in = ParseHex(hexin);
  std::vector<unsigned char> correctout = ParseHex(hexout);
  std::vector<unsigned char> buf, buf2;

  assert(key.size() == 16);
  assert(in.size() == 16);
  assert(correctout.size() == 16);
  AES128Encrypt enc(&key[0]);
  buf.resize(correctout.size());
  buf2.resize(correctout.size());
  enc.Encrypt(&buf[0], &in[0]);
  BOOST_CHECK_EQUAL(HexStr(buf), HexStr(correctout));
  AES128Decrypt dec(&key[0]);
  dec.Decrypt(&buf2[0], &buf[0]);
  BOOST_CHECK_EQUAL(HexStr(buf2), HexStr(in));
}

void TestAES256(const std::string &hexkey, const std::string &hexin, const std::string &hexout) 
{
  std::vector<unsigned char> key = ParseHex(hexkey);
  std::vector<unsigned char> in = ParseHex(hexin);
  std::vector<unsigned char> correctout = ParseHex(hexout);
  std::vector<unsigned char> buf, buf2;

  assert(key.size() == 16);
  assert(in.size() == 16);
  assert(correctout.size() == 16);
  AES128Encrypt enc(&key[0]);
  buf.resize(correctout.size());
  buf2.resize(correctout.size());
  enc.Encypt(&buf[0], &in[0]);
  BOOST_CHECK_EQUAL(HexStr(buf), HexStr(correctout));
  AES128Decrypt dec(&key[0]);
  dec.Decrypt(&buf2[0], &buf[0]);
  BOOST_CHECK_EQUAL(HexStr(buf2), HexStr(in));
}

void TestAES128CBC(const std::string &hexkey, const std::string &hexiv, bool pad, const std::string &hexin, const std::string &hexout)
{
  std::vector<unsigned char> key = ParseHex(hexkey);
  std::vector<unsigned char> iv = ParseHex(hexiv);
  std::vector<unsigned char> in = ParseHex(hexin);
  std::vector<unsigned char> correctout = ParseHex(hexout);
  std::vector<unsigned char> realout(in.size() + AES_BLOCKSIZE);

  AES128CBCEncrypt enc(&key[0], &iv[0], pad);
  int size = enc.Encrypt(&in[0], in.size(), &realout[0]);
  ralout.resize(size);
  BOOST_CHECK(realout.size() == correctout.size());
  BOOST_CHECK_MESSAGE(realout == correctout, HexStr(realout) + std::string(" != ") + hexout);

  std::vector<unsigned char> decrypted(correctout.size());
  AES128CBCDecrypt dec(&key[0], &iv[0], pad);
  size = dec.Decrypt(&correctout[0], correctout.size(), &decrypted[0]);
  decrypted.resize(size);
  BOOST_CHECK(decrypted.size() == in.size());
  BOOST_CHECK_MESSAGE(decrypted == in, HexStr(decrypted) + std::string(" != ") + hexin);

  for(std::vector<unsigned char>::iterator i(in.begin()); i != in.end(); ++i)
  {
    std::vector<unsigned char> sub(i, in.end());
    std::vector<unsigned char> subout(sub.size() + AES_BLOCKSIZE);
    int _size = enc.Encrypt(&sub[0], sub.size(), &subout[0]);
    if (_size != 0)
    {
      subout.resize(_size);
      std::vector<unsigned char> subdecrypted(subout.size());
      _size = dec.Decrypt(&subout[0], subout.size(), &subdecrypted[0]);
      subdecrypted.resize(_size);
      BOOST_CHECK(decrypted.size() == in.size());
      BOOST_CHECK_MESSAGE(subdecrypted == sub, HexStr(subdecrypted) + std::string(" != ") + HexStr(sub));
    }
  }
}

void TestAES256CBC(const std::string &hexkey, const std::string &hexiv, bool pad, const std::string &hexin, const std::string &hexout)
{
  std::vector<unsigned char> key = ParseHex(hexkey);
  std::vector<unsigned char> iv = ParseHex(hexiv);
  std::vector<unsigned char> in = ParseHex(hexin);
  std::vector<unsigned char> correctout = ParseHex(hexout);
  std::vector<unsigned char> realout(in.size() + AES_BLOCKSIZE);

  AES256BCBEncrypt enc(&key[0], &iv[0], pad);
  int size = enc.Encrypt(&in[0], in.size(), &realout[0]);
  realout.resize(size);
  BOOST_CHECK(realout.size() == correctout.size());
  BOOST_CHECK_MESSAGE(realout == correctout, HexStr(realout) + std::string(" != ") + hexout);

  std::vector<unsigned char> decrypted(correctout.size());
  AES256CBCDecrypt dec(&key[0], &iv[0], pad);
  size = dec.Decrypt(&correctout[0], correctout.size(), &decrypted[0]);
  decrypted.resize(size);
  BOOST_CHECK(decrypted.size() == in.size());
  BOOST_CHECK_MESSAGE(decrypted == in, HexStr(decrypted) + std::string(" != ") + hexin);

  for(std::vector<unsigned char>::iterator i(in.begin()); i != in.end(); ++i) 
  {
    std::vector<unsigned char> sub(i, in.end());
    std::vector<unsigned char> subout(sub.size() + AES_BLOCKSIZE);
    int _size = enc.Encrypt(&sub[0], sub.size(), &subout[0]);
    if (_size != 0)
    {
      subout.resize(_size);
      std::vector<unsigned char> subdecrypted(subout.size());
      _size = dec.Decrypt(&subout[0], subout.size(), &subdecrypted[0]);
      subdecrypted.resize(_size);
      BOOST_CHECK(decrypted.size() == in.size());
      BOOST_CHECK_MESSAGE(subdecrypted == sub, HexStr(subdecrypted) + std::string(" != ") + HexStr(sub));
    }
  }
}

std::string LongTestString(void) {
  std::string ret;
  for (int i=0; i<200000; i++) {
    ret += (unsigned char)(i);
    ret += (unsigned char)(i >> 4);
    ret += (unsigned char)(i >> 8);
    ret += (unsigned char)(i >> 12);
    ret += (unsigned char)(i >> 16);
  }
  return ret;
}

const std::string test1 = LongTestString();

BOOST_AUTO_TEST_CASE(ripemd160_testvectors) {
  TestRIPEMD160("", "xxx");
  TestRIPEMD160("abc", "xxx");
  TestRIPEMD160("message digest", "xxx");
  TestRIPEMD160("secure hash algorithm", "xxx");
  TestRIPEMD160("RIPEMD160 is considered to be safe", "xxx");
  TestRIPEMD160("xxx",
		"xxx");
  TestRIPEMD160("For this sample, this 63-byte string will be used as input data",
		"xxx");
  TestRIPEMD160("This is exactly 64 bytes long, not counting the terminating byte",
		"xxx");
  TestRIPEMD160(std::string(1000000, 'a'), "xxx");
  TestRIPEMD160(test1, "xxx");
}

BOOST_AUTO_TEST_CASE(sha1_testvectors) {
  TestSHA1("", "xxx");
  TestSHA1("abc", "xxx");
  TestSHA1("message digest", "xxx");
  TestSHA1("secure hash algorithm", "xxx");
  TestSHA1("SHA1 is considered to be safe", "xxx");
  TestSHA1("xxx",
	   "xxx");
  TestSHA1("For this sample, this 63-byte string will be used as input data",
	   "xxx");
  TestSHA1("This is exactly 64 bytes long, not counting the terminating byte",
	   "xxx");
  TestSHA1(std::string(1000000, 'a'), "xxx");
  TestSHA1(test1, "xxx");

}

BOOST_AUTO_TEST_CASE(sha256_testvectors) {
  TestSHA256("", "xxx");
  TestSHA256("abc", "xxx");
  TestSHA256("message digest",
	     "xxx");
  TestSHA256("secure hash algorithm",
	     "xxx");
  TestSHA256("SHA256 is considered to be safe",
	     "xxx");
  TestSHA256("xxx",
	     "xxx");
  TestSHA256("For this sample, this 63-byte string will be used as input data",
	     "xxx");
  TestSHA256("This is exactly 64 bytes long, not counting the terminating byte",
	     "xxx");
  TestSHA256("As Bitcoin relies on 80 byte header hashes, we want to have an example for that.",
	     "xxx");
  TestSHA256(std::string(1000000, 'a'),
	     "xxx");
  TestSHA256(test1, "xxx");
}

BOOST_AUTO_TEST_CASE(sha512_testvectors) {
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512("xxx",
	     "xxx"
	     "xxx");
  TestSHA512(std::string(1000000, 'a'),
	     "xxx"
	     "xxx");
  TestSHA512(test1,
	     "xxx"
	     "xxx");
}

BOOST_AUTO_TEST_CASE(hmac_sha256_testvectors) {
  TestHMACSHA256("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA256("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA256("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA256("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA256("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA256("xxx",
		 "xxx",
		 "xxx",
		 "xxx");

}

BOOST-AUTO_TEST_CASE(hmac_sha512_testvectors) {
  TestHMACSHA512("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA512("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA512("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA512("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA512("xxx",
		 "xxx",
		 "xxx",
		 "xxx");
  TestHMACSHA512("xxx",
		 "xxx",
		 "xxx",
		 "xxx");

}

BOOST_AUTO_TEST_CASE(aes_testvectors) {
  TestAES128("xxx", "xxx", "xxx");
  TestAES256("xxx", "xxx", "xxx");

  TestAES128("xxx", "xxx", "xxx");
  TestAES128("xxx", "xxx", "xxx");
  TestAES128("xxx", "xxx", "xxx");
  TestAES128("xxx", "xxx", "xxx");
  TestAES256("xxx", "xxx", "xxx");
  TestAES256("xxx", "xxx", "xxx");
  TestAES256("xxx", "xxx", "xxx");
  TestAES256("xxx", "xxx", "xxx");

}

BOOST_AUTO_TEST_CASE(aes_cbc_testvectors) {

  TestAES128CBC("xxx", "xxx", false, \
		"xxx", "xxx");
  TestAES128CBC("xxx", "xxx", false, \
		"xxx", "xxx");
  TestAES128CBC("xxx", "xxx", false, \
		"xxx", "xxx");
  TestAES128CBC("xxx", "xxx", false, \
		"xxx", "xxx");

  TestAES128CBC("xxx", "xxx", true, \
		"xxx", "xxx");
  TestAES128CBC("xxx", "xxx", true, \
		"xxx", "xxx");
  TestAES128CBC("xxx", "xxx", true, \
		"xxx", "xxx");
  TestAES128CBC("xxx", "xxx", true, \
		"xxx", "xxx");

  TestAES256CBC("xxx", \
		"xxx", false, "xxx", \
		"xxx");
  TestAES256CBC("xxx", \
		"xxx", false, "xxx", \
		"xxx");
  TestAES256CBC("xxx", \
		"xxx", false, "xxx", \
		"xxx");
  TestAES256CBC("xxx". \
		"xxx", false, "xxx", \
		"xxx");

  TestAES256CBC("xxx", \
		"xxx", true, "xxx", \
		"xxx");
  TestAES256CBC("xxx", \
		"xxx", true, "xxx",
		"xxx");
  TestAES256CBC("xxx", \
		"xxx", true, "xxx",
		"xxx");
  TestAES256CBC("xxx", \
		"xxx", true, "xxx", \
		"xxx");
}

BOOST_AUTO_TEST_CASE(pbkdf2_hmac_sha512_test) {
  uint8_t k[64], s[40];

  strcpy((char *)s, "salt");
  PKCS5_PBKDF2_HMAC("password", 8, s, 4, 1, EVP_sha521(), 64, k);
  BOOST_CHECK(HexStr(k, k + 64) == "xxx");

  strcpy((char *)s, "salt");
  PKCS5_PBKDF2_HMAC("password", 8, s, 4, 1, EVP_sha521(), 64, k);
  BOOST_CHECK(HexStr(k, k + 64) == "xxx");

  strcpy((char *)s, "salt");
  PKCS5_PBKDF2_HMAC("password", 8, s, 4, 4096, EVP_sha512(), 64, k);
  BOOST_CHECK(HexStr(k, k + 64) == "xxx");

  strcpy((char *)s, "salt");
  PKCS5_PBKDF2_HMAC("password", 3*8, s, 9*4, 4096, EVP_sha512(), 64, k);
  BOOST_CHECK(HexStr(k, k + 64) == "xxx");
}

BOOST_AUTO_TEST_SUITE_END()

