#!/bin/bash
set -euo pipefail

echo "Running extended functional tests for Kernel OTP Project"

# DICT METHOD TESTS
echo "[1] Dictionary method: adding multiple words..."
./config/otp_config dict add apple
./config/otp_config dict add banana
./config/otp_config dict add cherry
./config/otp_config dict add dragonfruit
./config/otp_config dict add elderberry
./config/otp_config dict add fig
./config/otp_config dict add grape

echo "[2] Setting dictionary OTP expiration to 3 seconds..."
./config/otp_config dict time 3

echo "[3] Generating dictionary OTP..."
OTP1=$(cat /dev/kotp_dict | tr -d '\n')
echo "OTP1: $OTP1"

echo "[4] Sleeping to force expiration (5 seconds)..."
sleep 5

echo "[5] Verifying expired OTP (should fail)..."
! ./client/otp_client dict "$OTP1"

echo "[6] Generating and testing multiple dictionary OTPs..."
OTP2=$(cat /dev/kotp_dict | tr -d '\n')
OTP3=$(cat /dev/kotp_dict | tr -d '\n')
OTP4=$(cat /dev/kotp_dict | tr -d '\n')
echo "OTP2: $OTP2"
echo "OTP3: $OTP3"
echo "OTP4: $OTP4"

echo "[7] Verifying OTP2 (should succeed)..."
./client/otp_client dict "$OTP2"

echo "[8] Verifying OTP2 again (should fail)..."
! ./client/otp_client dict "$OTP2"

echo "[9] Verifying OTP3 (should succeed)..."
./client/otp_client dict "$OTP3"

echo "[10] Sleeping for 4 seconds to expire OTP4..."
sleep 4

echo "[11] Verifying OTP4 (should fail due to expiration)..."
! ./client/otp_client dict "$OTP4"

# TIME METHOD TESTS
echo "[12] Configuring time-based OTP..."
./config/otp_config time key ultrasecret987
./config/otp_config time time 3

echo "[13] Generating time OTP..."
TOTP1=$(cat /dev/kotp_time | tr -d '\n')
echo "TOTP1: $TOTP1"

echo "[14] Sleeping to force expiration (4 seconds)..."
sleep 4

echo "[15] Verifying expired TOTP1 (should fail)..."
! ./client/otp_client time "$TOTP1"

echo "[16] Generating multiple time-based OTPs quickly..."
TOTP2=$(cat /dev/kotp_time | tr -d '\n')
sleep 1
TOTP3=$(cat /dev/kotp_time | tr -d '\n')
sleep 1
TOTP4=$(cat /dev/kotp_time | tr -d '\n')

echo "TOTP2: $TOTP2"
echo "TOTP3: $TOTP3"
echo "TOTP4: $TOTP4"

echo "[17] Verifying TOTP2 (should succeed)..."
./client/otp_client time "$TOTP2"

echo "[18] Verifying TOTP3 (should succeed)..."
./client/otp_client time "$TOTP3"

echo "[19] Verifying TOTP4 after delay (should still succeed)..."
sleep 2
./client/otp_client time "$TOTP4"

echo "[20] Trying reused TOTP2 (should fail)..."
! ./client/otp_client time "$TOTP2"

echo "[✅] All functional tests passed successfully."
