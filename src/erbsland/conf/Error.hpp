// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ErrorCategory.hpp"
#include "Location.hpp"
#include "NamePath.hpp"
#include "String.hpp"

#include <exception>
#include <filesystem>
#include <optional>


namespace erbsland::conf {


/// The exception for all errors.
///
/// @tested `ErrorTest`
///
class Error final : public std::exception {
public:
    /// Create a new error with the given message.
    ///
    /// @param category The error category.
    /// @param message The string with the message.
    ///
    template <typename Msg>
    Error(
        const ErrorCategory category,
        Msg&& message) noexcept
    :
        _category{category},
        _message{std::forward<Msg>(message)} {
    }

    /// Create a new error with the given message, location, and file path.
    ///
    /// @param category The error category.
    /// @param message The string with the message.
    /// @param args Optional arguments for the error message.
    ///
    template <typename Msg, typename... Args>
    requires ((std::is_same_v<std::decay_t<Args>, Location> ||
               std::is_same_v<std::decay_t<Args>, NamePath> ||
               std::is_same_v<std::decay_t<Args>, std::filesystem::path> ||
               std::is_same_v<std::decay_t<Args>, std::error_code>) && ...)
    Error(
        const ErrorCategory category,
        Msg&& message, Args&&... args) noexcept
    :
        _category{category},
        _message{std::forward<Msg>(message)} {

        (assignOptional(std::forward<Args>(args)), ...);
    }

    /// Default copy constructor.
    Error(const Error &) noexcept = default;
    /// Default move constructor.
    Error(Error &&) noexcept = default;
    /// Default destructor.
    ~Error() noexcept override = default;
    /// Default copy assignment.
    auto operator=(const Error &) noexcept -> Error & = default;
    /// Default move assignment.
    auto operator=(Error &&) noexcept -> Error & = default;

public: // accessors
    /// Get the error category.
    ///
    /// @return The error category.
    ///
    [[nodiscard]] auto category() const noexcept -> ErrorCategory { return _category; }

    /// Access the message.
    ///
    /// @return The message text.
    ///
    [[nodiscard]] auto message() const noexcept -> const String& { return _message; }

    /// Access the location.
    ///
    /// @return The location, or an undefined location if none was set.
    ///
    [[nodiscard]] auto location() const noexcept -> Location {
        return _location.value_or(Location{});
    }

    /// Access the name-path.
    ///
    /// @return The name-path, or an empty name-path if none was set.
    ///
    [[nodiscard]] auto namePath() const noexcept -> NamePath {
        return _namePath.value_or(NamePath{});
    }

    /// Access the file path.
    ///
    /// @return The file path, or an empty file path if none was set.
    ///
    [[nodiscard]] auto filePath() const noexcept -> std::filesystem::path {
        return _filePath.value_or(std::filesystem::path{});
    }

    /// Access the system-error code.
    ///
    /// @return The system-error code, or an undefined one if none was set.
    ///
    [[nodiscard]] auto errorCode() const noexcept -> std::error_code {
        return _errorCode.value_or(std::error_code{});
    }

public:
    /// Create a copy with the given location added or replaced.
    ///
    /// @return A copy of this error, with the location added or replaced.
    ///
    [[nodiscard]] auto withLocation(const Location &location) const noexcept -> Error;

public: // conversion
    /// Convert this error into a text representation.
    ///
    /// This method creates a text representation of this error message that is safe to be used in logs,
    /// console-outputs, or user-interfaces. Exceedingly long paths and texts are trimmed and control characters
    /// are escaped.
    ///
    /// @return The string representation of this error.
    ///
    [[nodiscard]] auto toText() const noexcept -> String;

public: // implement std::exception
    /// Return a null-terminated string describing the error (std::exception interface).
    ///
    /// @return A C-string with the error message; suitable for logging or exception handling.
    ///
    [[nodiscard]] auto what() const noexcept -> const char* override;

private:
    void assignOptional(const Location &loc) noexcept { _location = loc; }
    void assignOptional(Location &&loc) noexcept { _location = std::move(loc); }
    void assignOptional(const NamePath &path) noexcept { _namePath = path; }
    void assignOptional(NamePath &&path) noexcept { _namePath = std::move(path); }
    void assignOptional(const std::filesystem::path &fp) noexcept { _filePath = fp; }
    void assignOptional(std::filesystem::path &&fp) noexcept { _filePath = std::move(fp); }
    void assignOptional(const std::error_code &ec) noexcept { _errorCode = ec; }
    void assignOptional(std::error_code &&ec) noexcept { _errorCode = std::move(ec); }
    void updateWhatBuffer() const noexcept;

private:
    ErrorCategory _category{ErrorCategory::Internal}; ///< The error category.
    String _message; ///< The message.
    std::optional<Location> _location; ///< The optional location of the error.
    std::optional<NamePath> _namePath; ///< The optional name path of the error.
    std::optional<std::filesystem::path> _filePath; ///< The optional file path of the error.
    std::optional<std::error_code> _errorCode; ///< The optional error code.
    mutable std::string _whatBuffer; ///< An internal buffer for the `what()` method.
};


}


template <>
struct std::formatter<erbsland::conf::Error> : std::formatter<std::string> {
    auto format(const erbsland::conf::Error &error, format_context& ctx) const {
        return std::formatter<std::string>::format(error.toText().toCharString(), ctx);
    }
};
