/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commande.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wivallee <wivallee@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 14:27:31 by luda-cun          #+#    #+#             */
/*   Updated: 2026/08/22 18:09:54 by wivallee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"

/**
 * @brief Wrapper class for command handling
 * Delegates to specific Command classes
 */
class Commande
{
private:
	std::vector<Client*> &clients_;
	std::vector<Channel*> &channels_;

public:
	Commande(std::vector<Client *> &clients, std::vector<Channel *> &channels);
	~Commande() {}

	std::vector<Client *> &getClients() { return clients_; }
	std::vector<Channel*> &getChannels() { return channels_; }
};

/**
 * @brief Parse functions for commands
 */

/**
 * @brief Parse JOIN command arguments
 *
 * @param args Raw arguments string from JOIN command
 * @param channelName Reference to store parsed channel name
 * @param password Reference to store parsed password (optional)
 * @return void
 */
void parseJoin(const std::string &args, std::string &channelName, std::string &password);

/**
 * @brief Parse KICK command arguments
 *
 * @param args Raw arguments string from KICK command
 * @param target Reference to store target user to kick
 * @param reason Reference to store kick reason (optional)
 * @return void
 */
void parseKick(const std::string &args, std::string &target, std::string &reason);

/**
 * @brief Parse MODE command arguments
 *
 * @param args Raw arguments string from MODE command
 * @param mode Reference to store mode string (+i, -k, etc)
 * @param modeArgs Reference to store mode arguments (password, limit, etc)
 * @return void
 */
void parseMode(const std::string &args, std::string &channelName, std::string &mode, std::string &modeArgs);

/**
 * @brief Parse TOPIC command arguments
 *
 * @param args Raw arguments string from TOPIC command
 * @param topic Reference to store new topic
 * @return void
 */
void parseTopic(const std::string &args, std::string &channel_name, std::string &topic);
