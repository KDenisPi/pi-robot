/**
 * @file WebSettings.cpp
 * @author Denis Kudia (dkudja@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "WebSettings.h"


namespace http {
namespace web {

std::string http::web::WebSettings::mime_html = "Content-Type: text/html; charset=utf-8\r\n";
std::string http::web::WebSettings::mime_js = "Content-Type: application/javascript; charset=utf-8\r\n";
std::string http::web::WebSettings::mime_plain = "Content-Type: text/plain; charset=utf-8\r\n";
std::string http::web::WebSettings::mime_css = "Content-Type: text/css; charset=utf-8\r\n";
std::string http::web::WebSettings::mime_json = "Content-Type: application/json; charset=utf-8\r\n";
std::string http::web::WebSettings::mime_csv = "Content-Type: text/csv; charset=utf-8\r\n";

}
}