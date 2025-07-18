// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "String.hpp"

#include <chrono>
#include <cstdint>


namespace erbsland::conf {


/// A time offset.
///
/// @tested `TimeOffsetTest`
///
class TimeOffset final {
private:
    /// Magic number to mark local time.
    ///
    static constexpr auto LOCAL_TIME_OFFSET = std::chrono::seconds{0x7fffffff};

public:
    /// The precision used for the text format.
    ///
    enum class Precision : uint8_t {
        Automatic, ///< Depends on if minutes, and seconds are zero.
        Seconds, ///< Format: [+-]hh:mm:ss
        Minutes, ///< Format: [+-]hh:mm
        Hours, ///< Format: [+-]hh
    };

public:
    /// Create local time.
    ///
    TimeOffset() = default;

    /// Create a time offset from the given number of seconds.
    ///
    /// @param seconds The offset in seconds. Zero for UTC.
    /// @throws std::out_of_range if the offset is out of the valid range.
    ///
    constexpr explicit TimeOffset(std::chrono::seconds seconds) : _seconds{seconds} {
        if (seconds < std::chrono::seconds{-86399} || seconds > std::chrono::seconds{86399}) {
            throw std::out_of_range("Offset is out of range");
        }
    }
    constexpr explicit TimeOffset(int32_t seconds) : _seconds{seconds} {
        if (seconds < -86399 || seconds > 86399) {
            throw std::out_of_range("Offset is out of range");
        }
    }

    /// Create a time offset from the given hours, minutes and seconds.
    ///
    /// @param isNegative If this is a negative offset.
    /// @param hours The hours in the range 0-23.
    /// @param minutes The minutes in the range 0-59.
    /// @param seconds The seconds in the range 0-59.
    /// @throws std::out_of_range if one of the values is out of the valid range.
    ///
    constexpr TimeOffset(bool isNegative, int hours, int minutes, int seconds)
        : _seconds{(seconds + minutes * 60 + hours * 3600) * (isNegative ? -1 : 1)} {
        if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59 ||
            _seconds < std::chrono::seconds{-86399} || _seconds > std::chrono::seconds{86399} ) {
            throw std::out_of_range("Offset is out of range");
        }
    }

    // defaults
    ~TimeOffset() = default;
    TimeOffset(const TimeOffset&) = default;
    auto operator=(const TimeOffset&) -> TimeOffset& = default;

public: // accessors
    /// Test if this is local time.
    ///
    [[nodiscard]] auto isLocalTime() const noexcept -> bool;

    /// Test if this is UTC (zero offset)
    ///
    [[nodiscard]] auto isUTC() const noexcept -> bool;

    /// Get the offset as positive or negative total number of seconds.
    ///
    /// @return The offset in seconds, and zero for UTC and local time.
    ///
    [[nodiscard]] auto totalSeconds() const noexcept -> std::chrono::seconds;

    /// If this offset is negative.
    ///
    /// @return `true` if this is a negative offset, `false` for zero, local time and positive offsets.
    ///
    [[nodiscard]] auto isNegative() const noexcept -> bool;

    /// The second part of the offset.
    ///
    /// @return The second part of the offset, always in the positive range 0-59. Zero for local time.
    ///
    [[nodiscard]] auto seconds() const noexcept -> int;

    /// The minute part of the offset.
    ///
    /// @return The minute part of the offset, always in the positive range 0-59. Zero for local time.
    ///
    [[nodiscard]] auto minutes() const noexcept -> int;

    /// The hour part of the offset.
    ///
    /// @return The hour part of the offset, always in the positive range 0-23. Zero for local time.
    ///
    [[nodiscard]] auto hours() const noexcept -> int;

public: // conversion
    /// Convert this offset into text.
    ///
    /// Returns an empty text for local time.
    /// Returns "z" for UTC
    /// For `precision` set to `Precision::Automatic`, the minimal required elements are returned.
    /// if minutes and seconds are zero, only the hours are returned. If minutes are non-zero, the
    /// minutes are returned too and if seconds are not zero, the seconds are returned as well.
    ///
    /// - `Precision::Hours`: Returns an offset in the format "+01".
    /// - `Precision::Minutes`: Returns an offset in the format "-01:23".
    /// - `Precision::Seconds`: Returns an offset in the format "+01:14:35".
    ///
    [[nodiscard]] auto toText(Precision precision = Precision::Automatic) const noexcept -> String;

public:
    /// Create a UTC offset.
    ///
    /// This static method is more expressive than an offset with zero seconds.
    ///
    [[nodiscard]] constexpr static auto utc() noexcept -> TimeOffset { return TimeOffset{0}; }

private:
    std::chrono::seconds _seconds{LOCAL_TIME_OFFSET};
};


}


template <>
struct std::formatter<erbsland::conf::TimeOffset> : std::formatter<std::string> {
    auto format(const erbsland::conf::TimeOffset timeOffset, format_context& ctx) const {
        return std::formatter<std::string>::format(timeOffset.toText().toCharString(), ctx);
    }
};

