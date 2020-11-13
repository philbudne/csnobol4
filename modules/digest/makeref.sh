TEST="This is a test string"
for hash in md5 sha1 sha224 sha256 sha384 sha512 \
	    sha3-224 sha3-256 sha3-384 sha3-512; do
    echo $hash
    echo $TEST | openssl dgst -$hash | sed 's/^.*= //'
done
