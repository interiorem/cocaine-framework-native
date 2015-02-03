#pragma once

#include <cstdint>
#include <memory>

#include <cocaine/rpc/asio/encoder.hpp>

#include "cocaine/framework/forwards.hpp"

namespace cocaine {

namespace framework {

class basic_session_t;

class basic_sender_t {
    std::uint64_t id;
    std::shared_ptr<basic_session_t> session;

public:
    basic_sender_t(std::uint64_t id, std::shared_ptr<basic_session_t> session);

    /*!
     * Pack given args in the message and push it through session pointer.
     *
     * \return a future, which will be set when the message is completely sent or any network error
     * occured.
     *
     * This future can throw:
     *  - \sa encoder_error if the sender is unable to properly pack given arguments.
     *  - \sa network_error if the sender is in invalid state, for example after any network error.
     *
     * Setting an error in the receiver doesn't work, because there can be mute events, which
     * doesn't respond ever.
     */
    template<class Event, class... Args>
    auto
    send(Args&&... args) -> future_t<void> {
        return send(io::encoded<Event>(id, std::forward<Args>(args)...));
    }

private:
    auto send(io::encoder_t::message_type&& message) -> future_t<void>;
};

template<class T>
class sender {
public:
    typedef T tag_type;

private:
    std::shared_ptr<basic_sender_t> d;

public:
    sender(std::shared_ptr<basic_sender_t> d) :
        d(std::move(d))
    {}

    /*!
     * Copy constructor is deleted intentionally.
     *
     * This is necessary, because each `send` method call in the common case invalidates current
     * object's state and returns the new sender instead.
     */
    sender(const sender& other) = delete;
    sender(sender&& other) = default;

    sender& operator=(const sender& other) = delete;
    sender& operator=(sender&& other) = default;

    /*!
     * Encode arguments to the internal protocol message and push it into the session attached.
     *
     * \warning this sender will be invalidated after this call.
     */
    template<class Event, class... Args>
    future_t<sender<typename io::event_traits<Event>::dispatch_type>>
    send(Args&&... args) {
        auto future = d->send<Event>(std::forward<Args>(args)...);
        return future.then(std::bind(&sender::traverse<Event>, std::placeholders::_1, d));
    }

private:
    template<class Event>
    static
    sender<typename io::event_traits<Event>::dispatch_type>
    traverse(future_t<void>& f, std::shared_ptr<basic_sender_t> d) {
        f.get();
        return sender<typename io::event_traits<Event>::dispatch_type>(std::move(d));
    }
};

template<>
class sender<void> {
public:
    sender(std::shared_ptr<basic_sender_t>) {}
};

} // namespace framework

} // namespace cocaine
