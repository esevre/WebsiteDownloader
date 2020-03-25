//
// Created by esevre on 2/25/2020.
//

#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include "ES/string/streamable.hpp"

namespace ES::web{
    /**
     * HtmlPageIoManager : very simple object to get content from HTML page.
     * Most web pages can be written as http://www.github.com/esevre.
     * When written like this the HTTP specifies the protocal,
     * the www.github.com specifies the host
     * the port is not specified, and the default is 80
     * the target is the /esevre
     * \Parameter host: string -->   www.exmaple.com or example.com
     * \Parameter port: string -->   default 80
     * \Parameter target:   string --> /subdir/page2.html
     *
     * \Example HtmlPageIoManager obj(host, port, target);
     * \Example obj.connect();
     * \Example obj.setup();
     * \Example auto html_string = obj.read();
     */
    class HtmlPageIoManager {
    public:
        /**
         *
         * @param [in] host
         * @param [in] port
         * @param [in] target
         * @param [in] version
         */
        HtmlPageIoManager(std::string host, std::string port, std::string target="80", int version=11)
                : ioc()
                , resolver(ioc)
                , stream(ioc)
                , host(std::move(host))
                , port(std::move(port))
                , target(std::move(target))
                , version(version)
        {
        }

        void connect()
        {
            stream.connect(resolver.resolve(host.c_str(), port.c_str()));
        }

        void setup(){
            write();
        }

        auto setup_req()
        {
            boost::beast::http::request<boost::beast::http::string_body> req {boost::beast::http::verb::get, target.c_str(), version};
            req.set(boost::beast::http::field::host, host);
            req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            return req;
        }

        void write(const boost::beast::http::request<boost::beast::http::string_body> &req)
        {
            boost::beast::http::write(stream, req);
        }

        void write()
        {
            boost::beast::http::write(stream, setup_req());
        }

        auto read()
        {
            boost::beast::flat_buffer buffer;
            // Declare a container to hold the response
            boost::beast::http::response<boost::beast::http::dynamic_body> res;
            boost::beast::http::read(stream, buffer, res);
            return ES::string::streamable_to_string(res);
        }

        void close()
        {
            boost::beast::error_code ec;
            stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

            // not_connected happens sometimes
            // so don't bother reporting it.
            //
            if (ec && ec != boost::beast::errc::not_connected){
                throw boost::beast::system_error {ec};
            }

        }

        virtual ~HtmlPageIoManager() {
            close();
        }

    private:
        // IO management
        boost::asio::io_context        ioc;
        boost::asio::ip::tcp::resolver resolver;
        boost::beast::tcp_stream       stream;
        // HTML page info
        std::string host;
        std::string port;
        std::string target;
        int version;

    };

    /**
     * Gets the HTML from a website and returns it as a string.
     *
     * @param host ip address or url for hosting site
     * @param port port to use for connection to site
     * @param target route to the target page
     * @param version HTTP version, default of 1.1, can override with 10 for 1.0
     * @return string with site content
     */
    std::string get_html(
            const std::string &host,
            const std::string &target,
            const std::string &port="80",
            const int version = 11)
    {
        try {
            HtmlPageIoManager pageIO(host, port, target, version);
            pageIO.connect();
            pageIO.setup();
            return pageIO.read();
        }
        catch(std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(-1);
        }
        // logic should never reach here
    }

}




