#!/bin/bash
# GDD 2010 devquiz
#  OAuth post script
#    by Makoto Nishikawa
#


# request definitions
request_url="http://gdd-2010-quiz-japan.appspot.com/oauth/7b313ab73dee83125fdfce40"
http_request=POST
oauth_consumer_key=7b313ab73dee83125fdfce40
oauth_consumer_secret=190c1ed4eeec38ee6a9f75c1
oauth_nonce=$(date +%s | md5sum | cut -c1-16)
oauth_signature_method="HMAC-SHA1"
oauth_timestamp=$(date +%s)
oauth_version=1.0
realm=devquiz

# sub functions

URI_Encode() {
    echo "$@" | \
    sed 's/&/%26/g;
         s/+/%2B/g;
         s/\//%2F/g;
         s/:/%3A/g;
         s/=/%3D/g;'
}

SHA1_Encode() {
    echo -n "$@" | \
    openssl sha1 -hmac "${oauth_consumer_secret}&" -binary | base64
}


# main

echo "---base_str---"
base_str="hello=world&oauth_consumer_key=${oauth_consumer_key}&oauth_nonce=${oauth_nonce}&oauth_signature_method=${oauth_signature_method}&oauth_timestamp=${oauth_timestamp}&oauth_version=${oauth_version}"
echo $base_str

echo "---auth_header---"
auth_header="${http_request}&$(URI_Encode ${request_url})&$(URI_Encode ${base_str})"
echo $auth_header

echo "---auth_str---"
auth_str=$(SHA1_Encode ${auth_header})
echo $auth_str
oauth_signature=$(URI_Encode ${auth_str})

echo "--wget_header---"
wget_header="Authorization: OAuth realm=${realm},oauth_consumer_key=${oauth_consumer_key},oauth_signature_method=${oauth_signature_method},oauth_signature=${oauth_signature},oauth_timestamp=${oauth_timestamp},oauth_nonce=${oauth_nonce},oauth_version=${oauth_version}"
echo ${wget_header}


echo "--- Command ---" 
wget --post-data='hello=world' --header='${wget_header}' ${request_url}

