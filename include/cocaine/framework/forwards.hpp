#pragma once

#include "cocaine/framework/util/future.hpp"

namespace cocaine {
    namespace framework {

        /// Workaround for compilers, that doesn't understand using keyword for template aliaces.
        template<typename T>
        struct task {
            typedef future<T> future_type;
            typedef typename std::add_lvalue_reference<future_type>::type future_move_type;

            typedef promise<T> promise_type;
        };

        /// \internal
        struct event_loop_t;

        class scheduler_t;

        template<class Session>
        class basic_sender_t;

        template<class Session>
        class basic_receiver_t;

        template<class T, class Session>
        class sender;

        template<class T, class Session>
        class receiver;

        /// \internal
        class basic_session_t;

        template<class BasicSession>
        class session;

        class basic_service_t;

        template<class T>
        class service;

        class service_manager_t;

        /// Worker side.

        /// \internal
        class worker_session_t;

        class dispatch_t;
        class worker_t;
    } // namespace framework
} // namespace cocaine
