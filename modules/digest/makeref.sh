TEST="This is a test string"
for hash in md5 sha1 sha224 sha256 sha384 sha512; do
    echo $TEST | $hash
    echo $TEST | $hash
done
