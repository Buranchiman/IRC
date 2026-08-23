/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commande.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luda-cun <luda-cun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 14:27:31 by luda-cun          #+#    #+#             */
/*   Updated: 2026/08/23 14:52:04 by luda-cun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"


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

void parseJoin(const std::string &args, std::string &channelName, std::string &password);

void parseKick(const std::string &args, std::string &target, std::string &reason);

void parseMode(const std::string &args, std::string &channelName, std::string &mode, std::string &modeArgs);

void parseTopic(const std::string &args, std::string &channel_name, std::string &topic);
