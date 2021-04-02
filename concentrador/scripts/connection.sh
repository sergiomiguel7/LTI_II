sudo bluetoothctl <<EOF
power off
power on
agent on
scan on
trust 8C:AA:B5:B5:67:E2
connect 8C:AA:B5:B5:67:E2
quit
sleep 5
EOF