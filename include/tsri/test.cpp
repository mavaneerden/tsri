#include <type_traits>

#include "tsri.hpp"

class registers
{
private:
    using test_register_rw_test_field_rw_base_t =
        tsri::fields::field<0U, 4U, tsri::fields::field_types::read_write, 0U>;
    using test_register_rw_test_field_ro_base_t = tsri::fields::field<4U, 4U, tsri::fields::field_types::read_only, 0U>;
    using test_register_rw_test_field_wo_base_t =
        tsri::fields::field<12U, 1U, tsri::fields::field_types::write_only, 0U>;
    using test_register_rw_test_field_any_value_base_t =
        tsri::fields::field<13U, 3U, tsri::fields::field_types::read_write, 0U>;
    using test_register_rw_test_field_sc_base_t =
        tsri::fields::field<20U, 4U, tsri::fields::field_types::self_clearing, 0U>;
    using test_register_rw_test_field_wc_base_t =
        tsri::fields::field<28U, 4U, tsri::fields::field_types::write_clear, 0U>;

public:
    struct test_register_rw :
        public tsri::registers::register_read_write<
            0x14000000U,
            0x04U,
            0U,
            true,
            test_register_rw_test_field_rw_base_t,
            test_register_rw_test_field_ro_base_t,
            test_register_rw_test_field_wo_base_t,
            test_register_rw_test_field_sc_base_t,
            test_register_rw_test_field_wc_base_t,
            test_register_rw_test_field_any_value_base_t>
    {
        struct test_field_rw : public test_register_rw_test_field_rw_base_t
        {
            using test_register_rw_test_field_rw_base_t::test_register_rw_test_field_rw_base_t;

            struct bit : public test_register_rw_test_field_rw_base_t::bit
            {
                using test_register_rw_test_field_rw_base_t::bit::bit;

                static constexpr auto BIT0 = test_register_rw_test_field_rw_base_t::bit{ 0U };
                static constexpr auto BIT1 = test_register_rw_test_field_rw_base_t::bit{ 1U };
                static constexpr auto BIT2 = test_register_rw_test_field_rw_base_t::bit{ 2U };
                static constexpr auto BIT3 = test_register_rw_test_field_rw_base_t::bit{ 3U };
            };

            struct value
            {
                static constexpr auto SOME_VALUE  = test_register_rw_test_field_rw_base_t::value{ 0U };
                static constexpr auto SOME_VALUE2 = test_register_rw_test_field_rw_base_t::value{ 1U };
                static constexpr auto SOME_VALUE3 = test_register_rw_test_field_rw_base_t::value{ 2U };
                static constexpr auto NULL_       = test_register_rw_test_field_rw_base_t::value{ 3U };

                value()                                = delete;
                value(value&&)                         = delete;
                value(const value&)                    = delete;
                auto operator=(value&&) -> value&      = delete;
                auto operator=(const value&) -> value& = delete;
                ~value()                               = delete;
            };
        };

        struct test_field_ro : public test_register_rw_test_field_ro_base_t
        {
            using test_register_rw_test_field_ro_base_t::test_register_rw_test_field_ro_base_t;

            struct bit : public test_register_rw_test_field_ro_base_t::bit
            {
                using test_register_rw_test_field_ro_base_t::bit::bit;

                static constexpr auto BIT0 = test_register_rw_test_field_ro_base_t::bit{ 0U };
                static constexpr auto BIT1 = test_register_rw_test_field_ro_base_t::bit{ 1U };
                static constexpr auto BIT2 = test_register_rw_test_field_ro_base_t::bit{ 2U };
                static constexpr auto BIT3 = test_register_rw_test_field_ro_base_t::bit{ 3U };
            };
        };

        struct test_field_wo : public test_register_rw_test_field_wo_base_t
        {
            using test_register_rw_test_field_wo_base_t::test_register_rw_test_field_wo_base_t;

            struct bit : public test_register_rw_test_field_wo_base_t::bit
            {
                using test_register_rw_test_field_wo_base_t::bit::bit;

                static constexpr auto BIT0 = test_register_rw_test_field_wo_base_t::bit{ 0U };
                static constexpr auto BIT1 = test_register_rw_test_field_wo_base_t::bit{ 1U };
                static constexpr auto BIT2 = test_register_rw_test_field_wo_base_t::bit{ 2U };
                static constexpr auto BIT3 = test_register_rw_test_field_wo_base_t::bit{ 3U };
            };

            struct value
            {
                // For bool
                static constexpr auto ZERO = test_register_rw_test_field_wo_base_t::value{ 0U };
                static constexpr auto ONE  = test_register_rw_test_field_wo_base_t::value{ 1U };

                value()                                = delete;
                value(value&&)                         = delete;
                value(const value&)                    = delete;
                auto operator=(value&&) -> value&      = delete;
                auto operator=(const value&) -> value& = delete;
                ~value()                               = delete;
            };
        };

        struct test_field_sc : public test_register_rw_test_field_sc_base_t
        {
            struct value
            {
                // For SC
                static constexpr auto ONE = test_register_rw_test_field_sc_base_t::value{ 1U };

                value()                                = delete;
                value(value&&)                         = delete;
                value(const value&)                    = delete;
                auto operator=(value&&) -> value&      = delete;
                auto operator=(const value&) -> value& = delete;
                ~value()                               = delete;
            };
        };

        struct test_field_wc : public test_register_rw_test_field_wc_base_t
        {
            struct value
            {
                // For WC
                static constexpr auto ONE = test_register_rw_test_field_wc_base_t::value{ 1U };

                value()                                = delete;
                value(value&&)                         = delete;
                value(const value&)                    = delete;
                auto operator=(value&&) -> value&      = delete;
                auto operator=(const value&) -> value& = delete;
                ~value()                               = delete;
            };
        };

        struct test_field_any_value : public test_register_rw_test_field_any_value_base_t
        {
            using test_register_rw_test_field_any_value_base_t::test_register_rw_test_field_any_value_base_t;

            struct bit : public test_register_rw_test_field_any_value_base_t::bit
            {
                using test_register_rw_test_field_any_value_base_t::bit::bit;

                static constexpr auto BIT0 = test_register_rw_test_field_any_value_base_t::bit{ 0U };
                static constexpr auto BIT1 = test_register_rw_test_field_any_value_base_t::bit{ 1U };
                static constexpr auto BIT2 = test_register_rw_test_field_any_value_base_t::bit{ 2U };
            };

            struct value : test_register_rw_test_field_any_value_base_t::value
            {
                using test_register_rw_test_field_any_value_base_t::value::value;
            };
        };
    };

private:
    using test_register_ro_test_field_ro_base_t = tsri::fields::field<0U, 4U, tsri::fields::field_types::read_only, 0U>;

public:
    struct test_register_ro :
        public tsri::registers::
            register_read_write<0x14000000U, 0x08U, 0U, false, test_register_ro_test_field_ro_base_t>
    {
        struct test_field_ro : public test_register_rw_test_field_ro_base_t
        {
            using test_register_rw_test_field_ro_base_t::test_register_rw_test_field_ro_base_t;

            struct bit : public test_register_rw_test_field_ro_base_t::bit
            {
                using test_register_rw_test_field_ro_base_t::bit::bit;

                static constexpr auto BIT0 = test_register_rw_test_field_ro_base_t::bit{ 0U };
                static constexpr auto BIT1 = test_register_rw_test_field_ro_base_t::bit{ 1U };
                static constexpr auto BIT2 = test_register_rw_test_field_ro_base_t::bit{ 2U };
                static constexpr auto BIT3 = test_register_rw_test_field_ro_base_t::bit{ 3U };
            };
        };
    };
};

