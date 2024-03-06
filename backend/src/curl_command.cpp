/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2022, Kassow Robots
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Kassow Robots nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
//TEST

#include <curl_command/curl_command.h>
#include <iostream>
#include <string>
#include <cstdlib> //not needed, just implemented to check ping if needed
#include <curl/curl.h> // Include curl header

using namespace kswx_curl;

// Callback-Function
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t total_size = size * nmemb;
  output ->append((char*)contents, total_size);
  return total_size;
}

CurlCommand::CurlCommand(kr2_program_api::ProgramInterface* a_api)
{}

CurlCommand::~CurlCommand()
{}

CBUN_PCALL CurlCommand::setValue(kr2_program_api::Number& target, std::string url, std::string b)
{
  // Check if b is "curl" to proceed with curl check
  if (b == "curl") {
    CURL *curl;
    CURLcode res;

    // Initialize curl handle
    curl = curl_easy_init();

    if (curl) {
      // Set a simple URL to check (replace with your preferred URL)
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

      // Set the HTTP PUT 
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

      // Set data wich are send by the PUT request
      const char* json_data = R"(
        {
          "id": "0bf3980e-35dc-4062-90da-7c6ef9d70d55",
          "name": "PickPart",
          "deviceName": "RobotHandler.Extension",
          "deviceHierarchyPath": "station.RobotHandler",
          "inParameters": [],
          "outParameters": []

        }
      )";
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

      // Set the Content-Type header on "application/json-patch+json"
      struct curl_slist* headers = NULL;
      headers = curl_slist_append(headers, "Content-Type: application/json-patch+json");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

      // Callback-Function, to process the recieved Data
      std::string response_data;
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

      // Optional: Set timeout in seconds (replace 5 with your desired timeout)
      curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

      // Perform the curl operation
      res = curl_easy_perform(curl);

      // Check for curl operation success
      if (res == CURLE_OK) {
        target = kr2_program_api::Number(1.0); // Set to 1 if curl succeeds
      } else {
        // Handle curl operation failure
        target = kr2_program_api::Number(0.0); // Set to 0 on curl failure
        std::cerr << "curl_easy_perform failed: " << curl_easy_strerror(res) << std::endl;
      }

      // Cleanup curl handle
      curl_easy_cleanup(curl);
    } else {
      // Handle curl initialization failure
      target = kr2_program_api::Number(5.0); // Set to 5 on curl init failure
      std::cerr << "curl_easy_init failed" << std::endl;
    }
  } else {
    // Set target to 404 if b is not "curl" (unchanged)
    target = kr2_program_api::Number(404.0);
  }
    CBUN_PCALL_RET_OK
}
