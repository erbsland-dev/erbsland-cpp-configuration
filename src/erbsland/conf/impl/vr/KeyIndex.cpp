// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "KeyIndex.hpp"


#include "../utilities/InternalError.hpp"

#include <algorithm>
#include <utility>


namespace erbsland::conf::impl {


namespace {


template<CaseSensitivity tCaseSensitivity>
struct KeyHash {
    auto operator()(const Key &key) const noexcept -> std::size_t {
        return key.hash(tCaseSensitivity);
    }
};

using KeyHashCaseInsensitive = KeyHash<CaseSensitivity::CaseInsensitive>;
using KeyHashCaseSensitive = KeyHash<CaseSensitivity::CaseSensitive>;

template<CaseSensitivity tCaseSensitivity>
struct KeyElementHash {
    auto operator()(const String &element) const noexcept -> std::size_t {
        return Key::elementHash(element, tCaseSensitivity);
    }
};

using KeyElementHashCaseInsensitive = KeyElementHash<CaseSensitivity::CaseInsensitive>;
using KeyElementHashCaseSensitive = KeyElementHash<CaseSensitivity::CaseSensitive>;

template<CaseSensitivity tCaseSensitivity>
struct KeyEqual {
    auto operator()(const Key &lhs, const Key &rhs) const noexcept -> bool {
        return lhs.isEqual(rhs, tCaseSensitivity);
    }
};

using KeyEqualCaseInsensitive = KeyEqual<CaseSensitivity::CaseInsensitive>;
using KeyEqualCaseSensitive = KeyEqual<CaseSensitivity::CaseSensitive>;

template<CaseSensitivity tCaseSensitivity>
struct KeyElementEqual {
    auto operator()(const String &lhs, const String &rhs) const noexcept -> bool {
        if constexpr (tCaseSensitivity == CaseSensitivity::CaseSensitive) {
            return lhs == rhs;
        } else {
            return lhs.characterCompare(rhs, CaseSensitivity::CaseInsensitive) == std::strong_ordering::equal;
        }
    }
};

using KeyElementEqualCaseInsensitive = KeyElementEqual<CaseSensitivity::CaseInsensitive>;
using KeyElementEqualCaseSensitive = KeyElementEqual<CaseSensitivity::CaseSensitive>;


template<typename tKeyHash, typename tKeyEqual>
class KeyIndexDataSingle : public KeyIndexData {
public:
    [[nodiscard]] auto hasKey(const Key &key) const noexcept -> bool override {
        return _keys.contains(key);
    }
    [[nodiscard]] auto hasKeyElement(const String &element, const std::size_t index) const noexcept -> bool override {
        if (index > 0) {
            return false;
        }
        return hasKey(Key{element});
    }
    auto tryAddKey(const Key &key) -> bool override {
        return _keys.insert(key).second;
    }

private:
    std::unordered_set<Key, tKeyHash, tKeyEqual> _keys;
};


using KeyIndexDataSingleCaseInsensitive = KeyIndexDataSingle<KeyHashCaseInsensitive, KeyEqualCaseInsensitive>;
using KeyIndexDataSingleCaseSensitive = KeyIndexDataSingle<KeyHashCaseSensitive, KeyEqualCaseSensitive>;


template<typename tKeyHash, typename tKeyEqual, typename tKeyElementHash, typename tKeyElementEqual>
class KeyIndexDataMultiple : public KeyIndexData {
public:
    explicit KeyIndexDataMultiple(const std::size_t elementCount) : _keysByElement(elementCount) {}

    [[nodiscard]] auto hasKey(const Key &key) const noexcept -> bool override {
        return _keys.contains(key);
    }
    [[nodiscard]] auto hasKeyElement(const String &element, const std::size_t index) const noexcept -> bool override {
        if (index >= _keysByElement.size()) {
            return false;
        }
        return _keysByElement[index].contains(element);
    }
    auto tryAddKey(const Key &key) -> bool override {
        if (_keys.insert(key).second) {
            for (std::size_t i = 0; i < _keysByElement.size(); ++i) {
                _keysByElement[i].insert(key.element(i));
            }
            return true;
        }
        return false;
    }

private:
    std::unordered_set<Key, tKeyHash, tKeyEqual> _keys;
    std::vector<std::unordered_set<String, tKeyElementHash, tKeyElementEqual>> _keysByElement;
};


using KeyIndexDataMultipleCaseInsensitive = KeyIndexDataMultiple<
    KeyHashCaseInsensitive, KeyEqualCaseInsensitive, KeyElementHashCaseInsensitive, KeyElementEqualCaseInsensitive>;
using KeyIndexDataMultipleCaseSensitive = KeyIndexDataMultiple<
    KeyHashCaseSensitive, KeyEqualCaseSensitive, KeyElementHashCaseSensitive, KeyElementEqualCaseSensitive>;


}



KeyIndex::KeyIndex(
    Name name,
    const CaseSensitivity caseSensitivity,
    const std::size_t elementCount)
:
    _name{std::move(name)},
    _caseSensitivity{caseSensitivity},
    _elementCount{elementCount} {

    ERBSLAND_CONF_REQUIRE_SAFETY(elementCount > 0, "The element count must be greater than zero");
    if (_elementCount == 1) {
        if (_caseSensitivity == CaseSensitivity::CaseInsensitive) {
            _data = std::make_unique<KeyIndexDataSingleCaseInsensitive>();
        } else {
            _data = std::make_unique<KeyIndexDataSingleCaseSensitive>();
        }
    } else {
        if (_caseSensitivity == CaseSensitivity::CaseInsensitive) {
            _data = std::make_unique<KeyIndexDataMultipleCaseInsensitive>(_elementCount);
        } else {
            _data = std::make_unique<KeyIndexDataMultipleCaseSensitive>(_elementCount);
        }
    }
}


auto KeyIndex::tryAddKey(const Key &key) -> bool {
    ERBSLAND_CONF_REQUIRE_SAFETY(key.size() == _elementCount, "The key must have the correct size");
    return _data->tryAddKey(key);
}


auto KeyIndex::hasKey(const String &keyString) const noexcept -> bool {
    if (_elementCount > 1) {
        auto key = Key{keyString.split(U',', _elementCount - 1)};
        if (key.size() != _elementCount) {
            return false;
        }
        return hasKey(key);
    }
    return hasKey(Key{keyString});
}


auto KeyIndex::hasKey(const Key &key) const noexcept -> bool {
    return _data->hasKey(key);
}


auto KeyIndex::hasKey(const String &keyString, const std::size_t index) const noexcept -> bool {
    if (index >= _elementCount) {
        return false;
    }
    if (_elementCount == 1) {
        if (index == 0) {
            return hasKey(Key{keyString});
        }
        return false;
    }
    return _data->hasKeyElement(keyString, index);
}


}

