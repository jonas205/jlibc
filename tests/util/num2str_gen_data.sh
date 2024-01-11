#!/bin/sh

set -e

wget -q --spider http://google.com
if [ $? != 0 ]; then
    echo "Offline, need access to pip"
    exit -1
fi

PWD=$(pwd)

cd /tmp
mkdir -p jlibc_num2str_venv
cd jlibc_num2str_venv
python3 -m venv .
source bin/activate
pip install --upgrade pip
pip install num2words

cd "$PWD"

get_expected() {
    python -c "from num2words import num2words; print(num2words($1))"
}

test_number() {
    expected=$(get_expected $2)
    echo "$1;$expected"
}

for i in $(seq -1000 1000); do
    test_number $i
done

MAX_INT=$(python -c 'print(2**63-1)')
MIN_INT=$(python -c 'print(-2**63)')

for i in $MIN_INT $MAX_INT; do
    test_number $i
done

for i in $(seq 1000); do
    # get a 64 bit signed random number
    random_number=$(python -c 'import random; rng = random.SystemRandom(); print(rng.randint(-2**63, 2**63-1))')

    test_number $random_number
done
