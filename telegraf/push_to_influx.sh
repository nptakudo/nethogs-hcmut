#!/bin/bash

# InfluxDB configuration
INFLUXDB_HOST=""
BUCKET_NAME="VM-Monitoring"
ORG_NAME="grafana"
API_TOKEN=""
LOG_FILE="/etc/telegraf/influxdb_errors.log"

# Process stats collection
ps -eo user,comm,pid,ppid,%cpu,%mem --sort=-%cpu | tail -n +2 | while read user comm pid ppid cpu mem; do

    read_bytes=0
    write_bytes=0
    ppid_comm=""

    # Get read and write bytes for the process
    if [[ -r "/proc/$pid/io" ]]; then
        read_bytes=$(awk "/^read_bytes:/ {print \$2}" "/proc/$pid/io" 2>/dev/null)
        write_bytes=$(awk "/^write_bytes:/ {print \$2}" "/proc/$pid/io" 2>/dev/null)
    fi

    # Get the parent process name (ppid_comm)
    if [[ "$ppid" -eq 1 ]]; then
        ppid_comm=$comm
    elif [[ "$ppid" -gt 1 ]]; then
        ppid_comm=$(ps -o comm= -p "$ppid" 2>/dev/null || echo "Unknown")
    else
        ppid_comm="systemd"
    fi

    # Format read/write bytes
    read_bytes=$(echo "$read_bytes.0" | bc)
    write_bytes=$(echo "$write_bytes.0" | bc)

    timestamp=$(date -u +%s%N)

    # Format data in InfluxDB Line Protocol
    line_protocol="process_metrics,user=$user,comm=$comm,pid=$pid,app=$ppid_comm cpu=$cpu,mem=$mem,read_bytes=$read_bytes,write_bytes=$write_bytes $timestamp"

    # Send data to InfluxDB
    response=$(curl -s -o /dev/null -w "%{http_code}" -XPOST "$INFLUXDB_HOST/api/v2/write?org=$ORG_NAME&bucket=$BUCKET_NAME&precision=ns" \
        --header "Authorization: Token $API_TOKEN" \
        --header "Content-Type: text/plain" \
        --data-binary "$line_protocol")

    # Check if the request was successful (HTTP 204 = success)
    if [[ "$response" -ne 204 ]]; then
        echo "$(date -u +"%Y-%m-%d %H:%M:%S") ERROR: Failed to send data. HTTP Status: $response. Data: $line_protocol" >> "$LOG_FILE"
    fi

done

