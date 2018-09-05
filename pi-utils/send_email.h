/*
 * send_email.h
 *
 *  CURL based email send
 *
 *  Created on: Sep 4, 2018
 *      Author: Denis Kudia
 */

namespace piutils {
namespace email {

#include <string>
#include <array>
#include <ctime>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#include <curl/curl.h>

#include "smallthings.h"
#include "logger.h"

class SendEmail {
public:
    SendEmail(const std::string server, const std::string cert) :
        _lines_read(0), _server(server), _cert(cert) {

    }

  virtual ~SendEmail() {}

  std::string _server;
  std::string _cert;

  std::string _user;
  std::string _password;

  std::string _from;
  std::string _to;

  std::string _subject;
  std::string _body;
  std::string _postfix;

  /*
  * Validate parameters
  */
  bool is_valid(){
      if(_server.empty()) return false;
      if(_cert.empty()) return false;
      if(_user.empty()) return false;
      if(_password.empty()) return false;
      if(_from.empty()) return false;
      if(_postfix.empty()) return false;
      if(_subject.empty()) return false;

      return true;
  }

  /*
  * Prepare email data
  */
  bool prepare_data(){
    _data[0] = "Date: " + piutils::get_time_string(true) + "\r\n";
    _data[1] = "To: " + _to + "\r\n";
    _data[2] = "From: " + _from + " \r\n";
    _data[3] = "Message-ID: <" + get_message_id(_postfix) + ">\r\n";
    _data[4] = "Subject: " + _subject + "\r\n";
    _data[5] = "\r\n"; /* empty line to divide headers from body, see RFC5322 */
    _data[6] = _body + "\r\n";

    return true;
  }

  /*
  *
  */
  static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
  {
    SendEmail *upload_ctx = (SendEmail*)userp;

    if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
      return 0;
    }

    if(upload_ctx->is_eof()){
      return 0;
    }

    std::string data = upload_ctx->get_line();

    memcpy(ptr, data.c_str(), data.length());
    return data.length();
  }

  /*
  * Detect if all data lines loaded
  */
  bool is_eof(){
    return (_lines_read >= _data.size());
  }

  /*
  * Get next data line
  */
  const std::string get_line(){
    return _data[_lines_read++];
  }

  /*
  *
  */
  int send_email(){
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;

    _lines_read = 0;

    prepare_data();

    if(!is_valid()){
        logger::log(logger::LLOG::ERROR, "email", std::string(__func__) + " Invalid configuration");
        return 1;
    }

    curl = curl_easy_init();
    if(curl) {
      /* Set username and password */
      curl_easy_setopt(curl, CURLOPT_USERNAME, _user.c_str());
      curl_easy_setopt(curl, CURLOPT_PASSWORD, _password.c_str());

      /* This is the URL for your mailserver. Note the use of port 587 here,
      * instead of the normal SMTP port (25). Port 587 is commonly used for
      * secure mail submission (see RFC4403), but you should use whatever
      * matches your server configuration. */
      curl_easy_setopt(curl, CURLOPT_URL, _server.c_str());

      /* In this example, we'll start with a plain text connection, and upgrade
      * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
      * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
      * will continue anyway - see the security discussion in the libcurl
      * tutorial for more details. */
      curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

      /* If your server doesn't have a valid certificate, then you can disable
      * part of the Transport Layer Security protection by setting the
      * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
      *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      * That is, in general, a bad idea. It is still better than sending your
      * authentication details in plain text though.  Instead, you should get
      * the issuer certificate (or the host certificate if the certificate is
      * self-signed) and add it to the set of certificates that are known to
      * libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See docs/SSLCERTS
      * for more information. */
      curl_easy_setopt(curl, CURLOPT_CAINFO, _cert.c_str());

      /* Note that this option isn't strictly required, omitting it will result
      * in libcurl sending the MAIL FROM command with empty sender data. All
      * autoresponses should have an empty reverse-path, and should be directed
      * to the address in the reverse-path which triggered them. Otherwise,
      * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
      * details.
      */
      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, _from.c_str());

      /* Add two recipients, in this particular case they correspond to the
      * To: and Cc: addressees in the header, but they could be any kind of
      * recipient. */
      recipients = curl_slist_append(recipients, _to.c_str());
      curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

      /* We're using a callback function to specify the payload (the headers and
      * body of the message). You could just use the CURLOPT_READDATA option to
      * specify a FILE pointer to read from. */
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
      curl_easy_setopt(curl, CURLOPT_READDATA, this);
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

      /* Since the traffic will be encrypted, it is very useful to turn on debug
      * information within libcurl to see what is happening during the transfer.
      */
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); //1L

      /* Send the message */
      res = curl_easy_perform(curl);

      /* Check for errors */
      if(res != CURLE_OK){
        //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        logger::log(logger::LLOG::ERROR, "email", std::string(__func__) + " Failed: " + curl_easy_strerror(res));
      }

      /* Free the list of recipients */
      curl_slist_free_all(recipients);

      /* Always cleanup */
      curl_easy_cleanup(curl);
    }

    return (int)res;
  }

  static const std::string get_message_id(const std::string postfix){
      std::chrono::time_point<std::chrono::system_clock> tp;
      tp = std::chrono::system_clock::now();

      int64_t ms = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count();

      std::srand(std::time(nullptr)); // use current time as seed for random generator
      int random_variable = std::rand();

      std::string result = std::to_string(ms) + "_" + std::to_string(random_variable) + "@" + postfix;
      return result;
  }

private:
    std::string messageid;
    int _lines_read;

    std::array<std::string, 7> _data;
};

}//namespace email
}//namespace piutils