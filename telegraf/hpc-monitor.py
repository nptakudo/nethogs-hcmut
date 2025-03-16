import os
import psutil
import time
import requests
from datetime import datetime

# Configuration
INFLUXDB_HOST =""
ORG_NAME = "grafana"
BUCKET_NAME = "VM-Monitoring"
API_TOKEN = ""
LOG_FILE = "/etc/telegraf/influxdb_errors.log"

def get_process_metrics():
    # Get the list of all running processes
    for proc in psutil.process_iter(['pid', 'name', 'username', 'cpu_percent', 'memory_percent']):
        pid = proc.info['pid']
        comm = proc.info['name']
        user = proc.info['username']
        cpu = proc.info['cpu_percent']
        mem = proc.info['memory_percent']

        # Get parent process id (ppid)
        ppid = proc.ppid()
        prev_ppid = proc.ppid()
        
        while (ppid > 1):
            parent_proc = psutil.Process(ppid)
            prev_ppid = ppid
            ppid = parent_proc.ppid()
            if (ppid == 1):
                break
        
        ppid = prev_ppid


        read_bytes = 0
        write_bytes = 0
        ppid_comm = ""

        try:
            # Get read and write bytes for the process
            io_stats = proc.io_counters()
            read_bytes = io_stats.read_bytes
            write_bytes = io_stats.write_bytes
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass

        try:
            # Get the parent process name (ppid_comm)
            if ppid == 1:
                ppid_comm = comm
            elif ppid > 1:
                parent_proc = psutil.Process(ppid)
                ppid_comm = parent_proc.name()
            else:
                ppid_comm = "systemd"
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            ppid_comm = "Unknown"

        # Format data in InfluxDB Line Protocol
        line_protocol = f"hpc-monitoring,user={user},comm={comm},pid={pid},ppid={ppid},app={ppid_comm} cpu_percent={cpu},mem_percent={mem},read_bytes={read_bytes},write_bytes={write_bytes}"
#        print(line_protocol)

        # Send data to InfluxDB
        try:
            response = requests.post(
                f"{INFLUXDB_HOST}/api/v2/write?org={ORG_NAME}&bucket={BUCKET_NAME}&precision=ns",
                headers={
                    "Authorization": f"Token {API_TOKEN}",
                    "Content-Type": "text/plain"
                },
                data=line_protocol
            )
            if response.status_code != 204:
                with open(LOG_FILE, 'a') as log:
                    log.write(f"{datetime.utcnow()} ERROR: Failed to send data. HTTP Status: {response.status_code}. Data: {line_protocol}\n")
        except requests.RequestException as e:
            with open(LOG_FILE, 'a') as log:
                log.write(f"{datetime.utcnow()} ERROR: Request failed. Exception: {str(e)}\n")

# Loop to send metrics every 7 seconds
while True:
    get_process_metrics()
    time.sleep(7)  # Wait for 7 seconds before running the function again
