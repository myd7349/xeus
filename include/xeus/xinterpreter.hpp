/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_INTERPRETER_HPP
#define XEUS_INTERPRETER_HPP

#include <functional>
#include <string>
#include <vector>

#include "xcomm.hpp"
#include "xeus.hpp"
#include "xjson.hpp"

namespace xeus
{
    class xinterpreter;

    XEUS_API bool register_interpreter(xinterpreter* interpreter);
    XEUS_API xinterpreter& get_interpreter();

    class XEUS_API xinterpreter
    {
    public:

        xinterpreter();
        virtual ~xinterpreter() = default;

        xinterpreter(const xinterpreter&) = delete;
        xinterpreter& operator=(const xinterpreter&) = delete;

        xinterpreter(xinterpreter&&) = delete;
        xinterpreter& operator=(xinterpreter&&) = delete;

        void configure();

        xjson execute_request(const std::string& code,
                              bool silent,
                              bool store_history,
                              xjson user_expressions,
                              bool allow_stdin);

        xjson complete_request(const std::string& code, int cursor_pos);

        xjson inspect_request(const std::string& code, int cursor_pos, int detail_level);

        xjson is_complete_request(const std::string& code);
        xjson kernel_info_request();

        void shutdown_request();

        // publish(msg_type, metadata, content)
        using publisher_type = std::function<void(const std::string&, xjson, xjson, buffer_sequence)>;
        void register_publisher(const publisher_type& publisher);

        void publish_stream(const std::string& name, const std::string& text);
        void display_data(xjson data, xjson metadata, xjson transient);
        void update_display_data(xjson data, xjson metadata, xjson transient);
        void publish_execution_input(const std::string& code, int execution_count);
        void publish_execution_result(int execution_count, xjson data, xjson metadata);
        void publish_execution_error(const std::string& ename,
                                     const std::string& evalue,
                                     const std::vector<std::string>& trace_back);
        void clear_output(bool wait);

        // send_stdin(msg_type, metadata, content)
        using stdin_sender_type = std::function<void(const std::string&, xjson, xjson)>;
        void register_stdin_sender(const stdin_sender_type& sender);
        using input_reply_handler_type = std::function<void(const std::string&)>;
        void register_input_handler(const input_reply_handler_type& handler);

        void input_request(const std::string& prompt, bool pwd);
        void input_reply(const std::string& value);

        void register_comm_manager(xcomm_manager* manager);
        xcomm_manager& comm_manager() noexcept;
        const xcomm_manager& comm_manager() const noexcept;

        using parent_header_type = std::function<const xjson&()>;
        void register_parent_header(const parent_header_type&);
        const xjson& parent_header() const noexcept;

    private:

        virtual void configure_impl() = 0;

        virtual xjson execute_request_impl(int execution_counter,
                                           const std::string& code,
                                           bool silent,
                                           bool store_history,
                                           xjson user_expressions,
                                           bool allow_stdin) = 0;

        virtual xjson complete_request_impl(const std::string& code,
                                            int cursor_pos) = 0;

        virtual xjson inspect_request_impl(const std::string& code,
                                           int cursor_pos,
                                           int detail_level) = 0;

        virtual xjson is_complete_request_impl(const std::string& code) = 0;

        virtual xjson kernel_info_request_impl() = 0;

        virtual void shutdown_request_impl() = 0;

        xjson build_display_content(xjson data, xjson metadata, xjson transient);

        publisher_type m_publisher;
        stdin_sender_type m_stdin;
        int m_execution_count;
        xcomm_manager* p_comm_manager;
        parent_header_type m_parent_header;
        input_reply_handler_type m_input_reply_handler;
    };

    inline xcomm_manager& xinterpreter::comm_manager() noexcept
    {
        return *p_comm_manager;
    }

    inline const xcomm_manager& xinterpreter::comm_manager() const noexcept
    {
        return *p_comm_manager;
    }
}

#endif
