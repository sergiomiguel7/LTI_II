sudo bluetoothctl <<EOF
power on
agent on
scan on
pair 8C:AA:B5:B5:67:E2
trust 8C:AA:B5:B5:67:E2
connect 8C:AA:B5:B5:67:E2
EOF