int main(int argc, char** argv)
{
    // clang-format off
    const unsigned int argc_unsigned = argc;

    using test_register_rw     = registers::test_register_rw;
    using test_register_ro     = registers::test_register_ro;
    using test_field_rw        = registers::test_register_rw::test_field_rw;
    using test_field_wo        = registers::test_register_rw::test_field_wo;
    using test_field_ro        = registers::test_register_rw::test_field_ro;
    using test_field_any_value = registers::test_register_rw::test_field_any_value;

    // auto result = test_register_rw::get();
    // auto result = test_register_rw::is_any_bit_set<test_register_rw::test_field_rw::bit::BIT3, test_register_rw::test_field_rw::bit::BIT2>();
    // auto result = test_register_rw::unsafe_operations::is_any_bit_set(argc_unsigned);
    // auto result = test_register_rw::are_all_bits_set<test_register_rw::test_field_rw::bit::BIT3>();
    // auto result = test_register_rw::unsafe_operations::are_all_bits_set(argc_unsigned);
    // auto result = test_register_rw::is_any_bit_set(); auto result = test_register_rw::are_all_bits_set();
    // auto result = test_register_rw::get_fields<test_register_rw::test_field_rw>().get();
    auto fields = test_register_rw::get_fields<test_register_rw::test_field_rw, test_register_rw::test_field_ro>();
    auto result = fields.get<test_register_rw::test_field_ro>();
    // auto result = 0;

    // auto result = test_register_rw::is_any_bit_set(
    //     test_field_rw{ test_field_rw::bit::BIT0, test_field_rw::bit::BIT1 }, test_field_ro{ test_field_ro::bit::BIT0 });

    test_register_rw::set_fields(test_field_rw::value::SOME_VALUE, test_field_any_value::value{ 0U }, test_field_wo::reset_value);

    auto x = 0U == test_field_rw::value::SOME_VALUE;


    test_register_rw::set_bits(
        test_field_rw {
            test_field_rw::bit::BIT0,
            test_field_rw::bit::BIT3,
            test_field_rw::bit{argc_unsigned}
        },
        test_field_wo {
            test_field_wo::bit::BIT0,
            test_field_wo::bit::BIT3
        }
    );

    test_register_rw::set_fields_overwrite(
        test_field_rw::value::SOME_VALUE2,
        test_field_any_value::value{ result }
    );

    // test_register_rw::set(1u);
    // test_register_rw::reset();
    // test_register_rw::set_fields<test_register_rw::test_field_rw, test_register_rw::test_field_wo,
    // test_register_rw::test_field_sc>(test_register_rw::test_field_rw::value::SOME_VALUE2, true, argc_unsigned);
    // test_register_rw::set_fields<test_register_rw::test_field_rw>(test_register_rw::test_field_rw::value::SOME_VALUE);
    // test_register_rw::set_fields_overwrite<test_register_rw::test_field_rw, test_register_rw::test_field_wo,
    // test_register_rw::test_field_sc>(test_register_rw::test_field_rw::value::SOME_VALUE2, true,
    // test_register_rw::test_field_sc::value_on_reset);
    // test_register_rw::set_fields_overwrite_size_optimized<test_register_rw::test_field_rw,
    // test_register_rw::test_field_wo,
    // test_register_rw::test_field_sc>(test_register_rw::test_field_rw::value::SOME_VALUE2, 1U, 1U);
    // test_register_rw::clear_fields<test_register_rw::test_field_rw, test_register_rw::test_field_wc>();

    // test_register_rw::set_bits<test_register_rw::test_field_rw::bit_t::BIT3,
    // test_register_rw::test_field_wo::bit_t::BIT3, 21U>();
    // test_register_rw::unsafe_operations::set_bits(test_register_rw::test_field_rw::bit_t::BIT3,
    // test_register_rw::test_field_wo::bit_t::BIT3, 21U, argc_unsigned);
    // test_register_rw::clear_bits<test_register_rw::test_field_rw::bit_t::BIT3>();
    // test_register_rw::unsafe_operations::clear_bits(test_register_rw::test_field_rw::bit_t::BIT3, argc_unsigned);
    // test_register_rw::toggle_bits<test_register_rw::test_field_rw::bit_t::BIT3>();
    // test_register_rw::unsafe_operations::toggle_bits(test_register_rw::test_field_rw::bit_t::BIT3, argc_unsigned);

    // clang-format on

    return static_cast<int>(result);
}
