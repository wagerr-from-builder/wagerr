#!/bin/bash

VERSION="$(git rev-parse --short=12 HEAD)"

cd output

KEY_FILE=${KEY_FILE:-~/codesigning/wagerr-project.p12}
if [[ ! -f "$KEY_FILE" ]]; then
    ls $KEY_FILE
    echo "Make sure that $KEY_FILE exists"
fi

if ! which osslsigncode > /dev/null 2>&1; then
    echo "Please install osslsigncode"
fi

rm -rf signed
mkdir -p signed >/dev/null 2>&1

unzip wagerr-$VERSION-win64.zip -d signed
cd signed/wagerr-$VERSION/bin

echo "Found $(ls *.exe | wc -w) files to sign."
for f in $(ls *.exe); do
    echo "Signing $f..."
    osslsigncode sign \
      -h sha256 \
      -pkcs12 "$KEY_FILE" \
      -pass "$VERTCOIN_PROJECT_KEY_PASSWORD" \
      -n "Wagerr-Core" \
      -i "https://wagerr.org/" \
      -t "http://timestamp.digicert.com/" \
      -in "$f" \
      -out "../../$f"
    ls ../../$f -lah
done

cd ../..
rm wagerr-$VERSION/bin/*
mv wagerr*.exe wagerr-$VERSION/bin
zip -r wagerr-$VERSION-win64.zip wagerr-$VERSION
rm -r wagerr-$VERSION

echo "Signing wagerr-$VERSION-win64-setup..."
osslsigncode sign \
  -h sha256 \
  -pkcs12 "$KEY_FILE" \
  -pass "$VERTCOIN_PROJECT_KEY_PASSWORD" \
  -n "Wagerr-Core" \
  -i "https://wagerr.org/" \
  -t "http://timestamp.digicert.com/" \
  -in "../wagerr-$VERSION-win64-setup-unsigned.exe" \
  -out "wagerr-$VERSION-win64-setup.exe"
