/*
 * Copyright (c) 2013-2015 John Connor (BM-NC49AxAjcqVcF5jNPu85Rb8MJ2d9JqZt)
 *
 * This file is part of vanillacoin.
 *
 * vanillacoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COIN_INCENTIVE_MANAGER_HPP
#define COIN_INCENTIVE_MANAGER_HPP

#include <chrono>
#include <cstdint>
#include <ctime>
#include <map>
#include <mutex>
#include <vector>

#include <boost/asio.hpp>

#include <coin/address_manager.hpp>
#include <coin/incentive_vote.hpp>
#include <coin/output.hpp>

namespace coin {

    class key;
    class key_public;
    class message;
    class stack_impl;
    class transaction_in;
    
    /**
     * Implements an incentive manager.
     */
    class incentive_manager
        : public std::enable_shared_from_this<incentive_manager>
    {
        public:
        
            /**
             * Constructor
             * @param ios The boost::asio::io_service.
             * @param s The boost::asio::strand.
             * @param owner The stack_impl.
             */
            incentive_manager(
                boost::asio::io_service & ios, boost::asio::strand & s,
                stack_impl & owner
            );
        
            /**
             * Starts
             */
            void start();
        
            /**
             * Stops
             */
            void stop();
    
            /**
             * Handles a message.
             * @param ep The boost::asio::ip::tcp::endpoint.
             * @param msg The message.
             */
            bool handle_message(
                const boost::asio::ip::tcp::endpoint & ep, message & msg
            );
        
            /**
             * The collateral balance.
             */
            const double & collateral_balance() const;
        
        private:
        
            /**
             * The tick handler.
             * @param interval The interval.
             */
            void do_tick(const std::uint32_t & interval);
        
            /**
             * The find input handler.
             * @param interval The interval.
             */
            void do_tick_check_inputs(const std::uint32_t & interval);
        
            /**
             * Votes for the wallet address for height + 2.
             * @param wallet_address The wallet address.
             */
            bool vote(const std::string & wallet_address);
        
            /**
             * If true the collateral is valid.
             */
            bool m_collateral_is_valid;
        
            /**
             * The collateral balance.
             */
            double m_collateral_balance;
        
        protected:
        
            /**
             * Returns the K closets nodes to the block height.
             * @param nodes The nodes.
             * @param block_height The block heigt.
             * @param k The maximum number of nodes.
             */
            std::vector<address_manager::recent_endpoint_t> k_closest(
                const std::vector<address_manager::recent_endpoint_t> & nodes,
                const std::uint32_t & block_height,
                const std::uint32_t & k = 20
            );
        
            /**
             * Selects coins that qualify as collateral (if enforced).
             */
            std::vector<output> select_coins();
        
            /**
             * Gets a transaction_in from an output.
             * @param out The output.
             * @param tx_in The transaction_in (out).
             * @param public_key The key_public.
             * @param k The key.
             */
            bool tx_in_from_output(
                const output & out, transaction_in & tx_in,
                key_public & public_key, key & k
            );

            /**
             * The boost::asio::io_service.
             */
            boost::asio::io_service & io_service_;
        
            /**
             * The boost::asio::strand.
             */
            boost::asio::strand & strand_;
        
            /**
             * The stack_impl.
             */
            stack_impl & stack_impl_;
        
            /**
             * The timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_;
        
            /**
             * The check inputs timer.
             */
            boost::asio::basic_waitable_timer<
                std::chrono::steady_clock
            > timer_check_inputs_;
        
            /**
             * The last block height.
             */
            std::uint32_t last_block_height_;

            /**
             * The candidates.
             */
            std::map<
                address_manager::recent_endpoint_t,
                std::pair<std::time_t, std::uint32_t>
            > candidates_;
        
            /**
             * The candidates mutex.
             */
            std::mutex mutex_candidates_;
        
            /**
             * The votes.
             */
            std::map<
                std::uint32_t,
                std::map<std::string, std::vector<incentive_vote> >
            > votes_;
        
            /**
             * The votes mutex.
             */
            std::mutex mutex_votes_;
    };
}

#endif // COIN_INCENTIVE_MANAGER_HPP
