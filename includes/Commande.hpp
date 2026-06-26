/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commande.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucien <lucien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 00:00:00 by lucien           #+#    #+#             */
/*   Updated: 2026/05/05 00:00:00 by lucien          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include <vector>

class Commande
{
private:
	std::vector<Channel>  *channels_;
	std::vector<Client *> &clients_;

public:
	Commande(std::vector<Client *> &clients, std::vector<Channel> &channels)
		: channels_(&channels), clients_(clients) {}

	std::vector<Channel>  *getChannels() { return channels_; }
	std::vector<Client *> &getClients()  { return clients_; }
};
