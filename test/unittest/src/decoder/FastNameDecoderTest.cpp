// Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0


#include "TestHelper.hpp"

#include <erbsland/conf/Error.hpp>
#include <erbsland/conf/Source.hpp>
#include <erbsland/conf/impl/decoder/FastNameDecoder.hpp>


using namespace el::conf;
using impl::Char;
using impl::FastNameDecoder;
using impl::internalView;


TESTED_TARGETS(FastNameDecoder)
class FastNameDecoderTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void requireAndNext(FastNameDecoder &decoder, const char32_t expectedUnicode) {
        REQUIRE(decoder.character() == expectedUnicode);
        decoder.next();
    }

    void requireEndOfData(FastNameDecoder &decoder) {
        REQUIRE(decoder.character() == Char::EndOfData);
    }

    void testSimpleSequence() {
        FastNameDecoder decoder(u8"abc\n😀\nxyz");
        decoder.initialize();
        WITH_CONTEXT(requireAndNext(decoder, U'a'));
        WITH_CONTEXT(requireAndNext(decoder, U'b'));
        WITH_CONTEXT(requireAndNext(decoder, U'c'));
        WITH_CONTEXT(requireAndNext(decoder, U'\n'));
        WITH_CONTEXT(requireAndNext(decoder, U'😀'));
        WITH_CONTEXT(requireAndNext(decoder, U'\n'));
        WITH_CONTEXT(requireAndNext(decoder, U'x'));
        WITH_CONTEXT(requireAndNext(decoder, U'y'));
        WITH_CONTEXT(requireAndNext(decoder, U'z'));
        WITH_CONTEXT(requireEndOfData(decoder));
    }

    void testTransactionFromStart() {
        FastNameDecoder decoder(u8"abcdef");
        decoder.initialize();
        {
            auto transaction = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'a'));
            WITH_CONTEXT(requireAndNext(decoder, U'b'));
            WITH_CONTEXT(requireAndNext(decoder, U'c'));
            REQUIRE_EQUAL(transaction.capturedString(), u8"abc");
            REQUIRE_EQUAL(transaction.capturedSize(), 3);
        } // rollback
        {
            auto transaction = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'a'));
            WITH_CONTEXT(requireAndNext(decoder, U'b'));
            REQUIRE_EQUAL(transaction.capturedString(), u8"ab");
            REQUIRE_EQUAL(transaction.capturedSize(), 2);
        } // rollback
        {
            auto transaction = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'a'));
            WITH_CONTEXT(requireAndNext(decoder, U'b'));
            WITH_CONTEXT(requireAndNext(decoder, U'c'));
            WITH_CONTEXT(requireAndNext(decoder, U'd'));
            REQUIRE_EQUAL(transaction.capturedString(), u8"abcd");
            REQUIRE_EQUAL(transaction.capturedSize(), 4);
        } // rollback
        WITH_CONTEXT(requireAndNext(decoder, U'a'));
        WITH_CONTEXT(requireAndNext(decoder, U'b'));
        {
            auto transaction = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'c'));
            WITH_CONTEXT(requireAndNext(decoder, U'd'));
            WITH_CONTEXT(requireAndNext(decoder, U'e'));
            WITH_CONTEXT(requireAndNext(decoder, U'f'));
            REQUIRE_EQUAL(transaction.capturedString(), u8"cdef");
            REQUIRE_EQUAL(transaction.capturedSize(), 4);
        } // rollback
        {
            auto transaction = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'c'));
            WITH_CONTEXT(requireAndNext(decoder, U'd'));
            REQUIRE_EQUAL(transaction.capturedString(), u8"cd");
            REQUIRE_EQUAL(transaction.capturedSize(), 2);
            transaction.commit();
        } // no rollback
        WITH_CONTEXT(requireAndNext(decoder, U'e'));
        WITH_CONTEXT(requireAndNext(decoder, U'f'));
        WITH_CONTEXT(requireEndOfData(decoder));
    }

    void testNestedTransactions1() {
        FastNameDecoder decoder(u8"abcdef");
        decoder.initialize();
        {
            auto transaction1 = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'a'));
            WITH_CONTEXT(requireAndNext(decoder, U'b'));
            REQUIRE_EQUAL(transaction1.capturedString(), u8"ab");
            REQUIRE_EQUAL(transaction1.capturedSize(), 2);
            {
                auto transaction2 = impl::Transaction{decoder};
                WITH_CONTEXT(requireAndNext(decoder, U'c'));
                WITH_CONTEXT(requireAndNext(decoder, U'd'));
                REQUIRE_EQUAL(transaction2.capturedString(), u8"cd");
                REQUIRE_EQUAL(transaction2.capturedSize(), 2);
                {
                    auto transaction3 = impl::Transaction{decoder};
                    WITH_CONTEXT(requireAndNext(decoder, U'e'));
                    WITH_CONTEXT(requireAndNext(decoder, U'f'));
                    REQUIRE_EQUAL(transaction3.capturedString(), u8"ef");
                    REQUIRE_EQUAL(transaction3.capturedSize(), 2);
                    transaction3.commit();
                }
                REQUIRE_EQUAL(transaction2.capturedString(), u8"cdef");
                REQUIRE_EQUAL(transaction2.capturedSize(), 4);
                transaction2.commit();
            }
            REQUIRE_EQUAL(transaction1.capturedString(), u8"abcdef");
            REQUIRE_EQUAL(transaction1.capturedSize(), 6);
            transaction1.commit();
        }
        WITH_CONTEXT(requireEndOfData(decoder));
    }

    void testNestedTransactions2() {
        FastNameDecoder decoder(u8"abcdef");
        decoder.initialize();
        {
            auto transaction1 = impl::Transaction{decoder};
            WITH_CONTEXT(requireAndNext(decoder, U'a'));
            WITH_CONTEXT(requireAndNext(decoder, U'b'));
            REQUIRE_EQUAL(transaction1.capturedString(), u8"ab");
            REQUIRE_EQUAL(transaction1.capturedSize(), 2);
            {
                auto transaction2 = impl::Transaction{decoder};
                WITH_CONTEXT(requireAndNext(decoder, U'c'));
                WITH_CONTEXT(requireAndNext(decoder, U'd'));
                REQUIRE_EQUAL(transaction2.capturedString(), u8"cd");
                REQUIRE_EQUAL(transaction2.capturedSize(), 2);
                {
                    auto transaction3 = impl::Transaction{decoder};
                    WITH_CONTEXT(requireAndNext(decoder, U'e'));
                    WITH_CONTEXT(requireAndNext(decoder, U'f'));
                    REQUIRE_EQUAL(transaction3.capturedString(), u8"ef");
                    REQUIRE_EQUAL(transaction3.capturedSize(), 2);
                    transaction3.commit();
                }
                REQUIRE_EQUAL(transaction2.capturedString(), u8"cdef");
                REQUIRE_EQUAL(transaction2.capturedSize(), 4);
                // ROLLBACK
            }
            WITH_CONTEXT(requireAndNext(decoder, U'c'));
            WITH_CONTEXT(requireAndNext(decoder, U'd'));
            {
                auto transaction2 = impl::Transaction{decoder};
                WITH_CONTEXT(requireAndNext(decoder, U'e'));
                WITH_CONTEXT(requireAndNext(decoder, U'f'));
                REQUIRE_EQUAL(transaction2.capturedString(), u8"ef");
                REQUIRE_EQUAL(transaction2.capturedSize(), 2);
                // ROLLBACK
            }
            WITH_CONTEXT(requireAndNext(decoder, U'e'));
            WITH_CONTEXT(requireAndNext(decoder, U'f'));
            REQUIRE_EQUAL(transaction1.capturedString(), u8"abcdef");
            REQUIRE_EQUAL(transaction1.capturedSize(), 6);
            transaction1.commit();
        }
        WITH_CONTEXT(requireEndOfData(decoder));
    }
};

