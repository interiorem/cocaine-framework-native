/*
    Copyright (c) 2015 Evgeny Safronov <division494@gmail.com>
    Copyright (c) 2011-2015 Other contributors as noted in the AUTHORS file.
    This file is part of Cocaine.
    Cocaine is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    Cocaine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "cocaine/framework/error.hpp"

#include <cocaine/format.hpp>
#include <cocaine/repository.hpp>
#include <cocaine/service/node/error.hpp>

using namespace cocaine::framework;

struct error_category_registrator_t {
    error_category_registrator_t() {
        using cocaine::error::registrar;
        using namespace cocaine::service;

        registrar::add(node::worker_category(), node::worker_category_t::id());
        registrar::add(node::worker_user_category(), node::worker_user_category_t::id());
    }

    static auto instance() -> error_category_registrator_t& {
        static error_category_registrator_t self;
        return self;
    }
};

static const error_category_registrator_t registrator = error_category_registrator_t::instance();

/// Extended description formatting patterns.
static const char ERROR_SERVICE_NOT_FOUND[] = "the service '{}' is not available";
static const char ERROR_VERSION_MISMATCH[]  = "version mismatch ({} expected, but {} actual)";

namespace {

struct service_category_t : public std::error_category {
    const char*
    name() const noexcept {
        return "service category";
    }

    std::string
    message(int err) const noexcept {
        switch (err) {
        case static_cast<int>(cocaine::framework::error::service_not_found):
            return "the specified service was not found in the locator";
        case static_cast<int>(cocaine::framework::error::version_mismatch):
            return "the service provides API with version different than required";
        default:
            return "unexpected service error";
        }
    }
};

struct response_category_t : public std::error_category {
    const char*
    name() const noexcept {
        return "service response category";
    }

    std::string
    message(int) const noexcept {
        return "error from the service";
    }
};

} // namespace

const std::error_category&
cocaine::framework::error::service_category() {
    static service_category_t category;
    return category;
}

const std::error_category&
cocaine::framework::error::response_category() {
    static response_category_t category;
    return category;
}

std::error_code
error::make_error_code(error::service_errors err) {
    return std::error_code(static_cast<int>(err), error::service_category());
}

std::error_condition
error::make_error_condition(error::service_errors err) {
    return std::error_condition(static_cast<int>(err), error::service_category());
}

std::error_code
error::make_error_code(error::response_errors err) {
    return std::error_code(static_cast<int>(err), error::response_category());
}

std::error_condition
error::make_error_condition(error::response_errors err) {
    return std::error_condition(static_cast<int>(err), error::response_category());
}

cocaine::framework::error_t::error_t(const std::error_code& ec, const std::string& description) :
    std::system_error(ec, description)
{}

cocaine::framework::error_t::~error_t() noexcept {}

service_not_found::service_not_found(const std::string& name) :
    error_t(error::service_not_found, cocaine::format(ERROR_SERVICE_NOT_FOUND, name)),
    name_(name)
{}

service_not_found::~service_not_found() noexcept {}

const std::string& service_not_found::name() const noexcept {
    return name_;
}

version_mismatch::version_mismatch(int expected, int actual) :
    error_t(error::version_mismatch, cocaine::format(ERROR_VERSION_MISMATCH, expected, actual)),
    expected_(expected),
    actual_(actual)
{}

int version_mismatch::expected() const noexcept {
    return expected_;
}

int version_mismatch::actual() const noexcept {
    return actual_;
}

response_error::response_error(std::tuple<std::error_code, std::string>& err) :
    error_t(error::unspecified,
            std::get<1>(err).empty() ?
                cocaine::format("[{}]: {}", std::get<0>(err).value(), std::get<0>(err).message()) :
                cocaine::format("[{}]: {} - {}", std::get<0>(err).value(), std::get<0>(err).message(), std::get<1>(err))
            ),
    ec_(std::get<0>(err))
{}

int
response_error::id() const noexcept {
    return ec_.value();
}

std::error_code
response_error::ec() const {
    // TODO: Enable, when there will be consensus reached about worker category number.
    throw std::runtime_error("`response_error::ec()`: not implemented yet");
}
