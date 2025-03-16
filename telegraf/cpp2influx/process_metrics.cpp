#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <dirent.h>
#include <unistd.h>

// Configuration
const std::string INFLUXDB_HOST = "";
const std::string ORG_NAME = "grafana";
const std::string BUCKET_NAME = "cpp2influx";
const std::string API_TOKEN = "";
const std::string LOG_FILE = "/etc/telegraf/influxdb_errors.log";

// Helper function to get CPU usage and memory usage
void get_process_metrics() {
    // Read process information from /proc/[pid] directory
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (dp == nullptr) {
        std::cerr << "Error opening /proc directory." << std::endl;
        return;
    }

    while ((entry = readdir(dp)) != nullptr) {
        // Skip non-numeric entries
        if (!isdigit(entry->d_name[0])) continue;

        std::string pid = entry->d_name;

        // Read the process metrics like CPU, memory usage
        std::ifstream stat_file("/proc/" + pid + "/stat");
        std::string line;
        if (stat_file && std::getline(stat_file, line)) {
            std::istringstream iss(line);
            std::string comm;
            long utime, stime, rss;
            iss >> pid >> comm >> utime >> stime >> rss;

            // Example: Get CPU and Memory usage (you can expand to get other metrics)
            float cpu_usage = (utime + stime) / sysconf(_SC_CLK_TCK);  // CPU usage in seconds
            float mem_usage = rss * sysconf(_SC_PAGESIZE) / (1024 * 1024);  // Memory usage in MB

            // Format data in InfluxDB Line Protocol
            std::string line_protocol = "process_metrics,comm=" + comm + ",pid=" + pid + " cpu_percent=" + std::to_string(cpu_usage) + ",mem_percent=" + std::to_string(mem_usage);
            std::cout << line_protocol << std::endl;
            // Send data to InfluxDB
            CURL *curl = curl_easy_init();
//            if (curl) {
//                std::string url = INFLUXDB_HOST + "/api/v2/write?org=" + ORG_NAME + "&bucket=" + BUCKET_NAME + "&precision=ns";
//                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);  // Add authorization header
//                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, line_protocol.c_str());

//                CURLcode res = curl_easy_perform(curl);
//                if (res != CURLE_OK) {
                    // Log errors
//                    std::ofstream log(LOG_FILE, std::ios_base::app);
//                    log << "ERROR: Request failed. Exception: " << curl_easy_strerror(res) << std::endl;
//                }
//
//                curl_easy_cleanup(curl);
//            }
        }
        stat_file.close();
    }

    closedir(dp);
}

int main() {
    // Loop to send metrics every 7 seconds
    while (true) {
        get_process_metrics();
        std::this_thread::sleep_for(std::chrono::seconds(7));
    }

    return 0;
}

